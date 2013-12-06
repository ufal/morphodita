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
class czech_elementary_features : public elementary_features<Map> {
 public:
  czech_elementary_features();

  enum features_per_form { FORM, FOLLOWING_VERB_TAG, FOLLOWING_VERB_LEMMA, NUM, CAP, DASH, PREFIX1, PREFIX2, PREFIX3, PREFIX4, SUFFIX1, SUFFIX2, SUFFIX3, SUFFIX4, PER_FORM_TOTAL };
  enum features_per_tag { TAG, TAG2, TAG4, TAG14, LEMMA, PER_TAG_TOTAL };
  enum features_dynamic { PREVIOUS_VERB_TAG, PREVIOUS_VERB_LEMMA, DYNAMIC_TOTAL };
  enum features_map { MAP_NONE = -1, MAP_FORM, MAP_LEMMA, MAP_PREFIX1, MAP_PREFIX2, MAP_PREFIX3, MAP_PREFIX4, MAP_SUFFIX1, MAP_SUFFIX2, MAP_SUFFIX3, MAP_SUFFIX4, MAP_TAG, MAP_TAG2, MAP_TAG4, MAP_TAG14, MAP_TOTAL } ;

  struct per_form_features { elementary_feature_value values[PER_FORM_TOTAL]; };
  struct per_tag_features { elementary_feature_value values[PER_TAG_TOTAL]; };
  struct dynamic_features { elementary_feature_value values[DYNAMIC_TOTAL]; };

  static vector<elementary_feature_description> descriptions;

