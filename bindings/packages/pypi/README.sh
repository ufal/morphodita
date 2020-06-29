#!/bin/sh

# Prepare API documentation and examples
make -C ../../../doc morphodita_bindings_api.txt >/dev/null

cat <<"EOF"
ufal.morphodita
===============

The ``ufal.morphodita`` is a Python binding to MorphoDiTa library <http://ufal.mff.cuni.cz/morphodita>.

The bindings is a straightforward conversion of the ``C++`` bindings API.
Python >=3 is supported.


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
sed '1,/^$/d' ../../../bindings/python/examples/run_morpho_cli.py | sed 's/^/  /'
cat <<EOF

run_tagger
----------

Simple example performing tokenization and PoS tagging::

EOF
sed '1,/^$/d' ../../../bindings/python/examples/run_tagger.py | sed 's/^/  /'
cat <<EOF


AUTHORS
=======

Milan Straka <straka@ufal.mff.cuni.cz>

Jana Straková <strakova@ufal.mff.cuni.cz>


COPYRIGHT AND LICENCE
=====================

Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
