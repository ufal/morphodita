#!/usr/bin/perl
use warnings;
use strict;
use open qw{:std :utf8};

my %cats;
my %classes;

open (my $unidata, "wget ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt -O- |") or die "Cannot download UnicodeData using wget: $!";
# load UnicodeData and produce required Ragel machines
while (<$unidata>) {
  chomp;
  my @parts = split /;/, $_, -1;
  die "Bad line $_ in UnicodeData.txt" unless @parts == 15;

  my ($code, $name, $cat, $upper, $lower, $title) = @parts[0, 1, 2, 12, 13, 14];
  next if $cat =~ /Co|Cs/;

  $code = hex($code);
  my $last_code = $code;
  if ($name =~ /^<(.*), First>$/) {
    my $range_name = $1;
    chomp (my $next_line = <$unidata>);
    my @next_parts = split /;/, $next_line, -1;
    my ($next_code, $next_name) = @next_parts[0, 1];
    $next_name =~ /^<$range_name, Last>$/ or die "Unrecognized end '$next_name' of range $range_name";
    $last_code = hex($next_code);
  }

  push @{$cats{$cat}}, ($code..$last_code);
  $classes{substr $cat, 0, 1}->{$cat} = 1;
}
close $unidata;

print <<EOF;
"%%{
  machine utf8;

  utf8_any = 0..127 | 128..255 (128..191)* \$(utf8_any,1) %(utf8_any,0);

EOF
foreach my $cat (sort keys %cats) {
  my @chrs = ();
  foreach my $chr (@{$cats{$cat}}) {
    my $utf8 = chr($chr);
    utf8::encode($utf8);
    push @chrs, $utf8;
  }

  my $i = 0;
  for (my $i = 0; $i < @chrs; $i++) {
    print !$i ? "  utf8_$cat =" : " |";
    my $fst = ord(substr $chrs[$i], -1, 1);
    while ($i + 1 < @chrs && substr($chrs[$i], 0, -1) eq substr($chrs[$i+1], 0, -1) && ord(substr($chrs[$i], -1, 1)) + 1 == ord(substr($chrs[$i+1], -1, 1))) {
      $i++;
    }
    my $lst = ord(substr $chrs[$i], -1, 1);

    foreach my $byte (split //, substr($chrs[$i], 0, -1)) {
      print " " . ord($byte);
    }
    print " " . ($fst eq $lst ? $fst : "$fst..$lst");
  }
  print ";\n";
}
foreach my $class (sort keys %classes) {
  my $i = 0;
  foreach my $cat (sort keys %{$classes{$class}}) {
    print !$i++ ? "  utf8_$class = " : " | ";
    print "utf8_$cat";
  }
  print ";\n";
}
print "}%%\n";
