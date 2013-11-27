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
#include "czech_tokenizer.h"

namespace ufal {
namespace utils {

template <class Form>
tokenizer<Form>* tokenizer<Form>::create(tokenizer_id id, bool split_hyphenated_words) {
  switch (id) {
    case tokenizer_ids::CZECH: return new czech_tokenizer<Form>(split_hyphenated_words);
  }
  return nullptr;
}

} // namespace utils
} // namespace ufal
