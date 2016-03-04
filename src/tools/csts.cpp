// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "morpho/morpho.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "utils/split.h"
#include "utils/xml_encoded.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc <= 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file use_guesser\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading dictionary: ";
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_failure("Cannot load dictionary from file '" << argv[1] << "'!");
  cerr << "done" << endl;
  bool use_guesser = parse_int(argv[2], "use_guesser");

  bool in_sentence = false;
  vector<tagged_lemma> tags;

  string line;
  vector<string> tokens;
  while (getline(cin, line)) {
    if (line.empty()) {
      if (in_sentence) cout << "</s>" << endl;
      in_sentence = false;
    } else {
      if (!in_sentence) cout << "<s>\n";
      in_sentence = true;

      split(line, '\t', tokens);
      if (tokens.size() != 3) runtime_failure("Input line '" << line << "' does not contain 3 columns!");
      dictionary->analyze(tokens[0], use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, tags);

      cout << "<f>" << xml_encoded(tokens[0]) << "<l>" << xml_encoded(tokens[1]) << "<t>" << xml_encoded(tokens[2]);
      for (auto&& tag : tags)
        cout << "<MMl>" << xml_encoded(tag.lemma) << "<MMt>" << xml_encoded(tag.tag);
      cout << '\n';
    }
  }
  if (in_sentence) cout << "</s>" << endl;

  return 0;
}
