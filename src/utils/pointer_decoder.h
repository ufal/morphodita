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

#include <stdexcept>

#include "common.h"

namespace ufal {
namespace morphodita {

// Declarations
class pointer_decoder {
 public:
  inline pointer_decoder(const unsigned char*& data);
  inline unsigned next_1B();
  inline unsigned next_2B();
  inline unsigned next_4B();
  template <class T> inline const T* next(unsigned elements);

 private:
  const unsigned char*& data;
};


// Definitions
pointer_decoder::pointer_decoder(const unsigned char*& data) : data(data) {}

unsigned pointer_decoder::next_1B() {
  return *data++;
}

unsigned pointer_decoder::next_2B() {
  unsigned result = *(uint16_t*)data;
  data += sizeof(uint16_t);
  return result;
}

unsigned pointer_decoder::next_4B() {
  unsigned result = *(uint32_t*)data;
  data += sizeof(uint32_t);
  return result;
}

template <class T> const T* pointer_decoder::next(unsigned elements) {
  const T* result = (const T*) data;
  data += sizeof(T) * elements;
  return result;
}

} // namespace morphodita
} // namespace ufal
