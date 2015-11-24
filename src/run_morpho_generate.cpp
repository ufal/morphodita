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
#include "tagset_converter/tagset_converter.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "utils/process_args.h"
#include "version/version.h"

using namespace ufal::morphodita;

static void generate_vertical(istream& is, ostream& os, const morpho& dictionary, bool use_guesser, const tagset_converter& tagset_converter);

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"convert_tagset",options::value::any},
                       {"from_tagger",options::value::none},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 3 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file use_guesser [file[:output_file]]...\n"
                    "Options: --convert_tagset=pdt_to_conll2009|strip_lemma_comment|strip_lemma_id\n"
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

  unique_ptr<tagset_converter> tagset_converter;
  if (options.count("convert_tagset")) {
    tagset_converter.reset(new_tagset_converter(options["convert_tagset"], dictionary));
    if (!tagset_converter) runtime_failure("Unknown tag set converter '" << options["convert_tagset"] << "'!");
  } else {
    tagset_converter.reset(tagset_converter::new_identity_converter());
    if (!tagset_converter) runtime_failure("Cannot create identity tag set converter!");
  }

  process_args(3, argc, argv, generate_vertical, dictionary, use_guesser, *tagset_converter);

  return 0;
}

void generate_vertical(istream& is, ostream& os, const morpho& dictionary, bool use_guesser, const tagset_converter& tagset_converter) {
  string line;
  vector<tagged_lemma_forms> forms;

  while (getline(is, line)) {
    if (!line.empty()) {
      string_piece lemma = line;
      const char* wildcard = nullptr;

      string::size_type space = line.find('\t');
      if (space != string::npos) {
        lemma.len = space;
        wildcard = line.c_str() + space + 1;
      }

      dictionary.generate(lemma, wildcard, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, forms);
      tagset_converter.convert_generated(forms);

      bool first = true;
      for (auto&& lemma : forms)
        for (auto&& form : lemma.forms) {
          if (!first) os << '\t';
          os << form.form << '\t' << lemma.lemma << '\t' << form.tag;
          first = false;
        }
    }
    os << endl;
  }
}
