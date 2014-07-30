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
#include "elementary_features.h"
#include "utils/binary_encoder.h"

namespace ufal {
namespace morphodita {

template <class Map>
inline bool elementary_features<Map>::save(FILE* f) {
  binary_encoder enc;

  enc.add_1B(maps.size());
  for (auto&& map : maps)
    map.save(enc);

  return compressor::save(f, enc);
}

} // namespace morphodita
} // namespace ufal
