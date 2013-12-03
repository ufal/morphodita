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
  virtual int analyze(const char* form, int form_len, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const = 0;

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
  virtual int generate(const char* lemma, int lemma_len, const char* tag_wildcard, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const = 0;

  // Rawlemma and lemma id identification
  virtual int raw_lemma_len(const char* lemma, int lemma_len) const = 0;
  virtual int lemma_id_len(const char* lemma, int lemma_len) const = 0;
};

class MORPHODITA_IMPORT tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(FILE* f);
  static tagger* load(const char* fname);

  // Return morpho associated with the tagger. Do not delete the pointer, it is
  // owned by the tagger instance and deleted in the tagger destructor.
  virtual const morpho* get_morpho() const = 0;

  // Perform morphologic analysis and subsequent disambiguation.
  virtual void tag(const std::vector<raw_form>& forms, std::vector<tagged_lemma>& tags) const = 0;
};

} // namespace morphodita
} // namespace ufal

extern "C" {

}

#endif
