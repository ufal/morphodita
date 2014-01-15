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
inline bool small_memeq(const void* a, const void* b, size_t len);
inline void small_memcpy(void* dest, const void* src, size_t len);

// Definitions
inline bool small_memeq(const void* a_void, const void* b_void, size_t len) {
  const char* a = (const char*)a_void;
  const char* b = (const char*)b_void;

  while (len--)
    if (*a++ != *b++)
      return false;
  return true;
}

inline void small_memcpy(void* dest_void, const void* src_void, size_t len) {
  char* dest = (char*)dest_void;
  const char* src = (const char*)src_void;

  while (len--)
    *dest++ = *src++;
}

} // namespace morphodita
} // namespace ufal
