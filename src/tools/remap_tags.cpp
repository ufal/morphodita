// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "morpho/morpho.h"
#include "utils/input.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s dict_file <data", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary %s!", argv[1]);
  eprintf("done\n");

  eprintf("Processing data: ");

  int forms = 0;
  enum match_type { COMPLETE = 0, LEMMA_ID = 1, RAWLEMMA = 2, TAG = 3, MATCH_TYPES = 4 };
  int total_matches[MATCH_TYPES] = {};
  vector<tagged_lemma> lemmas;
  vector<tagged_lemma> matching_lemmas[MATCH_TYPES];
  unordered_map<string, string> lemma_mappings[MATCH_TYPES];

  string line;
  vector<string> tokens;
  while (getline(stdin, line)) {
    if (line.empty()) {
      putchar('\n');
      continue;
    }

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("The line %s does not contain three columns!", line.c_str());

    // Analyse
    dictionary->analyze(tokens[0], morpho::NO_GUESSER, lemmas);
    int rawlemma_len = dictionary->raw_lemma_len(tokens[1]);
    int lemmaid_len = dictionary->lemma_id_len(tokens[1]);

    // Perform remapping if necessary
    forms++;
    int matches[MATCH_TYPES] = {};
    for (auto&& matching_lemma : matching_lemmas)
      matching_lemma.clear();

    for (auto&& lemma : lemmas) {
      if (lemma.tag != tokens[2]) continue;
      matches[TAG] = 1;
      matching_lemmas[TAG].push_back(lemma);
      if (lemma.lemma.compare(0, dictionary->raw_lemma_len(lemma.lemma), tokens[1], 0, rawlemma_len) != 0) continue;

      matches[RAWLEMMA] = 1;
      matching_lemmas[RAWLEMMA].push_back(lemma);
      if (lemma.lemma.compare(0, dictionary->lemma_id_len(lemma.lemma), tokens[1], 0, lemmaid_len) != 0) continue;
      matches[LEMMA_ID] = 1;
      matching_lemmas[LEMMA_ID].push_back(lemma);
      if (lemma.lemma.compare(tokens[1]) != 0) continue;

      matches[COMPLETE] = 1;
      matching_lemmas[COMPLETE].push_back(lemma);
    }

    if (matches[COMPLETE]) {
      if (matches[COMPLETE] != 1) eprintf("W: matches[COMPLETE]=%d\n", matches[COMPLETE]);
      total_matches[COMPLETE]++;
      printf("%s\t%s\t%s\n", tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());
    } else if (matches[LEMMA_ID]) {
      if (matches[LEMMA_ID] != 1) eprintf("W: matches[LEMMA_ID]=%d\n", matches[COMPLETE]);
      if (lemma_mappings[LEMMA_ID].emplace(tokens[1], matching_lemmas[LEMMA_ID][0].lemma).first->second != matching_lemmas[LEMMA_ID][0].lemma)
        {} //eprintf("Two different lemmaid mappings for %s: %s and %s.\n", tokens[1], matching_lemmas[LEMMA_ID][0].lemma.c_str(), lemma_mappings[LEMMA_ID][tokens[1]].c_str());
      total_matches[LEMMA_ID]++;
      printf("%s\t%s\t%s\n", tokens[0].c_str(), matching_lemmas[LEMMA_ID][0].lemma.c_str(), matching_lemmas[LEMMA_ID][0].tag.c_str());
    } else if (matches[RAWLEMMA] == 1) {
      if (matches[RAWLEMMA] != 1) eprintf("W: matches[RAWLEMMA]=%d\n", matches[COMPLETE]);
      if (lemma_mappings[RAWLEMMA].emplace(tokens[1], matching_lemmas[RAWLEMMA][0].lemma).first->second != matching_lemmas[RAWLEMMA][0].lemma)
        {} //eprintf("W: Two different rawlemma mappings for %s: %s and %s.\n", tokens[1], matching_lemmas[RAWLEMMA][0].lemma.c_str(), lemma_mappings[RAWLEMMA][tokens[1]].c_str());
      total_matches[RAWLEMMA]++;
      printf("%s\t%s\t%s\n", tokens[0].c_str(), matching_lemmas[RAWLEMMA][0].lemma.c_str(), matching_lemmas[RAWLEMMA][0].tag.c_str());
    } else if (lemmas.size() == 1 &&
               lemmas[0].tag.compare(0, 1, tokens[2], 0, 1) == 0 &&
               lemmas[0].lemma.compare(0, dictionary->lemma_id_len(lemmas[0].lemma), tokens[1], 0, lemmaid_len) == 0) {
      if (lemma_mappings[TAG].emplace(tokens[1], lemmas[0].tag).first->second != lemmas[0].tag)
        {} //eprintf("W: Two different tag mappings for %s-%s: %s and %s.\n", tokens[1], tokens[2], lemmas[0].tag.c_str(), lemma_mappings[TAG][tokens[1]].c_str());
      total_matches[TAG]++;
      printf("%s\t%s\t%s\n", tokens[0].c_str(), lemmas[0].lemma.c_str(), lemmas[0].tag.c_str());
    } else
      printf("%s\t%s\t%s\n", tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());
  }
  eprintf("done\n");

//  for (auto&& mode : {LEMMA_ID, RAWLEMMA, TAG}) {
//    eprintf("Remappings %d\n", mode);
//    vector<string> mappings;
//    for (auto&& map : lemma_mappings[mode])
//      mappings.emplace_back("  " + map.first + " -> " + map.second);
//    sort(mappings.begin(), mappings.end());
//    for (auto&& mapping : mappings)
//      eprintf("%s\n", mapping.c_str());
//  }

  eprintf("Forms: %d\n", forms);
  eprintf("Accuracy of all: %.3f%%\n", 100 * total_matches[COMPLETE] / double(forms));
  eprintf("Accuracy of tli: %.3f%%\n", 100 * (total_matches[COMPLETE] + total_matches[LEMMA_ID]) / double(forms));
  eprintf("Accuracy of trl: %.3f%%\n", 100 * (total_matches[COMPLETE] + total_matches[LEMMA_ID] + total_matches[RAWLEMMA]) / double(forms));
  eprintf("Accuracy of t:   %.3f%%\n", 100 * (total_matches[COMPLETE] + total_matches[LEMMA_ID] + total_matches[RAWLEMMA] + total_matches[TAG]) / double(forms));

  return 0;
}
