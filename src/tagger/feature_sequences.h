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
#include "elementary_features.h"
#include "form_with_tags.h"
#include "utils/binary_decoder.h"
#include "utils/compressor.h"
#include "utils/persistent_unordered_map.h"
#include "utils/small_stringops.h"
#include "vli.h"

namespace ufal {
namespace morphodita {

class binary_encoder;

// Declarations
typedef uint32_t feature_sequence_hash;

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

  void parse(int order, istream& is);
  bool load(istream& is);
  bool save(ostream& os);

  struct cache;

  inline void initialize_sentence(const vector<form_with_tags>& forms, int forms_size, cache& c) const;
  inline void compute_dynamic_features(int form_index, int tag_index, const dynamic_features* prev_dynamic, dynamic_features& dynamic, cache& c) const;
  inline feature_sequences_score score(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, cache& c) const;
  void feature_hashes(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, vector<feature_sequence_hash>& hashes, cache& c) const;

  ElementaryFeatures elementary;
  vector<Map> scores;
  vector<feature_sequence> sequences;
};

class persistent_feature_sequence_map {
 public:
  persistent_feature_sequence_map() {}
  template<class Iterator> persistent_feature_sequence_map(pair<Iterator, Iterator> entries);

  struct info {
    feature_sequence_score score;
    uint32_t index;

    info(feature_sequence_score score, uint32_t index) : score(score), index(index) {}
  };
  inline void load(binary_decoder& data) {
    table.clear();
    hashes.clear();
    for (uint32_t buckets = data.next_4B(); buckets; buckets--) {
      table.emplace_back(data.next_4B(), hashes.size());
      for (int num = data.next_1B(); num; num--)
        hashes.push_back(data.next_4B());
    }
    mask = table.size() - 1;
    table.emplace_back(0, hashes.size());
  }
  inline void save(binary_encoder& enc) const;

  feature_sequence_score score(feature_sequence_hash hash) const {
    feature_sequence_hash bucket = hash & mask;
    for (uint32_t index = table[bucket].index; index < table[bucket+1].index; index++)
      if (hashes[index] == hash)
        return table[bucket].score;
    return 0;
  }

 private:
  vector<info> table;
  vector<feature_sequence_hash> hashes;
  feature_sequence_hash mask;
};

template <class ElementaryFeatures> using persistent_feature_sequences = feature_sequences<ElementaryFeatures, persistent_feature_sequence_map>;


// Definitions
template <class ElementaryFeatures, class Map>
inline bool feature_sequences<ElementaryFeatures, Map>::load(istream& is) {
  if (!elementary.load(is)) return false;

  binary_decoder data;
  if (!compressor::load(is, data)) return false;

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
struct feature_sequences<ElementaryFeatures, Map>::cache {
  const vector<form_with_tags>* forms; int forms_size;
  vector<per_form_features> elementary_per_form;
  vector<vector<per_tag_features>> elementary_per_tag;

  struct cache_element {
    feature_sequence_hash hash;
    feature_sequence_score score;
  };
  vector<cache_element> caches;
  vector<const per_tag_features*> window;
  feature_sequences_score score;

  cache(const feature_sequences<ElementaryFeatures, Map>& self) : score(0) {
    caches.resize(self.sequences.size());
    int max_window_size = 1;
    for (auto&& sequence : self.sequences)
      for (auto&& element : sequence.elements)
        if (element.type == PER_TAG && 1 - element.sequence_index > max_window_size)
          max_window_size = 1 - element.sequence_index;
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
    cache.hash = cache.score = 0;
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

    feature_sequence_hash hash = 0;
    bool found = true;
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
        found = false;
        break;
      }

      // Use MurmurHash3 mixing
      value *= 0xcc9e2d51;
      value = (value << 15) | (value >> 17);
      value *= 0x1b873593;
      hash ^= value;
      hash = ((hash << 13) | (hash >> 19)) * 5 + 0xe6546b64;
    }

    result -= c.caches[i].score;
    if (!found) {
      c.caches[i].score = 0;
      c.caches[i].hash = 0;
    } else if (hash != c.caches[i].hash) {
      c.caches[i].score = scores[i].score(hash);
      c.caches[i].hash = hash;
    }
    result += c.caches[i].score;
  }

  c.score = result;
  return result;
}

template <class ElementaryFeatures, class Map>
void feature_sequences<ElementaryFeatures, Map>::feature_hashes(int form_index, int tags_window[], int tags_unchanged, dynamic_features& dynamic, vector<feature_sequence_hash>& hashes, cache& c) const {
  score(form_index, tags_window, tags_unchanged, dynamic, c);

  hashes.resize(c.caches.size());
  for (unsigned i = 0; i < c.caches.size(); i++)
    hashes[i] = c.caches[i].hash;
}

} // namespace morphodita
} // namespace ufal
