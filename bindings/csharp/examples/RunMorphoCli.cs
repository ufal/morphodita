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

class RunMorphoCli {
    public static int Main(string[] args) {
        if (args.Length < 1) {
            Console.Error.WriteLine("Usage: RunMorphoCli dict_file");
            return 1;
        }

        Console.Error.Write("Loading dictionary: ");
        Morpho morpho = Morpho.load(args[0]);
        if (morpho == null) {
            Console.Error.WriteLine("Cannot load dictionary from file '{0}'", args[0]);
            return 1;
        }
        Console.Error.WriteLine("done");

        TaggedLemmas lemmas = new TaggedLemmas();
        TaggedLemmasForms lemmasForms = new TaggedLemmasForms();
        for (string line; (line = Console.In.ReadLine()) != null; ) {
            string[] tokens = line.Split('\t');

            if (tokens.Length == 1) /* analyze */ {
                int result = morpho.analyze(tokens[0], Morpho.GUESSER, lemmas);

                string guesser = result == Morpho.GUESSER ? "Guesser " : "";
                foreach (TaggedLemma lemma in lemmas)
                    Console.WriteLine("{0}Lemma: {1} {2}", guesser, lemma.lemma, lemma.tag);
            } else if (tokens.Length == 2) /* generate */ {
                int result = morpho.generate(tokens[0], tokens[1], Morpho.GUESSER, lemmasForms);
                String guesser = result == Morpho.GUESSER ? "Guesser " : "";
                foreach (TaggedLemmaForms lemmaForms in lemmasForms) {
                    Console.WriteLine("{0}Lemma: {1}", guesser, lemmaForms.lemma);
                    foreach (TaggedForm form in lemmaForms.forms)
                            Console.WriteLine("  {0} {1}", form.form, form.tag);
                }
            }
        }

        return 0;
    }
}
