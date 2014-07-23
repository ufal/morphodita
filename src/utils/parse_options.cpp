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
