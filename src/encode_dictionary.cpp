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

#include <cstring>

#include "morpho/morpho_ids.h"
#include "morpho/czech_morpho_encoder.h"
#include "morpho/english_morpho_encoder.h"
#include "morpho/external_morpho_encoder.h"
#include "morpho/generic_morpho_encoder.h"
#include "utils/file_ptr.h"
#include "utils/parse_int.h"
#include "utils/parse_options.h"
#include "utils/set_binary_stdout.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  show_version_if_requested(argc, argv);

  if (argc <= 1) runtime_errorf("Usage: %s morpho_identifier [options]\n", argv[0]);

  morpho_id id;
  if (!morpho_ids::parse(argv[1], id)) runtime_errorf("Cannot parse morpho_identifier '%s'!\n", argv[1]);

  // Switch stdout to binary mode. Needed on Windows only.
  set_binary_stdout();

  switch (id) {
    case morpho_ids::CZECH:
      {
        if (argc < 3) runtime_errorf("Usage: %s czech max_suffix_len [prefix_guesser [statistical_guesser [tag_length]]]\n", argv[0]);
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");

        file_ptr prefix_guesser;
        if (argc > 3 && strlen(argv[3])) {
          prefix_guesser = fopen(argv[3], "r");
          if (!prefix_guesser) runtime_errorf("Cannot open prefix guesser file '%s'!", argv[3]);
        }

        file_ptr statistical_guesser;
        if (argc > 4 && strlen(argv[4])) {
          statistical_guesser = fopen(argv[4], "r");
          if (!statistical_guesser) runtime_errorf("Cannot open statistical guesser file '%s'!", argv[4]);
        }

        fputc(id, stdout);
        czech_morpho_encoder::encode(stdin, max_suffix_len, prefix_guesser, statistical_guesser, argc > 5 ? parse_int(argv[5], "tag_length") : 15, stdout);

        break;
      }
    case morpho_ids::ENGLISH:
      {
        if (argc < 4) runtime_errorf("Usage: %s english max_suffix_len english_guesser_file [english_negation_prefixes]\n", argv[0]);
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");

        file_ptr guesser = fopen(argv[3], "r");
        if (!guesser) runtime_errorf("Cannot open guesser file '%s'!", argv[3]);

        file_ptr negations;
        if (argc > 4 && strlen(argv[4])) {
          negations = fopen(argv[4], "r");
          if (!negations) runtime_errorf("Cannot open negations file '%s'!", argv[4]);
        }

        fputc(id, stdout);
        english_morpho_encoder::encode(stdin, max_suffix_len, guesser, negations, stdout);
        break;
      }
    case morpho_ids::EXTERNAL:
      {
        if (argc < 3) runtime_errorf("Usage: %s external unknown_tag\n", argv[0]);
        string unknown_tag = argv[2];

        fputc(id, stdout);
        external_morpho_encoder::encode(unknown_tag, stdout);
        break;
      }
    case morpho_ids::GENERIC:
      {
        if (argc < 7) runtime_errorf("Usage: %s generic max_suffix_len unknown_tag number_tag punctuation_tag symbol_tag [statistical_guesser]\n", argv[0]);
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");
        generic_morpho_encoder::tags tags;
        tags.unknown_tag = argv[3];
        tags.number_tag = argv[4];
        tags.punctuation_tag = argv[5];
        tags.symbol_tag = argv[6];
        file_ptr statistical_guesser;
        if (argc > 7 && strlen(argv[7])) {
          statistical_guesser = fopen(argv[7], "r");
          if (!statistical_guesser) runtime_errorf("Cannot open statistical guesser file '%s'!", argv[7]);
        }

        fputc(id, stdout);
        generic_morpho_encoder::encode(stdin, max_suffix_len, tags, statistical_guesser, stdout);
        break;
      }
    case morpho_ids::SLOVAK_PDT:
      {
        if (argc < 2) runtime_errorf("Usage: %s slovak_pdt max_suffix_len [statistical_guesser [tag_length]]\n", argv[0]);
        int max_suffix_len = parse_int(argv[2], "max_suffix_len");

        file_ptr statistical_guesser;
        if (argc > 3 && strlen(argv[3])) {
          statistical_guesser = fopen(argv[3], "r");
          if (!statistical_guesser) runtime_errorf("Cannot open statistical guesser file '%s'!", argv[3]);
        }

        fputc(id, stdout);
        czech_morpho_encoder::encode(stdin, max_suffix_len, nullptr, statistical_guesser, argc > 4 ? parse_int(argv[4], "tag_length") : 15, stdout);

        break;
      }
    default:
      runtime_errorf("Unimplemented morpho_identifier '%s'!\n", argv[1]);
  }

  return 0;
}
