// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

// Headers available in all sources
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

// Assert that int is at least 4B
static_assert(sizeof(int) >= sizeof(int32_t), "Int must be at least 4B wide!");

// Assert that we are on a little endian system
#ifdef __BYTE_ORDER__
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Only little endian systems are supported!");
#endif

// Define namespace ufal::morphodita.
namespace ufal {
namespace morphodita {

using namespace std;

namespace unilib {}
using namespace unilib;

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

} // namespace morphodita
} // namespace ufal
