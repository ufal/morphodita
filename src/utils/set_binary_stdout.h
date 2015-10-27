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

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

namespace ufal {
namespace morphodita {

// Set stdout to binary mode. Needed on Windows only.
inline void set_binary_stdout() {
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
#endif
}

} // namespace morphodita
} // namespace ufal
