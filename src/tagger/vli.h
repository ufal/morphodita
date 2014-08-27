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
namespace morphodita {

// Declarations
template <class T>
class vli {
 public:
  static int max_length();
  static void encode(T value, unsigned char*& where);
  static T decode(const unsigned char*& from);
};


// Definitions
template <>
inline int vli<uint32_t>::max_length() {
  return 5;
}

template <>
inline void vli<uint32_t>::encode(uint32_t value, unsigned char*& where) {
  if (value < 0x80) *where++ = value;
  else if (value < 0x4000) *where++ = (value >> 7) | 0x80, *where++ = value & 0x7F;
  else if (value < 0x200000) *where++ = (value >> 14) | 0x80, *where++ = ((value >> 7) & 0x7F) | 0x80, *where++ = value & 0x7F;
  else if (value < 0x10000000) *where++ = (value >> 21) | 0x80, *where++ = ((value >> 14) & 0x7F) | 0x80, *where++ = ((value >> 7) & 0x7F) | 0x80, *where++ = value & 0x7F;
  else *where++ = (value >> 28) | 0x80, *where++ = ((value >> 21) & 0x7F) | 0x80, *where++ = ((value >> 14) & 0x7F) | 0x80, *where++ = ((value >> 7) & 0x7F) | 0x80, *where++ = value & 0x7F;
}

template <>
inline uint32_t vli<uint32_t>::decode(const unsigned char*& from) {
  uint32_t value = 0;
  while (*from & 0x80) value = (value << 7) | ((*from++) ^ 0x80);
  value = (value << 7) | (*from++);
  return value;
}

} // namespace morphodita
} // namespace ufal
