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

#include "tagger/conllu_elementary_features.h"
#include "tagger/czech_elementary_features.h"
#include "tagger/feature_sequences.h"
#include "tagger/generic_elementary_features.h"
#include "tagger/perceptron_tagger_trainer.h"
#include "tagger/tagger_ids.h"
#include "tagger/tagger_trainer.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  tagger_id id;
  options::map options;
  if (argc == 1 || !tagger_ids::parse(argv[1], id))
    if (!options::parse({{"version", options::value::none},
                         {"help", options::value::none}}, argc, argv, options) ||
        options.count("help") ||
        (argc < 2 && !options.count("version")))
      runtime_failure("Usage: " << argv[0] << " [options] tagger_identifier [tagger_identifier_specific_options]\n"
                      "Options: --version\n"
                      "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  if (!tagger_ids::parse(argv[1], id)) runtime_failure("Cannot parse tagger_identifier '" << argv[1] << "'!");

  // Switch stdout to binary mode.
  iostreams_init_binary_output();

  switch (id) {
    case tagger_ids::CZECH2:
    case tagger_ids::CZECH2_3:
    case tagger_ids::CZECH3:
    case tagger_ids::GENERIC2:
    case tagger_ids::GENERIC2_3:
    case tagger_ids::GENERIC3:
    case tagger_ids::GENERIC4:
    case tagger_ids::CONLLU2:
    case tagger_ids::CONLLU2_3:
    case tagger_ids::CONLLU3:
      {
        if (argc < 7) runtime_failure("Usage: " << argv[0] << ' ' << argv[1] << " dict use_guesser features iterations prune_features [heldout_data [early_stopping]]");
        const char* dict_file = argv[2];
        bool use_guesser = parse_int(argv[3], "use_guesser");
        const char* features_file = argv[4];
        int iterations = parse_int(argv[5], "iterations");
        bool prune_features = parse_int(argv[6], "prune_features");
        const char* heldout_file = argc >= 8 ? argv[7] : nullptr;
        bool early_stopping = argc >= 9 ? parse_int(argv[8], "early_stopping") : false;

        // Open needed files
        ifstream dict(dict_file, ifstream::binary);
        if (!dict) runtime_failure("Cannot open dictionary file '" << dict_file << "'!");

        ifstream feature_templates(features_file);
        if (!feature_templates) runtime_failure("Cannot open feature template file '" << features_file << "'!");

        ifstream heldout;
        if (heldout_file && strlen(heldout_file)) {
          heldout.open(heldout_file);
          if (!heldout) runtime_failure("Cannot open heldout file '" << heldout_file << "'!");
        } else {
          heldout.setstate(ios::failbit);
        }

        // Encode the tagger_id and the tagger itself
        cout.put(id);
        switch (id) {
          case tagger_ids::CZECH2:
          case tagger_ids::CZECH2_3:
          case tagger_ids::CZECH3:
            tagger_trainer<perceptron_tagger_trainer<train_feature_sequences<czech_elementary_features>>>::train(tagger_ids::decoding_order(id), tagger_ids::window_size(id), iterations, dict, use_guesser, feature_templates, prune_features, cin, heldout, early_stopping, cout);
            break;
          case tagger_ids::GENERIC2:
          case tagger_ids::GENERIC2_3:
          case tagger_ids::GENERIC3:
          case tagger_ids::GENERIC4:
            tagger_trainer<perceptron_tagger_trainer<train_feature_sequences<generic_elementary_features>>>::train(tagger_ids::decoding_order(id), tagger_ids::window_size(id), iterations, dict, use_guesser, feature_templates, prune_features, cin, heldout, early_stopping, cout);
            break;
          case tagger_ids::CONLLU2:
          case tagger_ids::CONLLU2_3:
          case tagger_ids::CONLLU3:
            tagger_trainer<perceptron_tagger_trainer<train_feature_sequences<conllu_elementary_features>>>::train(tagger_ids::decoding_order(id), tagger_ids::window_size(id), iterations, dict, use_guesser, feature_templates, prune_features, cin, heldout, early_stopping, cout);
            break;
        }
        cerr << "Tagger saved." << endl;

        break;
      }
    default:
      runtime_failure("Unimplemented tagger_identifier '" << argv[1] << "'!");
  }

  return 0;
}
