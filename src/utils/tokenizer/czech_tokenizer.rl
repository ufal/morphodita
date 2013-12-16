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

#include "czech_tokenizer.h"

namespace ufal {
namespace utils {

%%{
  machine czech_tokenizer;
  write data;
}%%

bool czech_tokenizer::next_sentence(vector<string_piece>& forms) {
  int cs, act;
  const char* ts;
  const char* te;
  %%{
    variable p text;
    variable pe text_end;
    variable eof text_end;
    alphtype unsigned char;
    getkey (unsigned char)*text;

    include utf8 "ragel/utf8.rl";
    include url "ragel/url.rl";

    whitespace = [\r\t\n] | utf8_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = utf8_L (utf8_L | utf8_M | '-')* -- '--';
    number = '-'? '.'? utf8_Nd (utf8_Nd | '.')*;

    main := |*
      word |
      number |
      url |
      (utf8_any - whitespace)
        { forms.emplace_back(ts, te - ts); };

      whitespace+ -- eol eol;

      eol eol
        { fbreak; };
    *|;

    write init;
    write exec;
  }%%

  return !forms.empty();
}

} // namespace utils
} // namespace ufal


