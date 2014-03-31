#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=Ufal-MorphoDiTa
rm -rf $dir
mkdir -p $dir

# Local files
cp Build.PL Changes README $dir

# MorphoDiTa sources
mkdir $dir/morphodita
cwd="`pwd`"
(cd $dir/morphodita && "$cwd"/../../common/gen_sources.sh)

# SWIG files
make -C .. morphodita_perl.cpp
cp ../morphodita_perl.cpp $dir/morphodita
mkdir -p $dir/lib/Ufal
cp ../Ufal/MorphoDiTa.pm MorphoDiTa.xs $dir/lib/Ufal

# Fill in version
sed "s/\\(dist_version *=> *\\)'[^']*'/\\1'$1'/" -i $dir/Build.PL
perl -e '%p=();while(<>){if (/^package ([^;]*);$/) {print;print "our \$VERSION = '"'$1'"';\n" if not $p{$1}; $p{$1}=1;} else {print;}}' -i $dir/lib/Ufal/MorphoDiTa.pm
