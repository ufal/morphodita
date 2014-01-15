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

#include <map>
#include <unordered_map>

#include "binary_decoder.h"
#include "binary_encoder.h"
#include "common.h"
#include "small_stringops.h"
#include "pointer_decoder.h"

namespace ufal {
namespace morphodita {

// Declarations
class persistent_unordered_map {
 public:
  // Accessing function
  template <class EntrySize>
  inline const unsigned char* at(const char* str, int len, EntrySize entry_size) const;

  template <class T>
  inline const T* at_typed(const char* str, int len) const;

  template <class EntryProcess>
  inline void iter(const char* str, int len, EntryProcess entry_process) const;

  template <class EntryProcess>
  inline void iter_all(EntryProcess entry_process) const;

  // Two helper functions accessing some internals
  inline int max_length() const;
  inline const unsigned char* data_start(int len) const;

  // Creation functions
  persistent_unordered_map();
  template <class Entry, class EntryEncode>
  persistent_unordered_map(const unordered_map<string, Entry>& map, double load_factor, EntryEncode entry_encode);
  template <class Entry, class EntryEncode>
  persistent_unordered_map(const unordered_map<string, Entry>& map, double load_factor, bool add_prefixes, bool add_suffixes, EntryEncode entry_encode);

  // Manual creation functions
  inline void resize(unsigned elems);
  inline void add(const char* str, int str_len, int data_len);
  inline void done_adding();
  inline unsigned char* fill(const char* str, int str_len, int data_len);
  inline void done_filling();

  // Serialization
  void load(binary_decoder& data);
  void save(binary_encoder& enc);

 private:
  struct fnv_hash;
  vector<fnv_hash> hashes;

  template <class Entry, class EntryEncode>
  void construct(const map<string, Entry>& map, double load_factor, EntryEncode entry_encode);
};


// Definitions
struct persistent_unordered_map::fnv_hash {
  fnv_hash(unsigned num) {
    mask = 1;
    while (mask < num)
      mask <<= 1;
    hash.resize(mask + 1);
    mask--;
  }
  fnv_hash(binary_decoder& data);

  inline uint32_t index(const char* data, int len) const {
    if (len <= 0) return 0;
    if (len == 1) return *(const uint8_t*)data;
    if (len == 2) return *(const uint16_t*)data;

    uint32_t hash = 2166136261U;
    while (len--)
      hash = (hash ^ unsigned(*data++)) * 16777619U;
    return hash & mask;
  }

  void save(binary_encoder& enc);

