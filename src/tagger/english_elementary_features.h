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

#include "elementary_features.h"
#include "utils/utf8.h"
#include "viterbi.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class Map>
class english_elementary_features : public elementary_features<Map> {
 public:
  english_elementary_features();

  enum features_per_form { FORM, NUM, CAP, DASH, PREFIX1, PREFIX2, PREFIX3, PREFIX4, PREFIX5, PREFIX6, PREFIX7, PREFIX8, PREFIX9, SUFFIX1, SUFFIX2, SUFFIX3, SUFFIX4, SUFFIX5, SUFFIX6, SUFFIX7, SUFFIX8, SUFFIX9, PER_FORM_TOTAL };
  enum features_per_tag { TAG, TAG1, LEMMA, PER_TAG_TOTAL };
  enum features_dynamic { DYNAMIC_TOTAL };
  enum features_map { MAP_NONE = -1, MAP_FORM, MAP_PREFIX1, MAP_PREFIX2, MAP_PREFIX3, MAP_PREFIX4, MAP_PREFIX5, MAP_PREFIX6, MAP_PREFIX7, MAP_PREFIX8, MAP_PREFIX9, MAP_SUFFIX1, MAP_SUFFIX2, MAP_SUFFIX3, MAP_SUFFIX4, MAP_SUFFIX5, MAP_SUFFIX6, MAP_SUFFIX7, MAP_SUFFIX8, MAP_SUFFIX9, MAP_TAG, MAP_TAG1, MAP_LEMMA, MAP_TOTAL } ;

  struct per_form_features { elementary_feature_value values[PER_FORM_TOTAL]; };
  struct per_tag_features { elementary_feature_value values[PER_TAG_TOTAL]; };
  struct dynamic_features { elementary_feature_value values[DYNAMIC_TOTAL]; };

  static vector<elementary_feature_description> descriptions;

