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

inline void english_morpho_guesser::add(const string& tag, const string& tag2, const string& form, vector<tagged_lemma>& lemmas) const {
  add(tag, form, lemmas);
  add(tag2, form, lemmas);
}

inline void english_morpho_guesser::add(const string& tag, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  lemmas.emplace_back(negation_len ? form.substr(negation_len) + "^" + form.substr(0, negation_len) : form, tag);
}

inline void english_morpho_guesser::add(const string& tag, const string& tag2, const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  add(tag, form, negation_len, lemmas);
  add(tag2, form, negation_len, lemmas);
}

// Common definitions
#define REM(str, len) (str.substr(0, str.size() - len))
#define REM_ADD(str, len, add) (str.substr(0, str.size() - len).append(add))
%% machine common;
%% V = [aeiou];
%% VY = [aeiouy];
%% C = [bcdfghjklmnpqrstvwxyz];
%% C2 = 'bb'|'cc'|'dd'|'ff'|'gg'|'hh'|'jj'|'kk'|'ll'|'mm'|'nn'|'pp'|'qq'|'rr'|'ss'|'tt'|'vv'|'ww'|'xx'|'yy'|'zz';
%% CXY = C - 'y';
%% CXY2 = C2 - 'yy';
%% S = ([sxz] | [cs]'h');
%% S2 = ('ss' | 'zz');
%% PRE = ('be' | 'ex' | 'in' | 'mis' | 'pre' | 'pro' | 're');

%% machine NNS; include common; write data;
void english_morpho_guesser::add_NNS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { add(NNS, REM_ADD(form, 2, "an"), negation_len, lemmas); return; } a = 'men' % add_a;
    action add_b { add(NNS, REM(form, 1), negation_len, lemmas); return; } b = 'shoes' % add_b;
    action add_c { add(NNS, REM_ADD(form, 3, "fe"), negation_len, lemmas); return; } c = 'wives' % add_c;
    action add_d { add(NNS, REM(form, 2), negation_len, lemmas); return; } d = C 'uses' % add_d;
    action add_e { add(NNS, REM(form, 1), negation_len, lemmas); return; } e = V 'ses' % add_e;
    action add_f { add(NNS, REM(form, 2), negation_len, lemmas); return; } f = any CXY 'zes' % add_f;
    action add_g { add(NNS, REM(form, 1), negation_len, lemmas); return; } g = VY 'zes' % add_g;
    action add_h { add(NNS, REM(form, 2), negation_len, lemmas); return; } h = S2 'es' % add_h;
    action add_i { add(NNS, REM(form, 1), negation_len, lemmas); return; } i = any V 'rses' % add_i;
    action add_j { add(NNS, REM(form, 1), negation_len, lemmas); return; } j = 'onses' % add_j;
    action add_k { add(NNS, REM(form, 2), negation_len, lemmas); return; } k = S 'es' % add_k;
    action add_l { add(NNS, REM_ADD(form, 3, "y"), negation_len, lemmas); return; } l = any C 'ies' % add_l;
    action add_m { add(NNS, REM(form, 2), negation_len, lemmas); return; } m = CXY 'oes' % add_m;
    action add_n { add(NNS, REM(form, 1), negation_len, lemmas); return; } n = any 's' % add_n;

    main := any* (a | b | c | d | e | f | g | h | i | j | k | l | m | n);
    write init;
    write exec;
  }%%
}

%% machine NNPS; include common; write data;
void english_morpho_guesser::add_NNPS(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());
    Vi = V | [AEIOU];
    VYi = VY | [AEIOUY];
    Ci = C | [BCDFGHJKLMNPQRSTVWXYZ];
    CXYi = CXY | [BCDFGHJKLMNPQRSTVWXZ];
    Si = S | ([SXZ] | [csCS]'h'i);
    S2i = S2 | ('ss'i | 'zz'i);

    action add_a0 { add(NNPS, REM_ADD(form, 2, "AN"), lemmas); return; } a0 = 'MEN' % add_a0;
    action add_a { add(NNPS, REM_ADD(form, 2, "an"), lemmas); return; } a = 'men'i % add_a;
    action add_b { add(NNPS, REM(form, 1), lemmas); return; } b = 'shoes'i % add_b;
    action add_c0 { add(NNPS, REM_ADD(form, 3, "FE"), lemmas); return; } c0 = 'WIVES' % add_c0;
    action add_c { add(NNPS, REM_ADD(form, 3, "fe"), lemmas); return; } c = 'wives'i % add_c;
    action add_d { add(NNPS, REM(form, 2), lemmas); return; } d = Ci 'uses'i % add_d;
    action add_e { add(NNPS, REM(form, 1), lemmas); return; } e = Vi 'ses'i % add_e;
    action add_f { add(NNPS, REM(form, 2), lemmas); return; } f = any CXYi 'zes'i % add_f;
    action add_g { add(NNPS, REM(form, 1), lemmas); return; } g = VYi 'zes'i % add_g;
    action add_h { add(NNPS, REM(form, 2), lemmas); return; } h = S2i 'es'i % add_h;
    action add_i { add(NNPS, REM(form, 1), lemmas); return; } i = any Vi 'rses'i % add_i;
    action add_j { add(NNPS, REM(form, 1), lemmas); return; } j = 'onses'i % add_j;
    action add_k { add(NNPS, REM(form, 2), lemmas); return; } k = Si 'es'i % add_k;
    action add_l0 { add(NNPS, REM_ADD(form, 3, "Y"), lemmas); return; } l0 = any Ci 'IES' % add_l0;
    action add_l { add(NNPS, REM_ADD(form, 3, "y"), lemmas); return; } l = any Ci 'ies'i % add_l;
    action add_m { add(NNPS, REM(form, 2), lemmas); return; } m = CXYi 'oes'i % add_m;
    action add_n { add(NNPS, REM(form, 1), lemmas); return; } n = any 's'i % add_n;

    main := any* (a0 | a | b | c0 | c | d | e | f | g | h | i | j | k | l0 | l | m | n);
    write init;
    write exec;
  }%%
}

