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
namespace morphodita {

// Declarations
class tag_filter {
 public:
  tag_filter(const char* filter = nullptr);

  inline bool matches(const char* tag) const;

 private:
  struct char_filter {
    char_filter(int pos, bool negate, const char* chars, int len) : pos(pos), negate(negate), chars(chars), len(len) {}

    int pos;
    bool negate;
    const char* chars;
    int len;
  };

  std::vector<char_filter> filters;
};

// Definitions
inline bool tag_filter::matches(const char* tag) const {
  if (filters.empty()) return true;

  int tag_pos = 0;
  for (auto& filter : filters) {
    while (tag_pos < filter.pos)
      if (!tag[tag_pos++])
        return true;

    // We assume filter.len >= 1.
    bool matched = (*filter.chars == tag[tag_pos]) ^ filter.negate;
    for (int i = 1; i < filter.len && !matched; i++)
      matched = (filter.chars[i] == tag[tag_pos]) ^ filter.negate;
    if (!matched) return false;
  }
  return true;
}

} // namespace morphodita
} // namespace ufal
