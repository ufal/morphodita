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

#include <cstddef>
#include <iterator>
#include <string>

namespace ufal {
namespace morphodita {

class utf8 {
 public:
  static bool valid(const char* str);
  static bool valid(const char* str, size_t len);
  static inline bool valid(const std::string& str);
  static inline char32_t decode(const char*& str);
  static inline char32_t decode(const char*& str, size_t& len);
  static inline char32_t first(const char* str);
  static inline char32_t first(const char* str, size_t len);
  static inline char32_t first(const std::string& str);
  static inline void append(char*& str, char32_t chr);
  static inline void append(std::string& str, char32_t chr);

  class string_decoder_helper {
   public:
    class iterator;
    inline iterator begin();
    inline iterator end();
   private:
    inline string_decoder_helper(const char* str);
    const char* str;
    friend class utf8;
  };
  static inline string_decoder_helper decoder(const char* str);
  static inline string_decoder_helper decoder(const std::string& str);

  class buffer_decoder_helper {
   public:
    class iterator;
    inline iterator begin();
    inline iterator end();
   private:
    inline buffer_decoder_helper(const char* str, size_t len);
    const char* str;
    size_t len;
    friend class utf8;
  };
  static inline buffer_decoder_helper decoder(const char* str, size_t len);

  static inline bool is_Ll(char32_t chr);
  static bool is_Ll(const char* str);
  static bool is_Ll(const char* str, size_t len);
  static inline bool is_Ll(const std::string& str);

  static inline bool is_Lu(char32_t chr);
  static bool is_Lu(const char* str);
  static bool is_Lu(const char* str, size_t len);
  static inline bool is_Lu(const std::string& str);

  static inline bool is_Lt(char32_t chr);
  static bool is_Lt(const char* str);
  static bool is_Lt(const char* str, size_t len);
  static inline bool is_Lt(const std::string& str);

  static inline bool is_Lut(char32_t chr);
  static bool is_Lut(const char* str);
  static bool is_Lut(const char* str, size_t len);
  static inline bool is_Lut(const std::string& str);

  static inline bool is_LC(char32_t chr);
  static bool is_LC(const char* str);
  static bool is_LC(const char* str, size_t len);
  static inline bool is_LC(const std::string& str);

  static inline bool is_Lm(char32_t chr);
  static bool is_Lm(const char* str);
  static bool is_Lm(const char* str, size_t len);
  static inline bool is_Lm(const std::string& str);

  static inline bool is_Lo(char32_t chr);
  static bool is_Lo(const char* str);
  static bool is_Lo(const char* str, size_t len);
  static inline bool is_Lo(const std::string& str);

  static inline bool is_L(char32_t chr);
  static bool is_L(const char* str);
  static bool is_L(const char* str, size_t len);
  static inline bool is_L(const std::string& str);

  static inline bool is_Mn(char32_t chr);
  static bool is_Mn(const char* str);
  static bool is_Mn(const char* str, size_t len);
  static inline bool is_Mn(const std::string& str);

  static inline bool is_Mc(char32_t chr);
  static bool is_Mc(const char* str);
  static bool is_Mc(const char* str, size_t len);
  static inline bool is_Mc(const std::string& str);

  static inline bool is_Me(char32_t chr);
  static bool is_Me(const char* str);
  static bool is_Me(const char* str, size_t len);
  static inline bool is_Me(const std::string& str);

  static inline bool is_M(char32_t chr);
  static bool is_M(const char* str);
  static bool is_M(const char* str, size_t len);
  static inline bool is_M(const std::string& str);

  static inline bool is_Nd(char32_t chr);
  static bool is_Nd(const char* str);
  static bool is_Nd(const char* str, size_t len);
  static inline bool is_Nd(const std::string& str);

  static inline bool is_Nl(char32_t chr);
  static bool is_Nl(const char* str);
  static bool is_Nl(const char* str, size_t len);
  static inline bool is_Nl(const std::string& str);

  static inline bool is_No(char32_t chr);
  static bool is_No(const char* str);
  static bool is_No(const char* str, size_t len);
  static inline bool is_No(const std::string& str);

  static inline bool is_N(char32_t chr);
  static bool is_N(const char* str);
  static bool is_N(const char* str, size_t len);
  static inline bool is_N(const std::string& str);

  static inline bool is_Pc(char32_t chr);
  static bool is_Pc(const char* str);
  static bool is_Pc(const char* str, size_t len);
  static inline bool is_Pc(const std::string& str);

