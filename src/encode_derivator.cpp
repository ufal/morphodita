// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "derivator/derivator_dictionary_encoder.h"
#include "morpho/morpho.h"
#include "morpho/morpho_ids.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"from_tagger", options::value::none},
                       {"verbose", options::value::none},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] morpho_dictionary\n"
                    "Options: --from_tagger (use tagger instead of morpho)\n"
                    "         --verbose (dump all derivator graph edges)\n"
                    "         --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  // Switch stdout to binary mode.
  iostreams_init_binary_output();

  ifstream dictionary(argv[1]);
  if (!dictionary.is_open()) runtime_failure("Cannot open mode file '" << argv[1] << "'!");

  if (options.count("from_tagger"))
    cout.put(dictionary.get());

  derivator_dictionary_encoder::encode(cin, dictionary, options.count("verbose"), cout);

  if (options.count("from_tagger"))
    cout << dictionary.rdbuf();

  return 0;
}
