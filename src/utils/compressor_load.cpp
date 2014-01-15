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

bool compressor::load(FILE* f, binary_decoder& data) {
  uint32_t uncompressed_len, compressed_len, poor_crc;
  unsigned char props_encoded[LZMA_PROPS_SIZE];

  if (fread(&uncompressed_len, sizeof(uncompressed_len), 1, f) != 1) return false;
  if (fread(&compressed_len, sizeof(compressed_len), 1, f) != 1) return false;
  if (fread(&poor_crc, sizeof(poor_crc), 1, f) != 1) return false;
  if (poor_crc != uncompressed_len * 19991 + compressed_len * 199999991 + 1234567890) return false;
  if (fread(props_encoded, sizeof(props_encoded), 1, f) != 1) return false;

  vector<unsigned char> compressed(compressed_len);
  if (fread(compressed.data(), 1, compressed_len, f) != compressed_len) return false;

  lzma::ELzmaStatus status;
  size_t uncompressed_size = uncompressed_len, compressed_size = compressed_len;
  auto res = lzma::LzmaDecode(data.fill(uncompressed_len), &uncompressed_size, compressed.data(), &compressed_size, props_encoded, LZMA_PROPS_SIZE, lzma::LZMA_FINISH_ANY, &status, &lzmaAllocator);
  if (res != SZ_OK || uncompressed_size != uncompressed_len || compressed_size != compressed_len) return false;

  return true;
}

} // namespace morphodita
} // namespace ufal