  static inline bool is_Pd(char32_t chr);
  static bool is_Pd(const char* str);
  static bool is_Pd(const char* str, size_t len);
  static inline bool is_Pd(const std::string& str);

  static inline bool is_Ps(char32_t chr);
  static bool is_Ps(const char* str);
  static bool is_Ps(const char* str, size_t len);
  static inline bool is_Ps(const std::string& str);

  static inline bool is_Pe(char32_t chr);
  static bool is_Pe(const char* str);
  static bool is_Pe(const char* str, size_t len);
  static inline bool is_Pe(const std::string& str);

  static inline bool is_Pi(char32_t chr);
  static bool is_Pi(const char* str);
  static bool is_Pi(const char* str, size_t len);
  static inline bool is_Pi(const std::string& str);

  static inline bool is_Pf(char32_t chr);
  static bool is_Pf(const char* str);
  static bool is_Pf(const char* str, size_t len);
  static inline bool is_Pf(const std::string& str);

  static inline bool is_Po(char32_t chr);
  static bool is_Po(const char* str);
  static bool is_Po(const char* str, size_t len);
  static inline bool is_Po(const std::string& str);

  static inline bool is_P(char32_t chr);
  static bool is_P(const char* str);
  static bool is_P(const char* str, size_t len);
  static inline bool is_P(const std::string& str);

  static inline bool is_Sm(char32_t chr);
  static bool is_Sm(const char* str);
  static bool is_Sm(const char* str, size_t len);
  static inline bool is_Sm(const std::string& str);

  static inline bool is_Sc(char32_t chr);
  static bool is_Sc(const char* str);
  static bool is_Sc(const char* str, size_t len);
  static inline bool is_Sc(const std::string& str);

  static inline bool is_Sk(char32_t chr);
  static bool is_Sk(const char* str);
  static bool is_Sk(const char* str, size_t len);
  static inline bool is_Sk(const std::string& str);

  static inline bool is_So(char32_t chr);
  static bool is_So(const char* str);
  static bool is_So(const char* str, size_t len);
  static inline bool is_So(const std::string& str);

  static inline bool is_S(char32_t chr);
  static bool is_S(const char* str);
  static bool is_S(const char* str, size_t len);
  static inline bool is_S(const std::string& str);

  static inline bool is_Zs(char32_t chr);
  static bool is_Zs(const char* str);
  static bool is_Zs(const char* str, size_t len);
  static inline bool is_Zs(const std::string& str);

  static inline bool is_Zl(char32_t chr);
  static bool is_Zl(const char* str);
  static bool is_Zl(const char* str, size_t len);
  static inline bool is_Zl(const std::string& str);

  static inline bool is_Zp(char32_t chr);
  static bool is_Zp(const char* str);
  static bool is_Zp(const char* str, size_t len);
  static inline bool is_Zp(const std::string& str);

  static inline bool is_Z(char32_t chr);
  static bool is_Z(const char* str);
  static bool is_Z(const char* str, size_t len);
  static inline bool is_Z(const std::string& str);

  static inline bool is_Cc(char32_t chr);
  static bool is_Cc(const char* str);
  static bool is_Cc(const char* str, size_t len);
  static inline bool is_Cc(const std::string& str);

  static inline bool is_Cf(char32_t chr);
  static bool is_Cf(const char* str);
  static bool is_Cf(const char* str, size_t len);
  static inline bool is_Cf(const std::string& str);

  static inline bool is_Cs(char32_t chr);
  static bool is_Cs(const char* str);
  static bool is_Cs(const char* str, size_t len);
  static inline bool is_Cs(const std::string& str);

  static inline bool is_Co(char32_t chr);
  static bool is_Co(const char* str);
  static bool is_Co(const char* str, size_t len);
  static inline bool is_Co(const std::string& str);

  static inline bool is_Cn(char32_t chr);
  static bool is_Cn(const char* str);
  static bool is_Cn(const char* str, size_t len);
  static inline bool is_Cn(const std::string& str);

  static inline bool is_C(char32_t chr);
  static bool is_C(const char* str);
  static bool is_C(const char* str, size_t len);
  static inline bool is_C(const std::string& str);

  static inline char32_t uppercase(char32_t chr);
  static void uppercase(const char* str, std::string& out);
  static void uppercase(const char* str, size_t len, std::string& out);
  static inline void uppercase(const std::string& str, std::string& out);
  static inline char32_t lowercase(char32_t chr);
  static void lowercase(const char* str, std::string& out);
  static void lowercase(const char* str, size_t len, std::string& out);
  static inline void lowercase(const std::string& str, std::string& out);

