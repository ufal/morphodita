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

class version {
 public:
  unsigned major;
  unsigned minor;
  unsigned patch;

  // Returns current MorphoDiTa version.
  static version current();
};

class tokenizer {
 public:
  virtual ~tokenizer() {}

  virtual void set_text(string_piece text, bool make_copy = false) = 0;
  virtual bool next_sentence(std::vector<string_piece>* forms, std::vector<token_range>* tokens) = 0;

  // Static factory methods
  static tokenizer* new_vertical_tokenizer();

  static tokenizer* new_czech_tokenizer();
  static tokenizer* new_english_tokenizer();
  static tokenizer* new_generic_tokenizer();
};

class morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(const char* fname);
  static morpho* load(FILE* f);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1 };

  // Perform morphologic analysis of a form. The form is given by a pointer and
  // length and therefore does not need to be '\0' terminated.  The guesser
  // parameter specifies whether a guesser can be used if the form is not found
  // in the dictionary. Output is assigned to the lemmas vector.
  //
  // If the form is found in the dictionary, analyses are assigned to lemmas
  // and NO_GUESSER returned. If guesser == GUESSER and the form analyses are
  // found using a guesser, they are assigned to lemmas and GUESSER is
  // returned.  Otherwise <0 is returned and lemmas are filled with one
  // analysis containing given form as lemma and a tag for unknown word.
  virtual int analyze(string_piece form, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const = 0;

  // Perform morphologic generation of a lemma. The lemma is given by a pointer
  // and length and therefore does not need to be '\0' terminated. Optionally
  // a tag_wildcard can be specified (or be NULL) and if so, results are
  // filtered using this wildcard. The guesser parameter speficies whether
  // a guesser can be used if the lemma is not found in the dictionary. Output
  // is assigned to the forms vector.
  //
  // Tag_wildcard can be either NULL or a wildcard applied to the results.
  // A ? in the wildcard matches any character, [bytes] matches any of the
  // bytes and [^bytes] matches any byte different from the specified ones.
  // A - has no special meaning inside the bytes and if ] is first in bytes, it
  // does not end the bytes group.
  //
  // If the given lemma is only a raw lemma, all lemma ids with this raw lemma
  // are returned. Otherwise only matching lemma ids are returned, ignoring any
  // lemma comments. For every found lemma, matching forms are filtered using
  // the tag_wildcard. If at least one lemma is found in the dictionary,
  // NO_GUESSER is returned. If guesser == GUESSER and the lemma is found by
  // the guesser, GUESSER is returned. Otherwise, forms are cleared and <0 is
  // returned.
  virtual int generate(string_piece lemma, const char* tag_wildcard, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const = 0;

  // Rawlemma and lemma id identification
  virtual int raw_lemma_len(string_piece lemma) const = 0;
  virtual int lemma_id_len(string_piece lemma) const = 0;

  // Rawform identification
  virtual int raw_form_len(string_piece form) const = 0;

  // Construct a new tokenizer instance appropriate for this morphology.
  // Can return NULL if no such tokenizer exists.
  virtual tokenizer* new_tokenizer() const = 0;
};

class tagger {
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
  // Can return NULL if no such tokenizer exists.
  // Is equal to get_morpho()->new_tokenizer.
  tokenizer* new_tokenizer() const;
};

class tagset_converter {
 public:
  virtual ~tagset_converter() {}

  // Convert a tag-lemma pair to a different tag set.
  virtual void convert(tagged_lemma& tagged_lemma) const = 0;
  // Convert a result of analysis to a different tag set. Apart from calling
  // convert, any repeated entry is removed.
  virtual void convert_analyzed(std::vector<tagged_lemma>& tagged_lemmas) const = 0;
  // Convert a result of generation to a different tag set. Apart from calling
  // convert, any repeated entry is removed.
  virtual void convert_generated(std::vector<tagged_lemma_forms>& forms) const = 0;

  // Static factory methods
  static tagset_converter* new_identity_converter();

  static tagset_converter* new_pdt_to_conll2009_converter();
  static tagset_converter* new_strip_lemma_comment_converter(const morpho& dictionary);
  static tagset_converter* new_strip_lemma_id_converter(const morpho& dictionary);
};

} // namespace morphodita
} // namespace ufal

#endif