  unsigned mask;
  vector<uint32_t> hash;
  vector<unsigned char> data;
};

template <class EntrySize>
const unsigned char* persistent_unordered_map::at(const char* str, int len, EntrySize entry_size) const {
  if (unsigned(len) >= hashes.size()) return nullptr;

  unsigned index = hashes[len].index(str, len);
  const unsigned char* data = hashes[len].data.data() + hashes[len].hash[index];
  const unsigned char* end = hashes[len].data.data() + hashes[len].hash[index+1];

  if (len <= 2)
    return data != end ? data + len : nullptr;

  while (data < end) {
    if (small_memeq(str, data, len)) return data + len;
    data += len;
    pointer_decoder decoder(data);
    entry_size(decoder);
  }

  return nullptr;
}

template <class T>
const T* persistent_unordered_map::at_typed(const char* str, int len) const {
  if (unsigned(len) >= hashes.size()) return nullptr;

  unsigned index = hashes[len].index(str, len);
  const unsigned char* data = hashes[len].data.data() + hashes[len].hash[index];
  const unsigned char* end = hashes[len].data.data() + hashes[len].hash[index+1];

  if (len <= 2)
    return data != end ? (const T*)(data + len) : nullptr;

  while (data < end) {
    if (small_memeq(str, data, len)) return (const T*)(data + len);
    data += len + sizeof(T);
  }

  return nullptr;
}

template <class EntryProcess>
void persistent_unordered_map::iter(const char* str, int len, EntryProcess entry_process) const {
  if (unsigned(len) >= hashes.size()) return;

  unsigned index = hashes[len].index(str, len);
  const unsigned char* data = hashes[len].data.data() + hashes[len].hash[index];
  const unsigned char* end = hashes[len].data.data() + hashes[len].hash[index+1];

  while (data < end) {
    auto start = (const char*) data;
    data += len;
    pointer_decoder decoder(data);
    entry_process(start, decoder);
  }
}

template <class EntryProcess>
void persistent_unordered_map::iter_all(EntryProcess entry_process) const {
  for (unsigned len = 0; len < hashes.size(); len++) {
    const unsigned char* data = hashes[len].data.data();
    const unsigned char* end = data + hashes[len].data.size();

    while (data < end) {
      auto start = (const char*) data;
      data += len;
      pointer_decoder decoder(data);
      entry_process(start, len, decoder);
    }
  }
}

int persistent_unordered_map::max_length() const {
  return hashes.size();
}

const unsigned char* persistent_unordered_map::data_start(int len) const {
  return unsigned(len) < hashes.size() ? hashes[len].data.data() : nullptr;
}

template <class Entry, class EntryEncode>
persistent_unordered_map::persistent_unordered_map(const unordered_map<string, Entry>& map, double load_factor, EntryEncode entry_encode) {
  construct(std::map<string, Entry>(map.begin(), map.end()), load_factor, entry_encode);
}

template <class Entry, class EntryEncode>
persistent_unordered_map::persistent_unordered_map(const unordered_map<string, Entry>& map, double load_factor, bool add_prefixes, bool add_suffixes, EntryEncode entry_encode) {
  // Copy data, possibly including prefixes and suffixes
  std::map<string, Entry> enlarged_map(map.begin(), map.end());

  for (auto& entry : map) {
    const string& key = entry.first;

    if (!key.empty() && add_prefixes)
      for (unsigned i = key.size() - 1; i; i--)
        enlarged_map[key.substr(0, i)];

    if (!key.empty() && add_suffixes)
      for (unsigned i = 1; i < key.size(); i++)
        enlarged_map[key.substr(i)];
  }

  construct(enlarged_map, load_factor, entry_encode);
}

void persistent_unordered_map::resize(unsigned elems) {
  if (hashes.size() == 0) hashes.emplace_back(1);
  else if (hashes.size() == 1) hashes.emplace_back(1<<8);
  else if (hashes.size() == 2) hashes.emplace_back(1<<16);
  else hashes.emplace_back(elems);
}

void persistent_unordered_map::add(const char* str, int str_len, int data_len) {
  if (unsigned(str_len) < hashes.size())
    hashes[str_len].hash[hashes[str_len].index(str, str_len)] += str_len + data_len;
}

void persistent_unordered_map::done_adding() {
  for (auto& hash : hashes) {
    int total = 0;
    for (auto& len : hash.hash) total += len, len = total - len;
    hash.data.resize(total);
  }
}

unsigned char* persistent_unordered_map::fill(const char* str, int str_len, int data_len) {
  if (unsigned(str_len) < hashes.size()) {
    unsigned index = hashes[str_len].index(str, str_len);
    unsigned offset = hashes[str_len].hash[index];
    small_memcpy(hashes[str_len].data.data() + offset, str, str_len);
    hashes[str_len].hash[index] += str_len + data_len;
    return hashes[str_len].data.data() + offset + str_len;
  }
  return nullptr;
}

void persistent_unordered_map::done_filling() {
  for (auto& hash : hashes)
    for (int i = hash.hash.size() - 1; i >= 0; i--)
      hash.hash[i] = i > 0 ? hash.hash[i-1] : 0;
}

// We could (and used to) use unordered_map as input parameter.
// Nevertheless, as order is unspecified, the resulting persistent_unordered_map
// has different collision chains when generated on 32-bit and 64-bit machines.
// To guarantee uniform binary representation, we use map instead.
template <class Entry, class EntryEncode>
void persistent_unordered_map::construct(const map<string, Entry>& map, double load_factor, EntryEncode entry_encode) {
  // 1) Count number of elements for each size
  vector<int> sizes;
  for (auto elem : map) {
    unsigned len = elem.first.size();
    if (len >= sizes.size()) sizes.resize(len + 1);
    sizes[len]++;
  }
  for (auto& size : sizes)
    resize(unsigned(load_factor * size));

  // 2) Add sizes of element data
  for (auto elem : map) {
    binary_encoder enc;
    entry_encode(enc, elem.second);
    add(elem.first.c_str(), elem.first.size(), enc.data.size());
  }
  done_adding();

  // 3) Fill in element data
  for (auto elem : map) {
    binary_encoder enc;
    entry_encode(enc, elem.second);
    small_memcpy(fill(elem.first.c_str(), elem.first.size(), enc.data.size()), enc.data.data(), enc.data.size());
  }
  done_filling();
}

} // namespace morphodita
} // namespace ufal
