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

class RunTagger {
  public static String encodeEntities(String text) {
    return text.replaceAll("&", "&amp;").replaceAll("<", "&lt;").replaceAll(">", "&gt;").replaceAll("\"", "&quot;");
  }

  public static void main(String[] args) {
    if (args.length == 0) {
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
          System.out.printf("%s%s<token lemma=\"%s\" tag=\"%s\">%s</token>%s",
                            encodeEntities(text.substring(t, token_start)),
                            i == 0 ? "<sentence>" : "",
                            encodeEntities(lemma.getLemma()),
                            encodeEntities(lemma.getTag()),
                            encodeEntities(text.substring(token_start, token_end)),
                            i + 1 == lemmas.size() ? "</sentence>" : "");
          t = token_end;
        }
      }
      System.out.print(encodeEntities(text.substring(t)));
    }
  }
}
