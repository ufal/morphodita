# This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
#
# Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

use warnings;
use strict;
use open qw(:std :utf8);

use Ufal::MorphoDiTa;

@ARGV >= 1 or die "Usage: $0 dict_file\n";

print STDERR "Loading dictionary: ";
my $morpho = Ufal::MorphoDiTa::Morpho::load($ARGV[0]);
$morpho or die "Cannot load dictionary from file '$ARGV[0]'\n";
print STDERR "done\n";
shift @ARGV;

my $lemmas = Ufal::MorphoDiTa::TaggedLemmas->new();
my $lemmas_forms = Ufal::MorphoDiTa::TaggedLemmasForms->new();
while (<>) {
  chomp;
  my @tokens = split /\t/, $_, -1;
  if (@tokens == 1) { #Analyze
    my $result = $morpho->analyze($tokens[0], $Ufal::MorphoDiTa::Morpho::GUESSER, $lemmas);
    my $guesser = $result == $Ufal::MorphoDiTa::Morpho::GUESSER ? "Guesser " : "";

    for (my ($i, $size) = (0, $lemmas->size()); $i < $size; $i++) {
      my $lemma = $lemmas->get($i);
      printf "%sLemma: %s %s\n", $guesser, $lemma->{lemma}, $lemma->{tag};
    }
  } elsif (@tokens == 2) { #Generate
    my $result = $morpho->generate($tokens[0], $tokens[1], $Ufal::MorphoDiTa::Morpho::GUESSER, $lemmas_forms);
    my $guesser = $result == $Ufal::MorphoDiTa::Morpho::GUESSER ? "Guesser " : "";

    for (my $i = 0; $i < $lemmas_forms->size(); $i++) {
      my $lemma_forms = $lemmas_forms->get($i);
      printf "%sLemma: %s\n", $guesser, $lemma_forms->{lemma};
      for (my $i = 0; $i < $lemma_forms->{forms}->size(); $i++) {
        my $form = $lemma_forms->{forms}->get($i);
        printf "  %s %s\n", $form->{form}, $form->{tag};
      }
    }
  }
}
