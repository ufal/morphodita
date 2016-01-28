// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "morpho_statistical_guesser_trainer.h"
#include "unilib/utf8.h"
#include "unilib/unicode.h"
#include "utils/split.h"

namespace ufal {
namespace morphodita {

void morpho_statistical_guesser_trainer::train(istream& is, int /*suffix_len*/, int /*rules_per_suffix*/, ostream& /*os*/) {
  vector<instance> data;

  // Load training data
  string form;
  vector<string> tokens;
  for (string line; getline(is, line);) {
    if (line.empty()) continue;

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_failure("The guesser training line '" << line << "' does not contain three columns!");

    // Normalize case
    casing form_case = get_casing(tokens[0]);
    casing lemma_case = get_casing(tokens[1]);
    if ((lemma_case == CASE_LC && (form_case == CASE_UCLC || form_case == CASE_UC)) ||
        (lemma_case == CASE_UCLC && form_case == CASE_UC)) {
      set_casing(tokens[0], lemma_case, form);
//      cerr << "Recasing " << tokens[0] << "->" << form << " for lemma " << tokens[1] << endl;
    } else {
      form.swap(tokens[0]);
    }

    data.emplace_back(form, tokens[1], tokens[2]);
  }
}

morpho_statistical_guesser_trainer::instance::instance(const string& form, const string& lemma, const string& tag)
  : form(form), lemma(lemma), tag(tag)
{
  // TODO: Generate lemma_rule and form_prefix
}

morpho_statistical_guesser_trainer::casing morpho_statistical_guesser_trainer::get_casing(const string& word) {
  using namespace unilib;

  casing c = CASE_OTHER;
  int index = 0;
  for (auto&& chr : utf8::decoder(word)) {
    auto cat = unicode::category(chr);

    // Return OTHER for non-letters
    if (cat & ~unicode::L) return CASE_OTHER;

    if (index == 0) {
      c = cat & unicode::Ll ? CASE_LC : CASE_UC;
    } else if (c == CASE_UC && index == 1) {
      c = cat & unicode::Ll ? CASE_UCLC : CASE_UC;
    } else if (c == CASE_UC) {
      if (cat & ~unicode::Lut) return CASE_OTHER;
    } else /*CASE_LC or CASE_UCLC*/ {
      if (cat & ~unicode::Ll) return CASE_OTHER;
    }
    index++;
  }
  return c;
}

void morpho_statistical_guesser_trainer::set_casing(const string& original, casing c, string& word) {
  using namespace unilib;

  word.clear();
  bool first = true;
  for (auto&& chr : utf8::decoder(original)) {
    utf8::append(word, (c == CASE_UC || (c == CASE_UCLC && first)) ? unicode::uppercase(chr) : unicode::lowercase(chr));
    first = false;
  }
}

} // namespace morphodita
} // namespace ufal
