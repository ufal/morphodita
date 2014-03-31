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
perl -e '%p=(); while(<>) {
  print "=head1 VERSION\n\n'"$1"'\n\n" if /^=head1 DESCRIPTION/;
  print;
  print "our \$VERSION = '"'$1'"';\n" if /^package ([^;]*);$/ and not $p{$1}++;
} ' -i $dir/lib/Ufal/MorphoDiTa.pm

# Prepare API documentation and examples
make -C ../../../doc morphodita_bindings_api.txt

( echo =head2 C++ API bindings
  echo
  tail -n+4 ../../../doc/morphodita_bindings_api.txt | sed 's/^/  /'
  echo
  echo =head1 Examples
  echo
  echo =head2 run_morpho_cli
  echo
  echo Simple example performing morphological analysis and generation.
  echo
  sed '1,/^[^#]/d' ../examples/run_morpho_cli.pl | sed 's/^/  /'
  echo
  echo =head2 run_tagger
  echo
  echo Simple example performing tokenization and PoS tagging.
  echo
  sed '1,/^[^#]/d' ../examples/run_tagger.pl | sed 's/^/  /'
  echo
) > gen.sh.doc

# Add it to pm
sed '/^=head1 AUTHOR/ {
  r gen.sh.doc
  a =head1 AUTHOR
  d
}' -i $dir/lib/Ufal/MorphoDiTa.pm

rm gen.sh.doc
