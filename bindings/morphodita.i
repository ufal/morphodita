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

%template(Forms) std::vector<std::string>;
typedef std::vector<std::string> Forms;

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

%rename(TokenRange) token_range;
struct token_range {
  size_t start;
  size_t length;
};
%template(TokenRanges) std::vector<token_range>;
typedef std::vector<token_range> TokenTanges;

%rename(Tokenizer) tokenizer;
class tokenizer {
 public:
  virtual ~tokenizer() {}

  virtual void set_text(const char* text) = 0;

  %extend {
    bool next_sentence(std::vector<token_range>& tokens) {
      return $self->next_sentence(NULL, &tokens);
    }
  }
};

%rename(Morpho) morpho;
class morpho {
 public:
  virtual ~morpho() {}

  %newobject load;
  static morpho* load(const char* fname);

  enum { NO_GUESSER = 0, GUESSER = 1 };
  typedef int guesser_mode;

  virtual int analyze(const char* form, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const = 0;

  virtual int generate(const char* lemma, const char* tag_wildcard, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const = 0;

  %extend {
    %rename(rawLemma) raw_lemma_len;
    std::string raw_lemma_len(const char* lemma) const {
      return std::string(lemma, $self->raw_lemma_len(lemma));
    }

    %rename(lemmaId) lemma_id_len;
    std::string lemma_id_len(const char* lemma) const {
      return std::string(lemma, $self->lemma_id_len(lemma));
    }
  }

  %rename(newTokenizer) new_tokenizer;
  %newobject new_tokenizer;
  virtual tokenizer* new_tokenizer() const = 0;
};

%rename(Tagger) tagger;
class tagger {
 public:
  virtual ~tagger() {}

  %newobject load;
  static tagger* load(const char* fname);

  %rename(getMorpho) get_morpho;
  virtual const morpho* get_morpho() const = 0;

  %extend {
    void tag(const std::vector<std::string>& forms, std::vector<tagged_lemma>& tags) const {
      std::vector<string_piece> string_pieces;
      string_pieces.reserve(forms.size());
      for (auto& form : forms)
        string_pieces.emplace_back(form);
      self->tag(string_pieces, tags);
    }
  }

  %rename(tokenizeAndTag) tokenize_and_tag;
  %extend {
    void tokenize_and_tag(const char* text, std::vector<tagged_lemma>& tags, std::vector<token_range>* tokens) const {
      $self->tokenize_and_tag(text, tags, NULL, tokens);
    }
  }
};
