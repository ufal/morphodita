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

#include "generic_tokenizer.h"
#include "utils/utf8.h"

namespace ufal {
namespace morphodita {

%%{
  machine generic_tokenizer;
  write data;
}%%

bool generic_tokenizer::next_sentence(vector<string_piece>& forms) {
  int cs, act;
  const char* ts, *te;
  const char* text_start = text;

  char32_t unary_chr;
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

    action unary_minus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unary_chr = utf8::first(unary_text, text - unary_text), !utf8::is_L(unary_chr) && !utf8::is_M(unary_chr) && !utf8::is_N(unary_chr) && !utf8::is_Pd(unary_chr)) }
    action unary_plus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unary_chr = utf8::first(unary_text, text - unary_text), !utf8::is_L(unary_chr) && !utf8::is_M(unary_chr) && !utf8::is_N(unary_chr) && unary_chr != '+') }
    whitespace = [\r\t\n] | utf8_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = utf8_L (utf8_L | utf8_M)*;
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed)? utf8_Nd+ ([.] utf8_Nd+)? ([eE] [+\-]? utf8_Nd+)?;

    # Segmentation
    action mark_whitespace { whitespace = text; }
    eos = [.!?] | '…';
    closing = '"' | ';' | utf8_Pe | utf8_Pf;
    opening = '"' | utf8_Ps | utf8_Pi;

    main := |*
      word | number | url | (utf8_any - whitespace)
        { forms.emplace_back(ts, te - ts);

          if (emergency_sentence_split(forms)) fbreak;
        };

      eos closing* whitespace+ >mark_whitespace opening* (utf8_Lu | utf8_Lt)
        {
          // Does this eos character marks end of sentence?
          bool eos_exception = false;
          if (!forms.empty()) {
            // Is it single Lut?
            string_piece form = forms.back();
            eos_exception = utf8::is_Lut(utf8::decode(form.str, form.len)) && !form.len;
          }

          // Add all characters until first space to forms and break if eos.
          for (text = ts; text < whitespace; forms.emplace_back(ts, text - ts), ts = text) utf8_advance(text, whitespace);
          fexec whitespace;
          if (!eos_exception) fbreak;
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
