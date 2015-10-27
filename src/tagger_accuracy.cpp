// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>

#include "tagger/tagger.h"
#include "utils/input.h"
#include "utils/parse_options.h"

using namespace ufal::morphodita;

struct word {
  string form, lemma, tag;

  word(const string& form, const string& lemma, const string& tag) : form(form), lemma(lemma), tag(tag) {}
};

int main(int argc, char* argv[]) {
  show_version_if_requested(argc, argv);

  if (argc <= 1) runtime_errorf("Usage: %s tagger_file", argv[0]);

  eprintf("Loading tagger: ");
  unique_ptr<tagger> tagger(tagger::load(argv[1]));
  if (!tagger) runtime_errorf("Cannot load tagger from file '%s'!", argv[1]);
  eprintf("done\n");

  eprintf("Tagging: ");
  clock_t now = clock();

  vector<word> sentence;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;
  enum { TAGS, LEMMAS, BOTH, TOTAL };
  int correct[TOTAL] = {}, total = 0;

  string line;
  vector<string> tokens;
  for (bool eof; true; ) {
    eof = !getline(stdin, line);
    if (eof || line.empty()) {
      // End of sentence
      if (!sentence.empty()) {
        // Nonempty sentence, perform tagging
        for (auto&& w : sentence)
          forms.emplace_back(w.form.c_str(), w.form.size());

        tagger->tag(forms, tags);

        for (unsigned i = 0; i < tags.size(); i++) {
          correct[TAGS] += tags[i].tag == sentence[i].tag;
          correct[LEMMAS] += tags[i].lemma == sentence[i].lemma;
          correct[BOTH] += tags[i].lemma == sentence[i].lemma && tags[i].tag == sentence[i].tag;
          total++;
        }

        sentence.clear();
        forms.clear();
      }
      if (eof) break;
    } else {
      // Just add a word to the sentence
      split(line, '\t', tokens);
      if (tokens.size() != 3) runtime_errorf("Input line '%s' does not contain 3 columns!", line.c_str());

      sentence.emplace_back(tokens[0], tokens.size() > 1 ? tokens[1] : string(), tokens.size() > 2 ? tokens[2] : string());
    }
  }

  eprintf("done, in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));
  eprintf("Accuracy: %.2f%% tags, %.2f%% lemmas, %.2f%% both.\n", 100 * correct[TAGS] / double(total), 100 * correct[LEMMAS] / double(total), 100 * correct[BOTH] / double(total));

  return 0;
}
