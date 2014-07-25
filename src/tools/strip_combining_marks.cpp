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

#include "unilib/utf8.h"
#include "unilib/unistrip.h"
#include "utils/input.h"
#include "utils/parse_int.h"
#include "utils/parse_options.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  show_version_if_requested(argc, argv);

  bool only_one_column = argc > 1;
  int column = argc > 1 ? parse_int(argv[1], "column_index") : -1;

  string input, output;
  vector<string> tokens;
  while (getline(stdin, input)) {
    if (!utf8::valid(input)) runtime_errorf("Input line is not in UTF-8 encoding: '%s'", input.c_str());

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
