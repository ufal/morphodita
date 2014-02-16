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

#include <algorithm>
#include <cstring>

#include "morpho/czech_lemma_addinfo.h"
#include "pdt_to_conll2009_tagset_converter.h"

namespace ufal {
namespace morphodita {

static const char* names[15] = {"POS", "SubPOS", "Gen", "Num", "Cas", "PGe", "PNu", "Per", "Ten", "Gra", "Neg", "Voi", "", "", "Var"};

inline void pdt_to_conll2009_tagset_converter::convert_tag(const string& lemma, string& tag) const {
  char pdt_tag[15];
  strncpy(pdt_tag, tag.c_str(), 15);

  // Clear the tag
  tag.clear();

  // Fill FEAT of filled tag characters
  for (int i = 0; i < 15 && pdt_tag[i]; i++)
    if (pdt_tag[i] != '-') {
      if (!tag.empty()) tag.push_back('|');
      tag.append(names[i]);
      tag.push_back('=');
      tag.push_back(pdt_tag[i]);
    }

  // Try adding Sem FEAT
  for (unsigned i = 0; i + 2 < lemma.size(); i++)
    if (lemma[i] == '_' && lemma[i + 1] == ';') {
      if (!tag.empty()) tag.push_back('|');
      tag.append("Sem=");
      tag.push_back(lemma[i + 2]);
      break;
    }
}

inline bool pdt_to_conll2009_tagset_converter::convert_lemma(string& lemma) const {
  unsigned raw_lemma = czech_lemma_addinfo::raw_lemma_len(lemma);
  return raw_lemma < lemma.size() ? (lemma.resize(raw_lemma), true) : false;
}

void pdt_to_conll2009_tagset_converter::convert(tagged_lemma& tagged_lemma) const {
  convert_tag(tagged_lemma.lemma, tagged_lemma.tag);
  convert_lemma(tagged_lemma.lemma);
}

void pdt_to_conll2009_tagset_converter::convert_analyzed(vector<tagged_lemma>& tagged_lemmas) const {
  bool lemma_changed = false;

  for (auto&& tagged_lemma : tagged_lemmas) {
    convert_tag(tagged_lemma.lemma, tagged_lemma.tag);
    lemma_changed |= convert_lemma(tagged_lemma.lemma);
  }

  // If no lemma was changed or there is 1 analysis, no duplicates could be created.
  if (!lemma_changed || tagged_lemmas.size() < 2) return;

  // Remove possible lemma-tag pair duplicates
  struct tagged_lemma_comparator {
    inline static bool eq(const tagged_lemma& a, const tagged_lemma& b) { return a.lemma == b.lemma && a.tag == b.tag; }
    inline static bool lt(const tagged_lemma& a, const tagged_lemma& b) { int lemma_compare = a.lemma.compare(b.lemma); return lemma_compare < 0 || (lemma_compare == 0 && a.tag < b.tag); }
  };
  sort(tagged_lemmas.begin(), tagged_lemmas.end(), tagged_lemma_comparator::lt);
  tagged_lemmas.resize(unique(tagged_lemmas.begin(), tagged_lemmas.end(), tagged_lemma_comparator::eq) - tagged_lemmas.begin());
}

void pdt_to_conll2009_tagset_converter::convert_generated(vector<tagged_lemma_forms>& forms) const {
  // 1) Convert tags and lemmas
  for (auto&& tagged_lemma_forms : forms) {
    for (auto&& tagged_form : tagged_lemma_forms.forms)
      convert_tag(tagged_lemma_forms.lemma, tagged_form.tag);
    convert_lemma(tagged_lemma_forms.lemma);
  }

  // 2) Regroup and if needed remove duplicate form-tag pairs for each lemma
  for (unsigned i = 0; i < forms.size(); i++) {
    bool any_merged = false;
    for (unsigned j = forms.size() - 1; j > i; j--)
      if (forms[j].lemma == forms[i].lemma) {
        // Same lemma was found. Merge form-tag pairs
        for (auto&& tagged_form : forms[j].forms)
          forms[i].forms.emplace_back(move(tagged_form));

        // Remove lemma j by moving it to end and deleting
        if (j < forms.size() - 1) {
          forms[j].lemma.swap(forms[forms.size() - 1].lemma);
          forms[j].forms.swap(forms[forms.size() - 1].forms);
        }
        forms.pop_back();
        any_merged = true;
      }

    if (any_merged && forms[i].forms.size() > 1) {
      // Remove duplicate form-tag pairs
      struct tagged_form_comparator {
        inline static bool eq(const tagged_form& a, const tagged_form& b) { return a.tag == b.tag && a.form == b.form; }
        inline static bool lt(const tagged_form& a, const tagged_form& b) { int tag_compare = a.tag.compare(b.tag); return tag_compare < 0 || (tag_compare == 0 && a.form < b.form); }
      };
      sort(forms[i].forms.begin(), forms[i].forms.end(), tagged_form_comparator::lt);
      forms[i].forms.resize(unique(forms[i].forms.begin(), forms[i].forms.end(), tagged_form_comparator::eq) - forms[i].forms.begin());
    }
  }
}

} // namespace morphodita
} // namespace ufal
