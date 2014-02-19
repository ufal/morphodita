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

class tagger_ids {
 public:
  enum tagger_id { CZECH2 = 0, CZECH3 = 1, ENGLISH3 = 2 };

  static bool parse(const string& str, tagger_id& id) {
    if (str == "czech2") return id = CZECH2, true;
    if (str == "czech3") return id = CZECH3, true;
    if (str == "english3") return id = ENGLISH3, true;
    return false;
  }
};

typedef tagger_ids::tagger_id tagger_id;

} // namespace morphodita
} // namespace ufal
