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

#include <memory>

#include "morpho/morpho.h"
#include "utils/input.h"

using namespace ufal::morphodita;

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s [-czech | dict_file]", argv[0]);

  unique_ptr<tokenizer> tokenizer;
  if (strcmp(argv[1], "-czech") == 0) {
    tokenizer.reset(tokenizer::new_czech_tokenizer());
  } else {
    eprintf("Loading dictionary: ");
    unique_ptr<morpho> dictionary(morpho::load(argv[1]));
    if (!dictionary) runtime_errorf("Cannot load dictionary from file '%s'!", argv[1]);
    eprintf("done\n");

    tokenizer.reset(dictionary->new_tokenizer());
  }

  string line, text;
  vector<string_piece> forms;
  for (bool not_eof = true; not_eof; ) {
    // Read block of text
    text.clear();
    while ((not_eof = getline(stdin, line)) && !line.empty()) {
      text += line;
      text += '\n';
    }
    if (not_eof) text += '\n';

    // Tokenize
    tokenizer->set_text(text.c_str());
    while (tokenizer->next_sentence(&forms, nullptr)) {
      for (auto& form : forms) {
        fwrite(form.str, 1, form.len, stdout);
        putchar('\n');
      }
      putchar('\n');
    }
  }

  return 0;
}
