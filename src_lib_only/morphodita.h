// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef UFAL_MORPHODITA_H
#define UFAL_MORPHODITA_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace ufal {
namespace morphodita {

namespace utils {
struct string_piece {
  const char* str;
  size_t len;

  string_piece() : str(NULL), len(0) {}
  string_piece(const char* str) : str(str), len(strlen(str)) {}
  string_piece(const char* str, size_t len) : str(str), len(len) {}
  string_piece(const std::string& str) : str(str.c_str()), len(str.size()) {}
};
}
typedef utils::string_piece string_piece;

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

struct derivated_lemma {
  std::string lemma;
};

class version {
 public:
  unsigned major;
  unsigned minor;
  unsigned patch;
  std::string prerelease;

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

class derivator {
 public:
  virtual ~derivator() {}

  // For given lemma, return the parent in the derivation graph.
  // The lemma is assumed to be lemma id and any lemma comments are ignored.
  virtual bool parent(string_piece lemma, derivated_lemma& parent) const = 0;

  // For given lemma, return the children in the derivation graph.
  // The lemma is assumed to be lemma id and any lemma comments are ignored.
  virtual bool children(string_piece lemma, std::vector<derivated_lemma>& children) const = 0;
};

class morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(const char* fname);
  static morpho* load(std::istream& is);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1, GUESSER_UNSPECIFIED = -1 };

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

  // Return a derivator for this morphology, or NULL if it does not exist.
  // The returned instance is owned by the morphology and should not be deleted.
  virtual const derivator* get_derivator() const;
};

class tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(const char* fname);
  static tagger* load(std::istream& is);

  // Return morpho associated with the tagger. Do not delete the pointer, it is
  // owned by the tagger instance and deleted in the tagger destructor.
  virtual const morpho* get_morpho() const = 0;

  // Perform morphologic analysis and subsequent disambiguation.
  virtual void tag(const std::vector<string_piece>& forms, std::vector<tagged_lemma>& tags, morpho::guesser_mode guesser = morpho::GUESSER_UNSPECIFIED) const = 0;

  // Perform disambiguation only on given analyses.
  virtual void tag_analyzed(const std::vector<string_piece>& forms, const std::vector<std::vector<tagged_lemma> >& analyses, std::vector<int>& tags) const = 0;

  // Construct a new tokenizer instance appropriate for this tagger.
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

class derivation_formatter {
 public:
  virtual ~derivation_formatter() {}

  // Perform the required derivation and store it directly in the lemma.
  virtual void format_derivation(std::string& lemma) const;

  // Perform the required derivation and store it directly in the tagged_lemma.
  // If a tagset_converter is given, it is also applied.
  virtual void format_tagged_lemma(tagged_lemma& lemma, const tagset_converter* converter = nullptr) const = 0;

  // Perform the required derivation on a list of tagged_lemmas.
  // If a tagset_converter is given, it is also applied.
  // Either way, only unique entries are returned.
  virtual void format_tagged_lemmas(std::vector<tagged_lemma>& lemmas, const tagset_converter* converter = nullptr) const;

  // Static factory methods.
  static derivation_formatter* new_none_derivation_formatter();
  static derivation_formatter* new_root_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_path_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_tree_derivation_formatter(const derivator* derinet);
  // String version of static factory method.
  static derivation_formatter* new_derivation_formatter(string_piece name, const derivator* derinet);
};

} // namespace morphodita
} // namespace ufal

#endif
