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

// Try to parse a double from given string. If the double cannot be parsed, an
// error is displayed and program exits. If that happens, the value_name
// argument is used in the error message.
inline double parse_double(const char* str, const char* value_name) {
  char* end;

  errno = 0;
  double result = strtod(str, &end);
  if (*end || errno == ERANGE)
    runtime_errorf("Cannot parse %s double value: '%s'!", value_name, str);

  return result;
}

} // namespace utils
} // namespace ufal
