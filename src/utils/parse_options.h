// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

} // namespace morphodita
} // namespace ufal
