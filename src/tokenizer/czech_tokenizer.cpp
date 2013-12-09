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

#include "czech_tokenizer.h"
#include "utils/utf8.h"

namespace ufal {
namespace morphodita {


static const int czech_tokenizer_start = 1;
static const int czech_tokenizer_first_final = 4;
static const int czech_tokenizer_error = 0;

static const int czech_tokenizer_en_main = 1;




void czech_tokenizer::set_text(const char* text) {
  this->text = text;
  chars = 0;
}

bool czech_tokenizer::next_sentence(vector<string_piece>* forms, vector<token_range>* tokens) {
  if (forms) forms->clear();
  if (tokens) tokens->clear();
  if (!text || !*text) return false;

  int cs;
  bool empty = true;
  const char* next = text;
  char32_t chr = utf8::decode(next);
  
	{
	cs = czech_tokenizer_start;
	}

	{
	short _widec;
	switch ( cs )
	{
tr2:
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 1; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st1;
tr5:
	{
      if (forms) forms->back().len = text - forms->back().str;
      if (tokens) tokens->back().length = chars - tokens->back().start;
    }
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 1; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st1;
st1:
	( text) += 1;
case 1:
	_widec = (*( text));
	_widec = (short)(1152 + ((*( text)) - -128));
	if ( 
 ufal::utils::utf8::is_L(chr)  ) _widec += 256;
	if ( 
 ufal::utils::utf8::is_Z(chr)  ) _widec += 512;
	switch( _widec ) {
		case 1293: goto tr2;
		case 1312: goto tr2;
		case 1549: goto tr2;
		case 1568: goto tr2;
	}
	if ( _widec < 1408 ) {
		if ( _widec < 1289 ) {
			if ( 1152 <= _widec && _widec <= 1288 )
				goto tr0;
		} else if ( _widec > 1290 ) {
			if ( 1291 <= _widec && _widec <= 1407 )
				goto tr0;
		} else
			goto tr2;
	} else if ( _widec > 1544 ) {
		if ( _widec < 1547 ) {
			if ( 1545 <= _widec && _widec <= 1546 )
				goto tr2;
		} else if ( _widec > 1663 ) {
			if ( 1664 <= _widec && _widec <= 2175 )
				goto tr2;
		} else
			goto tr3;
	} else
		goto tr3;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
	{
      empty = false;
      if (forms) forms->emplace_back(text, 0);
      if (tokens) tokens->emplace_back(chars, 0);
    }
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 2; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st2;
tr4:
	{
      if (forms) forms->back().len = text - forms->back().str;
      if (tokens) tokens->back().length = chars - tokens->back().start;
    }
	{
      empty = false;
      if (forms) forms->emplace_back(text, 0);
      if (tokens) tokens->emplace_back(chars, 0);
    }
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 2; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st2;
st2:
	( text) += 1;
case 2:
	_widec = (*( text));
	_widec = (short)(1152 + ((*( text)) - -128));
	if ( 
 ufal::utils::utf8::is_L(chr)  ) _widec += 256;
	if ( 
 ufal::utils::utf8::is_Z(chr)  ) _widec += 512;
	switch( _widec ) {
		case 1293: goto tr5;
		case 1312: goto tr5;
		case 1549: goto tr5;
		case 1568: goto tr5;
	}
	if ( _widec < 1408 ) {
		if ( _widec < 1289 ) {
			if ( 1152 <= _widec && _widec <= 1288 )
				goto tr4;
		} else if ( _widec > 1290 ) {
			if ( 1291 <= _widec && _widec <= 1407 )
				goto tr4;
		} else
			goto tr5;
	} else if ( _widec > 1544 ) {
		if ( _widec < 1547 ) {
			if ( 1545 <= _widec && _widec <= 1546 )
				goto tr5;
		} else if ( _widec > 1663 ) {
			if ( 1664 <= _widec && _widec <= 2175 )
				goto tr5;
		} else
			goto tr6;
	} else
		goto tr6;
	goto st0;
tr3:
	{
      empty = false;
      if (forms) forms->emplace_back(text, 0);
      if (tokens) tokens->emplace_back(chars, 0);
    }
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 3; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st3;
tr7:
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 3; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st3;
tr6:
	{
      if (forms) forms->back().len = text - forms->back().str;
      if (tokens) tokens->back().length = chars - tokens->back().start;
    }
	{
      empty = false;
      if (forms) forms->emplace_back(text, 0);
      if (tokens) tokens->emplace_back(chars, 0);
    }
	{
      text = next; chars++;
      if (!(chr = utf8::decode(next))) {( text)++; cs = 3; goto _out;}
      {( text) = (( text))-1;}
    }
	goto st3;
st3:
	( text) += 1;
case 3:
	_widec = (*( text));
	_widec = (short)(1152 + ((*( text)) - -128));
	if ( 
 ufal::utils::utf8::is_L(chr)  ) _widec += 256;
	if ( 
 ufal::utils::utf8::is_Z(chr)  ) _widec += 512;
	switch( _widec ) {
		case 1293: goto tr5;
		case 1312: goto tr5;
	}
	if ( _widec < 1291 ) {
		if ( _widec > 1288 ) {
			if ( 1289 <= _widec && _widec <= 1290 )
				goto tr5;
		} else if ( _widec >= 1152 )
			goto tr4;
	} else if ( _widec > 1407 ) {
		if ( _widec < 1664 ) {
			if ( 1408 <= _widec && _widec <= 1663 )
				goto tr7;
		} else if ( _widec > 1919 ) {
			if ( 1920 <= _widec && _widec <= 2175 )
				goto tr7;
		} else
			goto tr5;
	} else
		goto tr4;
	goto st0;
	}

	_out: {}
	}


  return !empty;
}

} // namespace morphodita
} // namespace ufal


