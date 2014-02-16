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

#include "identity_tagset_converter.h"
#include "pdt_to_conll2009_tagset_converter.h"

namespace ufal {
namespace morphodita {

tagset_converter* tagset_converter::new_identity_converter() {
  return new identity_tagset_converter();
}

tagset_converter* tagset_converter::new_pdt_to_conll2009_converter() {
  return new pdt_to_conll2009_tagset_converter();
}

tagset_converter* new_tagset_converter(const string& name) {
  if (name == "pdt_to_conll2009") return tagset_converter::new_pdt_to_conll2009_converter();
  return nullptr;
}

} // namespace morphodita
} // namespace ufal
