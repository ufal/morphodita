// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "output.h"

namespace ufal {
namespace morphodita {

// Print xml content while encoding <>&" using XML entities.
void print_xml_content(FILE* out, const char* text, size_t length) {
  const char* to_print = text;

  while (length) {
    while (length && *text != '<' && *text != '>' && *text != '&' && *text != '"')
      text++, length--;

    if (length) {
      if (to_print < text) fwrite(to_print, 1, text - to_print, out);
      fputs(*text == '<' ? "&lt;" : *text == '>' ? "&gt;" : *text == '&' ? "&amp;" : "&quot;", out);
      text++, length--;
      to_print = text;
    }
  }

  if (to_print < text) fwrite(to_print, 1, text - to_print, out);
}

} // namespace morphodita
} // namespace ufal
