use Test::More tests => 5;
use utf8;

use Ufal::MorphoDiTa;

my $tokenizer = Ufal::MorphoDiTa::Tokenizer::newCzechTokenizer();
my $forms = Ufal::MorphoDiTa::Forms->new();

$tokenizer->setText("Prezidentem Československa v letech 1918-1935 byl prof. T. G. Masaryk. Zemřel 14. září 1937 ve věku 87 let.");

ok($tokenizer->nextSentence($forms, undef));
is_deeply([map { $forms->get($_) } (0 .. $forms->size() - 1)],
          [qw(Prezidentem Československa v letech 1918 - 1935 byl prof . T . G . Masaryk .)]);

ok($tokenizer->nextSentence($forms, undef));
is_deeply([map { $forms->get($_) } (0 .. $forms->size() - 1)],
          [qw(Zemřel 14 . září 1937 ve věku 87 let .)]);

ok(!$tokenizer->nextSentence($forms, undef));
