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

#include "morpho/morpho_ids.h"
#include "morpho/czech_morpho_encoder.h"
#include "morpho/english_morpho_encoder.h"
#include "morpho/external_morpho_encoder.h"
#include "morpho/generic_morpho_encoder.h"
#include "utils/iostreams.h"
#include "utils/parse_int.h"
#include "utils/parse_options.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  show_version_if_requested(argc, argv);

  if (argc <= 1) runtime_failure("Usage: " << argv[0] << " morpho_identifier [options]\n");

  morpho_id id;
  if (!morpho_ids::parse(argv[1], id)) runtime_failure("Cannot parse morpho_identifier '" << argv[1] << "'!\n");

  // Switch stdout to binary mode.
  iostreams_init_binary_output();

  switch (id) {
    case morpho_ids::CZECH:
      {
        if (argc < 3) runtime_failure("Usage: " << argv[0] << " czech max_suffix_len [prefix_guesser [statistical_guesser [tag_length]]]\n");
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");

        ifstream prefix_guesser;
        if (argc > 3 && strlen(argv[3])) {
          prefix_guesser.open(argv[3]);
          if (!prefix_guesser) runtime_failure("Cannot open prefix guesser file '" << argv[3] << "'!");
        }

        ifstream statistical_guesser;
        if (argc > 4 && strlen(argv[4])) {
          statistical_guesser.open(argv[4]);
          if (!statistical_guesser) runtime_failure("Cannot open statistical guesser file '" << argv[4] << "'!");
        }

        cout.put(id);
        czech_morpho_encoder::encode(cin, max_suffix_len, prefix_guesser, statistical_guesser, argc > 5 ? parse_int(argv[5], "tag_length") : 15, cout);

        break;
      }
    case morpho_ids::ENGLISH:
      {
        if (argc < 4) runtime_failure("Usage: " << argv[0] << " english max_suffix_len english_guesser_file [english_negation_prefixes]\n");
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");

        ifstream guesser(argv[3]);
        if (!guesser) runtime_failure("Cannot open guesser file '" << argv[3] << "'!");

        ifstream negations;
        if (argc > 4 && strlen(argv[4])) {
          negations.open(argv[4]);
          if (!negations) runtime_failure("Cannot open negations file '" << argv[4] << "'!");
        }

        cout.put(id);
        english_morpho_encoder::encode(cin, max_suffix_len, guesser, negations, cout);
        break;
      }
    case morpho_ids::EXTERNAL:
      {
        if (argc < 3) runtime_failure("Usage: " << argv[0] << " external unknown_tag\n");
        string unknown_tag = argv[2];

        cout.put(id);
        external_morpho_encoder::encode(unknown_tag, cout);
        break;
      }
    case morpho_ids::GENERIC:
      {
        if (argc < 7) runtime_failure("Usage: " << argv[0] << " generic max_suffix_len unknown_tag number_tag punctuation_tag symbol_tag [statistical_guesser]\n");
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");
        generic_morpho_encoder::tags tags;
        tags.unknown_tag = argv[3];
        tags.number_tag = argv[4];
        tags.punctuation_tag = argv[5];
        tags.symbol_tag = argv[6];
        ifstream statistical_guesser;
        if (argc > 7 && strlen(argv[7])) {
          statistical_guesser.open(argv[7]);
          if (!statistical_guesser) runtime_failure("Cannot open statistical guesser file '" << argv[7] << "'!");
        }

        cout.put(id);
        generic_morpho_encoder::encode(cin, max_suffix_len, tags, statistical_guesser, cout);
        break;
      }
    case morpho_ids::SLOVAK_PDT:
      {
        if (argc < 2) runtime_failure("Usage: " << argv[0] << " slovak_pdt max_suffix_len [statistical_guesser [tag_length]]\n");
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");

        ifstream prefix_guesser;

        ifstream statistical_guesser;
        if (argc > 3 && strlen(argv[3])) {
          statistical_guesser.open(argv[3]);
          if (!statistical_guesser) runtime_failure("Cannot open statistical guesser file '" << argv[3] << "'!");
        }

        cout.put(id);
        czech_morpho_encoder::encode(cin, max_suffix_len, prefix_guesser, statistical_guesser, argc > 4 ? parse_int(argv[4], "tag_length") : 15, cout);

        break;
      }
    default:
      runtime_failure("Unimplemented morpho_identifier '" << argv[1] << "'!\n");
  }

  return 0;
}
