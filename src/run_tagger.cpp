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

using namespace ufal::morphodita;

static void tag_vertical(const tagger& tagger);
static void tag_untokenized(const tagger& tagger);

int main(int argc, char* argv[]) {
  bool use_tokenizer = false;

  int argi = 1;
  if (argi < argc && strcmp(argv[argi], "-t") == 0) argi++, use_tokenizer = true;
  if (argi + 1 < argc) runtime_errorf("Usage: %s [-t] tagger_file", argv[0]);

  eprintf("Loading tagger: ");
  unique_ptr<tagger> tagger(tagger::load(argv[argi]));
  if (!tagger) runtime_errorf("Cannot load tagger from file '%s'!", argv[argi]);
  eprintf("done\n");

  eprintf("Tagging: ");
  clock_t now = clock();
  if (use_tokenizer) tag_untokenized(*tagger);
  else tag_vertical(*tagger);
  eprintf("done, in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));

  return 0;
}

void tag_vertical(const tagger& tagger) {
  string line;

  vector<string> strings;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  for (bool not_eof = true; not_eof; ) {
    // Read sentence
    strings.clear();
    forms.clear();
    while ((not_eof = getline(stdin, line)) && !line.empty()) {
      auto tab = line.find('\t');
      strings.emplace_back(tab == string::npos ? line : line.substr(0, tab));
      forms.emplace_back(strings.back());
    }

    // Tag
    if (!forms.empty()) {
      tagger.tag(forms, tags);

      for (auto& tag : tags)
        printf("%s\t%s\n", tag.lemma.c_str(), tag.tag.c_str());
      putchar('\n');
    }
  }
}

static void encode_entities_and_print(const char* text, size_t length);

void tag_untokenized(const tagger& tagger) {
  string line, text;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  unique_ptr<tokenizer> tokenizer(tagger.get_morpho()->new_tokenizer());

  for (bool not_eof = true; not_eof; ) {
    // Read block of text
    text.clear();
    while ((not_eof = getline(stdin, line)) && !line.empty()) {
      text += line;
      text += '\n';
    }
    if (not_eof) text += '\n';

    // Tokenize and tag
    const char* unprinted = text.c_str();
    tokenizer->set_text(unprinted);
    while (tokenizer->next_sentence(&forms, nullptr)) {
      tagger.tag(forms, tags);

      for (unsigned i = 0; i < forms.size(); i++) {
        if (unprinted < forms[i].str) encode_entities_and_print(unprinted, forms[i].str - unprinted);
        printf("<form lemma='%s' tag='%s'>", tags[i].lemma.c_str(), tags[i].tag.c_str());
        fwrite(forms[i].str, 1, forms[i].len, stdout);
        fputs("</form>", stdout);
        unprinted = forms[i].str + forms[i].len;
      }
    }
    if (unprinted < text.c_str() + text.size()) encode_entities_and_print(unprinted, text.c_str() + text.size() - unprinted);
  }
}

void encode_entities_and_print(const char* text, size_t length) {
  const char* to_print = text;
  while (length) {
    while (length && *text != '<' && *text != '>' && *text != '&')
      text++, length--;

    if (length) {
      if (to_print < text) fwrite(to_print, 1, text - to_print, stdout);
      fputs(*text == '<' ? "&lt;" : *text == '>' ? "&gt;" : "&amp;", stdout);
      text++, length--;
      to_print = text;
    }
  }
  if (to_print < text) fwrite(to_print, 1, text - to_print, stdout);
}
