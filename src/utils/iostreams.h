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

// Initialize iostream and turn off stdio synchronization.
void iostreams_init();

// Use binary mode on cin.
void iostreams_init_binary_input();

// Use binary mode on cout.
void iostreams_init_binary_output();

// Read paragraph until EOF or end line. All encountered \n are stored.
istream& getpara(istream& is, string& para);

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

} // namespace morphodita
} // namespace ufal
