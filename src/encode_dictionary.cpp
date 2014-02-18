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
#include "utils/file_ptr.h"
#include "utils/parse_int.h"
#include "utils/set_binary_stdout.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s morpho_identifier [options]\n", argv[0]);

  morpho_id id;
  if (!morpho_ids::parse(argv[1], id)) runtime_errorf("Cannot parse morpho_identifier '%s'!\n", argv[1]);

  // Switch stdout to binary mode. Needed on Windows only.
  set_binary_stdout();

  switch (id) {
    case morpho_ids::CZECH:
      {
        // options: prefix_guesser_file statistical_guesser_file max_tag_length
        file_ptr prefix_guesser, statistical_guesser;

        if (argc > 2 && strlen(argv[2])) {
          prefix_guesser = fopen(argv[2], "r");
          if (!prefix_guesser) runtime_errorf("Cannot open prefix guesser file '%s'!", argv[2]);
        }

        if (argc > 3 && strlen(argv[3])) {
          statistical_guesser = fopen(argv[3], "r");
          if (!statistical_guesser) runtime_errorf("Cannot open statistical guesser file '%s'!", argv[3]);
        }

        fputc(id, stdout);
        czech_morpho_encoder::encode(stdin, prefix_guesser, statistical_guesser, argc > 4 ? parse_int(argv[4], "tag_length") : 15, stdout);

        break;
      }
    case morpho_ids::ENGLISH:
      {
        fputc(id, stdout);
        english_morpho_encoder::encode(stdin, stdout);
        break;
      }
    default:
      runtime_errorf("Unimplemented morpho_identifier '%s'!\n", argv[1]);
  }

  return 0;
}
