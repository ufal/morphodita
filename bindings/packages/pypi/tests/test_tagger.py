#!/usr/bin/python
# vim:fileencoding=utf8
from __future__ import unicode_literals

import os
import unittest

class TestTagger(unittest.TestCase):
    def test_tagger(self):
        import ufal.morphodita

        tagger = ufal.morphodita.Tagger.load(os.path.join(os.path.dirname(__file__),  "data", "test.tagger"))
        self.assertTrue(tagger)

        def tag(self, sentence, results):
            tokenizer = tagger.newTokenizer()
            self.assertTrue(tokenizer)

            forms = ufal.morphodita.Forms()
            lemmas = ufal.morphodita.TaggedLemmas()

            tokenizer.setText(sentence);
            self.assertTrue(tokenizer.nextSentence(forms, None))

            tagger.tag(forms, lemmas)
            self.assertEqual(list(lemma.lemma+"-"+lemma.tag for lemma in lemmas), results.split())

            self.assertFalse(tokenizer.nextSentence(forms, None))

        tag(self, "Vidím kočky.", "vidět-VS1 kočka-NP4 .-PUNC");
        tag(self, "Kočky vidí.", "kočka-NP1 vidět-VP3 .-PUNC");

if __name__ == '__main__':
    unittest.main()
