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
#include <unordered_set>

#include "czech_tokenizer.h"
#include "utils/utf8.h"

namespace ufal {
namespace morphodita {

%%{
  machine czech_tokenizer;
  write data;
}%%

// The list of lower cased words that when preceding eos do not end sentence.
static unordered_set<string> eos_word_exceptions = {
  // Titles
  "prof", "csc", "drsc", "doc", "phd", "ph", "d",
  "judr", "mddr", "mudr", "mvdr", "paeddr", "paedr", "phdr", "rndr", "rsdr", "dr",
  "ing", "arch", "mgr", "bc", "mag", "mba", "bca", "mga",
  "gen", "plk", "pplk", "npor", "por", "ppor", "kpt", "mjr", "sgt", "pls", "p", "s",
  "p", "pí", "fa", "fy", "mr", "mrs", "ms", "miss", "tr", "sv",
  // Geographic names
  "angl", "fr", "čes", "ces", "čs", "cs", "slov", "něm", "nem", "it", "pol", "maď", "mad", "rus",
  "sev", "vých", "vych", "již", "jiz", "záp", "zap",
  // Common abbrevs
  "adr", "č", "c", "eg", "ev", "g", "hod", "j", "kr", "m", "max", "min", "mj", "např", "napr",
  "okr", "popř", "popr", "pozn", "r", "ř", "red", "rep", "resp", "srov", "st", "stř", "str",
  "sv", "tel", "tj", "tzv", "ú", "u", "uh", "ul", "um", "zl", "zn",
};

bool czech_tokenizer::next_sentence(vector<string_piece>& forms) {
  int cs, act;
  const char* ts;
  const char* te;
  const char* whitespace = nullptr; // Suppress may be uninitialized warning
  %%{
    variable p text;
    variable pe text_end;
    variable eof text_end;
    alphtype unsigned char;
    getkey (unsigned char)*text;

    include utf8 "ragel/utf8.rl";
    include url "ragel/url.rl";

    # Tokenization
    whitespace = [\r\t\n] | utf8_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = utf8_L (utf8_L | utf8_M | '-')* -- '--';
    number = '-'? '.'? utf8_Nd (utf8_Nd | '.')*;

    # Segmentation
    action mark_whitespace { whitespace = text; }
    eos = [.!?] | '…';
    closing = '"' | ';' | utf8_Pe | utf8_Pf;
    opening = '"' | utf8_Ps | utf8_Pi;

    main := |*
      word | number | url | (utf8_any - whitespace)
        { forms.emplace_back(ts, te - ts); };

      eos closing* whitespace+ >mark_whitespace opening* (utf8_Lu | utf8_Lt)
        {
          // Does this eos character marks end of sentence?
          bool eos_word_exception = false;
          if (!forms.empty()) {
            // Is it single Lut?
            string_piece form = forms.back();
            eos_word_exception = utf8::is_Lut(utf8::decode(form.str, form.len)) && !form.len;

            // Is the lower case variant in eos_word_exceptions?
            buffer.clear(); utf8::lowercase(forms.back().str, forms.back().len, buffer);
            eos_word_exception |= eos_word_exceptions.count(buffer);
          }

          // Add all characters until first space to forms and break if eos.
          for (text = ts; text < whitespace; forms.emplace_back(ts, text - ts), ts = text) utf8_advance(text, whitespace);
          fexec whitespace;
          if (!eos_word_exception) fbreak;
        };

      whitespace+ -- eol eol;

      eol eol
        { fbreak; };
    *|;

    write init;
    write exec;
  }%%
  (void)act; // Suppress unused variable warning

  return !forms.empty();
}

} // namespace morphodita
} // namespace ufal


