// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace ufal {
namespace morphodita {

class morpho_statistical_guesser_trainer {
 public:
  static void train(istream& is, int suffix_len, int rules_per_suffix, ostream& os);

 private:
  struct instance {
    string form, lemma, tag;
    string lemma_rule, form_prefix;

    instance(const string& form, const string& lemma, const string& tag);
  };

  enum casing { CASE_LC, CASE_UCLC, CASE_UC, CASE_OTHER };
  static casing get_casing(const string& word);
  static void set_casing(const string& original, casing c, string& word);
};

} // namespace morphodita
} // namespace ufal
