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

#include <atomic>
#include <memory>

#include "common.h"

namespace ufal {
namespace morphodita {

// Declarations
template <class T>
class threadsafe_stack {
 public:
  inline void push(T* t);
  inline T* pop();

 private:
  vector<unique_ptr<T>> stack;
  atomic_flag lock = ATOMIC_FLAG_INIT;
};


// Definitions
template <class T>
void threadsafe_stack<T>::push(T* t) {
  while (lock.test_and_set()) {}
  stack.emplace_back(t);
  lock.clear();
}

template <class T>
T* threadsafe_stack<T>::pop() {
  T* res = nullptr;

  while (lock.test_and_set()) {}
  if (!stack.empty()) {
    res = stack.back().release();
    stack.pop_back();
  }
  lock.clear();

  return res;
}

} // namespace morphodita
} // namespace ufal
