%module morphodita

%begin %{
#ifdef _WIN32
// On Windows, pyconfig.h defines macro hypot (#define hypot _hypot).
// That breaks cmath, which declares using ::hypot.
// As a temporary hack, cmath can be included first (it is included anyway).
// Nevertheless, including other headers before Python.h is officially not
// supported. so we may change the behaviour in the future if needed.
// Python 2.7 and 3.4 both require the hack and work well with it.
#include <cmath>
#endif
%}

// Typemap updates for vector<vector<tagged_lemma>>::value_type.
// Without them swig::from is used which returns a tuple.
%typemap(out, noblock=1) const std::vector<std::vector<tagged_lemma> >::value_type & __getitem__, const std::vector<std::vector<tagged_lemma> >::value_type & front, const std::vector<std::vector<tagged_lemma> >::value_type & back {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr($1), $descriptor(std::vector<tagged_lemma>*), 0);
}
%typemap(out, noblock=1) std::vector<std::vector<tagged_lemma> >::value_type pop {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr(new std::vector<tagged_lemma>($1)), $descriptor(std::vector<tagged_lemma>*), SWIG_POINTER_OWN);
}

%include "../common/morphodita.i"