%% machine VBG; include common; write data;
void english_morpho_guesser::add_VBG(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { add(NNS, REM(form, 3), lemmas); return; } a = any* CXY 'zing' % add_a;
    action add_b { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } b = any* VY 'zing' % add_b;
    action add_c { add(NNS, REM(form, 3), lemmas); return; } c = (any* S2 'ing' | any* C V 'lling' | any* C V CXY2 'ing' | CXY 'ing' | PRE* C V 'nging' | any* 'icking') % add_c;
    action add_d { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } d = any* C 'ining' % add_d;
    action add_e { add(NNS, REM(form, 3), lemmas); return; } e = any* C V [npwx] 'ing' % add_e;
    action add_f { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } f = (any* 'qu' V C 'ing' | any* 'u' V 'ding' | any* C 'leting' | PRE* C+ [ei] 'ting') % add_f;
    action add_g { add(NNS, REM(form, 3), lemmas); return; } g = any* ([ei] 'ting' | PRE CXY CXY 'eating') % add_g;
    action add_h { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } h = any* V CXY CXY 'eating' % add_h;
    action add_i { add(NNS, REM(form, 3), lemmas); return; } i = any* any [eo] 'ating' % add_i;
    action add_j { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } j = any* (any V 'ating' | V V [cgsv] 'ing') % add_j;
    action add_k { add(NNS, REM(form, 3), lemmas); return; } k = any* (V V C 'ing' | any [rw] 'ling') % add_k;
    action add_l { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } l = any* (any 'thing' | CXY [cglsv] 'ing') % add_l;
    action add_m { add(NNS, REM(form, 3), lemmas); return; } m = any* CXY CXY 'ing' % add_m;
    action add_n { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } n = any* 'uing' % add_n;
    action add_o { add(NNS, REM(form, 3), lemmas); return; } o = any* (VY VY 'ing' | 'ying' | CXY 'oing') % add_o;
    action add_p { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } p = (PRE* C+ 'oring' | any* C [clt] 'oring') % add_p;
    action add_q { add(NNS, REM(form, 3), lemmas); return; } q = any* [eo] 'ring' % add_q;
    action add_r { add(NNS, REM_ADD(form, 3, "e"), lemmas); return; } r = any* 'ing' % add_r;

    main := a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r;
    write init;
    write exec;
  }%%
}

%% machine VBD_VBN; include common; write data;
void english_morpho_guesser::add_VBD_VBN(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { add(VBD, VBN, REM(form, 1), lemmas); return; } a = any* 'en' % add_a;
    action add_b { add(VBD, VBN, REM(form, 2), lemmas); return; } b = any* CXY 'zed' % add_b;
    action add_c { add(VBD, VBN, REM(form, 1), lemmas); return; } c = any* VY 'zed' % add_c;
    action add_d { add(VBD, VBN, REM(form, 2), lemmas); return; } d = (any* S2 'ed' | any* C V 'lled' | any* C V CXY2 'ed' | CXY 'ed' | PRE* C V 'nged' | any* 'icked') % add_d;
    action add_e { add(VBD, VBN, REM(form, 1), lemmas); return; } e = any* (C 'ined' | C [clnt] 'ored') % add_e;
    action add_f { add(VBD, VBN, REM(form, 2), lemmas); return; } f = any* C V [npwx] 'ed' % add_f;
    action add_g { add(VBD, VBN, REM(form, 1), lemmas); return; } g = PRE* C+ 'ored' % add_g;
    action add_h { add(VBD, VBN, REM(form, 2), lemmas); return; } h = any* [eo] 'red' % add_h;
    action add_i { add(VBD, VBN, REM_ADD(form, 3, "y"), lemmas); return; } i = any* C 'ied' % add_i;
    action add_j { add(VBD, VBN, REM(form, 1), lemmas); return; } j = (any* 'qu' V C 'ed' | any* 'u' V 'ded' | any* C 'leted' | PRE* C+ [ei] 'ted') % add_j;
    action add_k { add(VBD, VBN, REM(form, 2), lemmas); return; } k = any* ([ei] 'ted' | PRE CXY CXY 'eated') % add_k;
    action add_l { add(VBD, VBN, REM(form, 1), lemmas); return; } l = any* V CXY CXY 'eated' % add_l;
    action add_m { add(VBD, VBN, REM(form, 2), lemmas); return; } m = any* any [eo] 'ated' % add_m;
    action add_n { add(VBD, VBN, REM(form, 1), lemmas); return; } n = any* (any V 'ated' | V V [cgsv] 'ed') % add_n;
    action add_o { add(VBD, VBN, REM(form, 2), lemmas); return; } o = any* (V V C 'ed' | any [rw] 'led') % add_o;
    action add_p { add(VBD, VBN, REM(form, 1), lemmas); return; } p = any* (any 'thed' | 'ued' | CXY [cglsv] 'ed') % add_p;
    action add_q { add(VBD, VBN, REM(form, 2), lemmas); return; } q = any* (CXY CXY 'ed' | VY VY 'ed') % add_q;
    action add_r { add(VBD, VBN, REM(form, 1), lemmas); return; } r = any* 'ed' % add_r;

    main := a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r;
    write init;
    write exec;
  }%%
}

