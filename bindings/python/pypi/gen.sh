#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=ufal.morphodita
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Changes ../../../LICENSE MANIFEST.in ../examples setup.py $dir

# MorphoDiTa sources
mkdir $dir/morphodita
cwd="`pwd`"
(cd $dir/morphodita && "$cwd"/../../common/gen_sources.sh)

# SWIG files
make -C .. morphodita_python.cpp
cp ../morphodita_python.cpp $dir/morphodita
cp -a ../ufal $dir

# Fill in version and sources
sed "s/^\\( *version *= *'\\)[^']*'/\\1$1'/" -i $dir/setup.py
sources=`(cd ufal.morphodita && find morphodita -name "*.cpp" -printf "'%p'\n" | sort | tr "\n" "," | head -c-1)`
sed "s#MORPHODITA_SOURCES#$sources#" -i $dir/setup.py

# README file
./README.sh >$dir/README
