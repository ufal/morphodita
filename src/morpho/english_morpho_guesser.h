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

#include "common.h"
#include "morpho.h"
#include "utils/binary_decoder.h"
#include "utils/persistent_unordered_map.h"

namespace ufal {
namespace morphodita {

class english_morpho_guesser {
 public:
  void load(binary_decoder& data);
  void analyze(string_piece form, string_piece form_lc, vector<tagged_lemma>& lemmas) const;

 private:
  inline void add(const string& tag, const string& form, vector<tagged_lemma>& lemmas) const;
  inline void add(const string& tag, const string& tag2, const string& form, vector<tagged_lemma>& lemmas) const;
  inline void add(const string& tag, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  inline void add(const string& tag, const string& tag2, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  void add_NNS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  void add_NNPS(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_VBG(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_VBD_VBN(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_VBZ(const string& form, vector<tagged_lemma>& lemmas) const;
  void add_JJR_RBR(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;
  void add_JJS_RBS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const;

  enum { NEGATION_LEN = 0, TO_FOLLOW = 1, TOTAL = 2 };
  persistent_unordered_map negations;
  string CD = "CD", FW = "FW", JJ = "JJ", JJR = "JJR", JJS = "JJS",
         NN = "NN", NNP = "NNP", NNPS = "NNPS", NNS = "NNS", RB = "RB",
         RBR = "RBR", RBS = "RBS", SYM = "SYM", VB = "VB", VBD = "VBD",
         VBG = "VBG", VBN = "VBN", VBP = "VBP", VBZ = "VBZ";
};

} // namespace morphodita
} // namespace ufal
