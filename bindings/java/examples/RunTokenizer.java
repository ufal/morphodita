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

class RunTokenizer {
  public static void main(String[] args) {
    if (args.length < 1) {
      System.err.println("Usage: RunTokenizer [-czech | dict_file]");
      System.exit(1);
    }

    Tokenizer tokenizer;
    if (args[0].equals("-czech")) {
      tokenizer = Tokenizer.newCzechTokenizer();
    } else {
      System.err.print("Loading dictionary: ");
      Morpho morpho = Morpho.load(args[0]);
      if (morpho == null) {
        System.err.println("Cannot load dictionary from file '" + args[0] + "'");
        System.exit(1);
      }
      System.err.println("done");
      tokenizer = morpho.newTokenizer();
    }

    TokenRanges tokens = new TokenRanges();
    Scanner reader = new Scanner(System.in);
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

      // Tokenize
      String text = textBuilder.toString();
      tokenizer.setText(text);
      while (tokenizer.nextSentence(null, tokens)) {
        for (int i = 0; i < tokens.size(); i++) {
          TokenRange token = tokens.get(i);
          int token_start = (int)token.getStart(), token_end = token_start + (int)token.getLength();
          System.out.print(text.substring(token_start, token_end));
          System.out.println();
        }
        System.out.println();
      }
    }
  }
}
