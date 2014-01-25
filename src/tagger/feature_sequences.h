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

#include <algorithm>

#include "common.h"
#include "elementary_features.h"
#include "form_with_tags.h"
#include "utils/binary_decoder.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"
#include "utils/input.h"
#include "utils/parse_int.h"
#include "utils/persistent_unordered_map.h"
#include "utils/small_stringops.h"
#include "vli.h"

namespace ufal {
namespace morphodita {

// Declarations
typedef int32_t feature_sequence_score;
typedef int64_t feature_sequences_score;

struct feature_sequence_element {
  elementary_feature_type type;
  int elementary_index;
  int sequence_index;

  feature_sequence_element() {}
  feature_sequence_element(elementary_feature_type type, int elementary_index, int sequence_index) : type(type), elementary_index(elementary_index), sequence_index(sequence_index) {}
};

struct feature_sequence {
  vector<feature_sequence_element> elements;
  int dependant_range = 1;
};

template <class ElementaryFeatures, class Map>
class feature_sequences {
 public:
  typedef typename ElementaryFeatures::per_form_features per_form_features;
  typedef typename ElementaryFeatures::per_tag_features per_tag_features;
  typedef typename ElementaryFeatures::dynamic_features dynamic_features;

  void parse(int order, FILE* f);
  bool load(FILE* f);
  bool save(FILE* f);

  struct cache;

  inline void initialize_sentence(const vector<form_with_tags>& forms, int forms_size, cache& c) const;
  inline void compute_dynamic_features(int form_index, int tag_index, const dynamic_features* prev_dynamic, dynamic_features& dynamic, cache& c) const;
  inline feature_sequences_score score(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, cache& c) const;
  void feature_keys(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, vector<string>& keys, cache& c) const;

  ElementaryFeatures elementary;
  vector<Map> scores;
  vector<feature_sequence> sequences;
};

class persistent_feature_sequence_map : public persistent_unordered_map {
 public:
  persistent_feature_sequence_map() : persistent_unordered_map() {}
  persistent_feature_sequence_map(const persistent_unordered_map&& map) : persistent_unordered_map(map) {}

