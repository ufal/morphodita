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

#include <algorithm>
#include <unordered_map>

#include "morpho_prefix_guesser_encoder.h"
#include "utils/input.h"
#include "utils/persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

void morpho_prefix_guesser_encoder::encode(FILE* f, binary_encoder& enc) {
  vector<string> filters;
  unordered_map<string, uint32_t> filters_map;
  unordered_map<string, uint32_t> prefixes_initial;
  unordered_map<string, uint32_t> prefixes_middle;
  auto* prefixes_current = &prefixes_initial;

  // Load prefix guesser
  string line;
  vector<string> tokens;
  while (getline(f, line)) {
    if (line.empty() && prefixes_current == &prefixes_initial) {
      prefixes_current = &prefixes_middle;
      continue;
    }
    split(line, '\t', tokens);
    if (tokens.size() != 2) runtime_errorf("Line %s in prefix guesser prefixes file does not contain two columns!", line.c_str());

    auto it = filters_map.emplace(tokens[1], 1<<filters.size());
    if (it.second)
      filters.emplace_back(tokens[1]);
    auto filter = it.first->second;
    if (!filter) runtime_errorf("Too much different tag filters in the prefix guesser when adding tag filter '%s'!", tokens[1].c_str());

    (*prefixes_current)[tokens[0]] |= filter;
  }

  // Encode prefix guesser
  enc.add_1B(filters.size());
  for (auto& filter : filters) {
    enc.add_1B(filter.size());
    enc.add_str(filter);
  }

  persistent_unordered_map(prefixes_initial, 5, true, false, [](binary_encoder& enc, uint32_t mask) { enc.add_4B(mask); }).save(enc);
  persistent_unordered_map(prefixes_middle, 5, true, false, [](binary_encoder& enc, uint32_t mask) { enc.add_4B(mask); }).save(enc);
}

} // namespace morphodita
} // namespace ufal
