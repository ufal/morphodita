// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>
#include <memory>

#include "morpho/morpho.h"
#include "utils/input.h"
#include "utils/parse_int.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 2) runtime_errorf("Usage: %s dict_file forms_to_analyze <raw_dict_file", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary %s!", argv[1]);
  eprintf("done\n");

  eprintf("Loading forms to analyze: ");
  vector<string> forms;
  string line;
  vector<string> tokens;
  for (int i = parse_int(argv[2], "forms_to_analyze"); i > 0; i--) {
    if (!getline(stdin, line)) break;
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("Line '%s' of the raw morpho dictionary does not have three columns!", line.c_str());
    forms.emplace_back(tokens[2]);
  }
  eprintf("done, %d forms read.\n", int(forms.size()));

  eprintf("Analyzing: ");
  clock_t now = clock();
  vector<tagged_lemma> lemmas;
  for (auto&& form : forms) {
    dictionary->analyze(form, morpho::NO_GUESSER, lemmas);
  }
  eprintf("done in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));

  return 0;
}
