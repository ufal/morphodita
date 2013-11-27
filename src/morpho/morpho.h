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

namespace ufal {
namespace morphodita {

struct EXPORT_ATTRIBUTES tagged_form {
  string form;
  string tag;

  tagged_form(const string& form, const string& tag) : form(form), tag(tag) {}
};

struct EXPORT_ATTRIBUTES tagged_lemma {
  string lemma;
  string tag;

  tagged_lemma(const string& lemma, const string& tag) : lemma(lemma), tag(tag) {}
};

struct EXPORT_ATTRIBUTES tagged_lemma_forms {
  string lemma;
  vector<tagged_form> forms;

  tagged_lemma_forms(const string& lemma) : lemma(lemma) {}
};

class EXPORT_ATTRIBUTES morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(FILE* f);
  static morpho* load(const char* fname);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1 };

  // Returns guesser_mode used or <0 on error.
  virtual int analyze(const char* form, int form_len, guesser_mode guesser, vector<tagged_lemma>& lemmas) const = 0;
  virtual int generate(const char* lemma, int lemma_len, const char* tag, guesser_mode guesser, string& form) const = 0;
  virtual int generate_all(const char* lemma, int lemma_len, guesser_mode guesser, vector<tagged_lemma_forms>& forms) const = 0;

  // Rawlemma and lemma identification
  virtual int raw_lemma_len(const char* lemma, int lemma_len) const = 0;
  virtual int lemma_id_len(const char* lemma, int lemma_len) const = 0;
};

} // namespace morphodita
} // namespace ufal
