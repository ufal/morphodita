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

#ifndef MORPHODITA_H
#define MORPHODITA_H

#include <cstdio>
#include <string>
#include <vector>

// Import attributes
#if defined(_WIN32) && !defined(MORPHODITA_STATIC)
  #define MORPHODITA_IMPORT __declspec(dllimport)
#else
  #define MORPHODITA_IMPORT
#endif

namespace ufal {
namespace morphodita {

struct MORPHODITA_IMPORT raw_form {
  const char* form;
  int form_len;

  raw_form(const char* form, int form_len) : form(form), form_len(form_len) {}
};

struct MORPHODITA_IMPORT tagged_form {
  std::string form;
  std::string tag;

  tagged_form(const std::string& form, const std::string& tag) : form(form), tag(tag) {}
};

struct MORPHODITA_IMPORT tagged_lemma {
  std::string lemma;
  std::string tag;

  tagged_lemma(const std::string& lemma, const std::string& tag) : lemma(lemma), tag(tag) {}
};

struct MORPHODITA_IMPORT tagged_lemma_forms {
  std::string lemma;
  std::vector<tagged_form> forms;

  tagged_lemma_forms(const std::string& lemma) : lemma(lemma) {}
};

class MORPHODITA_IMPORT morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(const char* fname);
  static morpho* load(FILE* f);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1 };

  // Returns guesser_mode used or <0 on error.
  virtual int analyze(const char* form, int form_len, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const = 0;
  virtual int generate(const char* lemma, int lemma_len, const char* tag, guesser_mode guesser, std::string& form) const = 0;
  virtual int generate_all(const char* lemma, int lemma_len, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const = 0;

  // Rawlemma and lemma identification
  virtual int raw_lemma_len(const char* lemma, int lemma_len) const = 0;
  virtual int lemma_id_len(const char* lemma, int lemma_len) const = 0;
};

class MORPHODITA_IMPORT tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(FILE* f);
  static tagger* load(const char* fname);

  virtual const morpho* get_morpho() const = 0;
  virtual void tag(const std::vector<raw_form>& forms, std::vector<tagged_lemma>& tags) const = 0;
};

} // namespace morphodita
} // namespace ufal

extern "C" {

}

#endif
