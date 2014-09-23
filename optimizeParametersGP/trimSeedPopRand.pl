#!/usr/bin/perl
# Phil Williams
#

use warnings;
use strict;

my $numArgs = $#ARGV + 1;
if ($numArgs < 3) { die("\nmust have input\n");}
# ./trimSeedPopRand.pl head-20000node19.pop.tmp 10000 node19.seedSource.pop.log
#
# Get command line args. 
my $Inputfile = $ARGV[0];
my $count = $ARGV[1];
my $outFile = $ARGV[2];

#my $outFile = $outFileStem . ".seedSource.pop.log";

#my @lineList;
my $line;
my $expre;
#my @splits;
my $ref_to_listOut;
my @listOut;
my $strLen;
my $setSize;

open (FILE, "<$Inputfile") || die("Cannot Open input File\n");
# write the header on the first line by itself
while ($line = <FILE>) {
  if (length($line) < 3500) {
    push(@listOut, $line);
  }
}
close(FILE);

$ref_to_listOut = shuffle(@listOut);

$setSize = scalar(@listOut);
if ($setSize < $count) {
  $count = $setSize;
}

open (FILE, ">$outFile") || die("Cannot Open input File\n");
for (my $x = 0; $x < $count; $x++) { 
  print FILE $ref_to_listOut->[$x];
}
close(FILE);

###############################################################
exit;

sub shuffle {
    my @a = splice @_;
    for my $i (0 .. $#a) {
        my $j = int rand @a;
        @a[$i, $j] = @a[$j, $i];
    }
    return \@a;
}

#############################################################

