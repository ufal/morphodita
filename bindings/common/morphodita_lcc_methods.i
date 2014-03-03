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

%rename("%(lowercamelcase)s") "tokenizer::set_text";
%rename("%(lowercamelcase)s") "tokenizer::next_sentence";
%rename("%(lowercamelcase)s") "tokenizer::new_vertical_tokenizer";
%rename("%(lowercamelcase)s") "tokenizer::new_czech_tokenizer";
%rename("%(lowercamelcase)s") "tokenizer::new_english_tokenizer";
%rename("%(lowercamelcase)s") "morpho::raw_lemma";
%rename("%(lowercamelcase)s") "morpho::lemma_id";
%rename("%(lowercamelcase)s") "morpho::new_tokenizer";
%rename("%(lowercamelcase)s") "tagger::get_morpho";
%rename("%(lowercamelcase)s") "tagger::new_tokenizer";
%rename("%(lowercamelcase)s") "tagset_converter::convert_analyzed";
%rename("%(lowercamelcase)s") "tagset_converter::convert_generated";
%rename("%(lowercamelcase)s") "tagset_converter::new_identity_converter";
%rename("%(lowercamelcase)s") "tagset_converter::new_pdt_to_conll2009_converter";

%include "morphodita.i"
