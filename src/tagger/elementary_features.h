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
#include "tagger.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"
#include "utils/persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

// Declarations
enum elementary_feature_type { PER_FORM, PER_TAG, DYNAMIC };
enum elementary_feature_range { ONLY_CURRENT, ANY_OFFSET };

typedef uint32_t elementary_feature_value;
enum :elementary_feature_value { elementary_feature_unknown = 0, elementary_feature_empty = 1 };

struct elementary_feature_description {
  string name;
  elementary_feature_type type;
  elementary_feature_range range;
  int index;
  int map_index;
};

template<class Map>
class elementary_features {
 public:
  bool load(FILE* f);
  bool save(FILE* f);

  vector<Map> maps;
};

class persistent_elementary_feature_map : public persistent_unordered_map {
 public:
  persistent_elementary_feature_map() : persistent_unordered_map() {}
  persistent_elementary_feature_map(const persistent_unordered_map&& map) : persistent_unordered_map(map) {}

  elementary_feature_value value(const char* feature, int len) const {
    auto* it = at_typed<elementary_feature_value>(feature, len);
    return it ? *it : elementary_feature_unknown;
  }
};


// Definitions
template <class Map>
inline bool elementary_features<Map>::load(FILE* f) {
  binary_decoder data;
  if (!compressor::load(f, data)) return false;

  try {
    maps.resize(data.next_1B());
    for (auto&& map : maps)
      map.load(data);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

} // namespace morphodita
} // namespace ufal
