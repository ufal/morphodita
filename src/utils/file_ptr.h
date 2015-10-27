// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>

#include "common.h"

namespace ufal {
namespace morphodita {

class file_ptr {
 public:
  file_ptr(FILE* f = nullptr) : f(f) {}
  file_ptr(file_ptr&& other) : f(other.f) { other.f = nullptr; }
  file_ptr& operator=(file_ptr&& other) { if (f) fclose(f); f = other.f; other.f = nullptr; return *this; }
  ~file_ptr() { if (f) fclose(f); }

  operator FILE* () const { return f; }
 private:
  FILE* f;
};

} // namespace morphodita
} // namespace ufal
