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

#include "utf8.h"

namespace ufal {
namespace morphodita {

bool utf8::valid(const char* str) {
  for (const unsigned char*& ptr = (const unsigned char*&) str; *ptr; ptr++)
    if (*ptr >= 0x80) {
      if (*ptr < 0xC0) return false;
      else if (*ptr < 0xE0) {
        ptr++; if (*ptr < 0x80 || *ptr >= 0xC0) return false;
      } else if (*ptr < 0xF0) {
        ptr++; if (*ptr < 0x80 || *ptr >= 0xC0) return false;
        ptr++; if (*ptr < 0x80 || *ptr >= 0xC0) return false;
      } else if (*ptr < 0xF8) {
        ptr++; if (*ptr < 0x80 || *ptr >= 0xC0) return false;
        ptr++; if (*ptr < 0x80 || *ptr >= 0xC0) return false;
        ptr++; if (*ptr < 0x80 || *ptr >= 0xC0) return false;
      } else return false;
    }
  return true;
}

bool utf8::valid(const char* str, size_t len) {
  for (const unsigned char*& ptr = (const unsigned char*&) str; len > 0; ptr++, len--)
    if (*ptr >= 0x80) {
      if (*ptr < 0xC0) return false;
      else if (*ptr < 0xE0) {
        ptr++; if (!--len || *ptr < 0x80 || *ptr >= 0xC0) return false;
      } else if (*ptr < 0xF0) {
        ptr++; if (!--len || *ptr < 0x80 || *ptr >= 0xC0) return false;
        ptr++; if (!--len || *ptr < 0x80 || *ptr >= 0xC0) return false;
      } else if (*ptr < 0xF8) {
        ptr++; if (!--len || *ptr < 0x80 || *ptr >= 0xC0) return false;
        ptr++; if (!--len || *ptr < 0x80 || *ptr >= 0xC0) return false;
        ptr++; if (!--len || *ptr < 0x80 || *ptr >= 0xC0) return false;
      } else return false;
    }
  return true;
}

void utf8::decode(const char* str, std::u32string& decoded) {
  decoded.clear();

  for (char32_t chr; (chr = decode(str)); )
    decoded.push_back(chr);
}

void utf8::decode(const char* str, size_t len, std::u32string& decoded) {
  decoded.clear();

  while (len)
    decoded.push_back(decode(str, len));
}

void utf8::encode(const std::u32string& str, std::string& encoded) {
  encoded.clear();

  for (auto&& chr : str)
    append(encoded, chr);
}

const char utf8::REPLACEMENT_CHAR;

} // namespace morphodita
} // namespace ufal
