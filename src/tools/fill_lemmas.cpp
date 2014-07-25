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
  if (argc <= 1) runtime_errorf("Usage: %s dict_file <data", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary %s!", argv[1]);
  eprintf("done\n");

  eprintf("Processing data: ");

  int forms = 0, matches = 0;
  vector<tagged_lemma> lemmas;

  string line;
  vector<string> tokens;
  while (getline(stdin, line)) {
    if (line.empty()) {
      putchar('\n');
      continue;
    }

    split(line, '\t', tokens);
    if (tokens.size() != 2) runtime_errorf("The line %s does not contain three columns!", line.c_str());

    // Analyse
    dictionary->analyze(tokens[0], morpho::GUESSER, lemmas);

    forms++;

    int match = -1;
    for (unsigned i = 0; i < lemmas.size(); i++)
      if (lemmas[i].tag == tokens[1] && lemmas[i].lemma == tokens[0]) {
        if (match == -1) match = i;
        else if (lemmas[i].lemma != lemmas[match].lemma) eprintf("Multiple matching samelemmaasform-tags with different lemmas for form '%s'.\n", tokens[0].c_str());
      }

    if (match == -1)
      for (unsigned i = 0; i < lemmas.size(); i++)
        if (lemmas[i].tag == tokens[1]) {
          if (match == -1) match = i;
          else if (lemmas[i].lemma != lemmas[match].lemma) eprintf("Multiple matching tags with different lemmas for form '%s'.\n", tokens[0].c_str());
        }

    matches += match != -1;
    if (match == -1) eprintf("Did not get tag '%s' for form '%s'.\n", tokens[1].c_str(), tokens[0].c_str());
    printf("%s\t%s\t%s\n", tokens[0].c_str(), match != -1 ? lemmas[match].lemma.c_str() : tokens[0].c_str() , tokens[1].c_str());
  }
  eprintf("done\n");
  eprintf("Forms: %d, matched: %.3f%%\n", forms, 100 * matches / double(forms));

  return 0;
}
