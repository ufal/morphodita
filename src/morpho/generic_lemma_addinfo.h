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
#include "tokenizer/string_piece.h"
#include "utils/small_stringops.h"

namespace ufal {
namespace morphodita {

// Declarations
struct generic_lemma_addinfo {
  inline static int raw_lemma_len(string_piece lemma);
  inline static int lemma_id_len(string_piece lemma);
  inline static string format(const unsigned char* addinfo, int addinfo_len);
  inline static bool generatable(const unsigned char* addinfo, int addinfo_len);

  inline int parse(string_piece lemma, bool die_on_failure = false);
  inline bool match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len);
};


// Definitions
int generic_lemma_addinfo::raw_lemma_len(string_piece lemma) {
  return lemma.len;
}

int generic_lemma_addinfo::lemma_id_len(string_piece lemma) {
  return lemma.len;
}

string generic_lemma_addinfo::format(const unsigned char* /*addinfo*/, int /*addinfo_len*/) {
  return string();
}

bool generic_lemma_addinfo::generatable(const unsigned char* /*addinfo*/, int /*addinfo_len*/) {
  return true;
}

int generic_lemma_addinfo::parse(string_piece lemma, bool /*die_on_failure*/) {
  return lemma.len;
}

bool generic_lemma_addinfo::match_lemma_id(const unsigned char* /*other_addinfo*/, int /*other_addinfo_len*/) {
  return true;
}

} // namespace morphodita
} // namespace ufal
