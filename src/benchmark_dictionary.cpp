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

#include <ctime>
#include <memory>

#include "morpho/morpho.h"
#include "utils/input.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 2) runtime_errorf("Usage: %s dict_file forms_to_analyze <raw_dict_file", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> d(morpho::load(argv[1]));
  if (!d) runtime_errorf("Cannot load dictionary %s!", argv[1]);
  eprintf("done\n");

  eprintf("Loading forms to analyze: ");
  vector<string> forms;
  string line;
  vector<string> tokens;
  for (int i = atoi(argv[2]); i > 0; i--) {
    if (!getline(stdin, line)) break;
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("Line '%s' of the raw morpho dictionary does not have three columns!", line.c_str());
    forms.emplace_back(tokens[2]);
  }
  eprintf("done, %d forms read.\n", int(forms.size()));

  eprintf("Analyzing: ");
  clock_t now = clock();
  vector<tagged_lemma> lemmas;
  for (auto& form : forms) {
    d->analyze(form.c_str(), form.size(), morpho::NO_GUESSER, lemmas);
  }
  eprintf("done in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));

  return 0;
}
