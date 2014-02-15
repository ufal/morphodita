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

# In Python2, wrap sys.stdin and sys.stdout to work with unicode.
if sys.version_info[0] < 3:
  import codecs
  import locale
  encoding = locale.getpreferredencoding()
  sys.stdin = codecs.getreader(encoding)(sys.stdin)
  sys.stdout = codecs.getwriter(encoding)(sys.stdout)

if len(sys.argv) < 2:
  sys.stderr.write('Usage: %s [--czech | dict_file]\n' % sys.argv[0])
  sys.exit(1)

if sys.argv[1] == "--czech":
  tokenizer = Tokenizer.newCzechTokenizer()
else:
  sys.stderr.write('Loading dictionary: ')
  morpho = Morpho.load(sys.argv[1])
  if not morpho:
    sys.stderr.write("Cannot load dictionary from file '%s'\n" % sys.argv[1])
    sys.exit(1)
  sys.stderr.write('done\n')
  tokenizer = morpho.newTokenizer()
  if tokenizer is None:
    sys.stderr.write("No tokenizer is defined for the supplied model!")
    sys.exit(1)

tokens = TokenRanges()
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

  # Tokenize
  tokenizer.setText(text)
  while tokenizer.nextSentence(None, tokens):
    for i in range(len(tokens)):
      token = tokens[i]
      sys.stdout.write(text[token.start : token.start + token.length])
      sys.stdout.write('\n')
    sys.stdout.write('\n')
