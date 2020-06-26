%module(package="ufal") morphodita

// Typemap updates for vector<vector<tagged_lemma>>::value_type.
// Without them swig::from is used which returns a tuple.
%typemap(out, noblock=1) const std::vector<std::vector<tagged_lemma> >::value_type & __getitem__, const std::vector<std::vector<tagged_lemma> >::value_type & front, const std::vector<std::vector<tagged_lemma> >::value_type & back {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), $descriptor(std::vector<tagged_lemma>*), 0);
}
%typemap(out, noblock=1) std::vector<std::vector<tagged_lemma> >::value_type pop {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr(new std::vector<tagged_lemma>($1)), $descriptor(std::vector<tagged_lemma>*), SWIG_POINTER_OWN);
}

%pythonbegin %{
# __version__ =
%}

%include "../common/morphodita.i"
