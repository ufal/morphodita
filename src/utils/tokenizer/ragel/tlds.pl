#!/usr/bin/perl
use warnings;
use strict;
use open qw{:std :utf8};

open (my $tlddata, "wget http://data.iana.org/TLD/tlds-alpha-by-domain.txt -O- |") or die "Cannot download TLDs using wget: $!";

my %tlds;
while (<$tlddata>) {
  chomp;
  next if /^#/;
  next if /^[Xx][Nn]-/;
  $tlds{$_} = 1;
}
close $tlddata;

print "%%{\n  machine tlds;\n\n";
my $i = 0;
foreach my $tld (sort keys %tlds) {
  print !$i++ ? "  tlds = " : "|";
  printf "'%s'|'%s'", uc($tld), lc($tld);
}
print ";\n}%%\n";
