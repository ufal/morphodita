// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>
#include <fstream>

#include "morpho/morpho_statistical_guesser_trainer.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 3 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] suffix_len rules_per_suffix\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  // Switch stdout to binary mode.
  iostreams_init_binary_output();

  int suffix_len = parse_int(argv[1], "suffix_len");
  int rules_per_suffix = parse_int(argv[2], "rules_per_suffix");

  cerr << "Training statistical guesser." << endl;
  morpho_statistical_guesser_trainer::train(cin, suffix_len, rules_per_suffix, cout);
  cerr << "Statistical guesser saved." << endl;

  return 0;
}
