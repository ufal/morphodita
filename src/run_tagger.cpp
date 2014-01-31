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
#include "utils/output.h"
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void tag_vertical(FILE* in, FILE* out, const tagger& tagger);
static void tag_untokenized(FILE* in, FILE* out, const tagger& tagger);

int main(int argc, char* argv[]) {
  bool use_vertical = false;

  int argi = 1;
  if (argi < argc && strcmp(argv[argi], "-v") == 0) argi++, use_vertical = true;
  if (argi >= argc) runtime_errorf("Usage: %s [-v] tagger_file [file[:output_file]]...", argv[0]);

  eprintf("Loading tagger: ");
  unique_ptr<tagger> tagger(tagger::load(argv[argi]));
  if (!tagger) runtime_errorf("Cannot load tagger from file '%s'!", argv[argi]);
  eprintf("done\n");
  argi++;

  eprintf("Tagging: ");
  clock_t now = clock();
  if (use_vertical) process_args(argi, argc, argv, tag_vertical, *tagger);
  else process_args(argi, argc, argv, tag_untokenized, *tagger);
  eprintf("done, in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));

  return 0;
}

void tag_vertical(FILE* in, FILE* out, const tagger& tagger) {
  string line;

  vector<string> words;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  for (bool not_eof = true; not_eof; ) {
    // Read sentence
    words.clear();
    forms.clear();
    while ((not_eof = getline(in, line)) && !line.empty()) {
      words.emplace_back(line);
      forms.emplace_back(words.back());
    }

    // Tag
    if (!forms.empty()) {
      tagger.tag(forms, tags);

      for (auto&& tag : tags)
        fprintf(out, "%s\t%s\n", tag.lemma.c_str(), tag.tag.c_str());
      fputc('\n', out);
    }
  }
}

void tag_untokenized(FILE* in, FILE* out, const tagger& tagger) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  unique_ptr<tokenizer> tokenizer(tagger.new_tokenizer());
  if (!tokenizer) runtime_errorf("No tokenizer is defined for the supplied model!");

  while (getpara(in, para)) {
    // Tokenize and tag
    tokenizer->set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer->next_sentence(&forms, nullptr)) {
      tagger.tag(forms, tags);

      for (unsigned i = 0; i < forms.size(); i++) {
        if (unprinted < forms[i].str) print_xml_content(out, unprinted, forms[i].str - unprinted);
        fputs("<form lemma=\"", out);
        print_xml_content(out, tags[i].lemma.c_str(), tags[i].lemma.size());
        fputs("\" tag=\"", out);
        print_xml_content(out, tags[i].tag.c_str(), tags[i].tag.size());
        fputs("\">", out);
        print_xml_content(out, forms[i].str, forms[i].len);
        fputs("</form>", out);
        unprinted = forms[i].str + forms[i].len;
      }
    }
    if (unprinted < para.c_str() + para.size()) print_xml_content(out, unprinted, para.c_str() + para.size() - unprinted);
  }
}
