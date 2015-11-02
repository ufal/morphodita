// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstring>

#include "czech_tokenizer.h"
#include "unilib/unicode.h"
#include "unilib/utf8.h"

namespace ufal {
namespace morphodita {

%%{
  machine czech_tokenizer;
  write data noerror nofinal;
}%%

// The list of lower cased words that when preceding eos do not end sentence.
const unordered_set<string> czech_tokenizer::eos_word_exceptions_czech = {
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

const unordered_set<string> czech_tokenizer::eos_word_exceptions_slovak = {
  // Titles
  "prof", "csc", "drsc", "doc", "phd", "ph", "d",
  "judr", "mddr", "mudr", "mvdr", "paeddr", "paedr", "phdr", "rndr", "rsdr", "dr",
  "ing", "arch", "mgr", "bc", "mag", "mba", "bca", "mga",
  "gen", "plk", "pplk", "npor", "por", "ppor", "kpt", "mjr", "sgt", "pls", "p", "s",
  "p", "pí", "fa", "fy", "mr", "mrs", "ms", "miss", "tr", "sv",
  // Geographic names
  "angl", "fr", "čes", "ces", "čs", "cs", "slov", "nem", "it", "poľ", "pol", "maď", "mad",
  "rus", "sev", "vých", "vych", "juž", "juz", "záp", "zap",
  // Common abbrevs
  "adr", "č", "c", "eg", "ev", "g", "hod", "j", "kr", "m", "max", "min", "mj", "napr",
  "okr", "popr", "pozn", "r", "red", "rep", "resp", "srov", "st", "str",
  "sv", "tel", "tj", "tzv", "ú", "u", "uh", "ul", "um", "zl", "zn",
};

// List of hyphenated sequences that should be tokenized as words. Additional letters
// are allowed after the listed sequences. The uppercase letters must be uppercase to match,
// but the lowercase letters might be uppercased (or titlecased).
const hyphenated_sequences_map czech_tokenizer::hyphenated_sequences_czech = {
  "CD-ROM", "Chang-čou", "Chuang-min", "Chuang-pchu", "Coca-Col", "Frýdecko-Místecka",
  "Frýdecko-Místecko", "Frýdecko-Místecku", "Frýdek-Místek", "Frýdkem-Místkem",
  "Frýdku-Místku", "Guth-Jarkovský", "Gutha-Jarkovského", "Guthem-Jarkovským",
  "Guthovi-Jarkovskému", "Guthu-Jarkovském", "Harley-Davidson", "Hewlett-Packard",
  "IP-adres", "KDU-ČSL", "Ki-mun", "Koh-i-noor", "Kuang-tun", "Kuang-čou", "Kuo-fen",
  "Mercedes-Benz", "Mjong-ba", "Notre-Dame", "Orient-expres", "RM-Systém",
  "Rakousko-Uherska", "Rakousko-Uherskem", "Rakousko-Uhersko", "Rakousko-Uhersku",
  "Rolls-Royce", "Seton-Watson", "T-Mobil", "T-Mobile", "Tchaj-wan", "U-ramp",
  "US-DEU", "al-Kajd", "al-Kajdá", "al-Káid", "beta-blokátor", "coca-col",
  "duty-free", "e-business", "e-mail", "e-mailov", "fair-play", "fifty-fifty",
  "hands-free", "hi-fi", "hi-tech", "high-tech", "know-how", "kung-fu", "make-up",
  "marxismu-leninismu", "marxismus-leninismus", "on-line", "ping-pong", "ping-pongář",
  "play-off", "pop-music", "propan-butan", "sci-fi", "sex-appeal", "show-business"
};

czech_tokenizer::czech_tokenizer(tokenizer_language language, unsigned version) : version(version) {
  // Fill eos_word_exceptions and hyphenated_sequences.
  switch (language) {
    case CZECH:
      eos_word_exceptions = &eos_word_exceptions_czech;
      hyphenated_sequences = &hyphenated_sequences_czech;
      break;
    case SLOVAK:
      eos_word_exceptions = &eos_word_exceptions_slovak;
      hyphenated_sequences = nullptr;
      break;
  }
}

bool czech_tokenizer::next_sentence(vector<string_piece>& forms) {
  using namespace unilib;

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

    # Tokenization
    action unary_minus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unicode::category(utf8::first(unary_text, text - unary_text)) & ~(unicode::L | unicode::M | unicode::N | unicode::Pd)) }
    action unary_plus_allowed { text == text_start || (utf8_back(unary_text=text, text_start), unicode::category(utf8::first(unary_text, text - unary_text)) & ~(unicode::L | unicode::M | unicode::N) && *unary_text != '+') }
    whitespace = [\r\t\n] | utf8_Zs;
    eol = '\r' ('' >(eol,0) | '\n' >(eol,1)) | '\n' ('' >(eol,0) | '\r' >(eol,1));
    word = utf8_L (utf8_L | utf8_M)*;
    number = ('-' when unary_minus_allowed | '+' when unary_plus_allowed)? utf8_Nd+ ([.,] utf8_Nd+)? ([eE] [+\-]? utf8_Nd+)?;

    # Segmentation
    action mark_whitespace { whitespace = text; }
    eos = [.!?] | '…';
    closing = '"' | "'" | ';' | utf8_Pe | utf8_Pf;
    opening = '"' | '`' | utf8_Ps | utf8_Pi;

    main := |*
      word | number | url | (utf8_any - whitespace)
        { forms.emplace_back(ts, te - ts);

          if (hyphenated_sequences) hyphenated_sequences->join(forms, buffer);
          if (emergency_sentence_split(forms)) fbreak;
        };

      eos closing* whitespace+ >mark_whitespace opening* (utf8_Lu | utf8_Lt)
        {
          // Does this eos character marks end of sentence?
          bool eos_word_exception = is_eos_exception(forms, eos_word_exceptions, buffer);

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


