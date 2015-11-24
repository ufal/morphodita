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
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/split.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"from_tagger",options::value::none},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file\n"
                    "Options: --from_tagger\n"
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

  string line;
  vector<string> tokens;
  while (getline(cin, line)) {
    split(line, '\t', tokens);
    if /* analyze */ (tokens.size() == 1) {
      vector<tagged_lemma> lemmas;
      auto result = dictionary.analyze(tokens[0], morpho::GUESSER, lemmas);

      string guesser_name = result == morpho::GUESSER ? "Guesser " : "";
      for (auto&& lemma : lemmas)
        cout << guesser_name << "Lemma: " << lemma.lemma << ' ' << lemma.tag << '\n';
      cout << flush;
    } else if /* generate */ (tokens.size() == 2) {
      vector<tagged_lemma_forms> forms;
      auto result = dictionary.generate(tokens[0], tokens[1].c_str(), morpho::GUESSER, forms);

      string guesser_name = result == morpho::GUESSER ? "Guesser " : "";
      for (auto&& lemma : forms) {
        cout << guesser_name << "Lemma: " << lemma.lemma << '\n';
        for (auto&& form : lemma.forms)
          cout << "  " << form.form << ' ' << form.tag << '\n';
      }
      cout << flush;
    }
  }

  return 0;
}
