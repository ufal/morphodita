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

#include <cstring>

#include "czech_tokenizer.h"
#include "english_tokenizer.h"
#include "generic_tokenizer.h"
#include "vertical_tokenizer.h"

namespace ufal {
namespace morphodita {

tokenizer* tokenizer::new_vertical_tokenizer() {
  return new vertical_tokenizer();
}

tokenizer* tokenizer::new_czech_tokenizer() {
  return new czech_tokenizer(czech_tokenizer::CZECH_GENERIC);
}

tokenizer* tokenizer::new_english_tokenizer() {
  return new english_tokenizer(english_tokenizer::LATEST);
}

tokenizer* tokenizer::new_generic_tokenizer() {
  return new generic_tokenizer();
}

} // namespace morphodita
} // namespace ufal


