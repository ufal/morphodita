// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>
#include <cstdlib>

#include "parse_options.h"
#include "version/version.h"

namespace ufal {
namespace morphodita {

const option_values option_values::none(NONE);
const option_values option_values::any(ANY);

bool parse_options(const unordered_map<string, option_values>& allowed, int& argc, char**& argv, options_map& options) {
  int args = 1;
  bool options_allowed = true;
  for (int argi = 1; argi < argc; argi++)
    if (argv[argi][0] == '-' && options_allowed) {
      if (argv[argi][1] == '-' && argv[argi][2] == '\0') {
        options_allowed = false;
        continue;
      }
      const char* option = argv[argi] + 1 + (argv[argi][1] == '-');
      const char* equal_sign = strchr(option, '=');

      string key = equal_sign ? string(option, equal_sign - option) : string(option);
      auto values = allowed.find(key);
      if (values == allowed.end()) return cerr << "Unknown option '" << argv[argi] << "'." << endl, false;

      string value;
      if (values->second.values == option_values::NONE && equal_sign) return cerr << "Option '" << key << "' cannot have value." << endl, false;
      if (values->second.values != option_values::NONE) {
        if (equal_sign) {
          value.assign(equal_sign + 1);
        } else {
          if (argi + 1 == argc) return cerr << "Missing value for option '" << key << "'." << endl, false;
          value.assign(argv[++argi]);
        }
        if (!(values->second.values == option_values::ANY || (values->second.values == option_values::SET && values->second.allowed.count(value))))
          return cerr << "Option '" << key << "' cannot have value '" << value << "'." << endl, false;
      }
      options[key] = value;
    } else {
      argv[args++] = argv[argi];
    }

  argc = args;
  return true;
}

} // namespace morphodita
} // namespace ufal
