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
#include "utils/parse_int.h"
#include "utils/process_args.h"
#include "utils/output.h"

using namespace ufal::morphodita;

static void analyze_vertical(FILE* in, FILE* out, morpho& dictionary, bool use_guesser);
static void analyze_untokenized(FILE* in, FILE* out, morpho& dictionary, bool use_guesser);

int main(int argc, char* argv[]) {
  bool use_vertical = false;

  int argi = 1;
  if (argi < argc && strcmp(argv[argi], "-v") == 0) argi++, use_vertical = true;
  if (argi + 2 > argc) runtime_errorf("Usage: %s [-v] dict_file use_guesser [file[:output_file]]...", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[argi]));
  if (!dictionary) runtime_errorf("Cannot load dictionary from file '%s'!", argv[argi]);
  eprintf("done\n");
  argi++;

  bool use_guesser = parse_int(argv[argi++], "use_guesser");
  if (use_vertical) process_args(argi, argc, argv, analyze_vertical, *dictionary, use_guesser);
  else process_args(argi, argc, argv, analyze_untokenized, *dictionary, use_guesser);

  return 0;
}

void analyze_vertical(FILE* in, FILE* out, morpho& dictionary, bool use_guesser) {
  string line;
  vector<tagged_lemma> lemmas;

  while (getline(in, line)) {
    if (!line.empty()) {
      dictionary.analyze(line, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);

      bool first = true;
      for (auto&& lemma : lemmas) {
        if (!first) fputc('\t', out);
        fprintf(out, "%s\t%s", lemma.lemma.c_str(), lemma.tag.c_str());
        first = false;
      }
    }
    fputc('\n', out);
  }
}

void analyze_untokenized(FILE* in, FILE* out, morpho& dictionary, bool use_guesser) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> lemmas;

  unique_ptr<tokenizer> tokenizer(dictionary.new_tokenizer());

  while (getpara(in, para)) {
    // Tokenize and analyze
    tokenizer->set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer->next_sentence(&forms, nullptr))
      for (unsigned i = 0; i < forms.size(); i++) {
        dictionary.analyze(forms[i], use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);

        if (unprinted < forms[i].str) print_xml_content(out, unprinted, forms[i].str - unprinted);
        fputs("<form>", out);
        for (auto&& lemma : lemmas) {
          fputs("<analysis lemma=\"", out);
          print_xml_content(out, lemma.lemma.c_str(), lemma.lemma.size());
          fputs("\" tag=\"", out);
          print_xml_content(out, lemma.tag.c_str(), lemma.tag.size());
          fputs("\"/>", out);
        }
        print_xml_content(out, forms[i].str, forms[i].len);
        fputs("</form>", out);
        unprinted = forms[i].str + forms[i].len;
      }

    if (unprinted < para.c_str() + para.size()) print_xml_content(out, unprinted, para.c_str() + para.size() - unprinted);
  }
}
