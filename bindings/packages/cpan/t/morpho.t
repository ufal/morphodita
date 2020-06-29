use Test::More tests => 23;
use utf8;

use Ufal::MorphoDiTa;

ok(my $morpho = Ufal::MorphoDiTa::Morpho::load('t/data/test.dict'));

sub analyze {
  my ($word, $results, $found) = @_;
  $found = $Ufal::MorphoDiTa::Morpho::NO_GUESSER unless $found;

  my $lemmas = Ufal::MorphoDiTa::TaggedLemmas->new();
  is($morpho->analyze($word, $Ufal::MorphoDiTa::Morpho::NO_GUESSER, $lemmas), $found);
  is_deeply([sort map { $lemmas->get($_)->{lemma}.'-'.$lemmas->get($_)->{tag} } (0 .. $lemmas->size()-1)],
            [sort split /\s+/, $results]);
}

sub generate {
  my ($word, $wildcard, $results) = @_;

  my $lemmas = Ufal::MorphoDiTa::TaggedLemmasForms->new();
  is($morpho->generate($word, $wildcard, $Ufal::MorphoDiTa::Morpho::NO_GUESSER, $lemmas), $Ufal::MorphoDiTa::Morpho::NO_GUESSER);
  is_deeply([sort map { my $l = $lemmas->get($_); map { $l->{lemma}.'-'.$l->{forms}->get($_)->{form}.'-'.$l->{forms}->get($_)->{tag} } (0 .. $l->{forms}->size()-1) } (0 .. $lemmas->size()-1)],
            [sort split /\s+/, $results]);
}

analyze("kočky", "kočka-NS2 kočka-NP1 kočka-NP4 kočka-NP5");
analyze("vidí", "vidět-VS3 vidět-VP3");
analyze("§", "§-PUNC");
analyze("<", "<-SYM");
analyze("…", "…-PUNC");
analyze("3.4e+12", "3.4e+12-NUM");
analyze("unknown", "unknown-UNK", -1);

generate("kočka", "", "kočka-kočka-NS1 kočka-kočky-NS2 kočka-kočce-NS3 kočka-kočku-NS4 kočka-kočko-NS5 kočka-kočce-NS6 kočka-kočkou-NS7 kočka-kočky-NP1 kočka-koček-NP2 kočka-kočkám-NP3 kočka-kočky-NP4 kočka-kočky-NP5 kočka-kočkách-NP6 kočka-kočkami-NP7");
generate("kočka", "?S[24]", "kočka-kočky-NS2 kočka-kočku-NS4");
generate("vidět", "", "vidět-vidím-VS1 vidět-vidíš-VS2 vidět-vidí-VS3 vidět-vidíme-VP1 vidět-vidíte-VP2 vidět-vidí-VP3");
generate("vidět", "[V][^S]3", "vidět-vidí-VP3");
