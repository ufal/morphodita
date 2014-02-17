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

def encode_entities(text):
  return text.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;').replace('"', '&quot;')

# In Python2, wrap sys.stdin and sys.stdout to work with unicode.
if sys.version_info[0] < 3:
  import codecs
  import locale
  encoding = locale.getpreferredencoding()
  sys.stdin = codecs.getreader(encoding)(sys.stdin)
  sys.stdout = codecs.getwriter(encoding)(sys.stdout)

if len(sys.argv) == 1:
  sys.stderr.write('Usage: %s tagger_file\n' % sys.argv[0])
  sys.exit(1)

sys.stderr.write('Loading tagger: ')
tagger = Tagger.load(sys.argv[1])
if not tagger:
  sys.stderr.write("Cannot load tagger from file '%s'\n" % sys.argv[1])
  sys.exit(1)
sys.stderr.write('done\n')

forms = Forms()
lemmas = TaggedLemmas()
tokens = TokenRanges()
tokenizer = tagger.newTokenizer()
if tokenizer is None:
  sys.stderr.write("No tokenizer is defined for the supplied model!")
  sys.exit(1)

not_eof = True
while not_eof:
  text = ''

  # Read block
  while True:
    line = sys.stdin.readline()
    not_eof = bool(line)
    if not not_eof: break
    line = line.rstrip('\r\n')
    text += line
    text += '\n';
    if not line: break



  # Tag
  tokenizer.setText(text)
  t = 0
  while tokenizer.nextSentence(forms, tokens):
    tagger.tag(forms, lemmas)

    for i in range(len(lemmas)):
      lemma = lemmas[i]
      token = tokens[i]
      sys.stdout.write('%s%s<token lemma="%s" tag="%s">%s</token>%s' % (
        encode_entities(text[t : token.start]),
        "<sentence>" if i == 0 else "",
        encode_entities(lemma.lemma),
        encode_entities(lemma.tag),
        encode_entities(text[token.start : token.start + token.length]),
        "</sentence>" if i + 1 == len(lemmas) else "",
      ))
      t = token.start + token.length
  sys.stdout.write(encode_entities(text[t : ]))
