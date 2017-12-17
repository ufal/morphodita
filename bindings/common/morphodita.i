// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%include "morphodita_stl.i"

%{
#include "morphodita.h"
using namespace ufal::morphodita;
%}

%template(Indices) std::vector<int>;
typedef std::vector<int> Indices;

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
%template(Analyses) std::vector<std::vector<tagged_lemma> >;
typedef std::vector<std::vector<tagged_lemma> > Analyses;

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

%rename(DerivatedLemma) derivated_lemma;
struct derivated_lemma {
  std::string lemma;
};
%template(DerivatedLemmas) std::vector<derivated_lemma>;
typedef std::vector<derivated_lemma> DerivatedLemmas;

%rename(Version) version;
class version {
 public:
  unsigned major;
  unsigned minor;
  unsigned patch;
  std::string prerelease;

  static version current();
};

%rename(Tokenizer) tokenizer;
%nodefaultctor tokenizer;
class tokenizer {
 public:
  virtual ~tokenizer() {}

  %extend {
    %rename(setText) set_text;
    void set_text(const char* text) {
      $self->set_text(text, true);
    }

    %rename(nextSentence) next_sentence;
    bool next_sentence(std::vector<std::string>* forms, std::vector<token_range>* tokens) {
      if (!forms) return $self->next_sentence(NULL, tokens);

      std::vector<string_piece> string_pieces;
      bool result = $self->next_sentence(&string_pieces, tokens);
      forms->resize(string_pieces.size());
      for (unsigned i = 0; i < string_pieces.size(); i++)
        forms->operator[](i).assign(string_pieces[i].str, string_pieces[i].len);
      return result;
    }
  }

  %rename(newVerticalTokenizer) new_vertical_tokenizer;
  %newobject new_vertical_tokenizer;
  static tokenizer* new_vertical_tokenizer();

  %rename(newCzechTokenizer) new_czech_tokenizer;
  %newobject new_czech_tokenizer;
  static tokenizer* new_czech_tokenizer();

  %rename(newEnglishTokenizer) new_english_tokenizer;
  %newobject new_english_tokenizer;
  static tokenizer* new_english_tokenizer();

  %rename(newGenericTokenizer) new_generic_tokenizer;
  %newobject new_generic_tokenizer;
  static tokenizer* new_generic_tokenizer();
};

%rename(Derivator) derivator;
%nodefaultctor derivator;
class derivator {
 public:
  virtual ~derivator() {}

  virtual bool parent(const char* lemma, derivated_lemma& parent) const = 0;

  virtual bool children(const char* lemma, std::vector<derivated_lemma>& children) const = 0;
};

%rename(DerivationFormatter) derivation_formatter;
%nodefaultctor derivation_formatter;
class derivation_formatter {
 public:
  virtual ~derivation_formatter() {}

  %extend {
    %rename(formatDerivation) format_derivation;
    std::string format_derivation(const char* lemma) const {
      std::string derivation(lemma);
      $self->format_derivation(derivation);
      return derivation;
    }
  }

  %rename(newNoneDerivationFormatter) new_none_derivation_formatter;
  %newobject new_none_derivation_formatter;
  static derivation_formatter* new_none_derivation_formatter();

  %rename(newRootDerivationFormatter) new_root_derivation_formatter;
  %newobject new_root_derivation_formatter;
  static derivation_formatter* new_root_derivation_formatter(const derivator* derinet);

  %rename(newPathDerivationFormatter) new_path_derivation_formatter;
  %newobject new_path_derivation_formatter;
  static derivation_formatter* new_path_derivation_formatter(const derivator* derinet);

  %rename(newTreeDerivationFormatter) new_tree_derivation_formatter;
  %newobject new_tree_derivation_formatter;
  static derivation_formatter* new_tree_derivation_formatter(const derivator* derinet);

  %rename(newDerivationFormatter) new_derivation_formatter;
  %newobject new_derivation_formatter;
  static derivation_formatter* new_derivation_formatter(const char* name, const derivator* derinet);
};

