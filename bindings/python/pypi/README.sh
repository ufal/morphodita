#!/bin/sh

# Prepare API documentation and examples
make -C ../../../doc morphodita_bindings_api.txt >/dev/null

cat <<"EOF"
ufal.morphodita
===============

The ``ufal.morphodita`` is a Python binding to MorphoDiTa library <http://ufal.mff.cuni.cz/morphodita>.

The bindings is a straightforward conversion of the ``C++`` bindings API.
In Python 2, strings can be both ``unicode`` and UTF-8 encoded ``str``, and the
library always produces ``unicode``. In Python 3, strings must be only ``str``.


Wrapped C++ API
---------------

The C++ API being wrapped follows. For a API reference of the original
C++ API, see <http://ufal.mff.cuni.cz/morphodita/api-reference>.

::

EOF
tail -n+4 ../../../doc/morphodita_bindings_api.txt | sed 's/^/  /'
cat <<EOF


Examples
========

run_morpho_cli
--------------

Simple example performing morphological analysis and generation::

EOF
sed '1,/^[^#]/d' ../examples/run_morpho_cli.py | sed 's/^/  /'
cat <<EOF

run_tagger
----------

Simple example performing tokenization and PoS tagging::

EOF
sed '1,/^[^#]/d' ../examples/run_tagger.py | sed 's/^/  /'
cat <<EOF


AUTHORS
=======

Milan Straka <straka@ufal.mff.cuni.cz>

Jana Straková <strakova@ufal.mff.cuni.cz>


COPYRIGHT AND LICENCE
=====================

Copyright 2014 by Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

MorphoDiTa is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

MorphoDiTa is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with MorphoDiTa.  If not, see <http://www.gnu.org/licenses/>.
