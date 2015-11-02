// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "unilib/utf8.h"
#include "unilib/unistrip.h"
#include "utils/iostreams.h"
#include "utils/parse_int.h"
#include "utils/parse_options.h"
#include "utils/split.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  show_version_if_requested(argc, argv);

  bool only_one_column = argc > 1;
  int column = argc > 1 ? parse_int(argv[1], "column_index") : -1;

  string input, output;
  vector<string> tokens;
  while (getline(cin, input)) {
    if (!utf8::valid(input)) runtime_failure("Input line is not in UTF-8 encoding: '" << input << '\'');

    output.clear();
    if (only_one_column) {
      split(input, '\t', tokens);
      for (unsigned i = 0; i < tokens.size(); i++) {
        if (i) output.push_back('\t');
        if (column == int(i) + 1) {
          for (auto&& chr : utf8::decoder(tokens[i]))
            if (!unistrip::is_combining_mark(chr))
              utf8::append(output, unistrip::strip_combining_marks(chr));
        } else {
          output.append(tokens[i]);
        }
      }
    } else {
      for (auto&& chr : utf8::decoder(input))
        if (!unistrip::is_combining_mark(chr))
          utf8::append(output, unistrip::strip_combining_marks(chr));
    }
    puts(output.c_str());
  }

  return 0;
}
