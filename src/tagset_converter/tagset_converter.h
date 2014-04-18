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
#include "morpho/morpho.h"

namespace ufal {
namespace morphodita {

class EXPORT_ATTRIBUTES tagset_converter {
 public:
  virtual ~tagset_converter() {}

  // Convert a tag-lemma pair to a different tag set.
  virtual void convert(tagged_lemma& tagged_lemma) const = 0;
  // Convert a result of analysis to a different tag set. Apart from calling
  // convert, any repeated entry is removed.
  virtual void convert_analyzed(vector<tagged_lemma>& tagged_lemmas) const = 0;
  // Convert a result of generation to a different tag set. Apart from calling
  // convert, any repeated entry is removed.
  virtual void convert_generated(vector<tagged_lemma_forms>& forms) const = 0;

  // Static factory methods
  static tagset_converter* new_identity_converter();

  static tagset_converter* new_pdt_to_conll2009_converter();
  static tagset_converter* new_strip_lemma_comment_converter(const morpho& dictionary);
  static tagset_converter* new_strip_lemma_id_converter(const morpho& dictionary);
};

// Helper method for creating tagset_converter from instance name.
tagset_converter* new_tagset_converter(const string& name, const morpho& dictionary);

// Helper methods making sure remapped results are unique.
void tagset_converter_unique_analyzed(vector<tagged_lemma>& tagged_lemmas);
void tagset_converter_unique_generated(vector<tagged_lemma_forms>& forms);

} // namespace morphodita
} // namespace ufal
