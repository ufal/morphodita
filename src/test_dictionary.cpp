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

#include <algorithm>
#include <ctime>
#include <memory>

#include "morpho/morpho.h"
#include "morpho/raw_morpho_dictionary_reader.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s dict_file <raw_dict_file", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> d(morpho::load(argv[1]));
  if (!d) runtime_errorf("Cannot load dictionary %s!", argv[1]);
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

    d->generate(lemma.c_str(), lemma.size(), nullptr, morpho::NO_GUESSER, lemmas_forms);

    bool same_results = false;
    for (auto& lemma_forms : lemmas_forms)
      if (lemma_forms.lemma == lemma && lemma_forms.forms.size() == raw_forms.size()) {
        same_results = true;
        for (auto& form : lemma_forms.forms)
          if (!binary_search(raw_forms.begin(), raw_forms.end(), pair<string, string>(form.form, form.tag))) {
            same_results = false;
            break;
          }
        if (same_results) break;
      }

    if (same_results)
      for (auto& tagged_form : raw_forms) {
        d->analyze(tagged_form.first.c_str(), tagged_form.first.size(), morpho::NO_GUESSER, lemmas);

        same_results = false;
        for (auto& tagged_lemma : lemmas)
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
