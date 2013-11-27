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

#include "input.h"

namespace ufal {
namespace utils {

bool getline(FILE *f, string& line) {
  if (feof(f)) return (line.clear(), false);

  int offset = 0;
  bool eof;
  line.resize(256);
  while (line.back() = '\1', eof = !fgets((char *) line.data() + offset, line.size() - offset, f), line.back() == '\0') {
    offset = line.size() - 1;
    if (line[offset-1] == '\n') break;
    line.resize(2 * line.size());
  }
  if (eof && !offset) return (line.clear(), false);

  int len = offset + strlen(line.data() + offset);
  if (len && line[len - 1] == '\n') len--;
  line.resize(len);

  return true;
}

void split(const string& text, char sep, vector<string>& tokens) {
  tokens.clear();
  if (text.empty()) return;

  string::size_type index = 0;
  for (string::size_type next; (next = text.find(sep, index)) != string::npos; index = next + 1)
    tokens.emplace_back(text, index, next - index);

  tokens.emplace_back(text, index);
}

} // namespace utils
} // namespace ufal
