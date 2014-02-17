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

#include "vertical_tokenizer.h"

namespace ufal {
namespace morphodita {

bool vertical_tokenizer::next_sentence(vector<string_piece>& forms) {
  if (text == text_end) return false;

  while (true) {
    const char* line_start = text;
    while (text < text_end && *text != '\r' && *text != '\n') text++;

    const char* line_end = text;
    if (text < text_end) {
      text++;
      if (text < text_end && ((text[-1] == '\r' && *text == '\n') || (text[-1] == '\n' && *text == '\r'))) text++;
    }

    if (line_start < line_end) forms.emplace_back(line_start, line_end - line_start);
    else break;
  }

  return true;
}

} // namespace morphodita
} // namespace ufal


