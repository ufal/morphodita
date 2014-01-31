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
  public static void tagVertical(Tagger tagger) {
    Forms forms = new Forms();
    TaggedLemmas lemmas = new TaggedLemmas();
    Scanner reader = new Scanner(System.in);

    boolean not_eof = true;
    while (not_eof) {
      String line;

      forms.clear();
      while ((not_eof = reader.hasNextLine()) && !(line = reader.nextLine()).isEmpty())
        forms.add(line);

      if (!forms.isEmpty()) {
        tagger.tag(forms, lemmas);

        for (int i = 0; i < lemmas.size(); i++) {
          TaggedLemma lemma = lemmas.get(i);
          System.out.printf("%s\t%s\n", lemma.getLemma(), lemma.getTag());
        }
        System.out.println();
      }
    }
  }

  public static String encodeEntities(String text) {
    return text.replaceAll("&", "&amp;").replaceAll("<", "&lt;").replaceAll(">", "&gt;").replaceAll("\"", "&quot;");
  }

  public static void tagUntokenized(Tagger tagger) {
    Forms forms = new Forms();
    TaggedLemmas lemmas = new TaggedLemmas();
    TokenRanges tokens = new TokenRanges();
    Scanner reader = new Scanner(System.in);
    Tokenizer tokenizer = tagger.newTokenizer();
    if (tokenizer == null) {
      System.err.println("No tokenizer is defined for the supplied model!");
      System.exit(1);
    }

    boolean not_eof = true;
    while (not_eof) {
      StringBuilder textBuilder = new StringBuilder();
      String line;

      // Read block
      while ((not_eof = reader.hasNextLine()) && !(line = reader.nextLine()).isEmpty()) {
        textBuilder.append(line);
        textBuilder.append('\n');
      }
      if (not_eof) textBuilder.append('\n');

      // Tokenize and tag
      String text = textBuilder.toString();
      tokenizer.setText(text);
      int t = 0;
      while (tokenizer.nextSentence(forms, tokens)) {
        tagger.tag(forms, lemmas);

        for (int i = 0; i < lemmas.size(); i++) {
          TaggedLemma lemma = lemmas.get(i);
          TokenRange token = tokens.get(i);
          int token_start = (int)token.getStart(), token_end = token_start + (int)token.getLength();
          System.out.printf("%s<form lemma=\"%s\" tag=\"%s\">%s</form>",
                            encodeEntities(text.substring(t, token_start)),
                            encodeEntities(lemma.getLemma()),
                            encodeEntities(lemma.getTag()),
                            encodeEntities(text.substring(token_start, token_end)));
          t = token_end;
        }
      }
      System.out.print(encodeEntities(text.substring(t)));
    }
  }

  public static void main(String[] args) {
    int argi = 0;
    if (argi < args.length && args[argi].equals("-v")) argi++;
    boolean use_vertical = argi > 0;

    if (!(argi < args.length)) {
      System.err.println("Usage: RunTagger tagger_file");
      System.exit(1);
    }

    System.err.print("Loading tagger: ");
    Tagger tagger = Tagger.load(args[argi]);
    if (tagger == null) {
      System.err.println("Cannot load tagger from file '" + args[argi] + "'");
      System.exit(1);
    }
    System.err.println("done");

    if (use_vertical) tagVertical(tagger);
    else tagUntokenized(tagger);
  }
}
