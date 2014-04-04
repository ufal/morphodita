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

#include "external_morpho.h"
#include "tag_filter.h"
#include "tokenizer/generic_tokenizer.h"
#include "utils/compressor.h"

namespace ufal {
namespace morphodita {

bool external_morpho::load(FILE* f) {
  binary_decoder data;
  if (!compressor::load(f, data)) return false;

  try {
    // Load unknown_tag
    unsigned length = data.next_1B();
    unknown_tag.assign(data.next<char>(length), length);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

int external_morpho::analyze(string_piece form, guesser_mode /*guesser*/, vector<tagged_lemma>& lemmas) const {
  lemmas.clear();

  if (form.len) {
    // Split form using ' ' into lemma-tag pairs.
    for (string_piece lemmatags = form; lemmatags.len; ) {
      auto lemma_start = lemmatags.str;
      while (lemmatags.len && *lemmatags.str != ' ') lemmatags.len--, lemmatags.str++;
      if (!lemmatags.len) break;
      auto lemma_len = lemmatags.str - lemma_start;
      lemmatags.len--, lemmatags.str++;

      auto tag_start = lemmatags.str;
      while (lemmatags.len && *lemmatags.str != ' ') lemmatags.len--, lemmatags.str++;
      auto tag_len = lemmatags.str - tag_start;
      if (lemmatags.len) lemmatags.len--, lemmatags.str++;

      lemmas.emplace_back(string(lemma_start, lemma_len), string(tag_start, tag_len));
    }

    if (!lemmas.empty()) return NO_GUESSER;
  }

  lemmas.emplace_back(string(form.str, form.len), unknown_tag);
  return -1;
}

int external_morpho::generate(string_piece lemma, const char* tag_wildcard, morpho::guesser_mode /*guesser*/, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  tag_filter filter(tag_wildcard);

  if (lemma.len) {
    bool any_result = false;
    // Split lemma using ' ' into form-lemma-tag pairs.
    for (string_piece formlemmatags = lemma; formlemmatags.len; ) {
      auto form_start = formlemmatags.str;
      while (formlemmatags.len && *formlemmatags.str != ' ') formlemmatags.len--, formlemmatags.str++;
      if (!formlemmatags.len) break;
      auto form_len = formlemmatags.str - form_start;
      formlemmatags.len--, formlemmatags.str++;

      auto lemma_start = formlemmatags.str;
      while (formlemmatags.len && *formlemmatags.str != ' ') formlemmatags.len--, formlemmatags.str++;
      if (!formlemmatags.len) break;
      auto lemma_len = formlemmatags.str - lemma_start;
      formlemmatags.len--, formlemmatags.str++;

      auto tag_start = formlemmatags.str;
      while (formlemmatags.len && *formlemmatags.str != ' ') formlemmatags.len--, formlemmatags.str++;
      auto tag_len = formlemmatags.str - tag_start;
      if (formlemmatags.len) formlemmatags.len--, formlemmatags.str++;

      any_result = true;
      string tag(tag_start, tag_len);
      if (filter.matches(tag.c_str())) {
        string lemma(lemma_start, lemma_len);
        // Find the lemma in forms, searching from back (so that when lemmas
        // are grouped, we can locale repeated lemma in constant time).
        int lemma_index = -1;
        for (int i = int(forms.size()) - 1; lemma_index < 0 && i >= 0; i--)
          if (forms[i].lemma == lemma)
            lemma_index = i;
        if (lemma_index < 0) lemma_index = forms.size(), forms.emplace_back(lemma);
        forms[lemma_index].forms.emplace_back(string(form_start, form_len), tag);
      }
    }

    if (any_result) return NO_GUESSER;
  }

  return -1;
}

int external_morpho::raw_lemma_len(string_piece lemma) const {
  return lemma.len;
}

int external_morpho::lemma_id_len(string_piece lemma) const {
  return lemma.len;
}

tokenizer* external_morpho::new_tokenizer() const {
  return new generic_tokenizer();
}

} // namespace morphodita
} // namespace ufal
