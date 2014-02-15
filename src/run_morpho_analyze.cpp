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
#include "utils/output.h"
#include "utils/parse_int.h"
#include "utils/parse_options.h"
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void analyze_vertical(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer);
static void analyze_xml(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer);

int main(int argc, char* argv[]) {
  options_map options;
  if (!parse_options({{"input",{"untokenized", "vertical"}},
                      {"convert_tagset",{""}},
                      {"output",{"vertical","xml"}}}, argc, argv, options) ||
      argc < 3)
    runtime_errorf("Usage: %s [options] dict_file use_guesser [file[:output_file]]...\n"
                   "Options: --input=untokenized|vertical\n"
                   "         --convert_tagset=pdt_to_conll2009\n"
                   "         --output=vertical|xml", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary from file '%s'!", argv[1]);
  eprintf("done\n");

  bool use_guesser = parse_int(argv[2], "use_guesser");

  unique_ptr<tokenizer> tokenizer(options.count("input") && options["input"] == "vertical" ? tokenizer::new_vertical_tokenizer() : dictionary->new_tokenizer());
  if (!tokenizer) runtime_errorf("Cannot create tokenizer!");

  if (options.count("output") && options["output"] == "vertical") process_args(3, argc, argv, analyze_vertical, *dictionary, use_guesser, *tokenizer);
  else process_args(3, argc, argv, analyze_xml, *dictionary, use_guesser, *tokenizer);

  return 0;
}

void analyze_vertical(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> lemmas;

  while (getpara(in, para)) {
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      for (auto&& form : forms) {
        dictionary.analyze(form, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);

        bool first = true;
        for (auto&& lemma : lemmas) {
          if (!first) fputc('\t', out);
          fprintf(out, "%s\t%s", lemma.lemma.c_str(), lemma.tag.c_str());
          first = false;
        }
        fputc('\n', out);
      }
      fputc('\n', out);
    }
  }
}

void analyze_xml(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> lemmas;

  while (getpara(in, para)) {
    // Tokenize and analyze
    tokenizer.set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer.next_sentence(&forms, nullptr))
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
