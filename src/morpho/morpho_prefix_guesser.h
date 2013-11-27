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

#include "common.h"
#include "morpho.h"
#include "morpho_dictionary.h"
#include "utils/binary_decoder.h"
#include "utils/persistent_unordered_map.h"
#include "utils/small_stringops.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class MorphoDictionary>
class morpho_prefix_guesser {
 public:
  morpho_prefix_guesser(const MorphoDictionary& dictionary) : dictionary(dictionary) {}

  void load(binary_decoder& data);
  void analyze(const char* form, int form_len, vector<tagged_lemma>& lemmas);
  void generate(const char* lemma, int lemma_len, const char* tag, string& form);
  void generate_all(const char* lemma, int lemma_len, vector<tagged_lemma_forms>& lemmas_forms);

 private:
  const MorphoDictionary& dictionary;
  vector<tag_filter> tag_filters;
  persistent_unordered_map prefixes_initial, prefixes_middle;
};


// Definitions
template <class MorphoDictionary>
void morpho_prefix_guesser<MorphoDictionary>::load(binary_decoder& data) {
  // Load and construct tag filters
  for (unsigned tag_filters_len = data.next_1B(); tag_filters_len; tag_filters_len--) {
    unsigned tag_filter_len = data.next_1B();
    string tag_filter(data.next<char>(tag_filter_len), tag_filter_len);

    tag_filters.emplace_back(tag_filter.c_str());
  }

  // Load prefixes
  prefixes_initial.load(data);
  prefixes_middle.load(data);
}

template <class MorphoDictionary>
void morpho_prefix_guesser<MorphoDictionary>::analyze(const char* form, int form_len, vector<tagged_lemma>& lemmas) {
  if (form_len <= 0) return;

  vector<char> form_tmp;
  vector<unsigned> middle_masks;
  middle_masks.reserve(form_len);

  int total_matches = 0;
  unsigned lemmas_initial_size = lemmas.size();
  for (int initial = 0; initial < form_len; initial++) {
    // Match the initial prefix.
    unsigned initial_mask = (1<<tag_filters.size()) - 1; // full mask for empty initial prefix
    if (initial) {
      auto found = prefixes_initial.at_typed<uint32_t>(form, initial);
      if (!found) break;
      initial_mask = *found;
    }

    // If we have found an initial prefix (including the empty one), match middle prefixes.
    if (initial_mask) {
      middle_masks.resize(initial);
      middle_masks.emplace_back(initial_mask);
      for (int middle = initial; middle < int(middle_masks.size()); middle++) {
        if (!middle_masks[middle]) continue;
        // Try matching middle prefixes from current index.
        for (int i = middle + 1; i < form_len; i++) {
          auto found = prefixes_middle.at_typed<uint32_t>(form + middle, i - middle);
          if (!found) break;
          if (*found) {
            if (i + 1 > int(middle_masks.size())) middle_masks.resize(i + 1);
            middle_masks[i] |= middle_masks[middle] & *found;
          }
        }

        // Try matching word forms if at least one middle prefix was found.
        if (middle > initial && middle < form_len ) {
          if (initial) {
            if (form_tmp.empty()) form_tmp.assign(form, form + form_len);
            small_memcpy(form_tmp.data() + middle - initial, form, initial);
          }
          unsigned lemmas_ori_size = lemmas.size();
          dictionary.analyze((initial ? form_tmp.data() : form) + middle - initial, form_len - middle + initial, lemmas);
          unsigned lemmas_new_size = lemmas_ori_size;
          for (unsigned i = lemmas_ori_size; i < lemmas.size(); i++) {
            for (unsigned filter = 0; filter < tag_filters.size(); filter++)
              if ((middle_masks[middle] & (1<<filter)) && tag_filters[filter].matches(lemmas[i].tag.c_str())) {
                if (i == lemmas_new_size) {
                  lemmas[lemmas_new_size].lemma.insert(0, form + initial, middle - initial);
                } else {
                  lemmas[lemmas_new_size].lemma.reserve(lemmas[i].lemma.size() + middle - initial);
                  lemmas[lemmas_new_size].lemma.assign(form + initial, middle - initial);
                  lemmas[lemmas_new_size].lemma.append(lemmas[i].lemma);
                  lemmas[lemmas_new_size].tag = lemmas[i].tag;
                }
                lemmas_new_size++;
                break;
              }
          }
          if (lemmas_new_size < lemmas.size()) lemmas.erase(lemmas.begin() + lemmas_new_size, lemmas.end());
          total_matches += lemmas_new_size > lemmas_ori_size;
        }
      }
    }
  }

  // Make sure results are unique lemma-tag pairs
  if (total_matches > 1) {
    sort(lemmas.begin() + lemmas_initial_size, lemmas.end(), [](const tagged_lemma& a, const tagged_lemma& b) {
      int lemma_compare = a.lemma.compare(b.lemma);
      return lemma_compare < 0 || (lemma_compare == 0 && a.tag < b.tag);
    });
    auto lemmas_end = unique(lemmas.begin() + lemmas_initial_size, lemmas.end(), [](const tagged_lemma& a, const tagged_lemma& b) {
      return a.lemma == b.lemma && a.tag == b.tag;
    });
    if (lemmas_end != lemmas.end()) lemmas.erase(lemmas_end, lemmas.end());
  }
}

template <class MorphoDictionary>
void morpho_prefix_guesser<MorphoDictionary>::generate(const char* /*lemma*/, int /*lemma_len*/, const char* /*tag*/, string& /*form*/) {
  // Not implemented yet. Is it actually needed?
}

template <class MorphoDictionary>
void morpho_prefix_guesser<MorphoDictionary>::generate_all(const char* /*lemma*/, int /*lemma_len*/, vector<tagged_lemma_forms>& /*lemmas_forms*/) {
  // Not implemented yet. Is it actually needed?
}
} // namespace morphodita
} // namespace ufal
