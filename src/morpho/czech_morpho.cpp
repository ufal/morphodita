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

#include "czech_morpho.h"
#include "czech_lemma_addinfo.h"
#include "morpho_dictionary.h"
#include "morpho_prefix_guesser.h"
#include "morpho_statistical_guesser.h"
#include "utils/compressor.h"
#include "utils/utf8.h"

namespace ufal {
namespace morphodita {

bool czech_morpho::load(FILE* f) {
  binary_decoder data;
  if (!compressor::load(f, data)) return false;

  try {
    // Load tag length
    unsigned tag_length = data.next_1B();
    if (tag_length < unknown_tag.size()) unknown_tag.erase(tag_length);
    if (tag_length < number_tag.size()) number_tag.erase(tag_length);
    if (tag_length < punctuation_tag.size()) punctuation_tag.erase(tag_length);

    // Load dictionary
    dictionary.load(data);

    // Optionally prefix guesser if present
    prefix_guesser.reset();
    if (data.next_1B()) {
      prefix_guesser.reset(new morpho_prefix_guesser<decltype(dictionary)>(dictionary));
      prefix_guesser->load(data);
    }

    // Optionally statistical guesser if present
    statistical_guesser.reset();
    if (data.next_1B()) {
      statistical_guesser.reset(new morpho_statistical_guesser());
      statistical_guesser->load(data);
    }
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

int czech_morpho::analyze(const char* form, int form_len, guesser_mode guesser, vector<tagged_lemma>& lemmas) const {
  lemmas.clear();

  if (form_len > 0) {
    // Start by calling analyze_special without any casing changes.
    analyze_special(form, form_len, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // Generate all casing variants if needed (they are different than given form).
    string form_uclc; // first uppercase, rest lowercase
    string form_lc;   // all lowercase
    generate_casing_variants(form, form_len, form_uclc, form_lc);

    // Start by analysing using the dictionary and all casing variants.
    dictionary.analyze(form, form_len, lemmas);
    if (!form_uclc.empty()) dictionary.analyze(form_uclc.c_str(), form_uclc.size(), lemmas);
    if (!form_lc.empty()) dictionary.analyze(form_lc.c_str(), form_lc.size(), lemmas);
    if (lemmas.empty()) {
      // Heuristics. If form ends by a dot, remove all dots from the end and try again.
      int dots = 0;
      while (form_len - 1 - dots >= 0 && form[form_len - 1 - dots] == '.') dots++;
      if (dots && form_len - dots > 0) {
        dictionary.analyze(form, form_len - dots, lemmas);
        if (!form_uclc.empty()) dictionary.analyze(form_uclc.c_str(), form_uclc.size() - dots, lemmas);
        if (!form_lc.empty()) dictionary.analyze(form_lc.c_str(), form_lc.size() - dots, lemmas);
      }
    }
    if (!lemmas.empty()) return NO_GUESSER;

    // For the prefix guesser, use only form_lc.
    if (guesser == GUESSER && prefix_guesser)
      prefix_guesser->analyze(form_lc.empty() ? form : form_lc.c_str(), form_lc.empty() ? form_len : form_lc.size(), lemmas);
    bool prefix_guesser_guesses = !lemmas.empty();

    // For the statistical guesser, use all casing variants.
    if (guesser == GUESSER && statistical_guesser) {
      if (form_uclc.empty() && form_lc.empty())
        statistical_guesser->analyze(form, form_len, lemmas, nullptr);
      else {
        morpho_statistical_guesser::used_rules used_rules; used_rules.reserve(3);
        statistical_guesser->analyze(form, form_len, lemmas, &used_rules);
        if (!form_uclc.empty()) statistical_guesser->analyze(form_uclc.c_str(), form_uclc.size(), lemmas, &used_rules);
        if (!form_lc.empty()) statistical_guesser->analyze(form_lc.c_str(), form_lc.size(), lemmas, &used_rules);
      }
    }

    // Make sure results are unique lemma-tag pairs
    if (prefix_guesser_guesses) {
      sort(lemmas.begin(), lemmas.end(), [](const tagged_lemma& a, const tagged_lemma& b) {
        int lemma_compare = a.lemma.compare(b.lemma);
        return lemma_compare < 0 || (lemma_compare == 0 && a.tag < b.tag);
      });
      auto lemmas_end = unique(lemmas.begin(), lemmas.end(), [](const tagged_lemma& a, const tagged_lemma& b) {
        return a.lemma == b.lemma && a.tag == b.tag;
      });
      if (lemmas_end != lemmas.end()) lemmas.erase(lemmas_end, lemmas.end());
    }
  }

  if (lemmas.empty())
    lemmas.emplace_back(string(form, form_len), unknown_tag);

  return guesser;
}

int czech_morpho::generate(const char* lemma, int lemma_len, const char* tag, guesser_mode guesser, string& form) const {
  form.clear();

  if (lemma_len > 0) {
    dictionary.generate(lemma, lemma_len, tag, form);
    if (!form.empty()) return NO_GUESSER;

    if (guesser == GUESSER && prefix_guesser)
      prefix_guesser->generate(lemma, lemma_len, tag, form);
  }

  return guesser;
}

int czech_morpho::generate_all(const char* lemma, int lemma_len, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  if (lemma_len > 0) {
    dictionary.generate_all(lemma, lemma_len, forms);
    if (!forms.empty()) return NO_GUESSER;

    if (guesser == GUESSER && prefix_guesser)
      prefix_guesser->generate_all(lemma, lemma_len, forms);
  }

  return guesser;
}


int czech_morpho::raw_lemma_len(const char* lemma, int lemma_len) const {
  return czech_lemma_addinfo::raw_lemma_len(lemma, lemma_len);
}

int czech_morpho::lemma_id_len(const char* lemma, int lemma_len) const {
  return czech_lemma_addinfo::lemma_id_len(lemma, lemma_len);
}

// What characters are considered punctuation except for the ones in unicode Punctuation category.
static bool punctuation_additional[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1/*$*/,
  0,0,0,0,0,0,1/*+*/,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1/*<*/,1/*=*/,1/*>*/,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,1/*^*/,0,1/*`*/,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1/*|*/,0,1/*~*/,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1/*ˇ*/};

// What characters of unicode Punctuation category are not considered punctuation.
static bool punctuation_exceptions[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,1/*§*/};

void czech_morpho::analyze_special(const char* form, int form_len, vector<tagged_lemma>& lemmas) const {
  // Analyzer for numbers and punctuation.
  // Number is anything matching [+-]? is_Pn* ([.,] is_Pn*)? for at least one is_Pn* nonempty.
  // Punctuation is any form beginning with either unicode punctuation or punctuation_exceptions character.
  // Beware that numbers takes precedence, so - is punctuation, -3 is number, -. is punctuation, -.3 is number.
  if (form_len <= 0) return;

  const char* form_ori = form; int form_len_ori = form_len;
  char32_t first = utf8::decode(form, form_len);

  // Try matching a number.
  char32_t codepoint = first;
  bool any_digit = false;
  if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(form, form_len);
  while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(form, form_len);
  if (codepoint == '.' || codepoint == ',') codepoint = utf8::decode(form, form_len);
  while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(form, form_len);

  if (!form_len && any_digit) {
    // We found a number. If it ends by , or ., drop it.
    if (form_ori[form_len_ori-1] == '.' || form_ori[form_len_ori-1] == ',') form_len_ori--;
    lemmas.emplace_back(string(form_ori, form_len_ori), number_tag);
  } else if ((first < sizeof(punctuation_additional) && punctuation_additional[first]) ||
             (utf8::is_P(first) && (first >= sizeof(punctuation_exceptions) || !punctuation_exceptions[first])))
    lemmas.emplace_back(string(form_ori, form_len_ori), punctuation_tag);
}

void czech_morpho::generate_casing_variants(const char* form, int form_len, string& form_uclc, string& form_lc) const {
  // Detect uppercase+titlecase characters.
  bool first_Lut = false; // first character is uppercase or titlecase
  bool rest_has_Lut = false; // any character but first is uppercase or titlecase
  {
    const char* form_tmp = form; int form_len_tmp = form_len;
    first_Lut = utf8::is_Lut(utf8::decode(form_tmp, form_len_tmp));
    while (form_len_tmp > 0 && !rest_has_Lut)
      rest_has_Lut = utf8::is_Lut(utf8::decode(form_tmp, form_len_tmp));
  }

  // Generate all casing variants if needed (they are different than given form).
  // We only replace letters with their lowercase variants.
  // - form_uclc: first uppercase, rest lowercase
  // - form_lc: all lowercase

  if (first_Lut && !rest_has_Lut) { // common case allowing fast execution
    form_lc.reserve(form_len);
    const char* form_tmp = form; int form_len_tmp = form_len;
    utf8::append(form_lc, utf8::lowercase(utf8::decode(form_tmp, form_len_tmp)));
    form_lc.append(form_tmp, form_len_tmp);
  } else if (!first_Lut && rest_has_Lut) {
    form_lc.reserve(form_len);
    utf8::lowercase(form, form_len, form_lc);
  } else if (first_Lut && rest_has_Lut) {
    form_lc.reserve(form_len);
    form_uclc.reserve(form_len);
    const char* form_tmp = form; int form_len_tmp = form_len;
    char32_t first = utf8::decode(form_tmp, form_len_tmp);
    utf8::append(form_lc, utf8::lowercase(first));
    utf8::append(form_uclc, first);
    while (form_len_tmp > 0) {
      char32_t lowercase = utf8::lowercase(utf8::decode(form_tmp, form_len_tmp));
      utf8::append(form_lc, lowercase);
      utf8::append(form_uclc, lowercase);
    }
  }
}

} // namespace morphodita
} // namespace ufal
