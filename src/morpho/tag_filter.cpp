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

#include "tag_filter.h"

namespace ufal {
namespace morphodita {

tag_filter::tag_filter(const char* filter) {
  if (!filter) return;

  wildcard.assign(filter);
  filter = wildcard.c_str();

  for (int tag_pos = 0; *filter; tag_pos++, filter++) {
    if (*filter == '?') continue;
    if (*filter == '[') {
      filter++;

      bool negate = false;
      if (*filter == '^') negate = true, filter++;

      const char* chars = filter;
      for (bool first = true; *filter && (first || *filter != ']'); first = false) filter++;

      filters.emplace_back(tag_pos, negate, chars, filter - chars);
      if (!*filter) break;
    } else {
      filters.emplace_back(tag_pos, false, filter, 1);
    }
  }
}

} // namespace morphodita
} // namespace ufal
