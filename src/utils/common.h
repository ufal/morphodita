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

// Headers available in all sources
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

// Define namespace name and open std in it.
namespace ufal {
namespace utils {

using namespace std;

// Printf-like logging function.
inline int eprintf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = vfprintf(stderr, fmt, ap);
  va_end(ap);
  return res;
}

// Printf-like exit function.
inline void runtime_errorf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fputc('\n', stderr);
  exit(1);
}

// Export attributes
#ifdef _WIN32
  #ifdef BUILDING_DLL
    #define EXPORT_ATTRIBUTES __attribute__ ((dllexport))
  #else
    #define EXPORT_ATTRIBUTES
  #endif
#else
  #define EXPORT_ATTRIBUTES __attribute__ ((visibility ("default")))
#endif

} // namespace utils
} // namespace ufal
