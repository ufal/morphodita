// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2016 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "derivator_dictionary_encoder.h"
#include "morpho/morpho.h"

namespace ufal {
namespace morphodita {

void derivator_dictionary_encoder::encode(istream& is, istream& dictionary, ostream& os) {
  // Load the morphology
  cerr << "Loading morphology: ";
  auto dictionary_start = dictionary.tellg();
  unique_ptr<morpho> morpho(morpho::load(dictionary));
  if (!morpho) runtime_failure("Cannot load morpho model from given file!");
  if (morpho->get_derivator()) runtime_failure("The given morpho model already has a derivator!");
  auto dictionary_end = dictionary.tellg();
  cerr << "done" << endl;

  // Load the derivator
  cerr << "Loading derivator: ";

  cerr << "done" << endl;

  // Encode the derivator
  cerr << "Encoding derivator: ";

  // Append the morphology after the derivator dictionary model
  if (!dictionary.seekg(dictionary_start, dictionary.beg)) runtime_failure("Cannot seek in the morpho model!");
  for (auto length = dictionary_end - dictionary_start; length; length--)
    os.put(dictionary.get());

  cerr << "done" << endl;
}

} // namespace morphodita
} // namespace ufal
