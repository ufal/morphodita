#!/usr/bin/python
# vim:fileencoding=utf8
from __future__ import unicode_literals

import os
import unittest

class TestTagger(unittest.TestCase):
    def test_tagger(self):
        import ufal.morphodita

        morpho = ufal.morphodita.Morpho.load(os.path.join(os.path.dirname(__file__),  "data", "test.dict"))
        self.assertTrue(morpho)

        def analyze(self, word, results, found = morpho.NO_GUESSER):
            lemmas = ufal.morphodita.TaggedLemmas()
            self.assertEqual(morpho.analyze(word, morpho.NO_GUESSER, lemmas), found)
            self.assertEqual(sorted(lemma.lemma+'-'+lemma.tag for lemma in lemmas), sorted(results.split()))

        def generate(self, word, wildcard, results):
            lemmas = ufal.morphodita.TaggedLemmasForms()
            self.assertEqual(morpho.generate(word, wildcard, morpho.NO_GUESSER, lemmas), morpho.NO_GUESSER)
            self.assertEqual(sorted(lemma.lemma+'-'+form.form+'-'+form.tag for lemma in lemmas for form in lemma.forms), sorted(results.split()))

        analyze(self, "kočky", "kočka-NS2 kočka-NP1 kočka-NP4 kočka-NP5");
        analyze(self, "vidí", "vidět-VS3 vidět-VP3");
        analyze(self, "§", "§-PUNC");
        analyze(self, "<", "<-SYM");
        analyze(self, "…", "…-PUNC");
        analyze(self, "3.4e+12", "3.4e+12-NUM");
        analyze(self, "unknown", "unknown-UNK", -1);

        generate(self, "kočka", "", "kočka-kočka-NS1 kočka-kočky-NS2 kočka-kočce-NS3 kočka-kočku-NS4 kočka-kočko-NS5 kočka-kočce-NS6 kočka-kočkou-NS7 kočka-kočky-NP1 kočka-koček-NP2 kočka-kočkám-NP3 kočka-kočky-NP4 kočka-kočky-NP5 kočka-kočkách-NP6 kočka-kočkami-NP7");
        generate(self, "kočka", "?S[24]", "kočka-kočky-NS2 kočka-kočku-NS4");
        generate(self, "vidět", "", "vidět-vidím-VS1 vidět-vidíš-VS2 vidět-vidí-VS3 vidět-vidíme-VP1 vidět-vidíte-VP2 vidět-vidí-VP3");
        generate(self, "vidět", "[V][^S]3", "vidět-vidí-VP3");

if __name__ == '__main__':
    unittest.main()
