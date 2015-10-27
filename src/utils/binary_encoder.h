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

// Declaration
class binary_encoder {
 public:
  inline binary_encoder();

  inline void add_1B(unsigned val);
  inline void add_2B(unsigned val);
  inline void add_4B(unsigned val);
  inline void add_double(double val);
  inline void add_str(const char* str);
  inline void add_str(const string& str);
  inline void add_data(const vector<unsigned char>& str);
  inline void add_data(const unsigned char* begin, const unsigned char* end);

  vector<unsigned char> data;
};


// Definition
binary_encoder::binary_encoder() {
  data.reserve(16);
}

void binary_encoder::add_1B(unsigned val) {
  if (uint8_t(val) != val) runtime_errorf("Should encode value %u in one byte!", val);
  data.push_back(val);
}

void binary_encoder::add_2B(unsigned val) {
  if (uint16_t(val) != val) runtime_errorf("Should encode value %u in one byte!", val);
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(uint16_t));
}

void binary_encoder::add_4B(unsigned val) {
  if (uint32_t(val) != val) runtime_errorf("Should encode value %u in one byte!", val);
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(uint32_t));
}

void binary_encoder::add_double(double val) {
  data.insert(data.end(), (unsigned char*) &val, ((unsigned char*) &val) + sizeof(double));
}


void binary_encoder::add_str(const char* str) {
  while (*str)
    data.push_back(*str++);
}

void binary_encoder::add_str(const string& str) {
  for (auto&& chr : str)
    data.push_back(chr);
}

void binary_encoder::add_data(const vector<unsigned char>& str) {
  for (auto&& chr : str)
    data.push_back(chr);
}

void binary_encoder::add_data(const unsigned char* begin, const unsigned char* end) {
  data.insert(data.end(), begin, end);
}

} // namespace morphodita
} // namespace ufal
