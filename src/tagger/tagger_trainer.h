// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#include "common.h"
#include "feature_sequences_optimizer.h"
#include "morpho/morpho.h"
#include "training_maps.h"
#include "viterbi.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class TaggerTrainer>
class tagger_trainer {
 public:
  struct sentence {
    vector<string> forms;
    vector<form_with_tags> forms_with_tags;
    vector<tagged_lemma> gold;
    vector<int> gold_index;
  };

  static void train(int iterations, FILE* in_morpho_dict, bool use_guesser, FILE* in_feature_templates, bool prune_features, FILE* in_train, FILE* in_heldout, bool early_stopping, FILE* out_tagger);

 private:
  static double load_data(FILE* f, const morpho& d, bool use_guesser, vector<sentence>& sentences, bool add_gold);
};


// Definitions
template <class TaggerTrainer>
void tagger_trainer<TaggerTrainer>::train(int iterations, FILE* in_morpho_dict, bool use_guesser, FILE* in_feature_templates, bool prune_features, FILE* in_train, FILE* in_heldout, bool early_stopping, FILE* out_tagger) {
  eprintf("Loading dictionary: ");
  unique_ptr<morpho> d(morpho::load(in_morpho_dict));
  if (!d) runtime_errorf("Cannot load dictionary!");
  eprintf("done\n");
  if (fseek(in_morpho_dict, 0, SEEK_SET) != 0) runtime_errorf("Cannot seek in dictionary file to the beginning!");

  vector<sentence> train_data;
  eprintf("Loading train data: ");
  eprintf("done, matched %.2f%%\n", 100 * load_data(in_train, *d, use_guesser, train_data, true));

  vector<sentence> heldout_data;
  if (in_heldout) {
    eprintf("Loading heldout data: ");
    eprintf("done, matched %.2f%%\n", 100 * load_data(in_heldout, *d, use_guesser, heldout_data, false));
  }

  // Encode morphological dictionary
  eprintf("Encoding morphological dictionary.\n");
  for (int c; (c = fgetc(in_morpho_dict)) != EOF; fputc(c, out_tagger)) ;
  fputc(use_guesser, out_tagger);

  // Train and encode the tagger
  TaggerTrainer::train(iterations, train_data, heldout_data, early_stopping, prune_features, in_feature_templates, out_tagger);
}

template <class TaggerTrainer>
double tagger_trainer<TaggerTrainer>::load_data(FILE* f, const morpho& d, bool use_guesser, vector<sentence>& sentences, bool add_gold) {
  sentences.clear();

  int forms = 0, forms_matched = 0;

  string line;
  vector<string> tokens;
  vector<tagged_lemma> lemmas;
  sentences.emplace_back();
  while (getline(f, line)) {
    if (line.empty()) {
      if (!sentences.back().forms_with_tags.empty())
        sentences.emplace_back();
      continue;
    }

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("The tagger data line '%s' does not contain three columns!", line.c_str());

    // Analyse
    d.analyze(tokens[0], use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);

    // Add form to sentence
    forms++;
    sentence& s = sentences.back();
    s.forms.emplace_back(tokens[0]);
    s.forms_with_tags.emplace_back(string_piece(s.forms.back().c_str(), d.raw_form_len(s.forms.back())));
    s.gold.emplace_back(tokens[1], tokens[2]);
    s.gold_index.emplace_back(-1);
    for (auto&& lemma : lemmas) {
      if (s.gold_index.back() == -1 && lemma.lemma == s.gold.back().lemma && lemma.tag == s.gold.back().tag) {
        s.gold_index.back() = s.forms_with_tags.back().tags.size();
        forms_matched++;
      }
      s.forms_with_tags.back().tags.emplace_back(lemma.lemma, lemma.tag);
    }

    if (s.gold_index.back() == -1 && add_gold) {
      s.gold_index.back() = s.forms_with_tags.back().tags.size();
      s.forms_with_tags.back().tags.emplace_back(tokens[1], tokens[2]);
    }
  }
  if (!sentences.empty() && sentences.back().forms_with_tags.empty()) sentences.pop_back();

  return forms_matched / double(forms);
}

} // namespace morphodita
} // namespace ufal
