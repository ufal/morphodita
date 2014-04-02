Java MorphoDiTa Bindings
========================

Installation
============

Binary Java bindings are available in MorphoDiTa binary packages.

To compile Java bindings manually, run make in the bindings/java directory,
optionally with the options descriged in MorphoDiTa Installation. Java 6 and
newer is supported.

The Java installation specified in the environment variable JAVA_HOME is used.
If the environment variable does not exist, the JAVA_HOME can be specified using

  make JAVA_HOME=path_to_Java_installation


Java Bindings API
=================

MorphoDiTa library bindings is available in the cz.cuni.mff.ufal.morphodita
package.

The bindings is a straightforward conversion of the C++ bindings API. Vectors do
not have native Java interface, see cz.cuni.mff.ufal.morphodita.Forms class for
reference. Also, class members are accessible and modifiable using using
getField and setField wrappers.

The bindings require native C++ library morphodita_java (called
libmorphodita_java on Linux). If the library is found in the current directory,
it is used, otherwise standard library search process is used.


C++ Bindings API
================


Helper Structures
-----------------

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


Main Classes
------------

  class Version {
   public:
    unsigned major;
    unsigned minor;
    unsigned patch;
  
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
  
  class Morpho {
   public:
    static Morpho* load(const char* fname);
  
    enum { NO_GUESSER = 0, GUESSER = 1 };
  
    virtual int analyze(const char* form, int guesser, TaggedLemmas& lemmas) const;
    virtual int generate(const char* lemma, const char* tag_wildcard, int guesser, TaggedLemmasForms& forms) const;
    virtual string rawLemma(const char* lemma) const;
    virtual string lemmaId(const char* lemma) const;
  
    virtual Tokenizer* newTokenizer() const;
  };
  
  class Tagger {
   public:
    static Tagger* load(const char* fname);
  
    virtual const Morpho* getMorpho() const;
  
    virtual void tag(Forms& forms, TaggedLemmas& tags) const;
  
    Tokenizer* newTokenizer() const;
  };
  
  class TagsetConverter {
   public:
    static TagsetConverter* newIdentityConverter();
    static TagsetConverter* newPdtToConll2009Converter();
  
    virtual void convert(TaggedLemma& lemma) const;
    virtual void convertAnalyzed(TaggedLemmas& lemmas) const;
    virtual void convertGenerated(TaggedLemmasForms& forms) const;
  };
