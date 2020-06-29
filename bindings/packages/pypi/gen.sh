#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=ufal.morphodita
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Changes ../../../LICENSE MANIFEST.in ../../../bindings/python/examples test $dir
mkdir $dir/test/data
cp ../test_data/test.dict ../test_data/test.tagger $dir/test/data

# MorphoDiTa sources
make -C ../../../src_lib_only morphodita.cpp
mkdir $dir/morphodita
cp ../../../src_lib_only/morphodita.[ch]* $dir/morphodita

# SWIG files
make -C ../../../bindings/python morphodita_python.cpp
cp ../../../bindings/python/morphodita_python.cpp $dir/morphodita
cp -a ../../../bindings/python/ufal $dir

# Fill in version
sed "s/^\\( *version *= *'\\)[^']*'/\\1$1'/" setup.py >$dir/setup.py
sed "s/^# *__version__ *=.*$/__version__ = \"$1\"/" $dir/ufal/morphodita.py -i

# README file
./README.sh >$dir/README

# Generate sdist
(cd ufal.morphodita && python setup.py sdist && cd dist && tar xf ufal.morphodita-$1.tar.gz)
