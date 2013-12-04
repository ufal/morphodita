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

def nextSentence(forms):
  forms.clear()

  not_eof = True
  while True:
    line = sys.stdin.readline()
    not_eof = bool(line)
    line = line.rstrip('\r\n')
    if not line: break
    forms.append(line)

  return not_eof or not forms.empty()

if len(sys.argv) < 2:
  sys.stderr.write('Usage: %s tagger_file\n' % sys.argv[0])
  sys.exit(1)

sys.stderr.write('Loading tagger: ')
tagger = Tagger.load(sys.argv[1])
if not tagger:
  sys.stderr.write("Cannot load tagger from file '%s'\n" % sys.argv[1])
sys.stderr.write('done\n')

forms = Forms()
lemmas = TaggedLemmas()
while nextSentence(forms):
  tagger.tag(forms, lemmas)

  for lemma in lemmas:
    print('%s\t%s' % (lemma.lemma, lemma.tag))
  print('')
