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

#include <ctime>
#include <memory>

#include "tagger/tagger.h"
#include "utils/input.h"

using namespace ufal::morphodita;

struct word {
  string form, lemma, tag;

  word(const string& form, const string& lemma, const string& tag) : form(form), lemma(lemma), tag(tag) {}
};

int main(int argc, char* argv[]) {
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
          printf("%s\t%s\n", tags[i].lemma.c_str(), tags[i].tag.c_str());
          correct[TAGS] += tags[i].tag == sentence[i].tag;
          correct[LEMMAS] += tags[i].lemma == sentence[i].lemma;
          correct[BOTH] += tags[i].lemma == sentence[i].lemma && tags[i].tag == sentence[i].tag;
          total++;
        }

        sentence.clear();
        forms.clear();
      }
      if (eof) break;
      putchar('\n');
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
