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

#include <memory>

#include "morpho/morpho.h"
#include "utils/input.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s dict_file <data", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> d(morpho::load(argv[1]));
  if (!d) runtime_errorf("Cannot load dictionary %s!", argv[1]);
  eprintf("done\n");

  eprintf("Processing data: ");

  int forms = 0, tags[2] = {}, unique_analyses[2] = {};
  int match_tag[2] = {}, match_tag_rawlemma[2] = {}, match_tag_lemmaid[2] = {}, match_all[2] = {};
  vector<tagged_lemma> lemmas;
  vector<const char*> wrong_lemmas;

  string line;
  vector<string> tokens;
  while (getline(stdin, line)) {
    if (line.empty()) continue;

    split(line, '\t', tokens);
    if (tokens.size() != 3) runtime_errorf("The line %s does not contain three columns!", line.c_str());

    // Analyse
    int guesser = d->analyze(tokens[0].c_str(), tokens[0].size(), morpho::GUESSER, lemmas) == morpho::GUESSER;

    // Update statistics
    forms++;
    tags[guesser] += lemmas.size();
    unique_analyses[guesser] += lemmas.size() == 1;
    int rawlemma_len = d->raw_lemma_len(tokens[1].c_str(), tokens[1].size());
    int lemmaid_len = d->lemma_id_len(tokens[1].c_str(), tokens[1].size());
    int tag = 0, tag_rawlemma = 0, tag_lemmaid = 0, all = 0;
    wrong_lemmas.clear();
    for (auto& lemma : lemmas) {
      if (lemma.tag != tokens[2]) continue;
      tag = 1;
      if (lemma.lemma.compare(0, d->raw_lemma_len(lemma.lemma.c_str(), lemma.lemma.size()), tokens[1], 0, rawlemma_len) != 0) continue;
      tag_rawlemma = 1;
      if (lemma.lemma.compare(0, d->lemma_id_len(lemma.lemma.c_str(), lemma.lemma.size()), tokens[1], 0, lemmaid_len) != 0) {
        wrong_lemmas.push_back(lemma.lemma.c_str());
        continue;
      }
      tag_lemmaid = 1;
      if (lemma.lemma.compare(tokens[1]) != 0) {
//        eprintf("Wrong comments in lemma '%s' versus golden '%s' of form '%s'\n", lemma.lemma.c_str(), tokens[1], tokens[0]);
        continue;
      }
      all = 1;
    }
//    if (tag_rawlemma && !tag_lemmaid) {
//      eprintf("Cannot match lemma '%s' of form '%s', got only", tokens[1], tokens[0]);
//      for (auto& wrong_lemma : wrong_lemmas)
//        eprintf(" '%s'", wrong_lemma);
//      eprintf("\n");
//    }
    match_tag[guesser] += tag;
    match_tag_rawlemma[guesser] += tag_rawlemma;
    match_tag_lemmaid[guesser] += tag_lemmaid;
    match_all[guesser] += all;

//    if (!tag) {
//      if (lemmas.size() > 1)
//        eprintf("Could not match form '%s' with lemma '%s' and tag '%s'\n", tokens[0], tokens[1], tokens[2]);
//    }
  }
  eprintf("done\n");

  printf("Forms: %d\n", forms);
  printf("Recognized tags: %d (avg %.2f per form), with guesser %d (avg %.2f per form)\n", tags[0], tags[0] / double(forms), tags[0] + tags[1], (tags[0] + tags[1]) / double(forms));
  printf("Unique analyses: %.2f%%, with guesser %.2f%%\n", 100. * unique_analyses[0] / double(forms), 100. * (unique_analyses[0] + unique_analyses[1]) / double(forms));
  printf("Accuracy of tags: %.3f%%, with guesser %.3f%%\n", 100 * match_tag[0] / double(forms), 100 * (match_tag[0] + match_tag[1]) / double(forms));
  printf("Accuracy of trl:  %.3f%%, with guesser %.3f%%\n", 100 * match_tag_rawlemma[0] / double(forms), 100 * (match_tag_rawlemma[0] + match_tag_rawlemma[1]) / double(forms));
  printf("Accuracy of tli:  %.3f%%, with guesser %.3f%%\n", 100 * match_tag_lemmaid[0] / double(forms), 100 * (match_tag_lemmaid[0] + match_tag_lemmaid[1]) / double(forms));
  printf("Accuracy of all:  %.3f%%, with guesser %.3f%%\n", 100 * match_all[0] / double(forms), 100 * (match_all[0] + match_all[1]) / double(forms));

  return 0;
}
