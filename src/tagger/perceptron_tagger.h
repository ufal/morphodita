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

#include <memory>

#include "tagger.h"
#include "utils/threadsafe_stack.h"
#include "viterbi.h"

namespace ufal {
namespace morphodita {

// Declarations
template<class FeatureSequences, int order>
class perceptron_tagger : public tagger {
 public:
  perceptron_tagger();

  bool load(FILE* f);
  virtual const morpho* get_morpho() const override;
  virtual void tag(const vector<raw_form>& forms, vector<tagged_lemma>& tags) const override;

 private:
  unique_ptr<morpho> dict;
  bool use_guesser;
  FeatureSequences features;
  typedef viterbi<FeatureSequences, order> viterbi_decoder;
  viterbi_decoder decoder;

  struct cache {
    vector<form_with_tags> tagged_forms;
    vector<int> tags;
    typename viterbi_decoder::cache decoder_cache;

    cache(const perceptron_tagger<FeatureSequences, order>& self) : decoder_cache(self.decoder) {}
  };

  mutable threadsafe_stack<cache> caches;
};


// Definitions

template<class FeatureSequences, int order>
perceptron_tagger<FeatureSequences, order>::perceptron_tagger() : decoder(features) {}

template<class FeatureSequences, int order>
bool perceptron_tagger<FeatureSequences, order>::load(FILE* f) {
  if (dict.reset(morpho::load(f)), !dict) return false;
  use_guesser = fgetc(f);
  if (!features.load(f)) return false;
  return true;
}

template<class FeatureSequences, int order>
const morpho* perceptron_tagger<FeatureSequences, order>::get_morpho() const {
  return dict.get();
}

template<class FeatureSequences, int order>
void perceptron_tagger<FeatureSequences, order>::tag(const vector<raw_form>& forms, vector<tagged_lemma>& tags) const {
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

    dict->analyze(c->tagged_forms[i].form.form, c->tagged_forms[i].form.form_len, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, c->tagged_forms[i].tags);
  }

  if (c->tags.size() < forms.size()) c->tags.resize(forms.size() * 2);
  decoder.tag(c->tagged_forms, forms.size(), c->decoder_cache, c->tags);

  for (unsigned i = 0; i < forms.size(); i++)
    tags.emplace_back(c->tagged_forms[i].tags[c->tags[i]]);

  caches.push(c);
}

} // namespace morphodita
} // namespace ufal
