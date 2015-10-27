// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "input.h"

namespace ufal {
namespace morphodita {

bool getline(FILE *f, string& line) {
  if (feof(f)) return (line.clear(), false);

  size_t offset = 0;
  bool eof;
  line.resize(128);
  while (line.back() = '\1', eof = !fgets((char *) line.data() + offset, line.size() - offset, f), !eof && line.back() == '\0') {
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

bool getpara(FILE *f, string& para) {
  if (feof(f)) return (para.clear(), false);

  size_t offset = 0, last_offset = offset;
  para.resize(256);

  // Read lines until EOF or until empty line is found
  do {
    last_offset = offset;

    // Read a line
    bool eof;
    if (para.size() - 1 <= offset) para.resize(2 * para.size());
    while (para.back() = '\1', eof = !fgets((char *) para.data() + offset, para.size() - offset, f), !eof && para.back() == '\0') {
      offset = para.size() - 1;
      if (para[offset-1] == '\n') break;
      para.resize(2 * para.size());
    }

    // EOF found
    if (eof) break;

    offset += strlen(para.data() + offset);
  } while (offset > last_offset + 1);

  para.resize(offset);
  return !para.empty();
}

void split(const string& text, char sep, vector<string>& tokens) {
  tokens.clear();
  if (text.empty()) return;

  string::size_type index = 0;
  for (string::size_type next; (next = text.find(sep, index)) != string::npos; index = next + 1)
    tokens.emplace_back(text, index, next - index);

  tokens.emplace_back(text, index);
}

} // namespace morphodita
} // namespace ufal
