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

namespace ufal {
namespace utils {

// These utf8_advance is consistent with all utf8_* ragel machines from
// utf8.rl, including broken UTF-8 encoding.
inline void utf8_advance(const char*& text);
inline void utf8_advance(const char*& text, const char* end);


// Definitions
inline void utf8_advance(const char*& text) {
  text++;
  while (*(const unsigned char*)text >= 0x80 && *(const unsigned char*)text < 0xC0) text++;
}

inline void utf8_advance(const char*& text, const char* end) {
  text++;
  while (text < end && *(const unsigned char*)text >= 0x80 && *(const unsigned char*)text < 0xC0) text++;
}

} // namespace utils
} // namespace ufal
