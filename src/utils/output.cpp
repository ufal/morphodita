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
