// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>

#include "morpho/morpho.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "utils/split.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc <= 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file forms_to_analyze <raw_dict_file\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading dictionary: ";
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_failure("Cannot load dictionary " << argv[1] << '!');
  cerr << "done" << endl;

  cerr << "Loading forms to analyze: ";
  vector<string> forms;
  string line;
  vector<string> tokens;
  for (int i = parse_int(argv[2], "forms_to_analyze"); i > 0; i--) {
    if (!getline(cin, line)) break;
    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_failure("Line '" << line << "' of the raw morpho dictionary does not have three columns!");
    forms.emplace_back(tokens[2]);
  }
  cerr << "done, " << forms.size() << " forms read." << endl;

  cerr << "Analyzing: ";
  clock_t now = clock();
  vector<tagged_lemma> lemmas;
  for (auto&& form : forms) {
    dictionary->analyze(form, morpho::NO_GUESSER, lemmas);
  }
  cerr << "done, in " << fixed << setprecision(3) << (clock() - now) / double(CLOCKS_PER_SEC) << " seconds." << endl;

  return 0;
}
