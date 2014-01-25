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

#include "persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

persistent_unordered_map::persistent_unordered_map() {}

void persistent_unordered_map::load(binary_decoder& data) {
  unsigned sizes = data.next_1B();

  hashes.clear();
  for (unsigned i = 0; i < sizes; i++)
    hashes.emplace_back(data);
}

void persistent_unordered_map::save(binary_encoder& enc) {
  enc.add_1B(hashes.size());

  for (auto&& hash : hashes)
    hash.save(enc);
}

persistent_unordered_map::fnv_hash::fnv_hash(binary_decoder& data) {
  uint32_t size = data.next_4B();
  mask = size - 2;
  hash.resize(size);
  memcpy(hash.data(), data.next<uint32_t>(size), size * sizeof(uint32_t));

  size = data.next_4B();
  this->data.resize(size);
  memcpy(this->data.data(), data.next<char>(size), size);
}

void persistent_unordered_map::fnv_hash::save(binary_encoder& enc) {
  enc.add_4B(hash.size());
  enc.add_data((const unsigned char*)hash.data(), (const unsigned char*)(hash.data() + hash.size()));

  enc.add_4B(data.size());
  enc.add_data(data.data(), data.data() + data.size());
}

} // namespace morphodita
} // namespace ufal
