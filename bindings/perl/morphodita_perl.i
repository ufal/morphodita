%module "Ufal::MorphoDiTa"

%runtime %{
#ifdef seed
  #undef seed
#endif
%}

%include "../common/morphodita.i"

%perlcode %{
@EXPORT_OK = qw(*Forms:: *TaggedForm:: *TaggedForms:: *TaggedLemma:: *TaggedLemmas::
                *TaggedLemmaForms:: *TaggedLemmasForms:: *TokenRange:: *TokenRanges::
                *Tokenizer:: *Morpho:: *Tagger:: *TagsetConverter::);
%EXPORT_TAGS = (all => [@EXPORT_OK]);
%}
