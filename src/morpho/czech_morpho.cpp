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
#include "czech_morpho.h"
#include "czech_lemma_addinfo.h"
#include "morpho_dictionary.h"
#include "morpho_prefix_guesser.h"
#include "morpho_statistical_guesser.h"
#include "tag_filter.h"
#include "tokenizer/czech_tokenizer.h"
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

int czech_morpho::analyze(string_piece form, guesser_mode guesser, vector<tagged_lemma>& lemmas) const {
  lemmas.clear();

  if (form.len) {
    // Start by calling analyze_special without any casing changes.
    analyze_special(form, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // Generate all casing variants if needed (they are different than given form).
    string form_uclc; // first uppercase, rest lowercase
    string form_lc;   // all lowercase
    generate_casing_variants(form, form_uclc, form_lc);

    // Start by analysing using the dictionary and all casing variants.
    dictionary.analyze(form, lemmas);
    if (!form_uclc.empty()) dictionary.analyze(form_uclc, lemmas);
    if (!form_lc.empty()) dictionary.analyze(form_lc, lemmas);
    if (!lemmas.empty()) return NO_GUESSER;

    // For the prefix guesser, use only form_lc.
    if (guesser == GUESSER && prefix_guesser)
      prefix_guesser->analyze(form_lc.empty() ? form : form_lc, lemmas);
    bool prefix_guesser_guesses = !lemmas.empty();

    // For the statistical guesser, use all casing variants.
    if (guesser == GUESSER && statistical_guesser) {
      if (form_uclc.empty() && form_lc.empty())
        statistical_guesser->analyze(form, lemmas, nullptr);
      else {
        morpho_statistical_guesser::used_rules used_rules; used_rules.reserve(3);
        statistical_guesser->analyze(form, lemmas, &used_rules);
        if (!form_uclc.empty()) statistical_guesser->analyze(form_uclc, lemmas, &used_rules);
        if (!form_lc.empty()) statistical_guesser->analyze(form_lc, lemmas, &used_rules);
      }
    }

    // Make sure results are unique lemma-tag pairs. Statistical guesser produces
    // unique lemma-tag pairs, but prefix guesser does not.
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

    if (!lemmas.empty()) return GUESSER;
  }

  lemmas.emplace_back(string(form.str, form.len), unknown_tag);
  return -1;
}

int czech_morpho::generate(string_piece lemma, const char* tag_wildcard, morpho::guesser_mode guesser, vector<tagged_lemma_forms>& forms) const {
  forms.clear();

  tag_filter filter(tag_wildcard);

  if (lemma.len) {
    if (dictionary.generate(lemma, filter, forms))
      return NO_GUESSER;

    if (guesser == GUESSER && prefix_guesser)
      if (prefix_guesser->generate(lemma, filter, forms))
        return GUESSER;
  }

  return -1;
}

int czech_morpho::raw_lemma_len(string_piece lemma) const {
  return czech_lemma_addinfo::raw_lemma_len(lemma);
}

int czech_morpho::lemma_id_len(string_piece lemma) const {
  return czech_lemma_addinfo::lemma_id_len(lemma);
}

tokenizer* czech_morpho::new_tokenizer() const {
  return new czech_tokenizer();
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

void czech_morpho::analyze_special(string_piece form, vector<tagged_lemma>& lemmas) const {
  // Analyzer for numbers and punctuation.
  // Number is anything matching [+-]? is_Pn* ([.,] is_Pn*)? ([Ee] [+-]? is_Pn+)? for at least one is_Pn* nonempty.
  // Punctuation is any form beginning with either unicode punctuation or punctuation_exceptions character.
  // Beware that numbers takes precedence, so - is punctuation, -3 is number, -. is punctuation, -.3 is number.
  if (!form.len) return;

  string_piece form_ori = form;
  char32_t first = utf8::decode(form.str, form.len);

  // Try matching a number.
  char32_t codepoint = first;
  bool any_digit = false;
  if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(form.str, form.len);
  while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(form.str, form.len);
  if (codepoint == '.' || codepoint == ',') codepoint = utf8::decode(form.str, form.len);
  while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(form.str, form.len);
  if (any_digit && (codepoint == 'e' || codepoint == 'E')) {
    codepoint = utf8::decode(form.str, form.len);
    if (codepoint == '+' || codepoint == '-') codepoint = utf8::decode(form.str, form.len);
    any_digit = false;
    while (utf8::is_N(codepoint)) any_digit = true, codepoint = utf8::decode(form.str, form.len);
  }

  if (any_digit && !form.len && !codepoint) {
    lemmas.emplace_back(string(form_ori.str, form_ori.len), number_tag);
  } else if (any_digit && !form.len && (codepoint == '.' || codepoint == ',')) { // allow numbers to end by . or ,
    lemmas.emplace_back(string(form_ori.str, form_ori.len - 1), number_tag);
  } else if ((first < sizeof(punctuation_additional) && punctuation_additional[first]) ||
             (utf8::is_P(first) && (first >= sizeof(punctuation_exceptions) || !punctuation_exceptions[first])))
    lemmas.emplace_back(string(form_ori.str, form_ori.len), punctuation_tag);
}

} // namespace morphodita
} // namespace ufal
