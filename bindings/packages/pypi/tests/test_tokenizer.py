#!/usr/bin/python
# vim:fileencoding=utf8
from __future__ import unicode_literals

import unittest

class TestTokenizer(unittest.TestCase):
    def test_tokenizer(self):
        import ufal.morphodita

        tokenizer = ufal.morphodita.Tokenizer.newCzechTokenizer()
        forms = ufal.morphodita.Forms()

        tokenizer.setText("Prezidentem Československa v letech 1918-1935 byl prof. T. G. Masaryk. Zemřel 14. září 1937 ve věku 87 let.");
        self.assertTrue(tokenizer.nextSentence(forms, None))
        self.assertEqual(list(forms), "Prezidentem Československa v letech 1918 - 1935 byl prof . T . G . Masaryk .".split())
        self.assertTrue(tokenizer.nextSentence(forms, None))
        self.assertEqual(list(forms), "Zemřel 14 . září 1937 ve věku 87 let .".split())
        self.assertFalse(tokenizer.nextSentence(forms, None))

if __name__ == '__main__':
    unittest.main()
