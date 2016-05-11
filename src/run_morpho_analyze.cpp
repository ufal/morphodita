// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "derivator/derivation_formatter.h"
#include "morpho/morpho.h"
#include "tagger/tagger.h"
#include "tagset_converter/tagset_converter.h"
#include "utils/getpara.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "utils/process_args.h"
#include "utils/xml_encoded.h"
#include "version/version.h"

using namespace ufal::morphodita;

static void analyze_vertical(istream& is, ostream& os, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter, const derivation_formatter& derivation);
static void analyze_xml(istream& is, ostream& os, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter, const derivation_formatter& derivation);

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"input",options::value{"untokenized", "vertical"}},
                       {"convert_tagset",options::value::any},
                       {"derivation",options::value{"none", "root", "path", "tree"}},
                       {"output",options::value{"vertical","xml"}},
                       {"from_tagger",options::value::none},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 3 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << "[options] dict_file use_guesser [file[:output_file]]...\n"
                    "Options: --input=untokenized|vertical\n"
                    "         --convert_tagset=pdt_to_conll2009|strip_lemma_comment|strip_lemma_id\n"
                    "         --derivation=none|root|path|tree\n"
                    "         --output=vertical|xml\n"
                    "         --from_tagger\n"
                    "         --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  unique_ptr<morpho> morpho;
  unique_ptr<tagger> tagger;
  if (options.count("from_tagger")) {
    cerr << "Loading dictionary from tagger: ";
    tagger.reset(tagger::load(argv[1]));
    if (!tagger) runtime_failure("Cannot load tagger from file '" << argv[1] << "'!");
  } else {
    cerr << "Loading dictionary: ";
    morpho.reset(morpho::load(argv[1]));
    if (!morpho) runtime_failure("Cannot load dictionary from file '" << argv[1] << "'!");
  }
  cerr << "done" << endl;
  auto& dictionary = options.count("from_tagger") ? *tagger->get_morpho() : *morpho;

  bool use_guesser = parse_int(argv[2], "use_guesser");

  unique_ptr<tokenizer> tokenizer(options.count("input") && options["input"] == "vertical" ? tokenizer::new_vertical_tokenizer() : dictionary.new_tokenizer());
  if (!tokenizer) runtime_failure("No tokenizer is defined for the supplied model!");

  unique_ptr<tagset_converter> tagset_converter;
  if (options.count("convert_tagset")) {
    tagset_converter.reset(new_tagset_converter(options["convert_tagset"], dictionary));
    if (!tagset_converter) runtime_failure("Unknown tag set converter '" << options["convert_tagset"] << "'!");
  } else {
    tagset_converter.reset(tagset_converter::new_identity_converter());
    if (!tagset_converter) runtime_failure("Cannot create identity tag set converter!");
  }

  unique_ptr<derivation_formatter> derivation;
  if (options.count("derivation")) {
    if (!options["derivation"].empty() && options["derivation"] != "none" && !morpho->get_derivator())
      runtime_failure("No derivator is defined for the supplied model!");
    derivation.reset(derivation_formatter::new_derivation_formatter(options["derivation"], morpho->get_derivator()));
    if (!derivation) runtime_failure("Cannot create derivation formatter '" << options["derivation"] << "' for the supplied model!");
  } else {
    derivation.reset(derivation_formatter::new_none_derivation_formatter());
    if (!derivation) runtime_failure("Cannot create trivial derivation formatter for the supplied model!");
  }

  if (options.count("output") && options["output"] == "vertical")
    process_args(3, argc, argv, analyze_vertical, dictionary, use_guesser, *tokenizer, *tagset_converter, *derivation);
  else
    process_args(3, argc, argv, analyze_xml, dictionary, use_guesser, *tokenizer, *tagset_converter, *derivation);

  return 0;
}

void analyze_vertical(istream& is, ostream& os, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter, const derivation_formatter& derivation) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> lemmas;

  while (getpara(is, para)) {
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      for (auto&& form : forms) {
        dictionary.analyze(form, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);
        tagset_converter.convert_analyzed(lemmas);

        os << form;
        for (auto&& lemma : lemmas) {
          derivation.format_derivation(lemma.lemma);
          os << '\t' << lemma.lemma << '\t' << lemma.tag;
        }
        os << '\n';
      }
      os << endl;
    }
  }
}

void analyze_xml(istream& is, ostream& os, const morpho& dictionary, bool use_guesser, tokenizer& tokenizer, const tagset_converter& tagset_converter, const derivation_formatter& derivation) {
  string para;
  vector<string_piece> forms;
  vector<tagged_lemma> lemmas;

  while (getpara(is, para)) {
    // Tokenize and analyze
    tokenizer.set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer.next_sentence(&forms, nullptr))
      for (unsigned i = 0; i < forms.size(); i++) {
        dictionary.analyze(forms[i], use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, lemmas);
        tagset_converter.convert_analyzed(lemmas);

        os << xml_encoded(string_piece(unprinted, forms[i].str - unprinted));
        if (!i) os << "<sentence>";
        os << "<token>";
        for (auto&& lemma : lemmas) {
          derivation.format_derivation(lemma.lemma);
          os << "<analysis lemma=\"" << xml_encoded(lemma.lemma, true) << "\" tag=\"" << xml_encoded(lemma.tag, true) << "\"/>";
        }
        os << xml_encoded(forms[i]) << "</token>";
        if (i + 1 == forms.size()) os << "</sentence>";
        unprinted = forms[i].str + forms[i].len;
      }

    os << xml_encoded(string_piece(unprinted, para.c_str() + para.size() - unprinted)) << flush;
  }
}
