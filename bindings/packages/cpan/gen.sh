#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=Ufal-MorphoDiTa
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Build.PL Changes ../../../LICENSE README ../../../bindings/perl/examples t $dir
mkdir $dir/t/data
cp ../test_data/test.dict ../test_data/test.tagger $dir/t/data

# MorphoDiTa sources
make -C ../../../src_lib_only morphodita.cpp
mkdir $dir/morphodita
cp ../../../src_lib_only/morphodita.[ch]* $dir/morphodita

# SWIG files
make -C ../../../bindings/perl morphodita_perl.cpp
cp ../../../bindings/perl/morphodita_perl.cpp $dir/morphodita
mkdir -p $dir/lib/Ufal
cp ../../../bindings/perl/Ufal/MorphoDiTa.pm MorphoDiTa.xs $dir/lib/Ufal

# Fill in version
perl -e '%p=(); while(<>) {
  print "=head1 VERSION\n\n'"$1"'\n\n" if /^=head1 DESCRIPTION/;
  print;
  print "our \$VERSION = '"'$1'"';\n" if /^package ([^;]*);$/ and not $p{$1}++;
} ' -i $dir/lib/Ufal/MorphoDiTa.pm

# POD file
./MorphoDiTa.pod.sh >$dir/lib/Ufal/MorphoDiTa.pod

# Generate manifest
(cd Ufal-MorphoDiTa && perl Build.PL && perl Build manifest && perl Build dist && mv Ufal-MorphoDiTa-*$1.tar.gz .. && perl Build distclean && rm -f MANIFEST.SKIP.bak)
