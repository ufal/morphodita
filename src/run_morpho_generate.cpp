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
#include "utils/parse_int.h"
#include "utils/parse_options.h"
#include "utils/process_args.h"

using namespace ufal::morphodita;

static void generate(FILE* in, FILE* out, morpho& dictionary, bool use_guesser);

int main(int argc, char* argv[]) {
  options_map options;
  if (!parse_options({{"convert_tagset",{""}}}, argc, argv, options) ||
      argc < 3)
    runtime_errorf("Usage: %s [options] dict_file use_guesser [file[:output_file]]...\n"
                   "Options: --convert_tagset=pdt_to_conll2009", argv[0]);

  eprintf("Loading dictionary: ");
  unique_ptr<morpho> dictionary(morpho::load(argv[1]));
  if (!dictionary) runtime_errorf("Cannot load dictionary from file '%s'!", argv[1]);
  eprintf("done\n");
  bool use_guesser = parse_int(argv[2], "use_guesser");

  process_args(3, argc, argv, generate, *dictionary, use_guesser);

  return 0;
}

void generate(FILE* in, FILE* out, morpho& dictionary, bool use_guesser) {
  string line;
  vector<tagged_lemma_forms> forms;

  while (getline(in, line)) {
    if (!line.empty()) {
      string_piece lemma = line;
      const char* wildcard = nullptr;

      string::size_type space = line.find('\t');
      if (space != string::npos) {
        lemma.len = space;
        wildcard = line.c_str() + space + 1;
      }

      dictionary.generate(lemma, wildcard, use_guesser ? morpho::GUESSER : morpho::NO_GUESSER, forms);

      bool first = true;
      for (auto&& lemma : forms)
        for (auto&& form : lemma.forms) {
          if (!first) fputc('\t', out);
          fprintf(out, "%s\t%s\t%s", form.form.c_str(), lemma.lemma.c_str(), form.tag.c_str());
          first = false;
        }
    }
    fputc('\n', out);
  }
}
