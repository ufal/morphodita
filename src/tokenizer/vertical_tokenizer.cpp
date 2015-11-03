// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
