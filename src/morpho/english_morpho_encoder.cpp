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

#include "english_lemma_addinfo.h"
#include "english_morpho_encoder.h"
#include "english_morpho_guesser_encoder.h"
#include "morpho_dictionary_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace morphodita {

void english_morpho_encoder::encode(FILE* dictionary, int max_suffix_len, FILE* guesser, FILE* negations, FILE* out) {
  binary_encoder enc;

  eprintf("Encoding dictionary.\n");
  morpho_dictionary_encoder<english_lemma_addinfo>::encode(dictionary, max_suffix_len, enc);

  eprintf("Encoding guesser.\n");
  english_morpho_guesser_encoder::encode(guesser, negations, enc);

  eprintf("Compressing dictionary.\n");
  if (!compressor::save(out, enc)) runtime_errorf("Cannot compress and write dictionary to file!");
  eprintf("Dictionary saved.\n");
}

} // namespace morphodita
} // namespace ufal
