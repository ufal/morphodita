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

#include "morpho_statistical_guesser_encoder.h"
#include "utils/input.h"
#include "utils/persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

void morpho_statistical_guesser_encoder::encode(FILE* f, ufal::morphodita::binary_encoder& enc) {
  unordered_map<string, vector<pair<vector<string>, vector<int>>>> statistical_guesser;
  vector<string> tags;
  unordered_map<string, int> tags_map;

  // Load statistical guesser
  string line;
  vector<string> tokens;
  if (!getline(f, line)) runtime_errorf("Missing first line with default tag in statistical guesser file");
  int statistical_guesser_default = tags_map.emplace(line.data(), tags.size()).first->second;
  if (unsigned(statistical_guesser_default) >= tags.size()) tags.emplace_back(line.data());

  while (getline(f, line)) {
    split(line, '\t', tokens);
    if (tokens.size() < 3 || (tokens.size() % 2) != 1) runtime_errorf("Cannot parse line %s in statistical guesser file!", line.c_str());

    vector<string> affixes;
    split(tokens[0], ' ', affixes);
    if (affixes.size() != 2) runtime_errorf("Cannot parse prefix_suffix '%s' in statistical guesser file!", tokens[0].c_str());
    reverse(affixes[1].begin(), affixes[1].end());

    auto& rules = statistical_guesser[affixes[1] + ' ' + affixes[0]];
    for (unsigned i = 1; i < tokens.size(); i+= 2) {
      vector<string> replacements;
      split(tokens[i], ' ', replacements);
      if (replacements.size() != 4) runtime_errorf("Cannot parse replacement rule '%s' in statistical guesser file!", tokens[i].c_str());

      vector<string> rule_tags;
      split(tokens[i+1], ' ', rule_tags);
      vector<int> decoded_tags;
      for (auto& rule_tag : rule_tags) {
        int tag = tags_map.emplace(rule_tag, tags.size()).first->second;
        if (unsigned(tag) >= tags.size()) tags.emplace_back(rule_tag);
        decoded_tags.emplace_back(tag);
      }

      rules.emplace_back(replacements, decoded_tags);
    }
  }

  // Encode statistical guesser
  enc.add_2B(tags.size());
  for (auto& tag : tags) {
    enc.add_1B(tag.size());
    enc.add_str(tag);
  }
  enc.add_2B(statistical_guesser_default);

  persistent_unordered_map(statistical_guesser, 5, true, false, [](binary_encoder& enc, vector<pair<vector<string>, vector<int>>> rules) {
    binary_encoder e;
    e.add_1B(rules.size());
    for (auto& rule : rules) {
      if (rule.first.size() != 4) runtime_errorf("Replacement rule not of size 4 in statistical guesser!");
      for (auto& affix : rule.first) {
        e.add_1B(affix.size());
        e.add_str(affix);
      }
      e.add_1B(rule.second.size());
      for (auto& tag : rule.second)
        e.add_2B(tag);
    }
    enc.add_2B(e.data.size());
    enc.add_data(e.data);
  }).save(enc);
}

} // namespace morphodita
} // namespace ufal
