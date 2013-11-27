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

#include "raw_morpho_dictionary_reader.h"
#include "utils/input.h"

namespace ufal {
namespace morphodita {

bool raw_morpho_dictionary_reader::next_lemma(string& lemma, vector<pair<string, string>>& tagged_forms) {
  if (line.empty()) {
    if (!getline(in, line))
      return false;
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("Line %s does not have three columns!", line.c_str());
  }

  lemma = tokens[0];
  tagged_forms.clear();
  tagged_forms.emplace_back(tokens[2], tokens[1]);
  while (getline(in, line)) {
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("Line %s does not have three columns!", line.c_str());

    if (lemma != tokens[0]) break;
    tagged_forms.emplace_back(tokens[2], tokens[1]);
  }

  return true;
}

} // namespace morphodita
} // namespace ufal
