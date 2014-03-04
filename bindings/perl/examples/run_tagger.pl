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

sub encode_entities($) {
  my ($text) = @_;
  $text =~ s/[&<>"]/$& eq "&" ? "&amp;" : $& eq "<" ? "&lt;" : $& eq ">" ? "&gt;" : "&quot;"/ge;
  return $text;
}

@ARGV >= 1 or die "Usage: $0 tagger_file\n";

print STDERR "Loading tagger: ";
my $tagger = Tagger::load($ARGV[0]);
$tagger or die "Cannot load tagger from file '$ARGV[0]'\n";
print STDERR "done\n";
shift @ARGV;

my $forms = Forms->new();
my $lemmas = TaggedLemmas->new();
my $tokens = TokenRanges->new();
my $tokenizer = $tagger->newTokenizer();
$tokenizer or die "No tokenizer is defined for the supplied model!";

for (my $not_eof = 1; $not_eof; ) {
  my $text = '';

  # Read block
  while (1) {
    my $line = <>;
    last unless ($not_eof = defined $line);
    $text .= $line;
    chomp($line);
    last unless length $line;
  }

  # Tag
  $tokenizer->setText($text);
  my $t = 0;
  while ($tokenizer->nextSentence($forms, $tokens)) {
    $tagger->tag($forms, $lemmas);

    for (my ($i, $size) = (0, $lemmas->size()); $i < $size; $i++) {
      my $lemma = $lemmas->get($i);
      my $token = $tokens->get($i);
      my ($token_start, $token_length) = ($token->{start}, $token->{length});

      printf "%s%s<token lemma=\"%s\" tag=\"%s\">%s</token>%s",
        encode_entities(substr $text, $t, $token_start - $t),
        $i == 0 ? "<sentence>" : "",
        encode_entities($lemma->{lemma}),
        encode_entities($lemma->{tag}),
        encode_entities(substr $text, $token_start, $token_length),
        $i + 1 == $size ? "</sentence>" : "";
      $t = $token_start + $token_length;
    }
  }
  print encode_entities(substr $text, $t);
}
