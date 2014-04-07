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

#include "common.h"
#include "morpho.h"

namespace ufal {
namespace morphodita {

class external_morpho : public morpho {
 public:
  virtual int analyze(string_piece form, morpho::guesser_mode guesser, vector<tagged_lemma>& lemmas) const override;
  virtual int generate(string_piece lemma, const char* tag_wildcard, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const;
  virtual int raw_lemma_len(string_piece lemma) const override;
  virtual int lemma_id_len(string_piece lemma) const override;
  virtual int raw_form_len(string_piece form) const override;
  virtual tokenizer* new_tokenizer() const override;

  bool load(FILE* f);

 private:
  string unknown_tag;
};

} // namespace morphodita
} // namespace ufal
