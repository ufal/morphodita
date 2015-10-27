// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common.h"

namespace ufal {
namespace morphodita {

// Read line and strip '\n'.
bool getline(FILE *f, string& line);

// Read a paragraph separated by an empty line. All '\n' are left intact.
bool getpara(FILE* f, string& para);

// Split given text on the separator character.
void split(const string& text, char sep, vector<string>& tokens);

} // namespace morphodita
} // namespace ufal