 private:
  static const char32_t N = 1114112;
  static const int BLOCKS = 4352;
  static const uint8_t DEFAULT_CAT = 30;
  static const char REPLACEMENT_CHAR = '?';

  static const uint8_t category_index[BLOCKS];
  static const uint8_t category_block[][256];
  static const uint8_t othercase_index[BLOCKS];
  static const int32_t othercase_block[][256];
};

bool utf8::valid(const std::string& str) {
  return valid(str.c_str());
}

char32_t utf8::decode(const char*& str) {
  const unsigned char*& ptr = (const unsigned char*&) str;
  if (*ptr < 0x80) return *ptr++;
  else if (*ptr < 0xC0) return ++ptr, REPLACEMENT_CHAR;
  else if (*ptr < 0xE0) {
    char32_t res = (*ptr++ & 0x1F) << 6;
    if (*ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    return res + ((*ptr++) & 0x3F);
  } else if (*ptr < 0xF0) {
    char32_t res = (*ptr++ & 0x0F) << 12;
    if (*ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    res += ((*ptr++) & 0x3F) << 6;
    if (*ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    return res + ((*ptr++) & 0x3F);
  } else if (*ptr < 0xF8) {
    char32_t res = (*ptr++ & 0x07) << 18;
    if (*ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    res += ((*ptr++) & 0x3F) << 12;
    if (*ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    res += ((*ptr++) & 0x3F) << 6;
    if (*ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    return res + ((*ptr++) & 0x3F);
  } else return ++ptr, REPLACEMENT_CHAR;
}

char32_t utf8::decode(const char*& str, size_t& len) {
  const unsigned char*& ptr = (const unsigned char*&) str;
  if (!len) return 0; len--;
  if (*ptr < 0x80) return *ptr++;
  else if (*ptr < 0xC0) return ++ptr, REPLACEMENT_CHAR;
  else if (*ptr < 0xE0) {
    char32_t res = (*ptr++ & 0x1F) << 6;
    if (len <= 0 || *ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    return res + ((--len, *ptr++) & 0x3F);
  } else if (*ptr < 0xF0) {
    char32_t res = (*ptr++ & 0x0F) << 12;
    if (len <= 0 || *ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    res += ((--len, *ptr++) & 0x3F) << 6;
    if (len <= 0 || *ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    return res + ((--len, *ptr++) & 0x3F);
  } else if (*ptr < 0xF8) {
    char32_t res = (*ptr++ & 0x07) << 18;
    if (len <= 0 || *ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    res += ((--len, *ptr++) & 0x3F) << 12;
    if (len <= 0 || *ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    res += ((--len, *ptr++) & 0x3F) << 6;
    if (len <= 0 || *ptr < 0x80 || *ptr >= 0xC0) return REPLACEMENT_CHAR;
    return res + ((--len, *ptr++) & 0x3F);
  } else return ++ptr, REPLACEMENT_CHAR;
}

char32_t utf8::first(const char* str) {
  return decode(str);
}

char32_t utf8::first(const char* str, size_t len) {
  return decode(str, len);
}

char32_t utf8::first(const std::string& str) {
  return first(str.c_str());
}

void utf8::append(char*& str, char32_t chr) {
  if (chr < 0x80) *str++ = chr;
  else if (chr < 0x800) { *str++ = 0xC0 + (chr >> 6); *str++ = 0x80 + (chr & 0x3F); }
  else if (chr < 0x10000) { *str++ = 0xE0 + (chr >> 12); *str++ = 0x80 + ((chr >> 6) & 0x3F); *str++ = 0x80 + (chr & 0x3F); }
  else if (chr < 0x200000) { *str++ = 0xF0 + (chr >> 18); *str++ = 0x80 + ((chr >> 12) & 0x3F); *str++ = 0x80 + ((chr >> 6) & 0x3F); *str++ = 0x80 + (chr & 0x3F); }
  else *str++ = REPLACEMENT_CHAR;
}

void utf8::append(std::string& str, char32_t chr) {
  if (chr < 0x80) str += chr;
  else if (chr < 0x800) { str += 0xC0 + (chr >> 6); str += 0x80 + (chr & 0x3F); }
  else if (chr < 0x10000) { str += 0xE0 + (chr >> 12); str += 0x80 + ((chr >> 6) & 0x3F); str += 0x80 + (chr & 0x3F); }
  else if (chr < 0x200000) { str += 0xF0 + (chr >> 18); str += 0x80 + ((chr >> 12) & 0x3F); str += 0x80 + ((chr >> 6) & 0x3F); str += 0x80 + (chr & 0x3F); }
  else str += REPLACEMENT_CHAR;
}

class utf8::string_decoder_helper::iterator : public std::iterator<std::input_iterator_tag, char32_t> {
 public:
  iterator(const char* str) : codepoint(0), next(str) { operator++(); }
  iterator(const iterator& it) : codepoint(it.codepoint), next(it.next) {}
  iterator& operator++() { if (next) { codepoint = decode(next); if (!codepoint) next = nullptr; } return *this; }
  iterator operator++(int) { iterator tmp(*this); operator++(); return tmp; }
  bool operator==(const iterator& other) const { return next == other.next; }
  bool operator!=(const iterator& other) const { return next != other.next; }
  const char32_t& operator*() { return codepoint; }
 private:
  char32_t codepoint;
  const char* next;
};

utf8::string_decoder_helper::string_decoder_helper(const char* str) : str(str) {}

utf8::string_decoder_helper::iterator utf8::string_decoder_helper::begin() {
  return iterator(str);
}

utf8::string_decoder_helper::iterator utf8::string_decoder_helper::end() {
  return iterator(nullptr);
}

utf8::string_decoder_helper utf8::decoder(const char* str) {
  return string_decoder_helper(str);
}

utf8::string_decoder_helper utf8::decoder(const std::string& str) {
  return string_decoder_helper(str.c_str());
}

class utf8::buffer_decoder_helper::iterator : public std::iterator<std::input_iterator_tag, char32_t> {
 public:
  iterator(const char* str, size_t len) : codepoint(0), next(str), len(len) { operator++(); }
  iterator(const iterator& it) : codepoint(it.codepoint), next(it.next), len(it.len) {}
  iterator& operator++() { if (next) { codepoint = decode(next, len); if (len <= 0) next = nullptr; } return *this; }
  iterator operator++(int) { iterator tmp(*this); operator++(); return tmp; }
  bool operator==(const iterator& other) const { return next == other.next; }
  bool operator!=(const iterator& other) const { return next != other.next; }
  const char32_t& operator*() { return codepoint; }
 private:
  char32_t codepoint;
  const char* next;
  size_t len;
};

utf8::buffer_decoder_helper::buffer_decoder_helper(const char* str, size_t len) : str(str), len(len) {}

utf8::buffer_decoder_helper::iterator utf8::buffer_decoder_helper::begin() {
  return iterator(str, len);
}

utf8::buffer_decoder_helper::iterator utf8::buffer_decoder_helper::end() {
  return iterator(nullptr, 0);
}

utf8::buffer_decoder_helper utf8::decoder(const char* str, size_t len) {
  return buffer_decoder_helper(str, len);
}

bool utf8::is_Ll(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 1;
}

bool utf8::is_Ll(const std::string& str) {
  return is_Ll(str.c_str());
}

bool utf8::is_Lu(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 2;
}

bool utf8::is_Lu(const std::string& str) {
  return is_Lu(str.c_str());
}

bool utf8::is_Lt(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 3;
}

bool utf8::is_Lt(const std::string& str) {
  return is_Lt(str.c_str());
}

bool utf8::is_Lut(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 2 && cat <= 3;
}

bool utf8::is_Lut(const std::string& str) {
  return is_Lut(str.c_str());
}

bool utf8::is_LC(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 1 && cat <= 3;
}

bool utf8::is_LC(const std::string& str) {
  return is_LC(str.c_str());
}

bool utf8::is_Lm(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 4;
}

bool utf8::is_Lm(const std::string& str) {
  return is_Lm(str.c_str());
}

bool utf8::is_Lo(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 5;
}

bool utf8::is_Lo(const std::string& str) {
  return is_Lo(str.c_str());
}

bool utf8::is_L(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 1 && cat <= 5;
}

bool utf8::is_L(const std::string& str) {
  return is_L(str.c_str());
}

bool utf8::is_Mn(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 6;
}

bool utf8::is_Mn(const std::string& str) {
  return is_Mn(str.c_str());
}

bool utf8::is_Mc(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 7;
}

bool utf8::is_Mc(const std::string& str) {
  return is_Mc(str.c_str());
}

bool utf8::is_Me(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 8;
}

bool utf8::is_Me(const std::string& str) {
  return is_Me(str.c_str());
}

bool utf8::is_M(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 6 && cat <= 8;
}

bool utf8::is_M(const std::string& str) {
  return is_M(str.c_str());
}

bool utf8::is_Nd(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 9;
}

bool utf8::is_Nd(const std::string& str) {
  return is_Nd(str.c_str());
}

bool utf8::is_Nl(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 10;
}

bool utf8::is_Nl(const std::string& str) {
  return is_Nl(str.c_str());
}

bool utf8::is_No(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 11;
}

bool utf8::is_No(const std::string& str) {
  return is_No(str.c_str());
}

bool utf8::is_N(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 9 && cat <= 11;
}

bool utf8::is_N(const std::string& str) {
  return is_N(str.c_str());
}

bool utf8::is_Pc(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 12;
}

bool utf8::is_Pc(const std::string& str) {
  return is_Pc(str.c_str());
}

bool utf8::is_Pd(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 13;
}

bool utf8::is_Pd(const std::string& str) {
  return is_Pd(str.c_str());
}

bool utf8::is_Ps(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 14;
}

bool utf8::is_Ps(const std::string& str) {
  return is_Ps(str.c_str());
}

bool utf8::is_Pe(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 15;
}

bool utf8::is_Pe(const std::string& str) {
  return is_Pe(str.c_str());
}

bool utf8::is_Pi(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 16;
}

bool utf8::is_Pi(const std::string& str) {
  return is_Pi(str.c_str());
}

bool utf8::is_Pf(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 17;
}

bool utf8::is_Pf(const std::string& str) {
  return is_Pf(str.c_str());
}

bool utf8::is_Po(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 18;
}

bool utf8::is_Po(const std::string& str) {
  return is_Po(str.c_str());
}

bool utf8::is_P(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 12 && cat <= 18;
}

bool utf8::is_P(const std::string& str) {
  return is_P(str.c_str());
}

bool utf8::is_Sm(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 19;
}

bool utf8::is_Sm(const std::string& str) {
  return is_Sm(str.c_str());
}

bool utf8::is_Sc(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 20;
}

bool utf8::is_Sc(const std::string& str) {
  return is_Sc(str.c_str());
}

bool utf8::is_Sk(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 21;
}

bool utf8::is_Sk(const std::string& str) {
  return is_Sk(str.c_str());
}

bool utf8::is_So(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 22;
}

bool utf8::is_So(const std::string& str) {
  return is_So(str.c_str());
}

bool utf8::is_S(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 19 && cat <= 22;
}

bool utf8::is_S(const std::string& str) {
  return is_S(str.c_str());
}

bool utf8::is_Zs(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 23;
}

bool utf8::is_Zs(const std::string& str) {
  return is_Zs(str.c_str());
}

bool utf8::is_Zl(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 24;
}

bool utf8::is_Zl(const std::string& str) {
  return is_Zl(str.c_str());
}

bool utf8::is_Zp(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 25;
}

bool utf8::is_Zp(const std::string& str) {
  return is_Zp(str.c_str());
}

bool utf8::is_Z(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 23 && cat <= 25;
}

bool utf8::is_Z(const std::string& str) {
  return is_Z(str.c_str());
}

bool utf8::is_Cc(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 26;
}

bool utf8::is_Cc(const std::string& str) {
  return is_Cc(str.c_str());
}

bool utf8::is_Cf(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 27;
}

bool utf8::is_Cf(const std::string& str) {
  return is_Cf(str.c_str());
}

bool utf8::is_Cs(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 28;
}

bool utf8::is_Cs(const std::string& str) {
  return is_Cs(str.c_str());
}

bool utf8::is_Co(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 29;
}

bool utf8::is_Co(const std::string& str) {
  return is_Co(str.c_str());
}

bool utf8::is_Cn(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat == 30;
}

bool utf8::is_Cn(const std::string& str) {
  return is_Cn(str.c_str());
}

bool utf8::is_C(char32_t chr) {
  uint8_t cat = chr < N ? category_block[category_index[chr >> 8]][chr & 0xFF] : DEFAULT_CAT;
  return cat >= 26 && cat <= 30;
}

bool utf8::is_C(const std::string& str) {
  return is_C(str.c_str());
}

char32_t utf8::uppercase(char32_t chr) {
  if (chr < N && is_Ll(chr)) return chr + othercase_block[othercase_index[chr >> 8]][chr & 0xFF];
  return chr;
}

void utf8::uppercase(const std::string& str, std::string& out) {
  out.reserve(out.size() + str.size());
  uppercase(str.c_str(), out);
}

char32_t utf8::lowercase(char32_t chr) {
  if (chr < N && is_Lut(chr)) return chr + othercase_block[othercase_index[chr >> 8]][chr & 0xFF];
  return chr;
}

void utf8::lowercase(const std::string& str, std::string& out) {
  out.reserve(out.size() + str.size());
  lowercase(str.c_str(), out);
}

} // namespace morphodita
} // namespace ufal
