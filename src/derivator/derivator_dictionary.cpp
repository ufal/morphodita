// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "derivator_dictionary.h"

namespace ufal {
namespace morphodita {

void derivator_dictionary::parent(string_piece lemma, derivated_lemma& parent) const {
}

void derivator_dictionary::children(string_piece lemma, vector<derivated_lemma>& children) const {
}

bool derivator_dictionary::load(istream& is) {
  return nullptr;
}

} // namespace morphodita
} // namespace ufal
