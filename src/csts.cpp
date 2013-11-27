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

#include <memory>

#include "morpho/morpho.h"
#include "utils/input.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 2) runtime_errorf("Usage: %s dict_file use_guesser", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> d(morpho::load(argv[1]));
  if (!d) runtime_errorf("Cannot load dictionary from file '%s'!", argv[1]);
  eprintf("done\n");
  bool use_guesser = atoi(argv[2]);

  bool in_sentence = false;
  vector<tagged_lemma> tags;

  string line;
  vector<string> tokens;
  while (getline(stdin, line)) {
    if (line.empty()) {
      if (in_sentence) printf("</s>\n");
      in_sentence = false;
    } else {
      if (!in_sentence) printf("<s>\n");
      in_sentence = true;

      split(line, '\t', tokens);
      if (tokens.size() != 3) runtime_errorf("Input line '%s' does not contain 3 columns!", line.c_str());
      d->analyze(tokens[0].c_str(), tokens[0].size(), use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, tags);

      printf("<f>%s<l>%s<t>%s", tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());
      for (auto& tag : tags)
        printf("<MMl>%s<MMt>%s", tag.lemma.c_str(), tag.tag.c_str());
      printf("\n");
    }
  }
  if (in_sentence) printf("</s>\n");

  return 0;
}
