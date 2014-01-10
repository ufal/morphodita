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

#include "distribution.h"

namespace ufal {
namespace utils {

int distribution::uniform(int min, int max) {
  unsigned range = max - min;
  unsigned r = g();

  // At first try coarse bound, which can be computed fast and works almost every time.
  unsigned bound = g.max() - range;
  if (r > bound) {
    unsigned bound = g.max() - g.max() % range;
    while (r >= bound) r = g();
  }

  return min + r % range;
}

double distribution::real(double min, double max) {
  return min + g() * (max - min) / (double(g.max()) + 1);
}

void distribution::permutation(int min, int max, vector<int>& perm) {
  int N = max - min;
  perm.resize(N);
  for (int i = 0; i < N; i++) perm[i] = i + min;

  for (int i = N-1; i > 0; i--) {
    int j = uniform(0, i + 1);
    if (j < i) {
      int tmp = perm[j];
      perm[j] = perm[i];
      perm[i] = tmp;
    }
  }
}

} // namespace utils
} // namespace ufal
