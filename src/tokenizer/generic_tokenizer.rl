// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "generic_tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace morphodita {

%%{
  machine generic_tokenizer;
  write data noerror nofinal;
}%%

bool generic_tokenizer::next_sentence(vector<string_piece>& forms) {
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

    action unary_minus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unicode::category(utf8::first(unary_text, text - unary_text)) & ~(unicode::L | unicode::M | unicode::N | unicode::Pd)) }
    action unary_plus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unicode::category(utf8::first(unary_text, text - unary_text)) & ~(unicode::L | unicode::M | unicode::N) && *unary_text != '+') }
    whitespace = [\r\t\n] | utf8_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = utf8_L (utf8_L | utf8_M)*;
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed)? utf8_Nd+ ([.] utf8_Nd+)? ([eE] [+\-]? utf8_Nd+)?;

    # Segmentation
    action mark_whitespace { whitespace = text; }
    eos = [.!?] | '…';
    closing = '"' | "'" | ';' | utf8_Pe | utf8_Pf;
    opening = '"' | '`' | utf8_Ps | utf8_Pi;

    main := |*
      word | number | url | (utf8_any - whitespace)
        { forms.emplace_back(ts, te - ts);

          if (emergency_sentence_split(forms)) fbreak;
        };

      eos closing* whitespace+ >mark_whitespace opening* (utf8_Lu | utf8_Lt)
        {
          // Does this eos character marks end of sentence?
          bool eos_exception = is_eos_exception(forms, nullptr, buffer);

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
