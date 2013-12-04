# This file is part of MorphoDiTa.
#
# Copyright 2013 by Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# MorphoDiTa is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# MorphoDiTa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with MorphoDiTa.  If not, see <http://www.gnu.org/licenses/>.

import sys

from ufal.morphodita import *

if len(sys.argv) < 2:
  sys.stderr.write('Usage: %s dict_file\n' % sys.argv[0])
  sys.exit(1)

sys.stderr.write('Loading dictionary: ')
morpho = Morpho.load(sys.argv[1])
if not morpho:
  sys.stderr.write("Cannot load dictionary from file '%s'\n" % sys.argv[1])
sys.stderr.write('done\n')

lemmas = TaggedLemmas()
lemmas_forms = TaggedLemmasForms()
line = sys.stdin.readline()
while line:
  tokens = line.rstrip('\r\n').split(' ')
  if len(tokens) == 1: # analyze
    result = morpho.analyze(tokens[0], morpho.GUESSER, lemmas)

    guesser = "Guesser " if result == morpho.GUESSER else ""
    for lemma in lemmas:
      print('%sLemma: %s %s' % (guesser, lemma.lemma, lemma.tag))
  elif len(tokens) == 2: # generate
    result = morpho.generate(tokens[0], tokens[1], morpho.GUESSER, lemmas_forms)

    guesser = "Guesser " if result == morpho.GUESSER else ""
    for lemma_forms in lemmas_forms:
      print('%sLemma: %s' % (guesser, lemma_forms.lemma))
      for form in lemma_forms.forms:
        print('  %s %s' % (form.form, form.tag))

  line = sys.stdin.readline()
