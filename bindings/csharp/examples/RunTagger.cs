// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
