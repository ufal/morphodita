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

#pragma once

#include "common.h"

namespace ufal {
namespace utils {

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
  for (auto chr : str)
    data.push_back(chr);
}

void binary_encoder::add_data(const vector<unsigned char>& str) {
  for (auto chr : str)
    data.push_back(chr);
}

void binary_encoder::add_data(const unsigned char* begin, const unsigned char* end) {
  data.insert(data.end(), begin, end);
}

} // namespace utils
} // namespace ufal
