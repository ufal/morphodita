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
#include "tagger/tagger.h"
#include "utils/input.h"
#include "utils/output.h"
#include "utils/parse_options.h"
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void tokenize_vertical(FILE* in, FILE* out, tokenizer& tokenizer);
static void tokenize_xml(FILE* in, FILE* out, tokenizer& tokenizer);

int main(int argc, char* argv[]) {
  options_map options;
  bool show_usage = false;
  show_usage = !parse_options({{"tokenizer", {"czech", "english", "generic"}},
                              {"morphology", {""}},
                              {"tagger", {""}},
                              {"output",{"vertical","xml"}}}, argc, argv, options);
  if (!show_usage && (options.count("tokenizer") + options.count("morphology") + options.count("tagger")) == 0) {
    eprintf("Missing one of --tokenizer, --morphology and --tagger options!\n");
    show_usage = true;
  }
  if (!show_usage && (options.count("tokenizer") + options.count("morphology") + options.count("tagger")) > 1) {
    eprintf("Only one of --tokenizer, --morphology and --tagger options can be specifed!\n");
    show_usage = true;
  }
  if (show_usage)
    runtime_errorf("Usage: %s [options] [file[:output_file]]...\n"
                   "Options: --tokenizer=czech|english|generic\n"
                   "         --morphology=morphology_model_file\n"
                   "         --tagger=tagger_model_file\n"
                   "         --output=vertical|xml", argv[0]);

  unique_ptr<tokenizer> tokenizer;
  if (options.count("tokenizer") && options["tokenizer"] == "czech") tokenizer.reset(tokenizer::new_czech_tokenizer());
  else if (options.count("tokenizer") && options["tokenizer"] == "english") tokenizer.reset(tokenizer::new_english_tokenizer());
  else if (options.count("tokenizer") && options["tokenizer"] == "generic") tokenizer.reset(tokenizer::new_generic_tokenizer());
  else if (options.count("morphology")) {
    eprintf("Loading dictionary: ");
    unique_ptr<morpho> dictionary(morpho::load(options["morphology"].c_str()));
    if (!dictionary) runtime_errorf("Cannot load dictionary from file '%s'!", options["morphology"].c_str());
    eprintf("done\n");

    tokenizer.reset(dictionary->new_tokenizer());
    if (!tokenizer) runtime_errorf("No tokenizer is defined for the supplied model!");
  } else /*if (options.count("tagger"))*/ {
    eprintf("Loading tagger: ");
    unique_ptr<tagger> tagger(tagger::load(options["tagger"].c_str()));
    if (!tagger) runtime_errorf("Cannot load dictionary from file '%s'!", options["tagger"].c_str());
    eprintf("done\n");

    tokenizer.reset(tagger->new_tokenizer());
    if (!tokenizer) runtime_errorf("No tokenizer is defined for the supplied model!");
  }

  if (options.count("output") && options["output"] == "vertical") process_args(1, argc, argv, tokenize_vertical, *tokenizer);
  else process_args(1, argc, argv, tokenize_xml, *tokenizer);

  return 0;
}

void tokenize_vertical(FILE* in, FILE* out, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  while (getpara(in, para)) {
    // Tokenize
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      for (auto&& form : forms) {
        fwrite(form.str, 1, form.len, out);
        fputc('\n', out);
      }
      fputc('\n', out);
    }
  }
}

static void tokenize_xml(FILE* in, FILE* out, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  while (getpara(in, para)) {
    // Tokenize
    tokenizer.set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer.next_sentence(&forms, nullptr))
      for (unsigned i = 0; i < forms.size(); i++) {
        if (unprinted < forms[i].str) print_xml_content(out, unprinted, forms[i].str - unprinted);
        if (!i) fputs("<sentence>", out);
        fputs("<token>", out);
        print_xml_content(out, forms[i].str, forms[i].len);
        fputs("</token>", out);
        if (i + 1 == forms.size()) fputs("</sentence>", out);
        unprinted = forms[i].str + forms[i].len;
      }

    if (unprinted < para.c_str() + para.size()) print_xml_content(out, unprinted, para.c_str() + para.size() - unprinted);
  }
}
