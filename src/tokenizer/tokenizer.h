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
#include "string_piece.h"

namespace ufal {
namespace morphodita {

// Range of a token, measured in Unicode characters, not UTF8 bytes.
struct token_range {
  size_t start;
  size_t length;

  token_range() {}
  token_range(size_t start, size_t length) : start(start), length(length) {}
};

class tokenizer {
 public:
  virtual ~tokenizer() {}

  virtual void set_text(const char* text, bool make_copy = false) = 0;

  virtual bool next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) = 0;
};

} // namespace morphodita
} // namespace ufal
