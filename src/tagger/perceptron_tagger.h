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
  virtual void tag(const vector<string_piece>& forms, vector<tagged_lemma>& tags) const override;

 private:
  unique_ptr<morpho> dict;
  bool use_guesser;
  FeatureSequences features;
  typedef viterbi<FeatureSequences, decoding_order, window_size> viterbi_decoder;
  viterbi_decoder decoder;

  struct cache {
    vector<form_with_tags> tagged_forms;
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
void perceptron_tagger<FeatureSequences, decoding_order, window_size>::tag(const vector<string_piece>& forms, vector<tagged_lemma>& tags) const {
  tags.clear();
  if (!dict) return;

  cache* c = caches.pop();
  if (!c) c = new cache(*this);

  if (c->tagged_forms.size() < forms.size()) c->tagged_forms.reserve(forms.size() * 2);
  for (unsigned i = 0; i < forms.size(); i++) {
    if (i >= c->tagged_forms.size())
      c->tagged_forms.emplace_back(forms[i]);
    else
      c->tagged_forms[i].form = forms[i];

    dict->analyze(c->tagged_forms[i].form, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, c->tagged_forms[i].tags);
    c->tagged_forms[i].form.len = dict->raw_form_len(c->tagged_forms[i].form);
  }

  if (c->tags.size() < forms.size()) c->tags.resize(forms.size() * 2);
  decoder.tag(c->tagged_forms, forms.size(), c->decoder_cache, c->tags);

  for (unsigned i = 0; i < forms.size(); i++)
    tags.emplace_back(c->tagged_forms[i].tags[c->tags[i]]);

  caches.push(c);
}

} // namespace morphodita
} // namespace ufal
