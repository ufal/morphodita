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
#include <memory>

#include "morpho/morpho.h"
#include "morpho/raw_morpho_dictionary_reader.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s dict_file <raw_dict_file", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary %s!", argv[1]);
  eprintf("done\n");

  raw_morpho_dictionary_reader raw(stdin);
  string lemma;
  vector<pair<string, string>> raw_forms;
  vector<tagged_lemma_forms> lemmas_forms;
  vector<tagged_lemma> lemmas;

  clock_t now = clock();
  int processed = 0;
  eprintf("Processing raw dictionary: ");
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
      eprintf("[error for lemma %s] ", lemma.c_str());
    }

    processed++;
    if (clock() > now + CLOCKS_PER_SEC) {
      eprintf("%-10d\b\b\b\b\b\b\b\b\b\b", processed);
      now = clock();
    }
  }
  eprintf("done      \n");

  return 0;
}
