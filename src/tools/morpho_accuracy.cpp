// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

  int forms = 0, tags[2] = {}, unique_analyses[2] = {};
  int match_tag[2] = {}, match_tag_rawlemma[2] = {}, match_tag_lemmaid[2] = {}, match_all[2] = {};
  vector<tagged_lemma> lemmas;
  vector<const char*> wrong_lemmas;

  string line;
  vector<string> tokens;
  while (getline(cin, line)) {
    if (line.empty()) continue;

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_failure("The line " << line << " does not contain three columns!");

    // Analyse
    int guesser = dictionary->analyze(tokens[0], morpho::GUESSER, lemmas) == morpho::GUESSER;

    // Update statistics
    forms++;
    tags[guesser] += lemmas.size();
    unique_analyses[guesser] += lemmas.size() == 1;
    int rawlemma_len = dictionary->raw_lemma_len(tokens[1]);
    int lemmaid_len = dictionary->lemma_id_len(tokens[1]);
    int tag = 0, tag_rawlemma = 0, tag_lemmaid = 0, all = 0;
    wrong_lemmas.clear();
    for (auto&& lemma : lemmas) {
      if (lemma.tag != tokens[2]) continue;
      tag = 1;
      if (lemma.lemma.compare(0, dictionary->raw_lemma_len(lemma.lemma), tokens[1], 0, rawlemma_len) != 0) continue;
      tag_rawlemma = 1;
      if (lemma.lemma.compare(0, dictionary->lemma_id_len(lemma.lemma), tokens[1], 0, lemmaid_len) != 0) {
        wrong_lemmas.push_back(lemma.lemma.c_str());
        continue;
      }
      tag_lemmaid = 1;
      if (lemma.lemma.compare(tokens[1]) != 0) {
//        cerr << "Wrong comments in lemma '" << lemma.lemma << "' versus golden '" << tokens[1] << "' of form '" << tokens[0] << '\'' << endl;
        continue;
      }
      all = 1;
    }
//    if (tag_rawlemma && !tag_lemmaid) {
//      cerr << "Cannot match lemma '" << tokens[1] << "' of form '" << tokens[0] << "', got only";
//      for (auto&& wrong_lemma : wrong_lemmas)
//        cerr << ' ' << wrong_lemma;
//      cerr << endl;
//    }
    match_tag[guesser] += tag;
    match_tag_rawlemma[guesser] += tag_rawlemma;
    match_tag_lemmaid[guesser] += tag_lemmaid;
    match_all[guesser] += all;

//    if (!tag) {
//      if (lemmas.size() > 1)
//        cerr << "Could not match form '" << tokens[0] << "' with lemma '" << tokens[1] << "' and tag '" << tokens[2] << '\'' << endl;
//    }
  }
  cerr << "done" << endl;

  cout << "Forms: " << forms << endl;
  cout << fixed << setprecision(2);
  cout << "Recognized tags: " << tags[0] << " (avg " << (tags[0] / double(forms)) << " per form),"
      << "with guesser: " << tags[0] + tags[1] << " (avg " << (tags[0] + tags[1]) / double(forms) << " per form)" << endl;
  cout << "Unique analyses: " << 100 * unique_analyses[0] / double(forms) << "%, with guesser " << 100 * (unique_analyses[0] + unique_analyses[1]) / double(forms) << '%' << endl;
  cout << fixed << setprecision(3);
  cout << "Accuracy of tags: " << 100 * match_tag[0] / double(forms) << "%, with guesser " << 100 * (match_tag[0] + match_tag[1]) / double(forms) << '%' << endl;
  cout << "Accuracy of trl:  " << 100 * match_tag_rawlemma[0] / double(forms) << "%, with guesser " << 100 * (match_tag_rawlemma[0] + match_tag_rawlemma[1]) / double(forms) << '%' << endl;
  cout << "Accuracy of tli:  " << 100 * match_tag_lemmaid[0] / double(forms) << "%, with guesser " << 100 * (match_tag_lemmaid[0] + match_tag_lemmaid[1]) / double(forms) << '%' << endl;
  cout << "Accuracy of all:  " << 100 * match_all[0] / double(forms) << "%, with guesser " << 100 * (match_all[0] + match_all[1]) / double(forms) << '%' << endl;

  return 0;
}
