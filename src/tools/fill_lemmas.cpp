// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "morpho/morpho.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/split.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file <data\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading dictionary: ";
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_failure("Cannot load dictionary " << argv[1] << '!');
  cerr << "done" << endl;

  cerr << "Processing data: ";

  int forms = 0, matches = 0;
  vector<tagged_lemma> lemmas;

  string line;
  vector<string> tokens;
  while (getline(cin, line)) {
    if (line.empty()) {
      cout << endl;
      continue;
    }

    split(line, '\t', tokens);
    if (tokens.size() != 2) runtime_failure("The line " << line << " does not contain three columns!");

    // Analyse
    dictionary->analyze(tokens[0], morpho::GUESSER, lemmas);

    forms++;

    int match = -1;
    for (unsigned i = 0; i < lemmas.size(); i++)
      if (lemmas[i].tag == tokens[1] && lemmas[i].lemma == tokens[0]) {
        if (match == -1) match = i;
        else if (lemmas[i].lemma != lemmas[match].lemma) cerr << "Multiple matching samelemmaasform-tags with different lemmas for form '" << tokens[0] << "'." << endl;
      }

    if (match == -1)
      for (unsigned i = 0; i < lemmas.size(); i++)
        if (lemmas[i].tag == tokens[1]) {
          if (match == -1) match = i;
          else if (lemmas[i].lemma != lemmas[match].lemma) cerr << "Multiple matching tags with different lemmas for form '" << tokens[0] << "'." << endl;
        }

    matches += match != -1;
    if (match == -1) cerr << "Did not get tag '" << tokens[1] << "' for form '" << tokens[0] << "'." << endl;

    cout << tokens[0] << '\t' << (match != -1 ? lemmas[match].lemma : tokens[0]) << '\t' << tokens[1] << '\n';
  }
  cerr << "done" << endl;
  cerr << "Forms: " << forms << ", matched: " << fixed << setprecision(3) << 100 * matches / double(forms) << "%." << endl;

  return 0;
}
