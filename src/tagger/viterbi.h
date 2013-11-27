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
#include "elementary_features.h"
#include "feature_sequences.h"
#include "morpho/morpho.h"
#include "tagger.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class FeatureSequences, int order>
class viterbi {
 public:
  viterbi(const FeatureSequences& features) : features(features) {}

  struct cache;
  void tag(const vector<form_with_tags>& forms, int forms_size, cache& c, vector<int>& tags) const;

 private:
  struct node;

  const FeatureSequences& features;
};


// Definitions
template <class FeatureSequences, int order>
struct viterbi<FeatureSequences, order>::cache {
  vector<node> nodes;
  typename FeatureSequences::cache features_cache;

  cache(const viterbi<FeatureSequences, order>& self) : features_cache(self.features) {}
};

template <class FeatureSequences, int order>
struct viterbi<FeatureSequences, order>::node {
  int tag;
  int prev;
  feature_sequences_score score;
  typename FeatureSequences::dynamic_features dynamic;
};

template <class FeatureSequences, int order>
void viterbi<FeatureSequences, order>::tag(const vector<form_with_tags>& forms, int forms_size, cache& c, vector<int>& tags) const {
  if (!forms_size) return;

  // Count number of nodes and allocate
  int nodes = 0;
  for (int i = 0, states = 1; i < forms_size; i++) {
    if (forms[i].tags.empty()) return;
    states = (i >= order-1 ? states / forms[i-order+1].tags.size() : states) * forms[i].tags.size();
    nodes += states;
  }
  if (nodes > int(c.nodes.size())) c.nodes.resize(nodes);

  // Init feature sequences
  features.initialize_sentence(forms, forms_size, c.features_cache);

  int window[order];
  typename FeatureSequences::dynamic_features dynamic;
  feature_sequences_score score;

  // Compute all nodes score
  int nodes_prev = -1, nodes_now = 0;
  for (int i = 0; i < forms_size; i++) {
    int nodes_next = nodes_now;

    for (int j = 0; j < order; j++) window[j] = -1;
    for (int tag = 0; tag < int(forms[i].tags.size()); tag++)
      for (int prev = nodes_prev; prev < nodes_now; prev++) {
        // Compute predecessors and number of unchanges
        int same_tags = window[0] == tag;
        window[0] = tag;
        for (int p = prev, n = 1; p >= 0 && n < order; p = c.nodes[p].prev, n++) {
          same_tags += same_tags == n && window[n] == c.nodes[p].tag;
          window[n] = c.nodes[p].tag;
        }

        // Compute dynamic elementary features and score
        features.compute_dynamic_features(i, tag, prev >= 0 ? &c.nodes[prev].dynamic : nullptr, dynamic, c.features_cache);
        score = (nodes_prev + 1 == nodes_now && forms[i].tags.size() == 1 ? 0 : features.score(i, window, same_tags, dynamic, c.features_cache)) +
            (prev >= 0 ? c.nodes[prev].score : 0);

        // Update existing node or create a new one
        if (same_tags >= order-1) {
          if (score <= c.nodes[nodes_next-1].score) continue;
          nodes_next--;
        }
        c.nodes[nodes_next].tag = tag;
        c.nodes[nodes_next].prev = prev;
        c.nodes[nodes_next].score = score;
        c.nodes[nodes_next++].dynamic = dynamic;
      }

    nodes_prev = nodes_now;
    nodes_now = nodes_next;
  }

  // Choose the best ending node
  int best = nodes_prev;
  for (int node = nodes_prev + 1; node < nodes_now; node++)
    if (c.nodes[node].score > c.nodes[best].score)
      best = node;

  for (int i = forms_size - 1; i >= 0; i--, best = c.nodes[best].prev)
    tags[i] = c.nodes[best].tag;
}

} // namespace morphodita
} // namespace ufal
