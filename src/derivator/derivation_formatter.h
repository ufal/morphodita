// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "derivator.h"

namespace ufal {
namespace morphodita {

class derivation_formatter {
 public:
  virtual ~derivation_formatter() {}

  virtual void format_derivation(string& lemma) const = 0;

  static derivation_formatter* new_none_derivation_formatter();
  static derivation_formatter* new_root_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_path_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_tree_derivation_formatter(const derivator* derinet);
  static derivation_formatter* new_derivation_formatter(const string& name, const derivator* derinet);
};

} // namespace morphodita
} // namespace ufal
