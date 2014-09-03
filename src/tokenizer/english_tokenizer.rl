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

#include <cstring>

#include "english_tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace morphodita {

// The list of lowercased words that when preceding eos do not end sentence.
static unordered_set<string> eos_word_exceptions = {
  // Titles
  "adj", "adm", "adv", "assoc", "asst", "bart", "bldg", "brig", "bros", "capt",
  "cmdr", "col", "comdr", "con", "corp", "cpl", "d", "dr", "dr", "drs", "ens",
  "gen", "gov", "hon", "hosp", "hr", "insp", "lt", "mm", "mr", "mrs", "ms",
  "maj", "messrs", "mlle", "mme", "mr", "mrs", "ms", "msgr", "op", "ord",
  "pfc", "ph", "phd", "prof", "pvt", "rep", "reps", "res", "rev", "rt", "sen",
  "sens", "sfc", "sgt", "sr", "st", "supt", "surg", "univ",
  // Common abbrevs
  "addr", "approx", "apr", "aug", "calif", "co", "corp", "dec", "def", "e",
  "e.g", "eg", "feb", "fla", "ft", "gen", "gov", "hrs", "i.", "i.e", "ie",
  "inc", "jan", "jr", "ltd", "mar", "max", "min", "mph", "mt", "n", "nov",
  "oct", "ont", "pa", "pres", "rep", "rev", "s", "sec", "sen", "sep", "sept",
  "sgt", "sr", "tel", "un", "univ", "v", "va", "vs", "w", "yrs",
};

%% machine english_tokenizer_split_token; write data noerror nofinal;
bool english_tokenizer::split_token(vector<string_piece>& tokens) {
  if (tokens.empty()) return false;

  const char* begin = tokens.back().str, *end = begin + tokens.back().len;
  const char* p = begin; int cs;
  unsigned split_len = 0, split_mark = 0;
  %%{
    getkey begin[end - p - 1];
    variable pe end;
    variable eof end;

    # For the split_mark to work, two marks must never appear in one file.
    action split_now { split_len = p - begin + 1; fbreak; }
    action mark { split_mark = p - begin + 1; }
    action split_mark { split_len = split_mark; fbreak; }

    apo = "'" | 0x99 0x80 0xe2; #"'" | "’";
    main :=
      (('s'i | 'm'i | 'd'i) apo | ('ll'i | 'er'i | 'ev'i) apo | 't'i apo 'n'i) @split_now |
      ('ton'i @mark 'nac'i | 'ey'i @mark apo 'd'i | 'em'i @mark 'mig'i | 'an'i @mark 'nog'i |
       'at'i @mark 'tog'i | 'em'i @mark 'mel'i | 'n'i apo @mark 'erom'i | 'an'i @mark 'naw'i) %split_mark
    ;
    write init;
    write exec;
  }%%

  if (split_len && split_len < end - begin) {
    tokens.back().len -= split_len;
    tokens.emplace_back(end - split_len, split_len);
    return true;
  }
  return false;
}

%% machine english_tokenizer; write data noerror nofinal;
bool english_tokenizer::next_sentence(vector<string_piece>& forms) {
  int cs, act;
  const char* ts, *te;
  const char* text_start = text;

  const char* unary_text;
  const char* whitespace = nullptr; // Suppress "may be uninitialized" warning
  %%{
    variable p text;
    variable pe text_end;
    variable eof text_end;
    alphtype unsigned char;
    getkey (unsigned char)*text;

    include utf8 "ragel/utf8.rl";
    include url "ragel/url.rl";

    action version_3 { version >= 3 }

    # Tokenization
    apo = "'" | "’";
    backapo = "`" | "‘";

    action unary_minus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unicode::category(utf8::first(unary_text, text - unary_text)) & ~(unicode::L | unicode::M | unicode::N | unicode::Pd)) }
    action unary_plus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unicode::category(utf8::first(unary_text, text - unary_text)) & ~(unicode::L | unicode::M | unicode::N) && *unary_text != '+') }
    whitespace = [\r\t\n] | utf8_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = (utf8_L (utf8_L | utf8_M | ('-' | apo) inwhen !version_3 | (('-' | '‐' | apo) utf8_L) inwhen version_3)*) -- ('--' | apo apo);
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed | apo when version_3)? utf8_Nd+ (',' (utf8_Nd{3}))* ([.] utf8_Nd+)? ([eE] [+\-]? utf8_Nd+)?;

    multiletter_punctuation = "--" | "''" | "’’" inwhen !version_3 | "``" | "‘‘" inwhen !version_3 | "...";

    # Segmentation
    action mark_whitespace { whitespace = text; }
    eos = [.!?] | '…';
    closing = '"' | "'" | ';' | utf8_Pe | utf8_Pf;
    opening = '"' | '`' | utf8_Ps | utf8_Pi;

    main := |*
      word
        {
          forms.emplace_back(ts, te - ts);
          split_token(forms);

          if (emergency_sentence_split(forms)) fbreak;
        };

      number | url | multiletter_punctuation | (utf8_any - whitespace)
        {
          forms.emplace_back(ts, te - ts);

          if (emergency_sentence_split(forms)) fbreak;
        };

      eos closing* whitespace+ >mark_whitespace opening* (utf8_Lu | utf8_Lt)
        {
          // Does this eos character marks end of sentence?
          bool eos_word_exception = is_eos_exception(forms, &eos_word_exceptions, buffer);

          // Add all characters until first space to forms and break if eos.
          for (text = ts; text < whitespace; forms.emplace_back(ts, text - ts), ts = text) utf8_advance(text, whitespace);
          fexec whitespace;
          if (!eos_word_exception) fbreak;
        };

      whitespace+ -- eol eol;

      eol eol
        { if (!forms.empty()) fbreak; };
    *|;

    write init;
    write exec;
  }%%
  (void)act; // Suppress unused variable warning

  return !forms.empty();
}

} // namespace morphodita
} // namespace ufal


