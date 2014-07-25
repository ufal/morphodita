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

#include <cstring>

#include "common.h"

namespace ufal {
namespace morphodita {

// Call a given processor on specified arguments. Every argument can be
// either input_file or input_file:output_file. If not output_file is specified,
// stdout is used. If there are no arguments at all, stdin and stdout are used.
template <class T, class... U>
void process_args(int argi, int argc, char* argv[], T processor, U&&... processor_args) {
  if (argi >= argc) {
    processor(stdin, stdout, std::forward<U>(processor_args)...);
  } else for (; argi < argc; argi++) {
    char* file_in = argv[argi];
    char* file_out = strchr(file_in, ':');
    if (file_out) *file_out++ = '\0';

    FILE* in = fopen(file_in, "r");
    if (!in) runtime_errorf("Cannot open file '%s' for reading!", file_in);

    FILE* out = nullptr;
    if (file_out) {
      out = fopen(file_out, "w");
      if (!out) runtime_errorf("Cannot open file '%s' for writing!", file_out);
    }

    processor(in, out ? out : stdout, std::forward<U>(processor_args)...);

    if (out) fclose(out);
    fclose(in);
  }
}

} // namespace morphodita
} // namespace ufal
