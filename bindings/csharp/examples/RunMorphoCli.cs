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
