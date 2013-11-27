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
#include "utils/small_stringops.h"

namespace ufal {
namespace morphodita {

// Declarations
struct czech_lemma_addinfo {
  inline static int raw_lemma_len(const char* lemma, int lemma_len);
  inline static int lemma_id_len(const char* lemma, int lemma_len);
  inline static string format(const unsigned char* addinfo, int addinfo_len);

  inline int parse(const char* lemma, int lemma_len, bool die_on_failure = false);
  inline bool match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len);
  inline bool match_comments_partial(const unsigned char* other_addinfo, int other_addinfo_len);
  inline bool match_comments_full(const unsigned char* other_addinfo, int other_addinfo_len);

  vector<unsigned char> data;
};


// Definitions
int czech_lemma_addinfo::raw_lemma_len(const char* lemma, int lemma_len) {
  // Lemma ends by a '-[0-9]', '`' or '_'.
  for (int len = 0; len < lemma_len; len++)
    if (lemma[len] == '`' || lemma[len] == '_' || (lemma[len] == '-' && len+1 < lemma_len && lemma[len+1] >= '0' && lemma[len+1] <= '9'))
      return len;
  return lemma_len;
}

int czech_lemma_addinfo::lemma_id_len(const char* lemma, int lemma_len) {
  // Lemma ends by a '-[0-9]', '`' or '_'.
  for (int len = 0; len < lemma_len; len++) {
    if (lemma[len] == '`' || lemma[len] == '_')
      return len;
    if (lemma[len] == '-' && len+1 < lemma_len && lemma[len+1] >= '0' && lemma[len+1] <= '9') {
      len += 2;
      while (len < lemma_len && lemma[len] >= '0' && lemma[len] <= '9') len++;
      return len;
    }
  }
  return lemma_len;
}

int czech_lemma_addinfo::parse(const char* lemma, int lemma_len, bool die_on_failure) {
  data.clear();

  const char* lemma_info = lemma + raw_lemma_len(lemma, lemma_len);
  if (lemma_info < lemma + lemma_len) {
    int lemma_num = 255;
    const char* lemma_additional_info = lemma_info;

    if (*lemma_info == '-') {
      lemma_num = strtol(lemma_info + 1, (char**) &lemma_additional_info, 10);

      if (lemma_additional_info == lemma_info + 1 || (*lemma_additional_info != '\0' && *lemma_additional_info != '`' && *lemma_additional_info != '_') || lemma_num < 0 || lemma_num >= 255) {
        if (die_on_failure)
          runtime_errorf("Lemma number %d in lemma %s out of range!", lemma_num, lemma);
        else
          lemma_num = 255;
      }
    }
    data.emplace_back(lemma_num);
    while (lemma_additional_info < lemma + lemma_len)
      data.push_back(*(unsigned char*)lemma_additional_info++);

    if (data.size() > 255) {
      if (die_on_failure)
        runtime_errorf("Too long lemma info %s in lemma %s!", lemma_info, lemma);
      else
        data.resize(255);
    }
  }

  return lemma_info - lemma;
}

bool czech_lemma_addinfo::match_lemma_id(const unsigned char* other_addinfo, int other_addinfo_len) {
  if (data.empty()) return true;
  if (data[0] != 255 && (!other_addinfo_len || other_addinfo[0] != data[0])) return false;
  return true;
}

bool czech_lemma_addinfo::match_comments_full(const unsigned char* other_addinfo, int other_addinfo_len) {
  if (int(data.size()) != other_addinfo_len) return false;
  return data.size() <= 1 ? true : small_memeq(data.data() + 1, other_addinfo + 1, data.size() - 1);
}

bool czech_lemma_addinfo::match_comments_partial(const unsigned char* other_addinfo, int other_addinfo_len) {
  if (data.size() <= 1) return other_addinfo_len <= 1;
  if (other_addinfo_len <= 1) return false;

  if (int(data.size()) <= other_addinfo_len) {
    for (int offset = 0; offset <= other_addinfo_len - int(data.size()); offset++)
      if (small_memeq(data.data() + 1, other_addinfo + 1 + offset, data.size() - 1))
        return true;
  } else {
    for (int offset = 0; offset <= int(data.size()) - other_addinfo_len; offset++)
      if (small_memeq(other_addinfo + 1, data.data() + 1 + offset, other_addinfo_len - 1))
        return true;
  }

  return false;
}

string czech_lemma_addinfo::format(const unsigned char* addinfo, int addinfo_len) {
  string res;

  if (addinfo_len) {
    res.reserve(addinfo_len + 4);
    if (addinfo[0] != 255) {
      char num[5];
      sprintf(num, "-%u", addinfo[0]);
      res += num;
    }
    for (int i = 1; i < addinfo_len; i++)
      res += addinfo[i];
  }

  return res;
}

} // namespace morphodita
} // namespace ufal
