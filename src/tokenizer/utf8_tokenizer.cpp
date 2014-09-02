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

#include <cstring>

#include "utf8_tokenizer.h"

namespace ufal {
namespace morphodita {

void utf8_tokenizer::set_text(string_piece text, bool make_copy /*= false*/) {
  if (make_copy && text.str) {
    text_copy.assign(text.str, text.len);
    text.str = text_copy.c_str();
  }
  this->text = text.str;
  this->text_end = text.str + text.len;
  chars = 0;
}

bool utf8_tokenizer::next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) {
  if (forms) forms->clear();
  if (tokens) tokens->clear();
  if (!text || text == text_end) return false;

  cache* c = nullptr;
  if (!forms) {
    c = caches.pop();
    if (!c) c = new cache();
    forms = &c->forms;
    forms->clear();
  }

  const char* text_start = text;
  bool result = next_sentence(*forms);
  for (auto&& form : *forms) {
    for (; text_start < form.str; chars++) utf8_advance(text_start, form.str);
    size_t chars_start = chars;
    for (; text_start < form.str + form.len; chars++) utf8_advance(text_start, form.str + form.len);
    if (tokens) tokens->emplace_back(chars_start, chars - chars_start);
  }
  for (; text_start < text; chars++) utf8_advance(text_start, text);

  if (c) caches.push(c);
  return result;
}

bool utf8_tokenizer::is_eos_exception(const vector<string_piece>& forms, const unordered_set<string>* eos_word_exceptions, string& buffer) {
  bool eos_word_exception = false;

  if (!forms.empty()) {
    // Is it single Lut?
    string_piece form = forms.back();
    eos_word_exception |= unicode::category(utf8::decode(form.str, form.len)) & unicode::Lut && !form.len;
  }

  if (!forms.empty() && eos_word_exceptions) {
    // Is the lower case variant in eos_word_exceptions?
    buffer.clear();
    utf8::map(unicode::lowercase, forms.back().str, forms.back().len, buffer);
    eos_word_exception |= eos_word_exceptions->count(buffer);
  }
  return eos_word_exception;
}

} // namespace morphodita
} // namespace ufal
