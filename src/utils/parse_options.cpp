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
      if (values == allowed.end()) return eprintf("Unknown option '%s'.\n", argv[argi]), false;

      string value;
      if (values->second.values == option_values::NONE && equal_sign) return eprintf("Option '%s' cannot have value.\n", key.c_str()), false;
      if (values->second.values != option_values::NONE) {
        if (equal_sign) {
          value.assign(equal_sign + 1);
        } else {
          if (argi + 1 == argc) return eprintf("Missing value for option '%s'.\n", key.c_str()), false;
          value.assign(argv[++argi]);
        }
        if (!(values->second.values == option_values::ANY || (values->second.values == option_values::SET && values->second.allowed.count(value))))
          return eprintf("Option '%s' cannot have value '%s'.\n", key.c_str(), value.c_str()), false;
      }
      options[key] = value;
    } else {
      argv[args++] = argv[argi];
    }

  argc = args;
  return true;
}

void show_version_if_requested(int argc, char* argv[]) {
  for (int argi = 1; argi < argc; argi++)
    if (argv[argi][0] == '-') {
      if (strcmp(argv[argi], "--") == 0) break;
      if (strcmp(argv[argi] + 1 + (argv[argi][1] == '-'), "version") == 0) {
        version version = version::current();

        eprintf("MorphoDiTa version %u.%u.%u\n"
                "Copyright 2014 by Institute of Formal and Applied Linguistics, Faculty of\n"
                "Mathematics and Physics, Charles University in Prague, Czech Republic.\n",
                version.major, version.minor, version.patch);

        exit(0);
      }
    }
}

} // namespace morphodita
} // namespace ufal
