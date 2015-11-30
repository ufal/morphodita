// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <ctime>

#include "morpho/morpho.h"
#include "morpho/raw_morpho_dictionary_reader.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "version/version.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      (argc < 2 && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] dict_file <raw_dict_file\n"
                    "Options: --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  cerr << "Loading dictionary: ";
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_failure("Cannot load dictionary " << argv[1] << '!');
  cerr << "done" << endl;

  raw_morpho_dictionary_reader raw(cin);
  string lemma;
  vector<pair<string, string>> raw_forms;
  vector<tagged_lemma_forms> lemmas_forms;
  vector<tagged_lemma> lemmas;

  clock_t now = clock();
  int processed = 0;
  cerr << "Processing raw dictionary: ";
  while (raw.next_lemma(lemma, raw_forms)) {
    sort(raw_forms.begin(), raw_forms.end());
    raw_forms.erase(unique(raw_forms.begin(), raw_forms.end()), raw_forms.end());

    dictionary->generate(lemma, nullptr, morpho::NO_GUESSER, lemmas_forms);

    bool same_results = false;
    for (auto&& lemma_forms : lemmas_forms)
      if (lemma_forms.lemma == lemma && lemma_forms.forms.size() == raw_forms.size()) {
        same_results = true;
        for (auto&& form : lemma_forms.forms)
          if (!binary_search(raw_forms.begin(), raw_forms.end(), pair<string, string>(form.form, form.tag))) {
            same_results = false;
            break;
          }
        if (same_results) break;
      }

    if (same_results)
      for (auto&& tagged_form : raw_forms) {
        dictionary->analyze(tagged_form.first, morpho::NO_GUESSER, lemmas);

        same_results = false;
        for (auto&& tagged_lemma : lemmas)
          if (tagged_lemma.lemma == lemma && tagged_lemma.tag == tagged_form.second) {
            same_results = true;
            break;
          }
        if (!same_results) break;
      }

    if (!same_results) {
      cerr << "[error for lemma " << lemma << "] ";
    }

    processed++;
    if (clock() > now + CLOCKS_PER_SEC) {
      cerr << setw(10) << processed << "\b\b\b\b\b\b\b\b\b\b";
      now = clock();
    }
  }
  cerr << "done      " << endl;

  return 0;
}