  void compute_features(const vector<form_with_tags>& forms, int forms_size, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const;
  inline void compute_dynamic_features(const tagged_lemma& tag, const per_form_features& per_form, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const;

  using elementary_features<Map>::maps;
};

typedef czech_elementary_features<persistent_elementary_feature_map> persistent_czech_elementary_features;


// Definitions
template <class Map>
czech_elementary_features<Map>::czech_elementary_features() {
  maps.resize(MAP_TOTAL);
}

template <class Map>
vector<elementary_feature_description> czech_elementary_features<Map>::descriptions = {
  {"Form", PER_FORM, ANY_OFFSET, FORM, MAP_FORM},
  {"FollowingVerbTag", PER_FORM, ANY_OFFSET, FOLLOWING_VERB_TAG, MAP_TAG},
  {"FollowingVerbLemma", PER_FORM, ANY_OFFSET, FOLLOWING_VERB_LEMMA, MAP_LEMMA },
  {"Num", PER_FORM, ONLY_CURRENT, NUM, MAP_NONE},
  {"Cap", PER_FORM, ONLY_CURRENT, CAP, MAP_NONE},
  {"Dash", PER_FORM, ONLY_CURRENT, DASH, MAP_NONE},
  {"Prefix1", PER_FORM, ONLY_CURRENT, PREFIX1, MAP_PREFIX1},
  {"Prefix2", PER_FORM, ONLY_CURRENT, PREFIX2, MAP_PREFIX2},
  {"Prefix3", PER_FORM, ONLY_CURRENT, PREFIX3, MAP_PREFIX3},
  {"Prefix4", PER_FORM, ONLY_CURRENT, PREFIX4, MAP_PREFIX4},
  {"Suffix1", PER_FORM, ONLY_CURRENT, SUFFIX1, MAP_SUFFIX1},
  {"Suffix2", PER_FORM, ONLY_CURRENT, SUFFIX2, MAP_SUFFIX2},
  {"Suffix3", PER_FORM, ONLY_CURRENT, SUFFIX3, MAP_SUFFIX3},
  {"Suffix4", PER_FORM, ONLY_CURRENT, SUFFIX4, MAP_SUFFIX4},

  {"Tag", PER_TAG, ANY_OFFSET, TAG, MAP_TAG},
  {"Tag2", PER_TAG, ANY_OFFSET, TAG2, MAP_TAG2},
  {"Tag4", PER_TAG, ANY_OFFSET, TAG4, MAP_TAG4},
  {"Tag14", PER_TAG, ANY_OFFSET, TAG14, MAP_TAG14},
  {"Lemma", PER_TAG, ANY_OFFSET, LEMMA, MAP_LEMMA},

  {"PreviousVerbTag", DYNAMIC, ANY_OFFSET, PREVIOUS_VERB_TAG, MAP_TAG},
  {"PreviousVerbLemma", DYNAMIC, ANY_OFFSET, PREVIOUS_VERB_LEMMA, MAP_LEMMA}
};

template <class Map>
void czech_elementary_features<Map>::compute_features(const vector<form_with_tags>& forms, int forms_size, vector<per_form_features>& per_form, vector<vector<per_tag_features>>& per_tag) const {
  // We process the sentence in reverse order, so that we can compute FollowingVerbTag and FollowingVerbLemma directly.
  elementary_feature_value following_verb_tag = elementary_feature_empty, following_verb_lemma = elementary_feature_empty;
  for (int i = forms_size - 1; i >= 0; i--) {
    int verb_candidate = -1;

    // Per_tag features and verb_candidate
    for (unsigned j = 0; j < forms[i].tags.size(); j++) {
      char tag14[2];
      per_tag[i][j].values[TAG] = maps[MAP_TAG].value(forms[i].tags[j].tag.c_str(), forms[i].tags[j].tag.size());
      per_tag[i][j].values[TAG2] = forms[i].tags[j].tag.size() >= 2 ? maps[MAP_TAG2].value(forms[i].tags[j].tag.c_str() + 1, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG4] = forms[i].tags[j].tag.size() >= 4 ? maps[MAP_TAG4].value(forms[i].tags[j].tag.c_str() + 3, 1) : elementary_feature_empty;
      per_tag[i][j].values[TAG14] = forms[i].tags[j].tag.size() >= 4 ? maps[MAP_TAG14].value((tag14[0] = forms[i].tags[j].tag[0], tag14[1] = forms[i].tags[j].tag[3], tag14), 2) : elementary_feature_empty;
      per_tag[i][j].values[LEMMA] = j && forms[i].tags[j-1].lemma == forms[i].tags[j].lemma ? per_tag[i][j-1].values[LEMMA] :
          maps[MAP_LEMMA].value(forms[i].tags[j].lemma.c_str(), forms[i].tags[j].lemma.size());

      if (!forms[i].tags[j].tag.empty() && forms[i].tags[j].tag[0] == 'V') {
        int tag_compare;
        verb_candidate = verb_candidate < 0 || (tag_compare = forms[i].tags[j].tag.compare(forms[i].tags[verb_candidate].tag), tag_compare < 0) || (tag_compare == 0 && forms[i].tags[j].lemma < forms[i].tags[verb_candidate].lemma) ? j : verb_candidate;
      }
    }
    if (verb_candidate >= 0) {
      following_verb_tag = per_tag[i][verb_candidate].values[TAG];
      following_verb_lemma = per_tag[i][verb_candidate].values[LEMMA];
    }

    // Per_form features
    per_form[i].values[FORM] = maps[MAP_FORM].value(forms[i].form.str, forms[i].form.len);
    per_form[i].values[FOLLOWING_VERB_TAG] = following_verb_tag;
    per_form[i].values[FOLLOWING_VERB_LEMMA] = following_verb_lemma;

    // Ortographic per_form features if needed
    if (forms[i].tags.size() == 1) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_unknown;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = per_form[i].values[PREFIX4] = elementary_feature_unknown;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = per_form[i].values[SUFFIX4] = elementary_feature_unknown;
    } else if (!forms[i].form.len <= 0) {
      per_form[i].values[NUM] = per_form[i].values[CAP] = per_form[i].values[DASH] = elementary_feature_empty + 1;
      per_form[i].values[PREFIX1] = per_form[i].values[PREFIX2] = per_form[i].values[PREFIX3] = per_form[i].values[PREFIX4] = elementary_feature_empty;
      per_form[i].values[SUFFIX1] = per_form[i].values[SUFFIX2] = per_form[i].values[SUFFIX3] = per_form[i].values[SUFFIX4] = elementary_feature_empty;
    } else {
      string_piece form = forms[i].form;
      const char* form_start = form.str;

      bool num = false, cap = false, dash = false;
      size_t indices[8] = {0, form.len, form.len, form.len, form.len, 0, 0, 0}; // careful here regarding forms shorter than 4 characters
      int index = 0;
      while (form.len) {
        indices[(index++)&7] = form.str - form_start;

        char32_t chr = utf8::decode(form.str, form.len);
        num |= utf8::is_N(chr);
        cap |= utf8::is_Lut(chr);
        dash |= utf8::is_Pd(chr);

        if (index == 5 || (!form.len && index < 5)) {
          per_form[i].values[PREFIX1] = maps[MAP_PREFIX1].value(form_start, indices[1]);
          per_form[i].values[PREFIX2] = maps[MAP_PREFIX2].value(form_start, indices[2]);
          per_form[i].values[PREFIX3] = maps[MAP_PREFIX3].value(form_start, indices[3]);
          per_form[i].values[PREFIX4] = maps[MAP_PREFIX4].value(form_start, indices[4]);
        }
      }
      per_form[i].values[SUFFIX1] = maps[MAP_SUFFIX1].value(form_start + indices[(index-1)&7], form.str - form_start - indices[(index-1)&7]);
      per_form[i].values[SUFFIX2] = maps[MAP_SUFFIX2].value(form_start + indices[(index-2)&7], form.str - form_start - indices[(index-2)&7]);
      per_form[i].values[SUFFIX3] = maps[MAP_SUFFIX3].value(form_start + indices[(index-3)&7], form.str - form_start - indices[(index-3)&7]);
      per_form[i].values[SUFFIX4] = maps[MAP_SUFFIX4].value(form_start + indices[(index-4)&7], form.str - form_start - indices[(index-4)&7]);
      per_form[i].values[NUM] = elementary_feature_empty + 1 + num;
      per_form[i].values[CAP] = elementary_feature_empty + 1 + cap;
      per_form[i].values[DASH] = elementary_feature_empty + 1 + dash;
    }
  }
}

template <class Map>
void czech_elementary_features<Map>::compute_dynamic_features(const tagged_lemma& tag, const per_form_features& /*per_form*/, const per_tag_features& per_tag, const dynamic_features* prev_dynamic, dynamic_features& dynamic) const {
  if (!tag.tag.empty() && tag.tag[0] == 'V') {
    dynamic.values[PREVIOUS_VERB_TAG] = per_tag.values[TAG];
    dynamic.values[PREVIOUS_VERB_LEMMA] = per_tag.values[LEMMA];
  } else if (prev_dynamic) {
    dynamic.values[PREVIOUS_VERB_TAG] = prev_dynamic->values[PREVIOUS_VERB_TAG];
    dynamic.values[PREVIOUS_VERB_LEMMA] = prev_dynamic->values[PREVIOUS_VERB_LEMMA];
  } else {
    dynamic.values[PREVIOUS_VERB_TAG] = elementary_feature_empty;
    dynamic.values[PREVIOUS_VERB_LEMMA] = elementary_feature_empty;
  }
}

} // namespace morphodita
} // namespace ufal
