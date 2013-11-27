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


// On x64 linux, memcpy is available as memcpy@GLIBC_2.2.5 and
// memcpy@GLIBC_2.14, the latter not being available even in Debian stable.

// The memcpy@GLIBC_2.2.5 is actually a memmove. We therefore override the
// memcpy from GLIBC and call memmove instead. Using -flto, the calls to
// memcpy are changed to calls to memmove, maybe except in libstdc++ runtime.

#include <cstring>

extern "C" {

void *memcpy(void* dest, const void* src, size_t n) {
  return memmove(dest, src, n);
}

}
