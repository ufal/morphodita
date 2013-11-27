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

#include <cstring>
#include <ctime>
#include <memory>

#include "tagger/tagger.h"
#include "utils/input.h"
#include "utils/tokenizer/tokenizer.h"

using namespace ufal::morphodita;

bool next_sentence(FILE* f, const tokenizer<raw_form>* t, vector<raw_form>& raw_forms) {
  static string line;

  if (t) {
    // Use tokenizer
    static string text;
    static const char* unprocessed_text = nullptr;
    if (unprocessed_text && t->next_sentence(unprocessed_text, raw_forms)) return true;

    // Read data until an empty line is found
    text.clear();
    while (getline(f, line))
      if (!line.empty()) {
        if (!text.empty()) text += '\n';
        text += line;
      } else if (!text.empty())
        break;

    unprocessed_text = text.c_str();
    return t->next_sentence(unprocessed_text, raw_forms);
  } else {
    // Use vertical format
    static vector<string> forms;

    forms.clear();
    raw_forms.clear();
    while (getline(f, line))
      if (!line.empty()) {
        auto tab = line.find('\t');
        forms.emplace_back(tab == string::npos ? line : line.substr(0, tab));
        raw_forms.emplace_back(forms.back().c_str(), forms.back().size());
      } else if (!forms.empty()) {
        break;
      }

    return !forms.empty();
  }
}

int main(int argc, char* argv[]) {
  if (argc <= 1 || (strcmp(argv[1], "-t") == 0 && argc <= 3))
    runtime_errorf("Usage: %s [-t tokenizer_id] tagger_file", argv[0]);

  int argi = 1;
  unique_ptr<tokenizer<raw_form>> tok;
  if (strcmp(argv[argi], "-t") == 0) {
    tokenizer_id id;
    if (!tokenizer_ids::parse(argv[argi + 1], id)) runtime_errorf("Unknown tokenizer_id '%s'!", argv[argi + 1]);
    tok.reset(tokenizer<raw_form>::create(id, /*split_hyphenated_words*/ false));
    if (!tok) runtime_errorf("Cannot create tokenizer '%s'!", argv[argi + 1]);
    argi += 2;
  }

  eprintf("Loading tagger: ");
  unique_ptr<tagger> t(tagger::load(argv[argi]));
  if (!t) runtime_errorf("Cannot load tagger from file '%s'!", argv[argi]);
  eprintf("done\n");

  eprintf("Tagging: ");
  clock_t now = clock();

  vector<raw_form> raw_forms;
  vector<tagged_lemma> tags;

  while (next_sentence(stdin, tok.get(), raw_forms)) {
    t->tag(raw_forms, tags);

    for (auto& tag : tags)
      printf("%s\t%s\n", tag.lemma.c_str(), tag.tag.c_str());
    putchar('\n');
  }

  eprintf("done, in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));

  return 0;
}
