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

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>

#include "common.h"

namespace ufal {
namespace morphodita {

typedef unordered_map<string, string> options_map;

struct option_values {
  enum values_t { NONE, ANY, SET };
  values_t values;
  unordered_set<string> allowed;

  option_values(initializer_list<string> allowed) : values(SET), allowed(allowed) {}
  static const option_values none;
  static const option_values any;

 private:
  option_values(values_t values) : values(values) {}
};

// Parse options according to allowed map. If successful, argv is reordered so
// that non-option arguments are placed in argv[1] to argv[argc-1]. The '--'
// indicates end of option arguments (as usual).  The allowed map contains
// values allowed for every option. If empty, no value is allowed, if it
// contains just an empty string, any value is allowed.
bool parse_options(const unordered_map<string, option_values>& allowed, int& argc, char**& argv, options_map& options);

// If requested, show current MorphoDiTa version.
void show_version_if_requested(int argc, char* argv[]);

} // namespace morphodita
} // namespace ufal
