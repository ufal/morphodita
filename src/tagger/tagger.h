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

#include <memory>

#include "common.h"
#include "morpho/morpho.h"

namespace ufal {
namespace morphodita {

class EXPORT_ATTRIBUTES tagger {
 public:
  virtual ~tagger() {}

  static tagger* load(FILE* f);
  static tagger* load(const char* fname);

  // Return morpho associated with the tagger. Do not delete the pointer, it is
  // owned by the tagger instance and deleted in the tagger destructor.
  virtual const morpho* get_morpho() const = 0;

  // Perform morphologic analysis and subsequent disambiguation.
  virtual void tag(const vector<string_piece>& forms, vector<tagged_lemma>& tags) const = 0;

  // Construct a new tokenizer instance appropriate for this tokenizer.
  // Is equal to get_morpho()->new_tokenizer.
  tokenizer* new_tokenizer() const;
};

} // namespace morphodita
} // namespace ufal
