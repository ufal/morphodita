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

#include <algorithm>
#include <memory>

#include "casing_variants.h"
#include "english_morpho.h"
#include "tag_filter.h"
#include "tokenizer/english_tokenizer.h"
#include "utils/compressor.h"
#include "utils/utf8.h"

namespace ufal {
namespace morphodita {

bool english_morpho::load(FILE* f) {
  binary_decoder data;
  if (!compressor::load(f, data)) return false;

  try {
    dictionary.load(data);
    morpho_guesser.load(data);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

int english_morpho::analyze(string_piece form, guesser_mode guesser, vector<tagged_lemma>& lemmas) const {
  lemmas.clear();

  if (form.len) {
    // Generate all casing variants if needed (they are different than given form).
    string form_uclc; // first uppercase, rest lowercase
    string form_lc;   // all lowercase
    generate_casing_variants(form, form_uclc, form_lc);

    // Start by analysing using the dictionary and all casing variants.
    dictionary.analyze(form, lemmas);
    if (!form_uclc.empty()) dictionary.analyze(form_uclc, lemmas);
    if (!form_lc.empty()) dictionary.analyze(form_lc, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // If not found, try analyze_special handling numbers and punctiation.
    analyze_special(form, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // Use English guesser on form_lc if allowed.
    if (guesser == GUESSER)
      morpho_guesser.analyze(form, form_lc.empty() ? form : form_lc, lemmas);
    if (!lemmas.empty()) return GUESSER;
  }

  lemmas.emplace_back(string(form.str, form.len), unknown_tag);
  return -1;
}

int english_morpho::generate(string_piece lemma, const char* tag_wildcard, morpho::guesser_mode /*guesser*/, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  tag_filter filter(tag_wildcard);

  if (lemma.len) {
    if (dictionary.generate(lemma, filter, forms))
      return NO_GUESSER;
  }

  return -1;
}

int english_morpho::raw_lemma_len(string_piece lemma) const {
  return english_lemma_addinfo::raw_lemma_len(lemma);
}

int english_morpho::lemma_id_len(string_piece lemma) const {
  return english_lemma_addinfo::lemma_id_len(lemma);
}

tokenizer* english_morpho::new_tokenizer() const {
  return new english_tokenizer();
}

void english_morpho::analyze_special(string_piece form, vector<tagged_lemma>& lemmas) const {
  // Analyzer for numbers and punctuation.
  if (!form.len) return;

  // Try matching a number: [+-]? is_Pn* (, is_Pn{3})? (. is_Pn*)? ([Ee] [+-]? is_Pn+)? with at least one digi
  string_piece number = form;
  char32_t codepoint = utf8::decode(number.str, number.len);
  bool any_digit = false;
  if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(number.str, number.len);
  while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  while (codepoint == ',') {
    string_piece group = number;
    if (!utf8::is_N(utf8::decode(group.str, group.len))) break;
    if (!utf8::is_N(utf8::decode(group.str, group.len))) break;
    if (!utf8::is_N(utf8::decode(group.str, group.len))) break;
    any_digit = true;
    number = group;
    codepoint = utf8::decode(number.str, number.len);
  }
  if (codepoint == '.') {
    codepoint = utf8::decode(number.str, number.len);
    while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  }
  if (any_digit && (codepoint == 'e' || codepoint == 'E')) {
    codepoint = utf8::decode(number.str, number.len);
    if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(number.str, number.len);
    any_digit = false;
    while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(number.str, number.len);
  }
  if (any_digit && !number.len && !codepoint) { lemmas.emplace_back(string(form.str, form.len), number_tag); return; }
  if (any_digit && !number.len && (codepoint == '.' || codepoint == ',')) { lemmas.emplace_back(string(form.str, form.len - 1), number_tag); return; }

  // Fullstop, question mark, exclamation mark -> dot_tag
  if (form.len == 1 && (*form.str == '.' || *form.str == '!' || *form.str == '?')) { lemmas.emplace_back(string(form.str, form.len), dot_tag); return; }
  // Comma -> comma_tag
  if (form.len == 1 && *form.str == ',') { lemmas.emplace_back(string(form.str, form.len), comma_tag); return; }

  // Open quotation, end quotation, open parentheses, end parentheses, symbol, or other
  string_piece punctuation = form;
  bool open_quotation = true, close_quotation = true, open_parenthesis = true, close_parenthesis = true, any_punctuation = true, symbol = true;
  while ((symbol || any_punctuation) && punctuation.len) {
    codepoint = utf8::decode(punctuation.str, punctuation.len);
    if (open_quotation) open_quotation = codepoint == '`' || utf8::is_Pi(codepoint);
    if (close_quotation) close_quotation = codepoint == '\'' || codepoint == '"' || utf8::is_Pf(codepoint);
    if (open_parenthesis) open_parenthesis = utf8::is_Ps(codepoint);
    if (close_parenthesis) close_parenthesis = utf8::is_Pe(codepoint);
    if (any_punctuation) any_punctuation = utf8::is_P(codepoint);
    if (symbol) symbol = utf8::is_S(codepoint);
  }
  if (!punctuation.len && open_quotation) { lemmas.emplace_back(string(form.str, form.len), open_quotation_tag); return; }
  if (!punctuation.len && close_quotation) { lemmas.emplace_back(string(form.str, form.len), close_quotation_tag); return; }
  if (!punctuation.len && open_parenthesis) { lemmas.emplace_back(string(form.str, form.len), open_parenthesis_tag); return; }
  if (!punctuation.len && close_parenthesis) { lemmas.emplace_back(string(form.str, form.len), close_parenthesis_tag); return; }
  if (!punctuation.len && any_punctuation) { lemmas.emplace_back(string(form.str, form.len), punctuation_tag); return; }
  if (!punctuation.len && symbol) { lemmas.emplace_back(string(form.str, form.len), symbol_tag); return; }
}

} // namespace morphodita
} // namespace ufal
