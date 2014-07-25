// This file is part of UniLib <http://github.com/ufal/unilib/>.
//
// Copyright 2014 by Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under 3-clause BSD licence.
//
// UniLib version: 2.0
// Unicode version: 7.0.0

#ifndef UFAL_UNILIB_UNISTRIP_H
#define UFAL_UNILIB_UNISTRIP_H

#include <string>

namespace ufal {
namespace morphodita {

class unistrip {
 public:
  static inline bool is_combining_mark(char32_t chr);
  static inline char32_t strip_combining_marks(char32_t chr);

 private:
  static const char32_t CHARS = 0x110000;

  static const uint8_t combining_mark_index[CHARS >> 8];
  static const uint8_t combining_mark_block[][32];

  static const uint8_t stripped_index[CHARS >> 8];
  static const uint16_t stripped_block[][256];
  static const char32_t stripped_data[];
};

bool unistrip::is_combining_mark(char32_t chr) {
  return chr < CHARS && combining_mark_block[combining_mark_index[chr >> 8]][(chr >> 3) & 0x1F] & (uint8_t(1) << (chr & 0x07));
}

char32_t unistrip::strip_combining_marks(char32_t chr) {
  if (chr >= CHARS) return chr;
  uint16_t index = stripped_block[stripped_index[chr >> 8]][chr & 0xFF];
  return index ? stripped_data[index] : chr;
}

} // namespace morphodita
} // namespace ufal

#endif // UFAL_UNILIB_UNISTRIP_H
