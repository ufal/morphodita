// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_set>

#include "common.h"
#include "tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"
#include "utils/threadsafe_stack.h"

namespace ufal {
namespace morphodita {

class utf8_tokenizer : public tokenizer {
 public:
  virtual void set_text(string_piece text, bool make_copy = false) override;
  virtual bool next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) override;

  virtual bool next_sentence(vector<string_piece>& forms) = 0;

 protected:
  inline void utf8_advance(const char*& text, const char* end) {
    if (text < end) text++;
    while (text < end && *(const unsigned char*)text >= 0x80 && *(const unsigned char*)text < 0xC0) text++;
  }
  inline void utf8_back(const char*& text, const char* start) {
    if (text > start) text--;
    while (text > start && *(const unsigned char*)text >= 0x80 && *(const unsigned char*)text < 0xC0) text--;
  }

  bool emergency_sentence_split(const vector<string_piece>& forms);
  bool is_eos_exception(const vector<string_piece>& forms, const unordered_set<string>* eos_word_exceptions, string& buffer);

  const char* text = nullptr;
  const char* text_end = nullptr;
  size_t chars = 0;
  string text_copy;

  struct cache {
    vector<string_piece> forms;
  };
  threadsafe_stack<cache> caches;
};

} // namespace morphodita
} // namespace ufal
