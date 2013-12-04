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

import cz.cuni.mff.ufal.morphodita.*;
import java.util.Scanner;

class RunMorpho {
  public static void main(String[] args) {
    if (args.length < 1) {
      System.err.println("Usage: RunMorpho dict_file");
      System.exit(1);
    }

    System.err.print("Loading dictionary: ");
    Morpho morpho = Morpho.load(args[0]);
    if (morpho == null) {
      System.err.println("Cannot load dictionary from file '" + args[0] + "'");
      System.exit(1);
    }
    System.err.println("done");

    TaggedLemmas lemmas = new TaggedLemmas();
    TaggedLemmasForms lemmas_forms = new TaggedLemmasForms();
    Scanner reader = new Scanner(System.in);
    while (reader.hasNextLine()) {
      String[] tokens = reader.nextLine().split(" ", -1);

      if (tokens.length == 1) /* analyze */ {
        int result = morpho.analyze(tokens[0], morpho.GUESSER, lemmas);

        String guesser = result == morpho.GUESSER ? "Guesser " : "";
        for (int i = 0; i < lemmas.size(); i++) {
          TaggedLemma lemma = lemmas.get(i);
          System.out.printf("%sLemma: %s %s\n", guesser, lemma.getLemma(), lemma.getTag());
        }
      } else if (tokens.length == 2) /* generate */ {
        int result = morpho.generate(tokens[0], tokens[1], morpho.GUESSER, lemmas_forms);
        String guesser = result == morpho.GUESSER ? "Guesser " : "";
        for (int i = 0; i < lemmas_forms.size(); i++) {
          TaggedLemmaForms lemma_forms = lemmas_forms.get(i);
          System.out.printf("%sLemma: %s\n", guesser, lemma_forms.getLemma());

          TaggedForms forms = lemma_forms.getForms();
          for (int j = 0; j < forms.size(); j++) {
            TaggedForm form = forms.get(j);
            System.out.printf("  %s %s\n", form.getForm(), form.getTag());
          }
        }
      }
    }
  }
}
