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

#include <map>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "utils/binary_encoder.h"
#include "utils/input.h"
#include "utils/persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class LemmaAddinfo>
class morpho_dictionary_encoder {
 public:
  static void encode(FILE* f, binary_encoder& enc);
};


// Definitions
class histogram {
 public:
  void add(const string& str) {
    if (str.size() >= lengths.size()) lengths.resize(str.size() + 1);
    lengths[str.size()].insert(str);
  }

  void encode(binary_encoder& enc) {
    enc.add_1B(lengths.size());
    for (auto& set : lengths)
      enc.add_4B(set.size());
  }

  vector<unordered_set<string>> lengths;
};

template <class LemmaAddinfo>
struct lemma_info {
  lemma_info(string lemma) {
    this->lemma = lemma.substr(0, addinfo.parse(lemma, true));
  }

  string lemma;
  LemmaAddinfo addinfo;
  struct lemma_form_info {
    lemma_form_info(string form, int clas) : form(form), clas(clas) {
      if (this->clas != clas) runtime_errorf("Class number does not fit in 16 bits!");
    }

    string form;
    uint16_t clas;

    bool operator<(const lemma_form_info& other) const { return form < other.form || (form == other.form && clas < other.clas); }
  };
  vector<lemma_form_info> forms;

  bool operator<(const lemma_info& other) const { return lemma < other.lemma || (lemma == other.lemma && addinfo.data < other.addinfo.data); }
};

template <class LemmaAddinfo>
void morpho_dictionary_encoder<LemmaAddinfo>::encode(FILE* f, binary_encoder& enc) {
  vector<lemma_info<LemmaAddinfo>> lemmas;
  histogram lemmas_hist, forms_hist;
  unordered_map<string, map<int, vector<int>>> suffixes;

  vector<string> tags;
  unordered_map<string, int> tags_map;

  // Load lemmas and classes
  string line;
  vector<string> tokens;
  while(getline(f, line)) {
    split(line, '\t', tokens);
    if (tokens.empty()) break;
    if (tokens.size() < 3 || (tokens.size() % 2) == 0) runtime_errorf("Line morphological dictionary line '%s' is corrupt!", line.c_str());

    lemmas.emplace_back(tokens[0]);
    auto& lemma = lemmas.back();

    lemmas_hist.add(lemma.lemma);
    for (unsigned i = 1; i < tokens.size(); i += 2) {
      lemma.forms.emplace_back(tokens[i], stoi(tokens[i+1]));
      forms_hist.add(tokens[i]);
    }
    stable_sort(lemma.forms.begin(), lemma.forms.end());
  }
  stable_sort(lemmas.begin(), lemmas.end());

  for (int clas = 0; getline(f, line); clas++) {
    split(line, '\t', tokens);
    if (tokens.size() < 2 || (tokens.size() % 2) == 1) runtime_errorf("Line morphological dictionary line '%s' is corrupt!", line.c_str());
    for (unsigned i = 0; i < tokens.size(); i += 2) {
      int tag = tags_map.emplace(tokens[i+1], tags.size()).first->second;
      if (tag >= int(tags.size())) tags.emplace_back(tokens[i+1]);

      suffixes[tokens[i]][clas].emplace_back(tag);
    }
  }

  // Encode lemmas and forms
  lemmas_hist.encode(enc);
  forms_hist.encode(enc);

  string prev = "";
  enc.add_4B(lemmas.size());
  for (auto& lemma : lemmas) {
    int cpl = 0;
    while (prev[cpl] && prev[cpl] == lemma.lemma[cpl]) cpl++;

    enc.add_1B(prev.length() - cpl);
    enc.add_1B(lemma.lemma.size() - cpl);
    enc.add_str(lemma.lemma.substr(cpl));
    enc.add_1B(lemma.addinfo.data.size());
    enc.add_data(lemma.addinfo.data);
    enc.add_1B(lemma.forms.size());

    string prev_form = lemma.lemma;
    for (auto& lemma_form : lemma.forms) {
      unsigned best_prev_from = 0, best_form_from = 0, best_len = 0;
      for (unsigned prev_from = 0; prev_from < prev_form.size(); prev_from++)
        for (unsigned form_from = 0; form_from < lemma_form.form.size(); form_from++) {
          unsigned len = 0;
          while (prev_from + len < prev_form.size() && form_from + len < lemma_form.form.size() && prev_form[prev_from+len] == lemma_form.form[form_from+len]) len++;
          if (len > best_len) best_prev_from = prev_from, best_form_from = form_from, best_len = len;
        }

      enum { REMOVE_START = 1, REMOVE_END = 2, ADD_START = 4, ADD_END = 8 };
      enc.add_1B(REMOVE_START * (best_prev_from>0) + REMOVE_END * (best_prev_from+best_len<prev_form.size()) +
             ADD_START * (best_form_from>0) + ADD_END * (best_form_from+best_len<lemma_form.form.size()));
      if (best_prev_from > 0) enc.add_1B(best_prev_from);
      if (best_prev_from + best_len < prev_form.size()) enc.add_1B(prev_form.size() - best_prev_from - best_len);
      if (best_form_from > 0) {
        enc.add_1B(best_form_from);
        enc.add_str(lemma_form.form.substr(0, best_form_from));
      }
      if (best_form_from + best_len < lemma_form.form.size()) {
        enc.add_1B(lemma_form.form.size() - best_form_from - best_len);
        enc.add_str(lemma_form.form.substr(best_form_from + best_len));
      }
      enc.add_2B(lemma_form.clas);

      prev_form = lemma_form.form;
    }

    prev = lemma.lemma;
  }

  // Encode tags
  enc.add_2B(tags.size());
  for (auto& tag : tags) {
    enc.add_1B(tag.size());
    enc.add_str(tag);
  }

  // Encode classes
  persistent_unordered_map(suffixes, 5, false, true, [](binary_encoder& enc, const map<int, vector<int>>& suffix) {
    enc.add_2B(suffix.size());
    for (auto& clas : suffix)
      enc.add_2B(clas.first);
    int tags = 0;
    for (auto& clas : suffix) {
      enc.add_2B(tags);
      tags += clas.second.size();
    }
    enc.add_2B(tags);
    for (auto& clas : suffix)
      for (auto& tag : clas.second)
        enc.add_2B(tag);
  }).save(enc);
}

} // namespace morphodita
} // namespace ufal
