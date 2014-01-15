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

#include "compressor.h"
#include "lzma/LzmaDec.h"
#include "lzma/LzmaEnc.h"

namespace ufal {
namespace morphodita {

static void *LzmaAlloc(void* /*p*/, size_t size) { return new char[size]; }
static void LzmaFree(void* /*p*/, void *address) { delete[] (char*) address; }
static lzma::ISzAlloc lzmaAllocator = { LzmaAlloc, LzmaFree };

bool compressor::save(FILE* f, const binary_encoder& enc) {
  size_t uncompressed_size = enc.data.size(), compressed_size = 2 * enc.data.size();
  vector<unsigned char> compressed(compressed_size);

  lzma::CLzmaEncProps props;
  lzma::LzmaEncProps_Init(&props);
  unsigned char props_encoded[LZMA_PROPS_SIZE];
  size_t props_encoded_size = LZMA_PROPS_SIZE;

  auto res = lzma::LzmaEncode(compressed.data(), &compressed_size, enc.data.data(), uncompressed_size, &props, props_encoded, &props_encoded_size, 0, nullptr, &lzmaAllocator, &lzmaAllocator);
  if (res != SZ_OK) return false;

  uint32_t poor_crc = uncompressed_size * 19991 + compressed_size * 199999991 + 1234567890;
  if (uint32_t(uncompressed_size) != uncompressed_size || uint32_t(compressed_size) != compressed_size) return false;
  if (fwrite(&uncompressed_size, sizeof(uint32_t), 1, f) != 1) return false;
  if (fwrite(&compressed_size, sizeof(uint32_t), 1, f) != 1) return false;
  if (fwrite(&poor_crc, sizeof(uint32_t), 1, f) != 1) return false;
  if (fwrite(props_encoded, sizeof(props_encoded), 1, f) != 1) return false;
  if (fwrite(compressed.data(), 1, compressed_size, f) != compressed_size) return false;

  return true;
}

} // namespace morphodita
} // namespace ufal
