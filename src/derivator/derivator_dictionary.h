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
#include "morpho/morpho.h"

namespace ufal {
namespace morphodita {

class derivator_dictionary : public derivator {
 public:
  virtual void parent(string_piece lemma, derivated_lemma& parent) const override;
  virtual void children(string_piece lemma, vector<derivated_lemma>& children) const override;

  bool load(istream& is);

 private:
  friend class morpho;
  const morpho* dictionary;
};

} // namespace morphodita
} // namespace ufal
