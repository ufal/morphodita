// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>
#include <ctime>

#include "tagger/tagger.h"
#include "tagset_converter/tagset_converter.h"
#include "utils/iostreams.h"
#include "utils/iostreams_xml.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "version/version.h"

using namespace ufal::morphodita;

static void tag_vertical(istream& is, ostream& os, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter);
static void tag_xml(istream& is, ostream& os, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter);

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"input",options::value{"untokenized", "vertical"}},
                       {"convert_tagset",options::value::any},
                       {"output",options::value{"vertical","xml"}},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] tagger_file [file[:output_file]]...\n"
                    "Options: --input=untokenized|vertical\n"
                    "         --convert_tagset=pdt_to_conll2009|strip_lemma_comment|strip_lemma_id\n"
                    "         --output=vertical|xml\n"
                    "         --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading tagger: ";
  unique_ptr<tagger> tagger(tagger::load(argv[1]));
  if (!tagger) runtime_failure("Cannot load tagger from file '" << argv[1] << "'!");
  cerr << "done" << endl;

  unique_ptr<tokenizer> tokenizer(options.count("input") && options["input"] == "vertical" ? tokenizer::new_vertical_tokenizer() : tagger->new_tokenizer());
  if (!tokenizer) runtime_failure("No tokenizer is defined for the supplied model!");

  unique_ptr<tagset_converter> tagset_converter;
  if (options.count("convert_tagset")) {
    tagset_converter.reset(new_tagset_converter(options["convert_tagset"], *tagger->get_morpho()));
    if (!tagset_converter) runtime_failure("Unknown tag set converter '" << options["convert_tagset"] << "'!");
  } else {
    tagset_converter.reset(tagset_converter::new_identity_converter());
    if (!tagset_converter) runtime_failure("Cannot create identity tag set converter!");
  }

  clock_t now = clock();
  if (options.count("output") && options["output"] == "vertical") process_args(2, argc, argv, tag_vertical, *tagger, *tokenizer, *tagset_converter);
  else process_args(2, argc, argv, tag_xml, *tagger, *tokenizer, *tagset_converter);
  cerr << "Tagging done in " << fixed << setprecision(3) << (clock() - now) / double(CLOCKS_PER_SEC) << " seconds." << endl;

  return 0;
}

void tag_vertical(istream& is, ostream& os, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  while (getpara(is, para)) {
    // Tokenize and tag
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      tagger.tag(forms, tags);

      for (unsigned i = 0; i < tags.size(); i++) {
        tagset_converter.convert(tags[i]);
        os << forms[i] << '\t' << tags[i].lemma << '\t' << tags[i].tag << '\n';
      }
      os << endl;
    }
  }
}

void tag_xml(istream& is, ostream& os, const tagger& tagger, tokenizer& tokenizer, const tagset_converter& tagset_converter) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  while (getpara(is, para)) {
    // Tokenize and tag
    tokenizer.set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer.next_sentence(&forms, nullptr)) {
      tagger.tag(forms, tags);

      for (unsigned i = 0; i < forms.size(); i++) {
        tagset_converter.convert(tags[i]);

        os << xml_encoded(string_piece(unprinted, forms[i].str - unprinted));
        if (!i) os << "<sentence>";
        os << "<token lemma=\"" << xml_encoded(tags[i].lemma, true) << "\" tag=\"" << xml_encoded(tags[i].tag, true) << "\">"
           << xml_encoded(forms[i]) << "</token>";
        if (i + 1 == forms.size()) os << "</sentence>";
        unprinted = forms[i].str + forms[i].len;
      }
    }
    os << xml_encoded(string_piece(unprinted, para.c_str() + para.size() - unprinted)) << flush;
  }
}
