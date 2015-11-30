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
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/split.h"
#include "version/version.h"

using namespace ufal::morphodita;

struct word {
  string form, lemma, tag;

  word(const string& form, const string& lemma, const string& tag) : form(form), lemma(lemma), tag(tag) {}
};

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " tagger_file\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading tagger: ";
  unique_ptr<tagger> tagger(tagger::load(argv[1]));
  if (!tagger) runtime_failure("Cannot load tagger from file '" << argv[1] << "'!");
  cerr << "done" << endl;

  cerr << "Tagging: ";
  clock_t now = clock();

  vector<word> sentence;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;
  enum { TAGS, LEMMAS, BOTH, TOTAL };
  int correct[TOTAL] = {}, total = 0;

  string line;
  vector<string> tokens;
  for (bool eof; true; ) {
    eof = !getline(cin, line);
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
      if (tokens.size() != 3) runtime_failure("Input line '" << line << "' does not contain 3 columns!");

      sentence.emplace_back(tokens[0], tokens.size() > 1 ? tokens[1] : string(), tokens.size() > 2 ? tokens[2] : string());
    }
  }

  cerr << "done, in " << fixed << setprecision(3) << (clock() - now) / double(CLOCKS_PER_SEC) << " seconds." << endl;
  cerr << "Accuracy: " << fixed << setprecision(2)
      << 100 * correct[TAGS] / double(total) << "% tags, "
      << 100 * correct[LEMMAS] / double(total) << "% lemmas, "
      << 100 * correct[BOTH] / double(total) << "% both." << endl;

  return 0;
}