  feature_sequence_score score(const char* feature, int len) const {
    auto* it = at_typed<feature_sequence_score>(feature, len);
    return it ? *it : 0;
  }
};

template <class ElementaryFeatures> using persistent_feature_sequences = feature_sequences<ElementaryFeatures, persistent_feature_sequence_map>;


// Definitions
template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::parse(int order, FILE* f) {
  unordered_map<string, elementary_feature_description> elementary_map;
  for (auto&& description : ElementaryFeatures::descriptions)
    if (!elementary_map.emplace(description.name, description).second)
      runtime_errorf("Repeated elementary feature with name %s!", description.name.c_str());

  string line;
  vector<string> tokens;
  while (getline(f, line)) {
    split(line, ',', tokens);
    if (tokens.empty()) runtime_errorf("Feature sequence cannot be empty!");

    bool contains_only_current = false;
    sequences.emplace_back();
    for (auto&& token : tokens) {
      vector<string> parts;
      split(token, ' ', parts);
      if (parts.size() != 2) runtime_errorf("Cannot parse feature sequence element '%s'!", token.c_str());
      auto it = elementary_map.find(parts[0]);
      if (it == elementary_map.end()) runtime_errorf("Unknown elementary feature '%s' used in feature sequence '%s'!", parts[0].c_str(), token.c_str());

      auto& desc = it->second;
      int sequence_index = parse_int(parts[1].c_str(), "sequence_index");
      if (desc.type == DYNAMIC && sequence_index != 0) runtime_errorf("Nonzero sequence index %d of dynamic elementary feature '%s'!", sequence_index, desc.name.c_str());
      if (desc.type == PER_TAG && (sequence_index > 0 || sequence_index <= -order)) runtime_errorf("Wrong sequence index %d of per-tag elementary feature '%s'!", sequence_index, desc.name.c_str());
      if (desc.range == ONLY_CURRENT && sequence_index != 0) runtime_errorf("Nonzero sequence index %d of elementary feature '%s' requiring zero offset!", sequence_index, desc.name.c_str());

      sequences.back().elements.emplace_back(it->second.type, it->second.index, sequence_index);
      if (desc.type == DYNAMIC) sequences.back().dependant_range = max(sequences.back().dependant_range, order + 1);
      if (desc.type == PER_TAG) sequences.back().dependant_range = max(sequences.back().dependant_range, 1 - sequence_index);
      contains_only_current |= desc.range == ONLY_CURRENT;
    }
    if (contains_only_current && sequences.back().dependant_range > 1) runtime_errorf("Feature sequence '%s' contains both a non-local elementary feature and exclusively-local elementary feature!", line.c_str());
  }

  stable_sort(sequences.begin(), sequences.end(), [](const feature_sequence& a, const feature_sequence& b) { return a.dependant_range > b.dependant_range; });
  scores.resize(sequences.size());
}

template <class ElementaryFeatures, class Map>
inline bool feature_sequences<ElementaryFeatures, Map>::load(FILE* f) {
  if (!elementary.load(f)) return false;

  binary_decoder data;
  if (!compressor::load(f, data)) return false;

  try {
    sequences.resize(data.next_1B());
    for (auto&& sequence : sequences) {
      sequence.dependant_range = data.next_4B();
      sequence.elements.resize(data.next_1B());
      for (auto&& element : sequence.elements) {
        element.type = elementary_feature_type(data.next_4B());
        element.elementary_index = data.next_4B();
        element.sequence_index = data.next_4B();
      }
    }

    scores.resize(data.next_1B());
    for (auto&& score : scores)
      score.load(data);
  } catch (binary_decoder_error&) {
    return false;
  }

  return data.is_end();
}

template <class ElementaryFeatures, class Map>
inline bool feature_sequences<ElementaryFeatures, Map>::save(FILE* f) {
  if (!elementary.save(f)) return false;

  binary_encoder enc;
  enc.add_1B(sequences.size());
  for (auto&& sequence : sequences) {
    enc.add_4B(sequence.dependant_range);
    enc.add_1B(sequence.elements.size());
    for (auto&& element : sequence.elements) {
      enc.add_4B(element.type);
      enc.add_4B(element.elementary_index);
      enc.add_4B(element.sequence_index);
    }
  }

  enc.add_1B(scores.size());
  for (auto&& score : scores)
    score.save(enc);

  return compressor::save(f, enc);
}

template <class ElementaryFeatures, class Map>
struct feature_sequences<ElementaryFeatures, Map>::cache {
  const vector<form_with_tags>* forms; int forms_size;
  vector<per_form_features> elementary_per_form;
  vector<vector<per_tag_features>> elementary_per_tag;

  struct cache_element {
    vector<char> key;
    int key_size;
    feature_sequence_score score;

    cache_element(int elements) : key(vli<elementary_feature_value>::max_length() * elements), key_size(0), score(0) {}
  };
  vector<cache_element> caches;
  vector<const per_tag_features*> window;
  vector<char> key;
  feature_sequences_score score;

