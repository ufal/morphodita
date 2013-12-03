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

%module morphodita

%{
#include "morphodita.h"

using namespace ufal::morphodita;
%}

%include "std_string.i"
%include "std_vector.i"

%rename(TaggedForm) tagged_form;
struct tagged_form {
  std::string form;
  std::string tag;
};
%template(TaggedForms) std::vector<tagged_form>;
typedef std::vector<tagged_form> TaggedForms;

%rename(TaggedLemma) tagged_lemma;
struct tagged_lemma {
  std::string lemma;
  std::string tag;
};
%template(TaggedLemmas) std::vector<tagged_lemma>;
typedef std::vector<tagged_lemma> TaggedLemmas;

%rename(TaggedLemmaForms) tagged_lemma_forms;
struct tagged_lemma_forms {
  std::string lemma;
  std::vector<tagged_form> forms;
};
%template(TaggedLemmasForms) std::vector<tagged_lemma_forms>;
typedef std::vector<tagged_lemma_forms> TaggedLemmasForms;

%rename(Morpho) morpho;
class morpho {
 public:
  virtual ~morpho() {}

  static morpho* load(const char* fname);

  enum guesser_mode { NO_GUESSER = 0, GUESSER = 1 };

  virtual int analyze(const char* form, int form_len, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const = 0;

  virtual int generate(const char* lemma, int lemma_len, const char* tag_wildcard, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const = 0;

  %rename(rawLemma) raw_lemma_len;
  virtual int raw_lemma_len(const char* lemma, int lemma_len) const = 0;

  %rename(lemmaId) lemma_id_len;
  virtual int lemma_id_len(const char* lemma, int lemma_len) const = 0;
};

%rename(Tagger) tagger;
class tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(const char* fname);

  %rename(getMorpho) get_morpho;
  virtual const morpho* get_morpho() const = 0;

  virtual void tag(const std::vector<raw_form>& forms, std::vector<tagged_lemma>& tags) const = 0;
};
