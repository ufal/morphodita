// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "tagger.h"
#include "utils/threadsafe_stack.h"
#include "viterbi.h"

namespace ufal {
namespace morphodita {

// Declarations
template<class FeatureSequences, int decoding_order, int window_size>
class perceptron_tagger : public tagger {
 public:
  perceptron_tagger();

  bool load(istream& is);
  virtual const morpho* get_morpho() const override;
  virtual void tag(const vector<string_piece>& forms, vector<tagged_lemma>& tags, morpho::guesser_mode guesser = morpho::guesser_mode(-1)) const override;
  virtual void tag_analyzed(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<int>& tags) const override;

 private:
  unique_ptr<morpho> dict;
  bool use_guesser;
  FeatureSequences features;
  typedef viterbi<FeatureSequences, decoding_order, window_size> viterbi_decoder;
  viterbi_decoder decoder;
  struct cache {
    vector<string_piece> forms;
    vector<vector<tagged_lemma>> analyses;
    vector<int> tags;
    typename viterbi_decoder::cache decoder_cache;

    cache(const perceptron_tagger<FeatureSequences, decoding_order, window_size>& self) : decoder_cache(self.decoder) {}
  };

  mutable threadsafe_stack<cache> caches;
};


// Definitions

template<class FeatureSequences, int decoding_order, int window_size>
perceptron_tagger<FeatureSequences, decoding_order, window_size>::perceptron_tagger() : decoder(features) {}

template<class FeatureSequences, int decoding_order, int window_size>
bool perceptron_tagger<FeatureSequences, decoding_order, window_size>::load(istream& is) {
  if (dict.reset(morpho::load(is)), !dict) return false;
  use_guesser = is.get();
  if (!features.load(is)) return false;
  return true;
}

template<class FeatureSequences, int decoding_order, int window_size>
const morpho* perceptron_tagger<FeatureSequences, decoding_order, window_size>::get_morpho() const {
  return dict.get();
}

template<class FeatureSequences, int decoding_order, int window_size>
void perceptron_tagger<FeatureSequences, decoding_order, window_size>::tag(const vector<string_piece>& forms, vector<tagged_lemma>& tags, morpho::guesser_mode guesser) const {
  tags.clear();
  if (!dict) return;

  cache* c = caches.pop();
  if (!c) c = new cache(*this);

  c->forms.resize(forms.size());
  if (c->analyses.size() < forms.size()) c->analyses.resize(forms.size());
  for (unsigned i = 0; i < forms.size(); i++) {
    c->forms[i] = forms[i];
    c->forms[i].len = dict->raw_form_len(forms[i]);
    dict->analyze(forms[i], guesser >= 0 ? guesser : use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, c->analyses[i]);
  }

  if (c->tags.size() < forms.size()) c->tags.resize(forms.size() * 2);
  decoder.tag(c->forms, c->analyses, c->decoder_cache, c->tags);

  for (unsigned i = 0; i < forms.size(); i++)
    tags.emplace_back(c->analyses[i][c->tags[i]]);

  caches.push(c);
}

template<class FeatureSequences, int decoding_order, int window_size>
void perceptron_tagger<FeatureSequences, decoding_order, window_size>::tag_analyzed(const vector<string_piece>& forms, const vector<vector<tagged_lemma>>& analyses, vector<int>& tags) const {
  tags.clear();

  cache* c = caches.pop();
  if (!c) c = new cache(*this);

  tags.resize(forms.size());
  decoder.tag(forms, analyses, c->decoder_cache, tags);

  caches.push(c);
}

} // namespace morphodita
} // namespace ufal
