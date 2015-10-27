// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

import cz.cuni.mff.ufal.morphodita.*;
import java.util.Scanner;

class RunMorphoCli {
  public static void main(String[] args) {
    if (args.length < 1) {
      System.err.println("Usage: RunMorphoCli dict_file");
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
      String[] tokens = reader.nextLine().split("\t", -1);

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