%rename(Morpho) morpho;
%nodefaultctor morpho;
class morpho {
 public:
  virtual ~morpho() {}

  %newobject load;
  static morpho* load(const char* fname);

  enum { NO_GUESSER = 0, GUESSER = 1, GUESSER_UNSPECIFIED = -1 };
  typedef int guesser_mode;

  virtual int analyze(const char* form, guesser_mode guesser, std::vector<tagged_lemma>& lemmas) const;

  virtual int generate(const char* lemma, const char* tag_wildcard, guesser_mode guesser, std::vector<tagged_lemma_forms>& forms) const;

  %extend {
    %rename(rawLemma) raw_lemma;
    std::string raw_lemma(const char* lemma) const {
      return std::string(lemma, $self->raw_lemma_len(lemma));
    }

    %rename(lemmaId) lemma_id;
    std::string lemma_id(const char* lemma) const {
      return std::string(lemma, $self->lemma_id_len(lemma));
    }

    %rename(rawForm) raw_form;
    std::string raw_form(const char* form) const {
      return std::string(form, $self->raw_form_len(form));
    }
  }

  %rename(newTokenizer) new_tokenizer;
  %newobject new_tokenizer;
  virtual tokenizer* new_tokenizer() const;

  %rename(getDerivator) get_derivator;
  virtual const derivator* get_derivator() const;
};

%rename(Tagger) tagger;
%nodefaultctor tagger;
class tagger {
 public:
  virtual ~tagger() {}

  %newobject load;
  static tagger* load(const char* fname);

  %rename(getMorpho) get_morpho;
  virtual const morpho* get_morpho() const;

  %extend {
    void tag(const std::vector<std::string>& forms, std::vector<tagged_lemma>& tags, int guesser = -1) const {
      std::vector<string_piece> string_pieces;
      string_pieces.reserve(forms.size());
      for (auto&& form : forms)
        string_pieces.emplace_back(form);
      $self->tag(string_pieces, tags, morpho::guesser_mode(guesser));
    }
  }

  %extend {
    void tagAnalyzed(const std::vector<std::string>& forms, const std::vector<std::vector<tagged_lemma> >& analyses, std::vector<int>& tags) const {
      std::vector<string_piece> string_pieces;
      string_pieces.reserve(forms.size());
      for (auto&& form : forms)
        string_pieces.emplace_back(form);
      $self->tag_analyzed(string_pieces, analyses, tags);
    }
  }

  %rename(newTokenizer) new_tokenizer;
  %newobject new_tokenizer;
  tokenizer* new_tokenizer() const;
};

%rename(TagsetConverter) tagset_converter;
%nodefaultctor tagset_converter;
class tagset_converter {
 public:
  virtual ~tagset_converter() {}

  virtual void convert(tagged_lemma& tagged_lemma) const;
  %rename(convertAnalyzed) convert_analyzed;
  virtual void convert_analyzed(std::vector<tagged_lemma>& tagged_lemmas) const;
  %rename(convertGenerated) convert_generated;
  virtual void convert_generated(std::vector<tagged_lemma_forms>& forms) const;

  %rename(newIdentityConverter) new_identity_converter;
  %newobject new_identity_converter;
  static tagset_converter* new_identity_converter();

  %rename(newPdtToConll2009Converter) new_pdt_to_conll2009_converter;
  %newobject new_pdt_to_conll2009_converter;
  static tagset_converter* new_pdt_to_conll2009_converter();

  %rename(newStripLemmaCommentConverter) new_strip_lemma_comment_converter;
  %newobject new_strip_lemma_comment_converter;
  static tagset_converter* new_strip_lemma_comment_converter(const morpho& dictionary);

  %rename(newStripLemmaIdConverter) new_strip_lemma_id_converter;
  %newobject new_strip_lemma_id_converter;
  static tagset_converter* new_strip_lemma_id_converter(const morpho& dictionary);
};
