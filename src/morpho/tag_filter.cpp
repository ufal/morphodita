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

  for (int tag_pos = 0; *filter; tag_pos++, filter++) {
    if (*filter == '?') continue;
    if (*filter == '[') {
      filter++;

      bool negate = false;
      if (*filter == '^') negate = true, filter++;
      filters.emplace_back(tag_pos, negate);

      for (bool first = true; *filter && (first || *filter != ']'); first = false, filter++)
        filters.back().matches[(unsigned) *filter] = !negate;
    } else {
      filters.emplace_back(tag_pos, false);
      filters.back().matches[(unsigned) *filter] = true;
    }
  }
}

bool tag_filter::matches(const char* tag) const {
  if (filters.empty()) return true;

  int tag_pos = 0;
  for (auto& filter : filters) {
    while (tag_pos < filter.pos && tag[tag_pos]) tag_pos++;
    if (!filter.matches[(unsigned) tag[tag_pos]]) return false;
  }
  return true;
}

tag_filter::char_filter::char_filter(int pos, bool def) : pos(pos) {
  memset(matches, def, sizeof(matches));
}

} // namespace morphodita
} // namespace ufal
