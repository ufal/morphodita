C# MorphoDiTa Bindings
======================

Installation
============

Binary C# bindings are available in MorphoDiTa binary packages.

To compile C# bindings manually, run make in the bindings/csharp directory,
optionally with the options descriged in MorphoDiTa Installation.


C# Bindings API
===============

MorphoDiTa library bindings is available in the Ufal.MorphoDiTa namespace.

The bindings is a straightforward conversion of the C++ bindings API. The
bindings requires native C++ library libmorphodita_csharp (called
morphodita_csharp on Windows).


C++ Bindings API
================


Helper Structures
-----------------

  typedef vector<int> Indices;

  typedef vector<string> Forms;

  struct TaggedForm {
    string form;
    string tag;
  };
  typedef vector<TaggedForm> TaggedForms;

  struct TaggedLemma {
    string lemma;
    string tag;
  };
  typedef vector<TaggedLemma> TaggedLemmas;
  typedef vector<TaggedLemmas> Analyses;

  struct TaggedLemmaForms {
    string lemma;
    TaggedForms forms;
  };
  typedef vector<TaggedLemmaForms> TaggedLemmasForms;

  struct TokenRange {
    size_t start;
    size_t length;
  };
  typedef vector<TokenRange> TokenRanges;

  struct DerivatedLemma {
    std::string lemma;
  };
  typedef vector<DerivatedLemma> DerivatedLemmas;


Main Classes
------------

  class Version {
   public:
    unsigned major;
    unsigned minor;
    unsigned patch;
    string prerelease;

    static Version current();
  };

  class Tokenizer {
   public:
    virtual void setText(const char* text);
    virtual bool nextSentence(Forms* forms, TokenRanges* tokens);

    static Tokenizer* newVerticalTokenizer();
    static Tokenizer* newCzechTokenizer();
    static Tokenizer* newEnglishTokenizer();
    static Tokenizer* newGenericTokenizer();
  };

  class Derivator {
   public:
    virtual bool parent(const char* lemma, DerivatedLemma& parent) const;
    virtual bool children(const char* lemma, DerivatedLemmas& children) const;
  };

  class DerivationFormatter {
   public:
    virtual string formatDerivation(const char* lemma) const;

    static DerivationFormatter* newNoneDerivationFormatter();
    static DerivationFormatter* newRootDerivationFormatter(const Derivator* derivator);
    static DerivationFormatter* newPathDerivationFormatter(const Derivator* derivator);
    static DerivationFormatter* newTreeDerivationFormatter(const Derivator* derivator);
    static DerivationFormatter* newDerivationFormatter(const char* name, const Derivator* derivator);
  };

  class Morpho {
   public:
    static Morpho* load(const char* fname);

    enum { NO_GUESSER = 0, GUESSER = 1, GUESSER_UNSPECIFIED = -1 };

    virtual int analyze(const char* form, int guesser, TaggedLemmas& lemmas) const;
    virtual int generate(const char* lemma, const char* tag_wildcard, int guesser, TaggedLemmasForms& forms) const;
    virtual string rawLemma(const char* lemma) const;
    virtual string lemmaId(const char* lemma) const;
    virtual string rawForm(const char* form) const;

    virtual Tokenizer* newTokenizer() const;

    virtual Derivator* getDerivator() const;
  };

  class Tagger {
   public:
    static Tagger* load(const char* fname);

    virtual const Morpho* getMorpho() const;

    virtual void tag(const Forms& forms, TaggedLemmas& tags, int guesser = Morpho::GUESSER_UNSPECIFIED) const;

    virtual void tagAnalyzed(const Forms& forms, const Analyses& analyses, Indices& tags) const;

    Tokenizer* newTokenizer() const;
  };

  class TagsetConverter {
   public:
    static TagsetConverter* newIdentityConverter();
    static TagsetConverter* newPdtToConll2009Converter();
    static TagsetConverter* newStripLemmaCommentConverter(const Morpho& morpho);
    static TagsetConverter* newStripLemmaIdConverter(const Morpho& morpho);

    virtual void convert(TaggedLemma& lemma) const;
    virtual void convertAnalyzed(TaggedLemmas& lemmas) const;
    virtual void convertGenerated(TaggedLemmasForms& forms) const;
  };
