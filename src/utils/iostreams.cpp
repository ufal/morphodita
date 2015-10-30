// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "iostreams.h"

namespace ufal {
namespace morphodita {

void iostreams_init() {
  iostream::sync_with_stdio(false);
}

void iostreams_init_binary_input() {
#ifdef _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
#endif
}

void iostreams_init_binary_output() {
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
#endif
}

istream& getpara(istream& is, string& para) {
  para.clear();

  for (string line; getline(is, line); ) {
    para.append(line);
    para.push_back('\n');

    if (line.empty()) break;
  }

  return is;
}

ostream& operator<<(ostream& os, xml_encoded data) {
  const char* to_print = data.text;

  while (data.length) {
    while (data.length && *data.text != '<' && *data.text != '>' && *data.text != '&' && (!data.encode_quot || *data.text != '"'))
      data.text++, data.length--;

    if (data.length) {
      if (to_print < data.text) os.write(to_print, data.text - to_print);
      os << (*data.text == '<' ? "&lt;" : *data.text == '>' ? "&gt;" : *data.text == '&' ? "&amp;" : "&quot;");
      data.text++, data.length--;
      to_print = data.text;
    }
  }

  if (to_print < data.text) os.write(to_print, data.text - to_print);

  return os;
}

} // namespace morphodita
} // namespace ufal
