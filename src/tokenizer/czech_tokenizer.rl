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

%%{
  machine czech_tokenizer;
  write data;
}%%

void czech_tokenizer::set_text(const char* text) {
  this->text = text;
  chars = 0;
}

bool czech_tokenizer::next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) {
  if (forms) forms->clear();
  if (tokens) tokens->clear();
  if (!text || !*text) return false;

  int cs;
  bool empty = true;
  const char* next = text;
  char32_t chr = utf8::decode(next);
  %%{
    variable p text;
    variable pe (text + chr);
    variable eof (text + chr);

    action utf8_decode {
      text = next; chars++;
      if (!(chr = utf8::decode(next))) fbreak;
      fexec text;
    }
    action is_unicode_letter { ufal::utils::utf8::is_L(chr) } unicode_letter = any when is_unicode_letter;
    action is_unicode_upper { ufal::utils::utf8::is_Lut(chr) } unicode_upper = any when is_unicode_upper;
    action is_unicode_space { ufal::utils::utf8::is_Z(chr) } unicode_space = any when is_unicode_space;

    action start_token {
      empty = false;
      if (forms) forms->emplace_back(text, 0);
      if (tokens) tokens->emplace_back(chars, 0);
    }
    action end_token {
      if (forms) forms->back().len = text - forms->back().str;
      if (tokens) tokens->back().length = chars - tokens->back().start;
    }

    whitespace = [ \r\t\n] | unicode_space;
    token = (unicode_letter unicode_letter* | (any - whitespace)) >start_token %end_token;
    sentence = (whitespace* <: token)** whitespace* <: (token & [.!?]);
    main := sentence $utf8_decode;

    write init;
    write exec noend;
  }%%
  return !empty;
}

} // namespace morphodita
} // namespace ufal


