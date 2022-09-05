#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=ufal.morphodita
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Changes ../../../LICENSE MANIFEST.in ../../../bindings/python/examples tests $dir
mkdir $dir/tests/data
cp ../test_data/test.dict ../test_data/test.tagger $dir/tests/data

# MorphoDiTa sources and SWIG files
make -C ../../../src_lib_only morphodita.cpp
make -C ../../../bindings/python clean
make -C ../../../bindings/python morphodita_python.cpp
cp -a ../../../bindings/python/ufal $dir
cp ../../../src_lib_only/morphodita.[ch]* $dir/ufal/morphodita
cp ../../../bindings/python/morphodita_python.cpp $dir/ufal/morphodita

# Fill in version
sed "s/^\\( *version *= *'\\)[^']*'/\\1$1'/" setup.py >$dir/setup.py
sed "s/^# *__version__ *=.*$/__version__ = \"$1\"/" $dir/ufal/morphodita/__init__.py -i

# README file
./README.sh >$dir/README

# Generate sdist
(cd ufal.morphodita && python3 setup.py sdist && cd dist && tar xf ufal.morphodita-$1.tar.gz)
