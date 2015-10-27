// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "czech_lemma_addinfo.h"
#include "czech_morpho_encoder.h"
#include "morpho_dictionary_encoder.h"
#include "morpho_prefix_guesser_encoder.h"
#include "morpho_statistical_guesser_encoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"

namespace ufal {
namespace morphodita {

void czech_morpho_encoder::encode(FILE* in_dictionary, int max_suffix_len, FILE* in_prefix_guesser, FILE* in_statistical_guesser, int max_tag_length, FILE* out_morpho) {
  binary_encoder enc;

  enc.add_1B(max_tag_length);

  eprintf("Encoding dictionary.\n");
  morpho_dictionary_encoder<czech_lemma_addinfo>::encode(in_dictionary, max_suffix_len, enc);

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
