# This file is part of MorphoDiTa.
#
# Copyright 2013 by Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# MorphoDiTa is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# MorphoDiTa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with MorphoDiTa.  If not, see <http://www.gnu.org/licenses/>.

use warnings;
use strict;
use open qw(:std :utf8);

use Ufal::MorphoDiTa qw(:all);

@ARGV >= 1 or die "Usage: $0 dict_file\n";

print STDERR "Loading dictionary: ";
my $morpho = Morpho::load($ARGV[0]);
$morpho or die "Cannot load dictionary from file '$ARGV[0]'\n";
print STDERR "done\n";
shift @ARGV;

my $lemmas = TaggedLemmas->new();
my $lemmas_forms = TaggedLemmasForms->new();
while (<>) {
  chomp;
  my @tokens = split /\s+/, $_, -1;
  if (@tokens == 1) { #Analyze
    my $result = $morpho->analyze($tokens[0], $Morpho::GUESSER, $lemmas);
    my $guesser = $result == $Morpho::GUESSER ? "Guesser " : "";

    for (my ($i, $size) = (0, $lemmas->size()); $i < $size; $i++) {
      my $lemma = $lemmas->get($i);
      printf "%sLemma: %s %s\n", $guesser, $lemma->{lemma}, $lemma->{tag};
    }
  } elsif (@tokens == 2) { #Generate
    my $result = $morpho->generate($tokens[0], $tokens[1], $Morpho::GUESSER, $lemmas_forms);
    my $guesser = $result == $Morpho::GUESSER ? "Guesser " : "";

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
