// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <initializer_list>
#include <unordered_map>

#include "common.h"
#include "string_piece.h"

namespace ufal {
namespace morphodita {

class hyphenated_sequences_map {
 public:
  hyphenated_sequences_map(std::initializer_list<string> sequences);
  bool join(vector<string_piece>& forms, string& buffer) const;

 private:
  unsigned max_hyphens;
  unordered_map<string, string> sequences;
};

} // namespace morphodita
} // namespace ufal
