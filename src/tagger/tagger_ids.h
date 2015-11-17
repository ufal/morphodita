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

class tagger_ids {
 public:
  enum tagger_id {
    CZECH2 = 0, CZECH3 = 1, CZECH2_3 = 6,
    /* 2 was used internally for ENGLISH3, but never released publicly */
    GENERIC2 = 3, GENERIC3 = 4, GENERIC4 = 5, GENERIC2_3 = 7,
  };

  static bool parse(const string& str, tagger_id& id) {
    if (str == "czech2") return id = CZECH2, true;
    if (str == "czech2_3") return id = CZECH2_3, true;
    if (str == "czech3") return id = CZECH3, true;
    if (str == "generic2") return id = GENERIC2, true;
    if (str == "generic2_3") return id = GENERIC2_3, true;
    if (str == "generic3") return id = GENERIC3, true;
    if (str == "generic4") return id = GENERIC4, true;
    return false;
  }
};

typedef tagger_ids::tagger_id tagger_id;

} // namespace morphodita
} // namespace ufal
