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

#include "strip_lemma_id_tagset_converter.h"

namespace ufal {
namespace morphodita {

inline bool strip_lemma_id_tagset_converter::convert_lemma(string& lemma) const {
  unsigned raw_lemma_len = dictionary.raw_lemma_len(lemma);
  return raw_lemma_len < lemma.size() ? (lemma.resize(raw_lemma_len), true) : false;
}

void strip_lemma_id_tagset_converter::convert(tagged_lemma& tagged_lemma) const {
  convert_lemma(tagged_lemma.lemma);
}

void strip_lemma_id_tagset_converter::convert_analyzed(vector<tagged_lemma>& tagged_lemmas) const {
  bool lemma_changed = false;

  for (auto&& tagged_lemma : tagged_lemmas)
    lemma_changed |= convert_lemma(tagged_lemma.lemma);

  // If no lemma was changed or there is 1 analysis, no duplicates could be created.
  if (!lemma_changed || tagged_lemmas.size() < 2) return;

  tagset_converter_unique_analyzed(tagged_lemmas);
}

void strip_lemma_id_tagset_converter::convert_generated(vector<tagged_lemma_forms>& forms) const {
  bool lemma_changed = false;

  for (auto&& tagged_lemma_forms : forms)
    lemma_changed |= convert_lemma(tagged_lemma_forms.lemma);

  // If no lemma was changed or there is 1 analysis, no duplicates could be created.
  if (!lemma_changed || forms.size() < 2) return;

  tagset_converter_unique_generated(forms);
}

} // namespace morphodita
} // namespace ufal
