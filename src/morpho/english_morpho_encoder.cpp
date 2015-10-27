// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
