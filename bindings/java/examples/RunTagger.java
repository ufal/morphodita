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

class RunTagger {
  public static boolean nextSentence(Scanner reader, Forms forms) {
    forms.clear();

    boolean not_eof = true;
    String line;
    while ((not_eof = reader.hasNextLine()) && !(line = reader.nextLine()).isEmpty())
      forms.add(line);

    return not_eof || !forms.isEmpty();
  }

  public static void main(String[] args) {
    if (args.length < 1) {
      System.err.println("Usage: RunTagger tagger_file");
      System.exit(1);
    }

    System.err.print("Loading tagger: ");
    Tagger tagger = Tagger.load(args[0]);
    if (tagger == null) {
      System.err.println("Cannot load tagger from file '" + args[0] + "'");
      System.exit(1);
    }
    System.err.println("done");

    Forms forms = new Forms();
    TaggedLemmas lemmas = new TaggedLemmas();
    Scanner reader = new Scanner(System.in);
    while (nextSentence(reader, forms)) {
      tagger.tag(forms, lemmas);

      for (int i = 0; i < lemmas.size(); i++) {
        TaggedLemma lemma = lemmas.get(i);
        System.out.printf("%s\t%s\n", lemma.getLemma(), lemma.getTag());
      }
      System.out.println();
    }
  }
}
