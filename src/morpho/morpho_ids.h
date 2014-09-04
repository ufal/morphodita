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
namespace morphodita {

class morpho_ids {
 public:
  enum morpho_id {
    CZECH = 0,
    ENGLISH_V1 = 1,
    GENERIC = 2,
    EXTERNAL = 3,
    ENGLISH_V2 = 4,
    ENGLISH_V3 = 5, ENGLISH = ENGLISH_V3,
    SLOVAK_PDT = 6,
  };

  static bool parse(const string& str, morpho_id& id) {
    if (str == "czech") return id = CZECH, true;
    if (str == "english") return id = ENGLISH, true;
    if (str == "external") return id = EXTERNAL, true;
    if (str == "generic") return id = GENERIC, true;
    if (str == "slovak_pdt") return id = SLOVAK_PDT, true;
    return false;
  }
};

typedef morpho_ids::morpho_id morpho_id;

} // namespace morphodita
} // namespace ufal
