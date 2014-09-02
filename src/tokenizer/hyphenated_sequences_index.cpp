// This file is part of MorphoDiTa.
//
// Copyright 2014 by Institute of Formal and Applied Linguistics, Faculty of
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

#include "hyphenated_sequences_index.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace morphodita {

hyphenated_sequences_index::hyphenated_sequences_index(std::initializer_list<string> sequences) {
  string buffer;

  max_hyphens = 0;
  for (auto&& sequence : sequences) {
    // Count number of dashes and ignore words without.
    unsigned hyphens = count(sequence.begin(), sequence.end(), '-');
    if (!hyphens) continue;

    if (hyphens > max_hyphens) max_hyphens = hyphens;

    // Add lowercased "before_last_hyphen-any_last_word_prefix" to the map.
    int last_word = sequence.find_last_of('-') + 1;
    buffer.assign(sequence, 0, last_word);
    for (auto&& chr : utf8::decoder(sequence.c_str() + last_word, sequence.size() - last_word)) {
      utf8::append(buffer, unicode::lowercase(chr));
      this->sequences.emplace(buffer, string());
    }
    this->sequences[buffer] = sequence;
  }
}

bool hyphenated_sequences_index::join(vector<string_piece>& forms, string& buffer) const {
  unsigned matched_hyphens = 0;
  for (unsigned hyphens = 1; hyphens <= max_hyphens; hyphens++) {
    // Are the forms sequence of 'hyphens' hyphenated words?
    if (forms.size() < 2*hyphens + 1) break;
    unsigned first_hyphen = forms.size() - 2*hyphens;
    unsigned last_hyphen = forms.size() - 2;
    if (forms[first_hyphen].len != 1 || !forms[first_hyphen].str || forms[first_hyphen].str[0] != '-' ||
        forms[first_hyphen].str + forms[first_hyphen].len != forms[first_hyphen + 1].str ||
        forms[first_hyphen-1].str + forms[first_hyphen-1].len != forms[first_hyphen].str) break;

    // Try finding lowercased "before_last_hyphen-last_word_prefix" in the map.
    utf8::map(unicode::lowercase, forms[first_hyphen-1].str, forms[last_hyphen+1].str - forms[first_hyphen-1].str, buffer);
    for (const char* ptr = forms[last_hyphen+1].str; ptr <= forms[last_hyphen+1].str + forms[last_hyphen+1].len; ) {
      auto sequences_it = sequences.find(buffer);
      if (sequences_it == sequences.end()) break;
      if (!sequences_it->second.empty()) {
        // A match was found, check correct casing.
        const string& correct_casing = sequences_it->second;
        const char* sequence = forms[first_hyphen-1].str;
        bool matched = true;
        for (auto&& correct_casing_chr : correct_casing)
          if (sequence >= forms[last_hyphen+1].str + forms[last_hyphen+1].len ||
              (unicode::category(utf8::decode(sequence)) & !unicode::Lut && unicode::category(correct_casing_chr) & unicode::Lut)) {
            matched = false;
            break;
          }
        if (matched) {
          matched_hyphens = hyphens;
          break;
        }
      }
      utf8::append(buffer, unicode::lowercase(utf8::decode(ptr)));
    }
  }

  if (matched_hyphens) {
    unsigned first_word = forms.size() - 2 * matched_hyphens - 1;
    forms[first_word].len = forms.back().str + forms.back().len - forms[first_word].str;
    forms.resize(first_word + 1);

    return true;
  }
  return false;
}

} // namespace morphodita
} // namespace ufal

