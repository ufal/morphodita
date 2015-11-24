// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace ufal {
namespace morphodita {

//
// Declarations
//

// Print xml content while encoding <>& and optionally " using XML entities.
class xml_encoded {
 public:
  xml_encoded(const string& str, bool encode_quot = false) : text(str.c_str()), length(str.size()), encode_quot(encode_quot) {}
  xml_encoded(const char* str, size_t length, bool encode_quot = false) : text(str), length(length), encode_quot(encode_quot) {}

  friend ostream& operator<<(ostream& os, xml_encoded data);
 private:
  const char* text;
  size_t length;
  bool encode_quot;
};

inline ostream& operator<<(ostream& os, xml_encoded data);

//
// Definitions
//

inline ostream& operator<<(ostream& os, xml_encoded data) {
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