  cache(const feature_sequences<ElementaryFeatures, Map>& self) : score(0) {
    caches.reserve(self.sequences.size());
    int max_sequence_elements = 0, max_window_size = 1;
    for (auto&& sequence : self.sequences) {
      caches.emplace_back(sequence.elements.size());
      if (int(sequence.elements.size()) > max_sequence_elements) max_sequence_elements = sequence.elements.size();
      for (auto&& element : sequence.elements)
        if (element.type == PER_TAG && 1 - element.sequence_index > max_window_size)
          max_window_size = 1 - element.sequence_index;
    }
    key.resize(max_sequence_elements * vli<elementary_feature_value>::max_length());
    window.resize(max_window_size);
  }
};

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::initialize_sentence(const vector<form_with_tags>& forms, int forms_size, cache& c) const {
  // Store forms and forms_size
  c.forms = &forms;
  c.forms_size = forms_size;

  // Enlarge elementary features vectors if needed
  if (forms_size > int(c.elementary_per_form.size())) c.elementary_per_form.resize(forms_size * 2);
  if (forms_size > int(c.elementary_per_tag.size())) c.elementary_per_tag.resize(forms_size * 2);
  for (int i = 0; i < forms_size; i++)
    if (forms[i].tags.size() > c.elementary_per_tag[i].size())
      c.elementary_per_tag[i].resize(forms[i].tags.size() * 2);

  // Compute elementary features
  elementary.compute_features(forms, forms_size, c.elementary_per_form, c.elementary_per_tag);

  // Clear score cache, because scores may have been modified
  c.score = 0;
  for (auto&& cache : c.caches)
    cache.key_size = cache.score = 0;
}

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::compute_dynamic_features(int form_index, int tag_index, const dynamic_features* prev_dynamic, dynamic_features& dynamic, cache& c) const {
  elementary.compute_dynamic_features((*c.forms)[form_index].tags[tag_index], c.elementary_per_form[form_index], c.elementary_per_tag[form_index][tag_index], form_index > 0 ? prev_dynamic : nullptr, dynamic);
}

template <class ElementaryFeatures, class Map>
feature_sequences_score feature_sequences<ElementaryFeatures, Map>::score(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, cache& c) const {
  // Start by creating a window of per_tag_features*
  for (int i = 0; i < int(c.window.size()) && form_index - i >= 0; i++)
    c.window[i] = &c.elementary_per_tag[form_index - i][tags_window[i]];

  // Compute the score
  feature_sequences_score result = c.score;
  for (unsigned i = 0; i < sequences.size(); i++) {
    if (tags_unchanged >= sequences[i].dependant_range)
      break;

    char* key = c.key.data();
    for (unsigned j = 0; j < sequences[i].elements.size(); j++) {
      auto& element = sequences[i].elements[j];
      elementary_feature_value value;

      switch (element.type) {
        case PER_FORM:
          value = form_index + element.sequence_index < 0 || form_index + element.sequence_index >= c.forms_size ? elementary_feature_empty :
              c.elementary_per_form[form_index + element.sequence_index].values[element.elementary_index];
          break;
        case PER_TAG:
          value = form_index + element.sequence_index < 0 ? elementary_feature_empty : c.window[-element.sequence_index]->values[element.elementary_index];
          break;
        case DYNAMIC:
        default:
          value = dynamic.values[element.elementary_index];
      }

      if (value == elementary_feature_unknown) {
        key = c.key.data();
        break;
      }
      vli<elementary_feature_value>::encode(value, (unsigned char*&) key);
    }

    result -= c.caches[i].score;
    int key_size = key - c.key.data();
    if (!key_size) {
      c.caches[i].score = 0;
      c.caches[i].key_size = 0;
    } else if (key_size != c.caches[i].key_size || !small_memeq(c.key.data(), c.caches[i].key.data(), key_size)) {
      c.caches[i].score = scores[i].score(c.key.data(), key_size);
      c.caches[i].key_size = key_size;
      small_memcpy(c.caches[i].key.data(), c.key.data(), key_size);
    }
    result += c.caches[i].score;
  }

  c.score = result;
  return result;
}

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::feature_keys(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, vector<string>& keys, cache& c) const {
  score(form_index, tags_window, tags_unchanged, dynamic, c);

  keys.resize(c.caches.size());
  for (unsigned i = 0; i < c.caches.size(); i++)
    keys[i].assign(c.caches[i].key.data(), c.caches[i].key_size);
}

} // namespace morphodita
} // namespace ufal
