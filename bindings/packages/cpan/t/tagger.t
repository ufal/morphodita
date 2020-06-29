use Test::More tests => 9;
use utf8;

use Ufal::MorphoDiTa;

ok(my $tagger = Ufal::MorphoDiTa::Tagger::load('t/data/test.tagger'));

sub tag {
  my ($sentence, $results) = @_;

  ok(my $tokenizer = $tagger->newTokenizer());

  my $forms = Ufal::MorphoDiTa::Forms->new();
  my $lemmas = Ufal::MorphoDiTa::TaggedLemmas->new();

  $tokenizer->setText($sentence);
  ok($tokenizer->nextSentence($forms, undef));

  $tagger->tag($forms, $lemmas);
  is_deeply([map { $lemmas->get($_)->{lemma}.'-'.$lemmas->get($_)->{tag} } (0 .. $lemmas->size()-1)],
            [split /\s+/, $results]);

  ok(not $tokenizer->nextSentence($forms, undef));
}

tag("Vidím kočky.", "vidět-VS1 kočka-NP4 .-PUNC");
tag("Kočky vidí.", "kočka-NP1 vidět-VP3 .-PUNC");
