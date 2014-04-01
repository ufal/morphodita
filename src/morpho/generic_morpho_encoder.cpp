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

#include "generic_lemma_addinfo.h"
#include "generic_morpho_encoder.h"
#include "morpho_dictionary_encoder.h"
#include "morpho_prefix_guesser_encoder.h"
#include "morpho_statistical_guesser_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace morphodita {

void generic_morpho_encoder::encode(FILE* in_dictionary, int max_suffix_len, const tags& tags, FILE* in_statistical_guesser, FILE* out_morpho) {
  binary_encoder enc;

  enc.add_1B(tags.unknown_tag.size());
  enc.add_str(tags.unknown_tag);
  enc.add_1B(tags.number_tag.size());
  enc.add_str(tags.number_tag);
  enc.add_1B(tags.punctuation_tag.size());
  enc.add_str(tags.punctuation_tag);
  enc.add_1B(tags.symbol_tag.size());
  enc.add_str(tags.symbol_tag);

  eprintf("Encoding dictionary.\n");
  morpho_dictionary_encoder<generic_lemma_addinfo>::encode(in_dictionary, max_suffix_len, enc);

  // Load and encode statistical guesser if requested
  enc.add_1B(in_statistical_guesser != nullptr);
  if (in_statistical_guesser) {
    eprintf("Encoding statistical guesser.\n");
    morpho_statistical_guesser_encoder::encode(in_statistical_guesser, enc);
  }

  // done, save the dictionary
  eprintf("Compressing dictionary.\n");
  if (!compressor::save(out_morpho, enc)) runtime_errorf("Cannot compress and write dictionary to file!");
  eprintf("Dictionary saved.\n");
}

} // namespace morphodita
} // namespace ufal