%% machine VBZ; include common; write data;
void english_morpho_guesser::add_VBZ(const string& form, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str(); int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { add(VBZ, REM(form, 1), lemmas); return; } a = V 'ses' % add_a;
    action add_b { add(VBZ, REM(form, 2), lemmas); return; } b = any CXY 'zes' % add_b;
    action add_c { add(VBZ, REM(form, 1), lemmas); return; } c = VY 'zes' % add_c;
    action add_d { add(VBZ, REM(form, 2), lemmas); return; } d = S2 'es' % add_d;
    action add_e { add(VBZ, REM(form, 1), lemmas); return; } e = (any V 'rses' | 'onses') % add_e;
    action add_f { add(VBZ, REM(form, 2), lemmas); return; } f = S 'es' % add_f;
    action add_g { add(VBZ, REM_ADD(form, 3, "y"), lemmas); return; } g = any C 'ies' % add_g;
    action add_h { add(VBZ, REM(form, 2), lemmas); return; } h = CXY 'oes' % add_h;
    action add_i { add(VBZ, REM(form, 1), lemmas); return; } i = any 's' % add_i;

    main := any* (a | b | c | d | e | f | g | h | i);
    write init;
    write exec;
  }%%
}

%% machine JJR_RBR; include common; write data;
void english_morpho_guesser::add_JJR_RBR(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { add(JJR, RBR, REM(form, 2), negation_len, lemmas); return; } a = [^e] 'ller' % add_a;
    action add_b { add(JJR, RBR, REM(form, 3), negation_len, lemmas); return; } b = C2 'er' % add_b;
    action add_c { add(JJR, RBR, REM_ADD(form, 3, "y"), negation_len, lemmas); return; } c = 'ier' % add_c;
    action add_d { add(JJR, RBR, REM(form, 2), negation_len, lemmas); return; } d = (V V C 'er' | C V [npwx] 'er') % add_d;
    action add_e { add(JJR, RBR, REM(form, 1), negation_len, lemmas); return; } e = (V C 'er' | (CXY - 'n') [cglsv] 'er' | [ue] 'er') % add_e;
    action add_f { add(JJR, RBR, REM(form, 2), negation_len, lemmas); return; } f = any 'er' % add_f;

    main := any* (a | b | c | d | e | f);
    write init;
    write exec;
  }%%
}

%% machine JJS_RBS; include common; write data;
void english_morpho_guesser::add_JJS_RBS(const string& form, unsigned negation_len, vector<tagged_lemma>& lemmas) const {
  const char* p = form.c_str() + negation_len; int cs;
  %%{
    variable pe (form.c_str() + form.size());
    variable eof (form.c_str() + form.size());

    action add_a { add(JJS, RBS, REM(form, 3), negation_len, lemmas); return; } a = [^e] 'llest' % add_a;
    action add_b { add(JJS, RBS, REM(form, 4), negation_len, lemmas); return; } b = C2 'est' % add_b;
    action add_c { add(JJS, RBS, REM_ADD(form, 4, "y"), negation_len, lemmas); return; } c = 'iest' % add_c;
    action add_d { add(JJS, RBS, REM(form, 3), negation_len, lemmas); return; } d = (V V C 'est' | C V [npwx] 'est') % add_d;
    action add_e { add(JJS, RBS, REM(form, 2), negation_len, lemmas); return; } e = (V C 'est' | (CXY - 'n') [cglsv] 'est' | [ue] 'est') % add_e;
    action add_f { add(JJS, RBS, REM(form, 3), negation_len, lemmas); return; } f = any{3,} 'est' % add_f;

    main := any* (a | b | c | d | e | f);
    write init;
    write exec;
  }%%
}

} // namespace morphodita
} // namespace ufal
