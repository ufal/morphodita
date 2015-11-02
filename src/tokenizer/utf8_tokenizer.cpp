// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "utf8_tokenizer.h"

namespace ufal {
namespace morphodita {

using namespace unilib;

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

bool utf8_tokenizer::emergency_sentence_split(const vector<string_piece>& forms) {
  // Implement emergency splitting for large sentences
  return forms.size() >= 500 ||
         (forms.size() >= 450 && unicode::category(utf8::first(forms.back().str)) & unicode::P) ||
         (forms.size() >= 400 && unicode::category(utf8::first(forms.back().str)) & unicode::Po);
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
