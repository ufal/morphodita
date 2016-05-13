// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "morphodita.h"

using namespace ufal::morphodita;
using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 2) return cerr << "Usage: " << argv[0] << " tagger_file" << endl, 1;

  cerr << "Loading tagger: ";
  unique_ptr<tagger> tagger(tagger::load(argv[1]));
  if (!tagger) return cerr << "Cannot load tagger from file '" << argv[1] << "'!" << endl, 1;
  cerr << "done" << endl;

  unique_ptr<tokenizer> tokenizer(tagger->new_tokenizer());
  if (!tokenizer) return cerr << "No tokenizer is defined for the supplied model!" << endl, 1;

  unique_ptr<tagset_converter> tagset_converter(tagset_converter::new_identity_converter());

  unique_ptr<derivation_formatter> derivation(derivation_formatter::new_none_derivation_formatter());

  string line, para;
  vector<string_piece> forms;
  vector<tagged_lemma> tags;

  clock_t now = clock();
  while (true) {
    para.clear();
    while (getline(cin, line)) {
      para.append(line).push_back('\n');
      if (line.empty()) break;
    }
    if (para.empty()) break;

    tokenizer->set_text(para);
    while (tokenizer->next_sentence(&forms, nullptr)) {
      tagger->tag(forms, tags);

      for (unsigned i = 0; i < tags.size(); i++) {
        tagset_converter->convert(tags[i]);
        derivation->format_derivation(tags[i].lemma);
        cout << string(forms[i].str, forms[i].len) << '\t' << tags[i].lemma << '\t' << tags[i].tag << '\n';
      }
      cout << endl;
    }
  }
  cerr << "Tagging done in " << fixed << setprecision(3) << (clock() - now) / double(CLOCKS_PER_SEC) << " seconds." << endl;

  return 0;
}
