// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>

#include "common.h"
#include "feature_sequences.h"
#include "utils/binary_encoder.h"
#include "utils/compressor.h"
#include "utils/input.h"
#include "utils/parse_int.h"
#include "utils/persistent_unordered_map_encoder.h"

namespace ufal {
namespace morphodita {

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

} // namespace morphodita
} // namespace ufal
