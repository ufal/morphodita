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

#include <map>
#include <set>
#include <unordered_map>

#include "english_morpho_guesser_encoder.h"
#include "utils/input.h"
#include "utils/parse_int.h"
#include "utils/persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

void english_morpho_guesser_encoder::encode(FILE* guesser_file, FILE* negations_file, binary_encoder& enc) {
  // Guesser exceptions
  typedef map<string, set<string>> lemma_info;
  unordered_map<string, lemma_info> guesser;
  unordered_map<string, unsigned> tags_map;
  vector<string> tags;

  string line;
  vector<string> tokens;

  // Load guesser exceptions
  while (getline(guesser_file, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("The line '%s' in english guesser file does not have three columns!", line.c_str());
    guesser[tokens[2]][tokens[0]].insert(tokens[1]);
    if (tags_map.emplace(tokens[1], tags_map.size()).second) tags.emplace_back(tokens[1]);
  }

  // Negations
  struct negation_info {
    unsigned negation_len;
    unsigned to_follow;

    negation_info() : negation_len(0), to_follow(0) {}
    negation_info(unsigned negation_len, unsigned to_follow) : negation_len(negation_len), to_follow(to_follow) {}
  };
  unordered_map<string, negation_info> negations;

  // Load negations
  while (negations_file && getline(negations_file, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("The line '%s' in english negation file does not have three columns!", line.c_str());

    unsigned negation_len = parse_int(tokens[2].c_str(), "negation_len in english negation file");
    if (!negation_len) runtime_errorf("Negation len in line '%s' in english negation file is zero!", line.c_str());

    unsigned to_follow = parse_int(tokens[1].c_str(), "to_follow in english negation file");

    for (unsigned i = 0; i < tokens[0].size(); i++)
      if (negations[tokens[0].substr(0, i)].negation_len) runtime_errorf("A prefix of negation '%s' in english negation file is repeated!", tokens[0].c_str());
    if (negations.count(tokens[0])) runtime_errorf("The negation '%s' in english negation file is repeated!", tokens[0].c_str());
    negations.emplace(tokens[0], negation_info(negation_len, to_follow));
  }

  // Save tags
  enc.add_2B(tags.size());
  for (auto&& tag : tags) {
    enc.add_1B(tag.size());
    enc.add_str(tag);
  }

  // Save guesser exceptions
  persistent_unordered_map(guesser, 5, [&tags_map](binary_encoder& enc, const lemma_info& lemmas) {
    enc.add_1B(lemmas.size());
    for (auto&& lemma : lemmas) {
      enc.add_1B(lemma.first.size());
      enc.add_str(lemma.first);
      enc.add_1B(lemma.second.size());
      for (auto&& tag : lemma.second)
        enc.add_2B(tags_map.at(tag));
    }
  }).save(enc);

  // Save negations
  persistent_unordered_map(negations, 5, [](binary_encoder& enc, const negation_info& negation) {
    enc.add_1B(negation.negation_len);
    enc.add_1B(negation.to_follow);
  }).save(enc);
}

} // namespace morphodita
} // namespace ufal
