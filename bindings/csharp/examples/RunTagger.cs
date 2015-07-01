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

using Ufal.MorphoDiTa;
using System;
using System.Text;
using System.Xml;

class RunTagger {
    public static int Main(string[] args) {
        if (args.Length < 1) {
            Console.Error.WriteLine("Usage: RunMorphoCli tagger_file");
            return 1;
        }

        Console.Error.Write("Loading tagger: ");
        Tagger tagger = Tagger.load(args[0]);
        if (tagger == null) {
            Console.Error.WriteLine("Cannot load tagger from file '{0}'", args[0]);
            return 1;
        }
        Console.Error.WriteLine("done");

        Forms forms = new Forms();
        TaggedLemmas lemmas = new TaggedLemmas();
        TokenRanges tokens = new TokenRanges();
        Tokenizer tokenizer = tagger.newTokenizer();
        if (tokenizer == null) {
            Console.Error.WriteLine("No tokenizer is defined for the supplied model!");
            return 1;
        }

        XmlTextWriter xmlOut = new XmlTextWriter(Console.Out);
        for (bool not_eof = true; not_eof; ) {
            string line;
            StringBuilder textBuilder = new StringBuilder();

            // Read block
            while ((not_eof = (line = Console.In.ReadLine()) != null) && line.Length > 0) {
                textBuilder.Append(line).Append('\n');
            }
            if (not_eof) textBuilder.Append('\n');

            // Tokenize and tag
            string text = textBuilder.ToString();
            tokenizer.setText(text);
            int t = 0;
            while (tokenizer.nextSentence(forms, tokens)) {
                tagger.tag(forms, lemmas);

                for (int i = 0; i < lemmas.Count; i++) {
                    TaggedLemma lemma = lemmas[i];
                    int token_start = (int)tokens[i].start, token_length = (int)tokens[i].length;
                    xmlOut.WriteString(text.Substring(t, token_start - t));
                    if (i == 0) xmlOut.WriteStartElement("sentence");
                    xmlOut.WriteStartElement("token");
                    xmlOut.WriteAttributeString("lemma", lemma.lemma);
                    xmlOut.WriteAttributeString("tag", lemma.tag);
                    xmlOut.WriteString(text.Substring(token_start, token_length));
                    xmlOut.WriteEndElement();
                    if (i + 1 == lemmas.Count) xmlOut.WriteEndElement();
                    t = token_start + token_length;
                }
            }
            xmlOut.WriteString(text.Substring(t));
        }
        return 0;
    }
}
