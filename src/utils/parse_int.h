// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cerrno>

#include "common.h"

namespace ufal {
namespace morphodita {

// Try to parse an int from given string. If the int cannot be parsed or does
// not fit into int, an error is displayed and program exits. If that happens,
// the value_name argument is used in the error message.
inline int parse_int(const char* str, const char* value_name) {
  char* end;

  errno = 0;
  long result = strtol(str, &end, 10);
  if (*end || errno == ERANGE || result != int(result))
    runtime_failure("Cannot parse " << value_name << " int value: '" << str << "'!");

  return int(result);
}

} // namespace morphodita
} // namespace ufal
