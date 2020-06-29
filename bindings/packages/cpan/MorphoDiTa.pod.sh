#!/bin/sh

# Prepare API documentation and examples
make -C ../../../doc morphodita_bindings_api.txt >/dev/null

cat <<"EOF"
=encoding utf-8

=head1 NAME

Ufal::MorphoDiTa - bindings to MorphoDiTa library L<http://ufal.mff.cuni.cz/morphodita>.

=head1 SYNOPSIS

  use Ufal::MorphoDiTa;

  my $tagger_file = 'english-morphium-wsj-140407.tagger';
  my $tagger = Ufal::MorphoDiTa::Tagger::load($tagger_file) or die "Cannot load tagger from file '$tagger_file'\n";
  my $forms  = Ufal::MorphoDiTa::Forms->new(); $forms->push($_) for qw(How are you ?);
  my $lemmas = Ufal::MorphoDiTa::TaggedLemmas->new();

  $tagger->tag($forms, $lemmas);

  for my $i (0 .. $lemmas->size()-1) {
    my $lemma = $lemmas->get($i);
    printf "token = %s, lemma = %s, tag = %s\n", $forms->get($i), $lemma->{lemma}, $lemma->{tag};
  }

=head1 REQUIREMENTS

To compile the module, C++11 compiler is needed, either C<g++> 4.7 or newer,
C<clang> 3.2 or newer or C<Visual Studio 2015>.

=head1 DESCRIPTION

C<Ufal::MorphoDiTa> is a Perl binding to MorphoDiTa library L<http://ufal.mff.cuni.cz/morphodita>.

The bindings is a straightforward conversion of the C++ bindings API.
Vectors do not have native Perl interface, see L<Ufal::MorphoDiTa::Forms>
source for reference. Static methods and enumerations are available only
through the module, not through object instance.

=head2 Wrapped C++ API

The C++ API being wrapped follows. For a API reference of the original
C++ API, see L\<http://ufal.mff.cuni.cz/morphodita/api-reference\>.

EOF
tail -n+4 ../../../doc/morphodita_bindings_api.txt | sed 's/^/  /'
cat <<EOF

=head1 Examples

=head2 run_morpho_cli

Simple example performing morphological analysis and generation.

EOF
sed '1,/^$/d' ../../../bindings/perl/examples/run_morpho_cli.pl | sed 's/^/  /'
cat <<EOF

=head2 run_tagger

Simple example performing tokenization and PoS tagging.

EOF
sed '1,/^$/d' ../../../bindings/perl/examples/run_tagger.pl | sed 's/^/  /'
cat <<EOF

=head1 AUTHORS

Milan Straka <straka@ufal.mff.cuni.cz>

Jana Straková <strakova@ufal.mff.cuni.cz>

=head1 COPYRIGHT AND LICENCE

Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

=cut
EOF
