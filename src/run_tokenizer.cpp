// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "morpho/morpho.h"
#include "tagger/tagger.h"
#include "utils/getpara.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "utils/xml_encoded.h"
#include "version/version.h"

using namespace ufal::morphodita;

static void tokenize_vertical(istream& is, ostream& os, tokenizer& tokenizer);
static void tokenize_xml(istream& is, ostream& os, tokenizer& tokenizer);

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  bool show_usage =
      !options::parse({{"tokenizer", options::value{"czech", "english", "generic"}},
                       {"morphology", options::value::any},
                       {"tagger", options::value::any},
                       {"output", options::value{"vertical","xml"}},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help");
  if (!show_usage && options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;
  if (!show_usage && options.count("tokenizer") + options.count("morphology") + options.count("tagger") == 0)
    cerr << "Missing one of --tokenizer, --morphology and --tagger options!" << endl, show_usage = true;
  if (!show_usage && options.count("tokenizer") + options.count("morphology") + options.count("tagger") > 1)
    cerr << "Only one of --tokenizer, --morphology and --tagger options can be specifed!" << endl, show_usage = true;
  if (show_usage)
    runtime_failure("Usage: " << argv[0] << " [options] [file[:output_file]]...\n"
                    "Options: --tokenizer=czech|english|generic\n"
                    "         --morphology=morphology_model_file\n"
                    "         --tagger=tagger_model_file\n"
                    "         --output=vertical|xml\n"
                    "         --version\n"
                    "         --help");

  unique_ptr<tokenizer> tokenizer;
  unique_ptr<morpho> morpho;
  unique_ptr<tagger> tagger;

  if (options.count("tokenizer") && options["tokenizer"] == "czech") tokenizer.reset(tokenizer::new_czech_tokenizer());
  else if (options.count("tokenizer") && options["tokenizer"] == "english") tokenizer.reset(tokenizer::new_english_tokenizer());
  else if (options.count("tokenizer") && options["tokenizer"] == "generic") tokenizer.reset(tokenizer::new_generic_tokenizer());
  else if (options.count("morphology")) {
    cerr << "Loading dictionary: ";
    morpho.reset(morpho::load(options["morphology"].c_str()));
    if (!morpho) runtime_failure("Cannot load dictionary from file '" << options["morphology"] << "'!");
    cerr << "done" << endl;

    tokenizer.reset(morpho->new_tokenizer());
    if (!tokenizer) runtime_failure("No tokenizer is defined for the supplied model!");
  } else /*if (options.count("tagger"))*/ {
    cerr << "Loading tagger: ";
    tagger.reset(tagger::load(options["tagger"].c_str()));
    if (!tagger) runtime_failure("Cannot load dictionary from file '" << options["tagger"] << "'!");
    cerr << "done" << endl;

    tokenizer.reset(tagger->new_tokenizer());
    if (!tokenizer) runtime_failure("No tokenizer is defined for the supplied model!");
  }

  if (options.count("output") && options["output"] == "vertical") process_args(1, argc, argv, tokenize_vertical, *tokenizer);
  else process_args(1, argc, argv, tokenize_xml, *tokenizer);

  return 0;
}

void tokenize_vertical(istream& is, ostream& os, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  while (getpara(is, para)) {
    // Tokenize
    tokenizer.set_text(para);
    while (tokenizer.next_sentence(&forms, nullptr)) {
      for (auto&& form : forms)
        os << form << '\n';
      os << endl;
    }
  }
}

static void tokenize_xml(istream& is, ostream& os, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  while (getpara(is, para)) {
    // Tokenize
    tokenizer.set_text(para);
    const char* unprinted = para.c_str();
    while (tokenizer.next_sentence(&forms, nullptr))
      for (unsigned i = 0; i < forms.size(); i++) {
        os << xml_encoded(string_piece(unprinted, forms[i].str - unprinted));
        if (!i) os << "<sentence>";
        os << "<token>" << xml_encoded(forms[i]) << "</token>";
        if (i + 1 == forms.size()) os << "</sentence>";
        unprinted = forms[i].str + forms[i].len;
      }

    os << xml_encoded(string_piece(unprinted, para.c_str() + para.size() - unprinted)) << flush;
  }
}
