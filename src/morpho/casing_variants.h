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

#pragma once

#include "common.h"
#include "tokenizer/string_piece.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace morphodita {

inline void generate_casing_variants(string_piece form, string& form_uclc, string& form_lc) {
  // Detect uppercase+titlecase characters.
  bool first_Lut = false; // first character is uppercase or titlecase
  bool rest_has_Lut = false; // any character but first is uppercase or titlecase
  {
    string_piece form_tmp = form;
    first_Lut = unicode::category(utf8::decode(form_tmp.str, form_tmp.len)) & unicode::Lut;
    while (form_tmp.len && !rest_has_Lut)
      rest_has_Lut = unicode::category(utf8::decode(form_tmp.str, form_tmp.len)) & unicode::Lut;
  }

  // Generate all casing variants if needed (they are different than given form).
  // We only replace letters with their lowercase variants.
  // - form_uclc: first uppercase, rest lowercase
  // - form_lc: all lowercase

  if (first_Lut && !rest_has_Lut) { // common case allowing fast execution
    form_lc.reserve(form.len);
    string_piece form_tmp = form;
    utf8::append(form_lc, unicode::lowercase(utf8::decode(form_tmp.str, form_tmp.len)));
    form_lc.append(form_tmp.str, form_tmp.len);
  } else if (!first_Lut && rest_has_Lut) {
    form_lc.reserve(form.len);
    utf8::map(unicode::lowercase, form.str, form.len, form_lc);
  } else if (first_Lut && rest_has_Lut) {
    form_lc.reserve(form.len);
    form_uclc.reserve(form.len);
    string_piece form_tmp = form;
    char32_t first = utf8::decode(form_tmp.str, form_tmp.len);
    utf8::append(form_lc, unicode::lowercase(first));
    utf8::append(form_uclc, first);
    while (form_tmp.len) {
      char32_t lowercase = unicode::lowercase(utf8::decode(form_tmp.str, form_tmp.len));
      utf8::append(form_lc, lowercase);
      utf8::append(form_uclc, lowercase);
    }
  }
}

} // namespace morphodita
} // namespace ufal
