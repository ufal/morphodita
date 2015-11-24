// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <unordered_map>

#include "morpho/morpho.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/split.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file <data\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading dictionary: ";
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_failure("Cannot load dictionary " << argv[1] << '!');
  cerr << "done" << endl;

  cerr << "Processing data: ";

  int forms = 0;
  enum match_type { COMPLETE = 0, LEMMA_ID = 1, RAWLEMMA = 2, TAG = 3, MATCH_TYPES = 4 };
  int total_matches[MATCH_TYPES] = {};
  vector<tagged_lemma> lemmas;
  vector<tagged_lemma> matching_lemmas[MATCH_TYPES];
  unordered_map<string, string> lemma_mappings[MATCH_TYPES];

  string line;
  vector<string> tokens;
  while (getline(cin, line)) {
    if (line.empty()) {
      cout << endl;
      continue;
    }

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_failure("The line " << line << " does not contain three columns!");

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
      if (matches[COMPLETE] != 1) cerr << "W: matches[COMPLETE]=" << matches[COMPLETE] << endl;
      total_matches[COMPLETE]++;
      cout << tokens[0] << '\t' << tokens[1] << '\t' << tokens[2] << '\n';
    } else if (matches[LEMMA_ID]) {
      if (matches[LEMMA_ID] != 1) cerr << "W: matches[LEMMA_ID]=" << matches[COMPLETE] << endl;
      if (lemma_mappings[LEMMA_ID].emplace(tokens[1], matching_lemmas[LEMMA_ID][0].lemma).first->second != matching_lemmas[LEMMA_ID][0].lemma)
        {} //cerr("Two different lemmaid mappings for " << tokens[1] << ": " << matching_lemmas[LEMMA_ID][0].lemma << " and " << lemma_mappings[LEMMA_ID][tokens[1]] << '.' << endl;
      total_matches[LEMMA_ID]++;
      cout << tokens[0] << '\t' << matching_lemmas[LEMMA_ID][0].lemma << '\t' << matching_lemmas[LEMMA_ID][0].tag << '\n';
    } else if (matches[RAWLEMMA] == 1) {
      if (matches[RAWLEMMA] != 1) cerr << "W: matches[RAWLEMMA]=" << matches[COMPLETE] << endl;
      if (lemma_mappings[RAWLEMMA].emplace(tokens[1], matching_lemmas[RAWLEMMA][0].lemma).first->second != matching_lemmas[RAWLEMMA][0].lemma)
        {} //cerr("Two different rawlemma mappings for " << tokens[1] << ": " << matching_lemmas[RAWLEMMA][0].lemma << " and " << lemma_mappings[RAWLEMMA][tokens[1]] << '.' << endl;
      total_matches[RAWLEMMA]++;
      cout << tokens[0] << '\t' << matching_lemmas[RAWLEMMA][0].lemma << '\t' << matching_lemmas[RAWLEMMA][0].tag << '\n';
    } else if (lemmas.size() == 1 &&
               lemmas[0].tag.compare(0, 1, tokens[2], 0, 1) == 0 &&
               lemmas[0].lemma.compare(0, dictionary->lemma_id_len(lemmas[0].lemma), tokens[1], 0, lemmaid_len) == 0) {
      if (lemma_mappings[TAG].emplace(tokens[1], lemmas[0].tag).first->second != lemmas[0].tag)
        {} //cerr("Two different tag mappings for " << tokens[1] << '-' << tokens[2] << ": " << lemmas[0].tag << " and " << lemma_mappings[TAG][tokens[1]] << '.' << endl;
      total_matches[TAG]++;
      cout << tokens[0] << '\t' << lemmas[0].lemma << '\t' << lemmas[0].tag << '\n';
    } else
      cout << tokens[0] << '\t' << tokens[1] << '\t' << tokens[2] << '\n';
  }
  cerr << "done" << endl;

//  for (auto&& mode : {LEMMA_ID, RAWLEMMA, TAG}) {
//    cerr << "Remappings " << mode << endl;
//    vector<string> mappings;
//    for (auto&& map : lemma_mappings[mode])
//      mappings.emplace_back("  " + map.first + " -> " + map.second);
//    sort(mappings.begin(), mappings.end());
//    for (auto&& mapping : mappings)
//      cerr << mapping << endl;
//  }

  cerr << "Forms: " << forms << endl << fixed << setprecision(3);
  cerr << "Accuracy of all: " << 100 * (total_matches[COMPLETE] / double(forms)) << '%' << endl;
  cerr << "Accuracy of tli: " << 100 * ((total_matches[COMPLETE] + total_matches[LEMMA_ID]) / double(forms)) << '%' << endl;
  cerr << "Accuracy of trl: " << 100 * ((total_matches[COMPLETE] + total_matches[LEMMA_ID] + total_matches[RAWLEMMA]) / double(forms)) << '%' << endl;
  cerr << "Accuracy of t:   " << 100 * ((total_matches[COMPLETE] + total_matches[LEMMA_ID] + total_matches[RAWLEMMA] + total_matches[TAG]) / double(forms)) << '%' << endl;

  return 0;
}
