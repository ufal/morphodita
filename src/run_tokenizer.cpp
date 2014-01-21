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
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void tokenize(FILE* in, FILE* out, tokenizer& tokenizer);

int main(int argc, char* argv[]) {
  if (argc < 2) runtime_errorf("Usage: %s -czech|dict_file [file[:output_file]]...", argv[0]);

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

  process_args(2, argc, argv, tokenize, *tokenizer);

  return 0;
}

void tokenize(FILE* in, FILE* out, tokenizer& tokenizer) {
  string para;
  vector<string_piece> forms;
  while (getpara(in, para)) {
    eprintf("Have para: '%s'\n", para.c_str());
    // Tokenize
    tokenizer.set_text(para.c_str());
    while (tokenizer.next_sentence(&forms, nullptr)) {
      for (auto& form : forms) {
        fwrite(form.str, 1, form.len, out);
        fputc('\n', out);
      }
      fputc('\n', out);
    }
  }
}
