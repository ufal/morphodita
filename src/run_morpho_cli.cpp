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
#include "utils/parse_options.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  show_version_if_requested(argc, argv);

  if (argc <= 1) runtime_errorf("Usage: %s dict_file", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary from file '%s'!", argv[1]);
  eprintf("done\n");

  string line;
  vector<string> tokens;
  while (getline(stdin, line)) {
    split(line, '\t', tokens);
    if /* analyze */ (tokens.size() == 1) {
      vector<tagged_lemma> lemmas;
      auto result = dictionary->analyze(tokens[0], morpho::GUESSER, lemmas);

      string guesser_name = result == morpho::GUESSER ? "Guesser " : "";
      for (auto&& lemma : lemmas)
        printf("%sLemma: %s %s\n", guesser_name.c_str(), lemma.lemma.c_str(), lemma.tag.c_str());

    } else if /* generate */ (tokens.size() == 2) {
      vector<tagged_lemma_forms> forms;
      auto result = dictionary->generate(tokens[0], tokens[1].c_str(), morpho::GUESSER, forms);

      string guesser_name = result == morpho::GUESSER ? "Guesser " : "";
      for (auto&& lemma : forms) {
        printf("%sLemma: %s\n", guesser_name.c_str(), lemma.lemma.c_str());
        for (auto&& form : lemma.forms)
          printf("  %s %s\n", form.form.c_str(), form.tag.c_str());
      }
    }
  }

  return 0;
}
