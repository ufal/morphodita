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

#include "english_morpho_guesser.h"

namespace ufal {
namespace morphodita {

void english_morpho_guesser::load(binary_decoder& data) {
  negations.load(data);
}

%% machine tag_guesser; write data;

void english_morpho_guesser::analyze(string_piece form, string_piece form_lc, vector<tagged_lemma>& lemmas) const {
  bool uppercase = form_lc.str != form.str;
  string lemma_lc(form_lc.str, form_lc.len), lemma;
  if (uppercase) lemma.assign(form.str, form.len);

  // Try finding negative prefix
  unsigned negation_len = 0;
  for (unsigned prefix = 0; prefix < form_lc.len; prefix++) {
    auto found = negations.at(form_lc.str, prefix, [](pointer_decoder& data){ data.next<unsigned char>(TOTAL); });
    if (!found) break;
    if (found[NEGATION_LEN]) {
      if (form_lc.len - prefix >= found[TO_FOLLOW]) negation_len = found[NEGATION_LEN];
      break;
    }
  }

  // Add default tags
  add(FW, lemma_lc, lemmas);
  add(JJ, lemma_lc, lemmas);
  add(RB, lemma_lc, lemmas);
  add(NN, lemma_lc, negation_len, lemmas);
  add_NNS(lemma_lc, negation_len, lemmas);
  if (uppercase) {
    add(NNP, lemma, lemmas);
    add_NNPS(lemma, lemmas);
  }

  // Add specialized tags
  const char* p = form_lc.str; int cs;
  %%{
    variable pe (form_lc.str + form_lc.len);
    variable eof (form_lc.str + form_lc.len);

    action add_JJR_RBR { add_JJR_RBR(lemma_lc, negation_len, lemmas); } JJR_RBR = any* ('er' | ('er'|'more'|'less') '-' any*) % add_JJR_RBR;
    action add_JJS_RBS { add_JJS_RBS(lemma_lc, negation_len, lemmas); } JJS_RBS = any* ('est' | ('est'|'most'|'least') '-' any*) % add_JJS_RBS;
    action add_VBG { add_VBG(lemma_lc, lemmas); } VBG = any* ('ing' | [^aeiouy]'in') % add_VBG;
    action add_VBD_VBN { add_VBD_VBN(lemma_lc, lemmas); } VBD_VBN = any* ('ed') % add_VBD_VBN;
    action add_VBZ { add_VBZ(lemma_lc, lemmas); } VBZ = any* ([^s]'s') % add_VBZ;
    action add_VB_VBP { add(VB, lemma_lc, lemmas); add(VBP, lemma_lc, lemmas); } VB_VBP = any* ('ss' | [^s]) % add_VB_VBP;
    action add_NNP { if (!uppercase) add(NNP, lemma_lc, lemmas); } NNP = [0-9'] > add_NNP;
    action add_SYM { add(SYM, lemma_lc, lemmas); } SYM = any* [^a-zA-Z0-9] any* % add_SYM;
    action add_CD { add(CD, lemma_lc, lemmas); } CD = (any* [0-9\-] any* | [ixvcmd\.]+) % add_CD;

    main := JJR_RBR | JJS_RBS | VBG | VBD_VBN | VBZ | VB_VBP | NNP | SYM | CD;
    write init;
    write exec;
  }%%
}

inline void english_morpho_guesser::add(const string& tag, const string& form, vector<tagged_lemma>& lemmas) const {
  lemmas.emplace_back(form, tag);
}

inline void english_morpho_guesser::add(const string& tag, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  lemmas.emplace_back(negation_len ? form.substr(negation_len) + "^" + form.substr(0, negation_len) : form, tag);
}

inline void english_morpho_guesser::add_NNS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  add(NNS, form, negation_len, lemmas);
}

inline void english_morpho_guesser::add_NNPS(const string& form, vector<tagged_lemma>& lemmas) const {
  add(NNPS, form, lemmas);
}

inline void english_morpho_guesser::add_VBG(const string& form, vector<tagged_lemma>& lemmas) const {
  add(VBG, form, lemmas);
}

inline void english_morpho_guesser::add_VBD_VBN(const string& form, vector<tagged_lemma>& lemmas) const {
  add(VBD, form, lemmas);
  add(VBN, form, lemmas);
}

inline void english_morpho_guesser::add_VBZ(const string& form, vector<tagged_lemma>& lemmas) const {
  add(VBZ, form, lemmas);
}

inline void english_morpho_guesser::add_JJR_RBR(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  add(JJR, form, negation_len, lemmas);
  add(RBR, form, negation_len, lemmas);
}

inline void english_morpho_guesser::add_JJS_RBS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  add(JJS, form, negation_len, lemmas);
  add(RBS, form, negation_len, lemmas);
}

} // namespace morphodita
} // namespace ufal
