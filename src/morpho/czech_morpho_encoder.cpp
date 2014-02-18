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

#include "czech_lemma_addinfo.h"
#include "czech_morpho_encoder.h"
#include "morpho_dictionary_encoder.h"
#include "morpho_prefix_guesser_encoder.h"
#include "morpho_statistical_guesser_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace morphodita {

void czech_morpho_encoder::encode(FILE* in_dictionary, FILE* in_prefix_guesser, FILE* in_statistical_guesser, int max_tag_length, FILE* out_morpho) {
  binary_encoder enc;

  enc.add_1B(max_tag_length);

  eprintf("Encoding dictionary.\n");
  morpho_dictionary_encoder<czech_lemma_addinfo>::encode(in_dictionary, enc);

  // Load and encode prefix guesser if requested
  enc.add_1B(in_prefix_guesser != nullptr);
  if (in_prefix_guesser) {
    eprintf("Encoding prefix guesser.\n");
    morpho_prefix_guesser_encoder::encode(in_prefix_guesser, enc);
  }

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
