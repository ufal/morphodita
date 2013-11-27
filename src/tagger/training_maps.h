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
    // We deliberately use feature_sequence*s*_score to check for overflow
    feature_sequences_score alpha = 0;
    feature_sequences_score gamma = 0;
    int last_gamma_update = 0;
  };

  inline feature_sequence_score score(const char* feature, int len) const;
  mutable unordered_map<string, info> map;
 private:
  mutable string key;
};

template <template <class> class ElementaryFeatures> using train_feature_sequences = feature_sequences<ElementaryFeatures<training_elementary_feature_map>, training_feature_sequence_map>;

// Definitions
elementary_feature_value training_elementary_feature_map::value(const char* feature, int len) const {
  key.assign(feature, len);
  return map.emplace(key, elementary_feature_empty + map.size()).first->second;
}

feature_sequence_score training_feature_sequence_map::score(const char* feature, int len) const {
  key.assign(feature, len);
  auto it = map.find(key);
  return it != map.end() ? it->second.alpha : 0;
}

} // namespace morphodita
} // namespace ufal
