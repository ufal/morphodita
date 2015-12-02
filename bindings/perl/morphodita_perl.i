%module "Ufal::MorphoDiTa"

%runtime %{
#ifdef seed
  #undef seed
#endif
%}

%include "../common/morphodita.i"
