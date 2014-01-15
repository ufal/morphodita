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
#include <cstring>
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

struct string_piece {
  const char* str;
  size_t len;

  string_piece() : str(NULL), len(0) {}
  string_piece(const char* str) : str(str), len(strlen(str)) {}
  string_piece(const char* str, size_t len) : str(str), len(len) {}
  string_piece(const std::string& str) : str(str.c_str()), len(str.size()) {}
};

struct tagged_form {
  std::string form;
  std::string tag;

  tagged_form() {}
  tagged_form(const std::string& form, const std::string& tag) : form(form), tag(tag) {}
};

struct tagged_lemma {
  std::string lemma;
  std::string tag;

  tagged_lemma() {}
  tagged_lemma(const std::string& lemma, const std::string& tag) : lemma(lemma), tag(tag) {}
};

struct tagged_lemma_forms {
  std::string lemma;
  std::vector<tagged_form> forms;

  tagged_lemma_forms() {}
  tagged_lemma_forms(const std::string& lemma) : lemma(lemma) {}
};

struct token_range {
  size_t start;
  size_t length;

  token_range() {}
  token_range(size_t start, size_t length) : start(start), length(length) {}
};

class MORPHODITA_IMPORT tokenizer {
 public:
  virtual ~tokenizer() {}

  virtual void set_text(const char* text, bool make_copy = false) = 0;
  virtual bool next_sentence(std::vector<string_piece>* forms, std::vector<token_range>* tokens) = 0;

  // Static factory methods
  static tokenizer* new_czech_tokenizer();
};

class MORPHODITA_IMPORT morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(const char* fname);
  static morpho* load(FILE* f);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1 };

  // Perform morphologic analysis of a form. The guesser parameter specifies
  // whether a guesser can be used if the form is not found in the dictionary.
  // Output is assigned to the lemmas vector.
  virtual int analyze(string_piece form, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const = 0;

  // Perform morphologic generation of a lemma. Optionally a tag_wildcard can
  // be specified (or be NULL) and if so, results are filtered using this
  // wildcard. The guesser parameter speficies whether a guesser can be used if
  // the lemma is not found in the dictionary. Output is assigned to the forms
  // vector.
  virtual int generate(string_piece lemma, const char* tag_wildcard, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const = 0;

  // Rawlemma and lemma id identification
  virtual int raw_lemma_len(string_piece lemma) const = 0;
  virtual int lemma_id_len(string_piece lemma) const = 0;

  // Construct a new tokenizer instance appropriate for this morphology.
  virtual tokenizer* new_tokenizer() const = 0;
};

class MORPHODITA_IMPORT tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(const char* fname);
  static tagger* load(FILE* f);

  // Return morpho associated with the tagger. Do not delete the pointer, it is
  // owned by the tagger instance and deleted in the tagger destructor.
  virtual const morpho* get_morpho() const = 0;

  // Perform morphologic analysis and subsequent disambiguation.
  virtual void tag(const std::vector<string_piece>& forms, std::vector<tagged_lemma>& tags) const = 0;

  // Construct a new tokenizer instance appropriate for this tokenizer.
  // Is equal to get_morpho()->new_tokenizer.
  tokenizer* new_tokenizer() const;
};

} // namespace morphodita
} // namespace ufal

#endif
