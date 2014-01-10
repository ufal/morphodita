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

#pragma once

#include <utility>

#include "common.h"

namespace ufal {
namespace utils {

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

} // namespace utils
}
