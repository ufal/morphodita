// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>
#include <unordered_map>

#include "feature_sequences.h"

namespace ufal {
namespace morphodita {

// Declarations
class training_elementary_feature_map {
 public:
  inline elementary_feature_value value(const char* feature, int len) const;
  mutable unordered_map<string, elementary_feature_value> map = {{"", elementary_feature_empty}};
 private:
  mutable string key;
};

class training_feature_sequence_map {
 public:
  struct info {
    vector<feature_sequence_hash> hashes;

    // We deliberately use feature_sequence*s*_score to check for overflow
    feature_sequences_score alpha = 0;
    feature_sequences_score gamma = 0;
    int last_gamma_update = 0;
  };

  inline feature_sequence_score score(feature_sequence_hash hash) const;
  inline info* entry(feature_sequence_hash hash, bool insert_if_missing);
  pair<vector<info>::iterator, vector<info>::iterator> entries() const;
 private:
  mutable vector<info> table = vector<info>(16);
  mutable feature_sequence_hash fill = 0;
};

template <template <class> class ElementaryFeatures> using train_feature_sequences = feature_sequences<ElementaryFeatures<training_elementary_feature_map>, training_feature_sequence_map>;

// Definitions
elementary_feature_value training_elementary_feature_map::value(const char* feature, int len) const {
  key.assign(feature, len);
  return map.emplace(key, elementary_feature_empty + map.size()).first->second;
}

feature_sequence_score training_feature_sequence_map::score(feature_sequence_hash hash) const {
  auto& bucket = table[hash & ((feature_sequence_hash)table.size() - 1)];
  for (auto&& h : bucket.hashes)
    if (h == hash)
      return bucket.alpha;
  return 0;
}

training_feature_sequence_map::info* training_feature_sequence_map::entry(feature_sequence_hash hash, bool insert_if_missing) {
  auto* bucket = &table[hash & ((feature_sequence_hash)table.size() - 1)];
  bool found = false;
  for (auto&& h : bucket->hashes)
    if (h == hash) {
      found = true;
      break;
    }

  if (!found && insert_if_missing) {
    bucket->hashes.push_back(hash);
    fill++;

    if (fill > table.size() / 2) {
      vector<info> bigger_table(table.size() * 2);
      for (auto&& bucket : table)
        for (auto&& hash : bucket.hashes) {
          auto& bigger_bucket = bigger_table[hash & ((feature_sequence_hash)bigger_table.size() - 1)];
          bigger_bucket.hashes.push_back(hash);
          bigger_bucket.alpha = bucket.alpha;
          bigger_bucket.gamma = bucket.gamma;
          bigger_bucket.last_gamma_update = bucket.last_gamma_update;
        }
      table.swap(bigger_table);
      return &table[hash & ((feature_sequence_hash)table.size() - 1)];
    }
    return bucket;
  }
  return found ? bucket : nullptr;
}

pair<vector<training_feature_sequence_map::info>::iterator, vector<training_feature_sequence_map::info>::iterator> training_feature_sequence_map::entries() const {
  return make_pair(table.begin(), table.end());
}

} // namespace morphodita
} // namespace ufal
