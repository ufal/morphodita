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

#include "czech_tokenizer.h"
#include "utils/utf8.h"

namespace ufal {
namespace morphodita {

void czech_tokenizer::set_text(const char* text) {
  this->text = text;
  chars = 0;
}

bool czech_tokenizer::next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) {
  if (forms) forms->clear();
  if (tokens) tokens->clear();

  char32_t chr;
  const char* next;
  while (chr = utf8::decode(next=text), utf8::is_Z(chr) || chr == '\t' || chr == '\r' || chr == '\n')
    text = next, chars++;

  if (!chr) return false;
  while (chr) {
    const char* start_text = text;
    size_t start_chars = chars;

    while (chr = utf8::decode(next=text), !utf8::is_Z(chr) && chr != '\t' && chr != '\r' && chr != '\n')
      text = next, chars++;

    if (forms) forms->emplace_back(start_text, text - start_text);
    if (tokens) tokens->emplace_back(start_chars, chars - start_chars);

    while (chr = utf8::decode(next=text), utf8::is_Z(chr) || chr == '\t' || chr == '\r' || chr == '\n') {
      text = next, chars++;
      if (text[-1] == '\n' && text[-2] == '\n')
        return true;
    }
  }

  return true;
}

} // namespace morphodita
} // namespace ufal


