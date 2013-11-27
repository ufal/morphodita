// This file is part of MorphoDiTa.
//
// Copyright 2013 by Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// MorphoDiTa is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// MorphoDiTa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with MorphoDiTa.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <unordered_map>

#include "common.h"
#include "feature_sequences_optimizer.h"
#include "morpho/morpho.h"
#include "tagger_trainer.h"
#include "training_maps.h"
#include "viterbi.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class FeatureSequences, int order>
class perceptron_tagger_trainer {
 public:
  typedef typename tagger_trainer<perceptron_tagger_trainer<FeatureSequences, order>>::sentence sentence;

  static void train(int iterations, const vector<sentence>& train, const vector<sentence>& heldout, FILE* in_feature_templates, FILE* out_tagger);

 private:
  static void train_viterbi(int iterations, const vector<sentence>& train, const vector<sentence>& heldout, FeatureSequences& features);
};


// Definitions
template <class FeatureSequences, int order>
void perceptron_tagger_trainer<FeatureSequences, order>::train(int iterations, const vector<sentence>& train, const vector<sentence>& heldout, FILE *in_feature_templates, FILE *out_tagger) {
  FeatureSequences features;

  eprintf("Parsing feature templates...\n");
  features.parse(order, in_feature_templates);

  eprintf("Training tagger...\n");
  train_viterbi(iterations, train, heldout, features);

  eprintf("Encoding tagger...\n");
  typedef feature_sequences_optimizer<FeatureSequences> optimizer;
  typename optimizer::optimized_feature_sequences optimized_features;
  optimizer::optimize(features, optimized_features);
  if (!optimized_features.save(out_tagger)) runtime_errorf("Cannot save feature sequences!");
}

template <class FeatureSequences, int order>
void perceptron_tagger_trainer<FeatureSequences, order>::train_viterbi(int iterations, const vector<sentence>& train, const vector<sentence>& heldout, FeatureSequences& features) {
  int best_correct = 0, best_total = 0;
  FeatureSequences best_features;

  viterbi<FeatureSequences, order> decoder(features);
  typename decltype(decoder)::cache decoder_cache(decoder);

  typename FeatureSequences::cache feature_sequences_cache(features);
  typename FeatureSequences::dynamic_features decoded_dynamic_features, gold_dynamic_features;
  vector<string> decoded_feature_sequences_keys, gold_feature_sequences_keys;

  // Train for given number of iterations
  for (int i = 0; i < iterations; i++) {
    // Train
    int train_correct = 0, train_total = 0;
    eprintf("Iteration %d: ", i + 1);

    vector<int> tags;
    for (unsigned s = 0; s < train.size(); s++) {
      auto& sentence = train[s];

      // Run Viterbi
      if (tags.size() < sentence.forms_with_tags.size()) tags.resize(sentence.forms_with_tags.size() * 2);
      decoder.tag(sentence.forms_with_tags, sentence.forms_with_tags.size(), decoder_cache, tags);

      // Compute feature sequence keys or decoded result and gold result and update alpha & gamma
      features.initialize_sentence(sentence.forms_with_tags, sentence.forms_with_tags.size(), feature_sequences_cache);
      for (int i = 0; i < int(sentence.forms_with_tags.size()); i++) {
        train_correct += tags[i] == sentence.gold_index[i];
        train_total++;

        int window[order];

        for (int j = 0; j < order && i - j >= 0; j++) window[j] = tags[i - j];
        features.compute_dynamic_features(i, window[0], &decoded_dynamic_features, decoded_dynamic_features, feature_sequences_cache);
        features.feature_keys(i, window, 0, decoded_dynamic_features, decoded_feature_sequences_keys, feature_sequences_cache);

        for (int j = 0; j < order && i - j >= 0; j++) window[j] = sentence.gold_index[i - j];
        features.compute_dynamic_features(i, window[0], &gold_dynamic_features, gold_dynamic_features, feature_sequences_cache);
        features.feature_keys(i, window, 0, gold_dynamic_features, gold_feature_sequences_keys, feature_sequences_cache);

        for (unsigned f = 0; f < features.scores.size(); f++)
          if (decoded_feature_sequences_keys[f] != gold_feature_sequences_keys[f]) {
            auto& decoded_info = features.scores[f].map[decoded_feature_sequences_keys[f]];
            decoded_info.gamma += decoded_info.alpha * (s - decoded_info.last_gamma_update);
            decoded_info.last_gamma_update = s;
            decoded_info.alpha--;

            auto& gold_info = features.scores[f].map[gold_feature_sequences_keys[f]];
            gold_info.gamma += gold_info.alpha * (s - gold_info.last_gamma_update);
            gold_info.last_gamma_update = s;
            gold_info.alpha++;
          }

      }
    }

    // Finalize incremental gamma updates
    for (auto& score : features.scores)
      for (auto& element : score.map) {
        element.second.gamma += element.second.alpha * (train.size() - element.second.last_gamma_update);
        element.second.last_gamma_update = 0;
      }
    eprintf("done, accuracy %.2f%%", train_correct * 100 / double(train_total));

    // If we have any heldout data, compute accuracy and store best tagger configuration
    if (!heldout.empty()) {
      enum { TAGS, LEMMAS, BOTH, TOTAL };
      int heldout_correct[TOTAL] = {}, heldout_total = 0;

      typedef feature_sequences_optimizer<FeatureSequences> optimizer;
      typename optimizer::optimized_feature_sequences frozen_features;
      optimizer::optimize(features, frozen_features);
      viterbi<decltype(frozen_features), order> frozen_decoder(frozen_features);
      typename decltype(frozen_decoder)::cache frozen_decoder_cache(frozen_decoder);

      for (auto& sentence : heldout) {
        if (tags.size() < sentence.forms_with_tags.size()) tags.resize(sentence.forms_with_tags.size() * 2);
        frozen_decoder.tag(sentence.forms_with_tags, sentence.forms_with_tags.size(), frozen_decoder_cache, tags);

        for (unsigned i = 0; i < sentence.forms_with_tags.size(); i++) {
          heldout_correct[TAGS] += sentence.gold[i].tag == sentence.forms_with_tags[i].tags[tags[i]].tag;
          heldout_correct[LEMMAS] += sentence.gold[i].lemma == sentence.forms_with_tags[i].tags[tags[i]].lemma;
          heldout_correct[BOTH] += sentence.gold[i].tag == sentence.forms_with_tags[i].tags[tags[i]].tag && sentence.gold[i].lemma == sentence.forms_with_tags[i].tags[tags[i]].lemma;
          heldout_total++;
        }
      }

      if (heldout_correct[BOTH] > best_correct) {
        best_correct = heldout_correct[BOTH];
        best_total = heldout_total;
        best_features = features;
      }

      eprintf(", heldout accuracy %.2f%%t/%.2f%%l/%.2f%%b", 100 * heldout_correct[TAGS] / double(heldout_total), 100 * heldout_correct[LEMMAS] / double(heldout_total), 100 * heldout_correct[BOTH] / double(heldout_total));
    }
    eprintf("\n");
  }

  if (best_correct > 0) {
    eprintf("Chosen tagger model with heldout accuracy %.2f%%\n", best_correct * 100 / double(best_total));
    features = best_features;
  }
}

} // namespace morphodita
} // namespace ufal