  void compute_features(const vector<form_with_tags>& forms, int forms_size, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const;
  inline void compute_dynamic_features(const tagged_lemma& tag, const per_form_features& per_form, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const;

  using elementary_features<Map>::maps;
};

typedef english_elementary_features<persistent_elementary_feature_map> persistent_english_elementary_features;


// Definitions
template <class Map>
english_elementary_features<Map>::english_elementary_features() {
  maps.resize(MAP_TOTAL);
}

template <class Map>
vector<elementary_feature_description> english_elementary_features<Map>::descriptions = {
  {"Form", PER_FORM, ANY_OFFSET, FORM, MAP_FORM},
  {"Num", PER_FORM, ONLY_CURRENT, NUM, MAP_NONE},
  {"Cap", PER_FORM, ONLY_CURRENT, CAP, MAP_NONE},
  {"Dash", PER_FORM, ONLY_CURRENT, DASH, MAP_NONE},
  {"Prefix1", PER_FORM, ONLY_CURRENT, PREFIX1, MAP_PREFIX1},
  {"Prefix2", PER_FORM, ONLY_CURRENT, PREFIX2, MAP_PREFIX2},
  {"Prefix3", PER_FORM, ONLY_CURRENT, PREFIX3, MAP_PREFIX3},
  {"Prefix4", PER_FORM, ONLY_CURRENT, PREFIX4, MAP_PREFIX4},
  {"Prefix5", PER_FORM, ONLY_CURRENT, PREFIX5, MAP_PREFIX5},
  {"Prefix6", PER_FORM, ONLY_CURRENT, PREFIX6, MAP_PREFIX6},
  {"Prefix7", PER_FORM, ONLY_CURRENT, PREFIX7, MAP_PREFIX7},
  {"Prefix8", PER_FORM, ONLY_CURRENT, PREFIX8, MAP_PREFIX8},
  {"Prefix9", PER_FORM, ONLY_CURRENT, PREFIX9, MAP_PREFIX9},
  {"Suffix1", PER_FORM, ONLY_CURRENT, SUFFIX1, MAP_SUFFIX1},
  {"Suffix2", PER_FORM, ONLY_CURRENT, SUFFIX2, MAP_SUFFIX2},
  {"Suffix3", PER_FORM, ONLY_CURRENT, SUFFIX3, MAP_SUFFIX3},
  {"Suffix4", PER_FORM, ONLY_CURRENT, SUFFIX4, MAP_SUFFIX4},
  {"Suffix5", PER_FORM, ONLY_CURRENT, SUFFIX5, MAP_SUFFIX5},
  {"Suffix6", PER_FORM, ONLY_CURRENT, SUFFIX6, MAP_SUFFIX6},
  {"Suffix7", PER_FORM, ONLY_CURRENT, SUFFIX7, MAP_SUFFIX7},
  {"Suffix8", PER_FORM, ONLY_CURRENT, SUFFIX8, MAP_SUFFIX8},
  {"Suffix9", PER_FORM, ONLY_CURRENT, SUFFIX9, MAP_SUFFIX9},

  {"Tag", PER_TAG, ANY_OFFSET, TAG, MAP_TAG},
  {"Tag1", PER_TAG, ANY_OFFSET, TAG1, MAP_TAG1},
  {"Lemma", PER_TAG, ANY_OFFSET, LEMMA, MAP_LEMMA},
};

template <class Map>
void english_elementary_features<Map>::compute_features(const vector<form_with_tags>& forms, int forms_size, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const {
  for (int i = 0; i < forms_size; i++) {
    // Per_tag features
    for (unsigned j = 0; j < forms[i].tags.size(); j++) {
      per_tag[i][j].values[TAG] = maps[MAP_TAG].value(forms[i].tags[j].tag.c_str(), forms[i].tags[j].tag.size());
      per_tag[i][j].values[TAG1] = forms[i].tags[j].tag.size() >= 1 ? maps[MAP_TAG1].value(forms[i].tags[j].tag.c_str(), 1) : elementary_feature_empty;
      per_tag[i][j].values[LEMMA] = j && forms[i].tags[j-1].lemma == forms[i].tags[j].lemma ? per_tag[i][j-1].values[LEMMA] :
          maps[MAP_LEMMA].value(forms[i].tags[j].lemma.c_str(), forms[i].tags[j].lemma.size());
    }

    // Per_form features
    per_form[i].values[FORM] = maps[MAP_FORM].value(forms[i].form.str, forms[i].form.len);

    // Ortographic per_form features if needed
    if (forms[i].tags.size() == 1) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_unknown;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = elementary_feature_unknown;
      per_form[i].values[PREFIX4] = per_form[i].values[PREFIX5] = per_form[i].values[PREFIX6] = elementary_feature_unknown;
      per_form[i].values[PREFIX7] = per_form[i].values[PREFIX8] = per_form[i].values[PREFIX9] = elementary_feature_unknown;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = elementary_feature_unknown;
      per_form[i].values[SUFFIX4] = per_form[i].values[SUFFIX5] = per_form[i].values[SUFFIX6] = elementary_feature_unknown;
      per_form[i].values[SUFFIX7] = per_form[i].values[SUFFIX8] = per_form[i].values[SUFFIX9] = elementary_feature_unknown;
    } else if (forms[i].form.len <= 0) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_empty + 1;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = elementary_feature_empty;
      per_form[i].values[PREFIX4] = per_form[i].values[PREFIX5] = per_form[i].values[PREFIX6] = elementary_feature_empty;
      per_form[i].values[PREFIX7] = per_form[i].values[PREFIX8] = per_form[i].values[PREFIX9] = elementary_feature_empty;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = elementary_feature_empty;
      per_form[i].values[SUFFIX4] = per_form[i].values[SUFFIX5] = per_form[i].values[SUFFIX6] = elementary_feature_empty;
      per_form[i].values[SUFFIX7] = per_form[i].values[SUFFIX8] = per_form[i].values[SUFFIX9] = elementary_feature_empty;
    } else {
      string_piece form = forms[i].form;
      const char* form_start = form.str;

      bool num = false, cap = false, dash = false;
      size_t indices[18] = {0, form.len, form.len, form.len, form.len, form.len, form.len, form.len, form.len, form.len, 0, 0, 0, 0, 0, 0, 0, 0}; // careful here regarding forms shorter than 9 characters
      int index = 0;
      while (form.len) {
        indices[(index++) % 18] = form.str - form_start;

        char32_t chr = utf8::decode(form.str, form.len);
        num |= utf8::is_N(chr);
        cap |= utf8::is_Lut(chr);
        dash |= utf8::is_Pd(chr);

        if (index == 10 || (!form.len && index < 10)) {
          per_form[i].values[PREFIX1] = maps[MAP_PREFIX1].value(form_start, indices[1]);
          per_form[i].values[PREFIX2] = maps[MAP_PREFIX2].value(form_start, indices[2]);
          per_form[i].values[PREFIX3] = maps[MAP_PREFIX3].value(form_start, indices[3]);
          per_form[i].values[PREFIX4] = maps[MAP_PREFIX4].value(form_start, indices[4]);
          per_form[i].values[PREFIX5] = maps[MAP_PREFIX5].value(form_start, indices[5]);
          per_form[i].values[PREFIX6] = maps[MAP_PREFIX6].value(form_start, indices[6]);
          per_form[i].values[PREFIX7] = maps[MAP_PREFIX7].value(form_start, indices[7]);
          per_form[i].values[PREFIX8] = maps[MAP_PREFIX8].value(form_start, indices[8]);
          per_form[i].values[PREFIX9] = maps[MAP_PREFIX9].value(form_start, indices[9]);
        }
      }
      per_form[i].values[SUFFIX1] = maps[MAP_SUFFIX1].value(form_start + indices[(index+18-1) % 18], form.str - form_start - indices[(index+18-1) % 18]);
      per_form[i].values[SUFFIX2] = maps[MAP_SUFFIX2].value(form_start + indices[(index+18-2) % 18], form.str - form_start - indices[(index+18-2) % 18]);
      per_form[i].values[SUFFIX3] = maps[MAP_SUFFIX3].value(form_start + indices[(index+18-3) % 18], form.str - form_start - indices[(index+18-3) % 18]);
      per_form[i].values[SUFFIX4] = maps[MAP_SUFFIX4].value(form_start + indices[(index+18-4) % 18], form.str - form_start - indices[(index+18-4) % 18]);
      per_form[i].values[SUFFIX5] = maps[MAP_SUFFIX5].value(form_start + indices[(index+18-5) % 18], form.str - form_start - indices[(index+18-5) % 18]);
      per_form[i].values[SUFFIX6] = maps[MAP_SUFFIX6].value(form_start + indices[(index+18-6) % 18], form.str - form_start - indices[(index+18-6) % 18]);
      per_form[i].values[SUFFIX7] = maps[MAP_SUFFIX7].value(form_start + indices[(index+18-7) % 18], form.str - form_start - indices[(index+18-7) % 18]);
      per_form[i].values[SUFFIX8] = maps[MAP_SUFFIX8].value(form_start + indices[(index+18-8) % 18], form.str - form_start - indices[(index+18-8) % 18]);
      per_form[i].values[SUFFIX9] = maps[MAP_SUFFIX9].value(form_start + indices[(index+18-9) % 18], form.str - form_start - indices[(index+18-9) % 18]);
      per_form[i].values[NUM] = elementary_feature_empty + 1 + num;
      per_form[i].values[CAP] = elementary_feature_empty + 1 + cap;
      per_form[i].values[DASH] = elementary_feature_empty + 1 + dash;
    }
  }
}

template <class Map>
void english_elementary_features<Map>::compute_dynamic_features(const tagged_lemma& /*tag*/, const per_form_features& /*per_form*/, const per_tag_features& /*per_tag*/, const dynamic_features* /*prev_dynamic*/, dynamic_features& /*dynamic*/) const {
}

} // namespace morphodita
} // namespace ufal
