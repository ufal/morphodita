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

#include "czech_morpho.h"
#include "english_morpho.h"
#include "external_morpho.h"
#include "generic_morpho.h"
#include "morpho.h"
#include "morpho_ids.h"
#include "utils/file_ptr.h"
#include "utils/new_unique_ptr.h"

namespace ufal {
namespace morphodita {

morpho* morpho::load(FILE* f) {
  switch (morpho_ids::morpho_id(fgetc(f))) {
    case morpho_ids::CZECH:
      {
        auto res = new_unique_ptr<czech_morpho>();
        if (res->load(f)) return res.release();
        break;
      }
    case morpho_ids::ENGLISH_V1:
      {
        auto res = new_unique_ptr<english_morpho>(1);
        if (res->load(f)) return res.release();
        break;
      }
    case morpho_ids::ENGLISH_V2:
      {
        auto res = new_unique_ptr<english_morpho>(2);
        if (res->load(f)) return res.release();
        break;
      }
    case morpho_ids::ENGLISH_V3:
      {
        auto res = new_unique_ptr<english_morpho>(3);
        if (res->load(f)) return res.release();
        break;
      }
    case morpho_ids::GENERIC:
      {
        auto res = new_unique_ptr<generic_morpho>();
        if (res->load(f)) return res.release();
        break;
      }
    case morpho_ids::EXTERNAL:
      {
        auto res = new_unique_ptr<external_morpho>();
        if (res->load(f)) return res.release();
        break;
      }
  }

  return nullptr;
}

morpho* morpho::load(const char* fname) {
  file_ptr f = fopen(fname, "rb");
  if (!f) return nullptr;

  return load(f);
}

} // namespace morphodita
} // namespace ufal
