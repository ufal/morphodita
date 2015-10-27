// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"
#include "hyphenated_sequences_map.h"
#include "utf8_tokenizer.h"

namespace ufal {
namespace morphodita {

class czech_tokenizer : public utf8_tokenizer {
 public:
  enum tokenizer_language { CZECH = 0, SLOVAK = 1 };
  enum { LATEST = 1 };
  czech_tokenizer(tokenizer_language language, unsigned version);

  virtual bool next_sentence(vector<string_piece>& forms) override;

 private:
  unsigned version;
  string buffer;
  const unordered_set<string>* eos_word_exceptions;
  const hyphenated_sequences_map* hyphenated_sequences;

  static const unordered_set<string> eos_word_exceptions_czech;
  static const unordered_set<string> eos_word_exceptions_slovak;
  static const hyphenated_sequences_map hyphenated_sequences_czech;
};

} // namespace morphodita
} // namespace ufal
