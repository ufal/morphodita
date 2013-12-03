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

#include <memory>

#include "common.h"
#include "czech_lemma_addinfo.h"
#include "morpho_dictionary.h"
#include "morpho_prefix_guesser.h"
#include "morpho_statistical_guesser.h"

namespace ufal {
namespace morphodita {

class czech_morpho : public morpho {
 public:
  virtual int analyze(const char* form, int form_len, morpho::guesser_mode guesser, vector<tagged_lemma>& lemmas) const override;
  virtual int generate(const char* lemma, int lemma_len, const char* tag_wildcard, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const;
  virtual int raw_lemma_len(const char* lemma, int lemma_len) const override;
  virtual int lemma_id_len(const char* lemma, int lemma_len) const override;

  bool load(FILE* f);
 private:
  inline void analyze_special(const char* form, int form_len, vector<tagged_lemma>& lemmas) const;
  inline void generate_casing_variants(const char* form, int form_len, string& form_uclc, string& form_lc) const;

  morpho_dictionary<czech_lemma_addinfo> dictionary;
  unique_ptr<morpho_prefix_guesser<decltype(dictionary)>> prefix_guesser;
  unique_ptr<morpho_statistical_guesser> statistical_guesser;

  string unknown_tag = "X@-------------";
  string number_tag = "C=-------------";
  string punctuation_tag = "Z:-------------";
};

} // namespace morphodita
} // namespace ufal
