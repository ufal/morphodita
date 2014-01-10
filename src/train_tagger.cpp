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

#include "common.h"
#include "tagger/czech_elementary_features.h"
#include "tagger/feature_sequences.h"
#include "tagger/perceptron_tagger_trainer.h"
#include "tagger/tagger_ids.h"
#include "tagger/tagger_trainer.h"
#include "utils/file_ptr.h"
#include "utils/parse_int.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc < 2) runtime_errorf("Usage: %s tagger_identifier [options]", argv[0]);

  tagger_id id;
  if (!tagger_ids::parse(argv[1], id)) runtime_errorf("Cannot parse tagger_identifier '%s'!\n", argv[1]);

  switch (id) {
    case tagger_ids::CZECH2:
    case tagger_ids::CZECH3:
      {
        if (argc < 6) runtime_errorf("Usage: %s %s dict use_guesser features iterations [heldout_data]", argv[0], argv[1]);
        const char* dict_file = argv[2];
        bool use_guesser = parse_int(argv[3], "use_guesser");
        const char* features_file = argv[4];
        int iterations = parse_int(argv[5], "iterations");
        const char* heldout_file = argc == 6 ? nullptr : argv[6];

        // Open needed files
        file_ptr dict = fopen(dict_file, "rb");
        if (!dict) runtime_errorf("Cannot open dictionary file '%s'!", dict_file);

        file_ptr feature_templates = fopen(features_file, "r");
        if (!feature_templates) runtime_errorf("Cannot open feature template file '%s'!", features_file);

        file_ptr heldout;
        if (heldout_file) {
          heldout = fopen(heldout_file, "r");
          if (!heldout) runtime_errorf("Cannot open heldout file '%s'!", heldout_file);
        }

        // Encode the tagger_id and the tagger itself
        fputc(id, stdout);
        switch (id) {
          case tagger_ids::CZECH2:
            tagger_trainer<perceptron_tagger_trainer<train_feature_sequences<czech_elementary_features>, 2>>::train(iterations, dict, use_guesser, feature_templates, stdin, heldout, stdout);
            break;
          case tagger_ids::CZECH3:
            tagger_trainer<perceptron_tagger_trainer<train_feature_sequences<czech_elementary_features>, 3>>::train(iterations, dict, use_guesser, feature_templates, stdin, heldout, stdout);
            break;
        }
        eprintf("Tagger saved.\n");

        break;
      }
    default:
      runtime_errorf("Unimplemented tagger_identifier '%s'!\n", argv[1]);
  }

  return 0;
}
