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
#include "tokenizer.h"
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
