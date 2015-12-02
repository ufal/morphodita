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

sub encode_entities($) {
  my ($text) = @_;
  $text =~ s/[&<>"]/$& eq "&" ? "&amp;" : $& eq "<" ? "&lt;" : $& eq ">" ? "&gt;" : "&quot;"/ge;
  return $text;
}

@ARGV >= 1 or die "Usage: $0 tagger_file\n";

print STDERR "Loading tagger: ";
my $tagger = Ufal::MorphoDiTa::Tagger::load($ARGV[0]);
$tagger or die "Cannot load tagger from file '$ARGV[0]'\n";
print STDERR "done\n";
shift @ARGV;

my $forms = Ufal::MorphoDiTa::Forms->new();
my $lemmas = Ufal::MorphoDiTa::TaggedLemmas->new();
my $tokens = Ufal::MorphoDiTa::TokenRanges->new();
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
