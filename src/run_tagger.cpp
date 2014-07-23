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
#include "tagset_converter/tagset_converter.h"
#include "utils/input.h"
#include "utils/output.h"
#include "utils/parse_options.h"
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void tag_vertical(FILE* in, FILE* out, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter);
static void tag_xml(FILE* in, FILE* out, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter);

int main(int argc, char* argv[]) {
  show_version_if_requested(argc, argv);

  options_map options;
  if (!parse_options({{"input",option_values{"untokenized", "vertical"}},
                      {"convert_tagset",option_values::any},
                      {"output",option_values{"vertical","xml"}}}, argc, argv, options) ||
      argc < 2)
    runtime_errorf("Usage: %s [options] tagger_file [file[:output_file]]...\n"
                   "Options: --input=untokenized|vertical\n"
                   "         --convert_tagset=pdt_to_conll2009|strip_lemma_comment|strip_lemma_id\n"
                   "         --output=vertical|xml", argv[0]);

  eprintf("Loading tagger: ");
  unique_ptr<tagger> tagger(tagger::load(argv[1]));
  if (!tagger) runtime_errorf("Cannot load tagger from file '%s'!", argv[1]);
  eprintf("done\n");

  unique_ptr<tokenizer> tokenizer(options.count("input") && options["input"] == "vertical" ? tokenizer::new_vertical_tokenizer() : tagger->new_tokenizer());
  if (!tokenizer) runtime_errorf("No tokenizer is defined for the supplied model!");

  unique_ptr<tagset_converter> tagset_converter;
  if (options.count("convert_tagset")) {
    tagset_converter.reset(new_tagset_converter(options["convert_tagset"], *tagger->get_morpho()));
    if (!tagset_converter) runtime_errorf("Unknown tag set converter '%s'!", options["convert_tagset"].c_str());
  } else {
    tagset_converter.reset(tagset_converter::new_identity_converter());
    if (!tagset_converter) runtime_errorf("Cannot create identity tag set converter!");
  }

  clock_t now = clock();
  if (options.count("output") && options["output"] == "vertical") process_args(2, argc, argv, tag_vertical, *tagger, *tokenizer, *tagset_converter);
  else process_args(2, argc, argv, tag_xml, *tagger, *tokenizer, *tagset_converter);
  eprintf("Tagging done, in %.3f seconds.\n", (clock() - now) / double(CLOCKS_PER_SEC));

  return 0;
}

void tag_vertical(FILE* in, FILE* out, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  while (getpara(in, para)) {
    // Tokenize and tag
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      tagger.tag(forms, tags);

      for (unsigned i = 0; i < tags.size(); i++) {
        tagset_converter.convert(tags[i]);
        fprintf(out, "%.*s\t%s\t%s\n", int(forms[i].len), forms[i].str, tags[i].lemma.c_str(), tags[i].tag.c_str());
      }
      fputc('\n', out);
    }
  }
}

void tag_xml(FILE* in, FILE* out, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  while (getpara(in, para)) {
    // Tokenize and tag
    tokenizer.set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer.next_sentence(&forms, nullptr)) {
      tagger.tag(forms, tags);

      for (unsigned i = 0; i < forms.size(); i++) {
        tagset_converter.convert(tags[i]);

        if (unprinted < forms[i].str) print_xml_content(out, unprinted, forms[i].str - unprinted);
        if (!i) fputs("<sentence>", out);
        fputs("<token lemma=\"", out);
        print_xml_content(out, tags[i].lemma.c_str(), tags[i].lemma.size());
        fputs("\" tag=\"", out);
        print_xml_content(out, tags[i].tag.c_str(), tags[i].tag.size());
        fputs("\">", out);
        print_xml_content(out, forms[i].str, forms[i].len);
        fputs("</token>", out);
        if (i + 1 == forms.size()) fputs("</sentence>", out);
        unprinted = forms[i].str + forms[i].len;
      }
    }
    if (unprinted < para.c_str() + para.size()) print_xml_content(out, unprinted, para.c_str() + para.size() - unprinted);
  }
}
