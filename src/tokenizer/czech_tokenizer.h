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
#include "hyphenated_sequences_index.h"
#include "utf8_tokenizer.h"

namespace ufal {
namespace morphodita {

class czech_tokenizer : public utf8_tokenizer {
 public:
  enum tokenizer_mode {
    CZECH_GENERIC = 0,
    CZECH_131112 = 1,
    SLOVAK_GENERIC = 2,
  };

  czech_tokenizer(tokenizer_mode mode);

  virtual bool next_sentence(vector<string_piece>& forms) override;

 private:
  string buffer;
  const unordered_set<string>* eos_word_exceptions;
  const hyphenated_sequences_index* hyphenated_sequences;

  static const unordered_set<string> eos_word_exceptions_czech;
  static const unordered_set<string> eos_word_exceptions_slovak;
  static const hyphenated_sequences_index hyphenated_sequences_czech_131112;
};

} // namespace morphodita
} // namespace ufal
