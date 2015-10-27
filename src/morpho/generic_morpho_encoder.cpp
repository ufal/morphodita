// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
