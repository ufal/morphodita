#!/bin/sh

set -e

[ -n "$1" ] || { echo Usage: $0 version >&2; exit 1; }

dir=Ufal-MorphoDiTa
rm -rf $dir
mkdir -p $dir

# Local files
cp -a Build.PL Changes ../../../LICENSE README ../examples $dir

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
perl -e '%p=(); while(<>) {
  print "=head1 VERSION\n\n'"$1"'\n\n" if /^=head1 DESCRIPTION/;
  print;
  print "our \$VERSION = '"'$1'"';\n" if /^package ([^;]*);$/ and not $p{$1}++;
} ' -i $dir/lib/Ufal/MorphoDiTa.pm

# POD file
./MorphoDiTa.pod.sh >$dir/lib/Ufal/MorphoDiTa.pod

# Generate manifest
(cd Ufal-MorphoDiTa && perl Build.PL && perl Build manifest && perl Build distclean && rm -f MANIFEST.SKIP.bak)
