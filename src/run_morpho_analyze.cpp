// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <memory>

#include "morpho/morpho.h"
#include "tagger/tagger.h"
#include "tagset_converter/tagset_converter.h"
#include "utils/input.h"
#include "utils/output.h"
#include "utils/parse_int.h"
#include "utils/parse_options.h"
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void analyze_vertical(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter);
static void analyze_xml(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter);

int main(int argc, char* argv[]) {
  show_version_if_requested(argc, argv);

  options_map options;
  if (!parse_options({{"input",option_values{"untokenized", "vertical"}},
                      {"convert_tagset",option_values::any},
                      {"output",option_values{"vertical","xml"}},
                      {"from_tagger",option_values::none}}, argc, argv, options) ||
      argc < 3)
    runtime_errorf("Usage: %s [options] dict_file use_guesser [file[:output_file]]...\n"
                   "Options: --input=untokenized|vertical\n"
                   "         --convert_tagset=pdt_to_conll2009|strip_lemma_comment|strip_lemma_id\n"
                   "         --output=vertical|xml\n"
                   "         --from_tagger", argv[0]);

  unique_ptr<morpho> morpho;
  unique_ptr<tagger> tagger;
  if (options.count("from_tagger")) {
    eprintf("Loading dictionary from tagger: ");
    tagger.reset(tagger::load(argv[1]));
    if (!tagger) runtime_errorf("Cannot load tagger from file '%s'!", argv[1]);
  } else {
    eprintf("Loading dictionary: ");
    morpho.reset(morpho::load(argv[1]));
    if (!morpho) runtime_errorf("Cannot load dictionary from file '%s'!", argv[1]);
  }
  eprintf("done\n");
  auto& dictionary = options.count("from_tagger") ? *tagger->get_morpho() : *morpho;

  bool use_guesser = parse_int(argv[2], "use_guesser");

  unique_ptr<tokenizer> tokenizer(options.count("input") && options["input"] == "vertical" ? tokenizer::new_vertical_tokenizer() : dictionary.new_tokenizer());
  if (!tokenizer) runtime_errorf("No tokenizer is defined for the supplied model!");

  unique_ptr<tagset_converter> tagset_converter;
  if (options.count("convert_tagset")) {
    tagset_converter.reset(new_tagset_converter(options["convert_tagset"], dictionary));
    if (!tagset_converter) runtime_errorf("Unknown tag set converter '%s'!", options["convert_tagset"].c_str());
  } else {
    tagset_converter.reset(tagset_converter::new_identity_converter());
    if (!tagset_converter) runtime_errorf("Cannot create identity tag set converter!");
  }

  if (options.count("output") && options["output"] == "vertical")
    process_args(3, argc, argv, analyze_vertical, dictionary, use_guesser, *tokenizer, *tagset_converter);
  else
    process_args(3, argc, argv, analyze_xml, dictionary, use_guesser, *tokenizer, *tagset_converter);

  return 0;
}

void analyze_vertical(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> lemmas;

  while (getpara(in, para)) {
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      for (auto&& form : forms) {
        dictionary.analyze(form, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);
        tagset_converter.convert_analyzed(lemmas);

        fprintf(out, "%.*s", int(form.len), form.str);
        for (auto&& lemma : lemmas)
          fprintf(out, "\t%s\t%s", lemma.lemma.c_str(), lemma.tag.c_str());
        fputc('\n', out);
      }
      fputc('\n', out);
    }
  }
}

void analyze_xml(FILE* in, FILE* out, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter) {
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
        tagset_converter.convert_analyzed(lemmas);

        if (unprinted < forms[i].str) print_xml_content(out, unprinted, forms[i].str - unprinted);
        if (!i) fputs("<sentence>", out);
        fputs("<token>", out);
        for (auto&& lemma : lemmas) {
          fputs("<analysis lemma=\"", out);
          print_xml_content(out, lemma.lemma.c_str(), lemma.lemma.size());
          fputs("\" tag=\"", out);
          print_xml_content(out, lemma.tag.c_str(), lemma.tag.size());
          fputs("\"/>", out);
        }
        print_xml_content(out, forms[i].str, forms[i].len);
        fputs("</token>", out);
        if (i + 1 == forms.size()) fputs("</sentence>", out);
        unprinted = forms[i].str + forms[i].len;
      }

    if (unprinted < para.c_str() + para.size()) print_xml_content(out, unprinted, para.c_str() + para.size() - unprinted);
  }
}
