#!/usr/bin/perl
# Phil Williams
#

use warnings;
use strict;

my $numArgs = $#ARGV + 1;
if ($numArgs < 3) { die("\nmust have input\n");}

# ./seedPopRand.pl head-200seedSource.pop.log 10 STEM

# Get command line args. 
my $Inputfile = $ARGV[0];
my $count = $ARGV[1];
my $outFileStem = $ARGV[2];

my $outFile = $outFileStem . ".pop.dat";

#my @lineList;
my $line;
my $expre;
my @splits;
my $ref_to_listOut;
my @listOut;

open (FILE, "<$Inputfile") || die("Cannot Open input File $Inputfile\n");
# write the header on the first line by itself
while ($line = <FILE>) {
  @splits = split(/,/,$line);
  push(@listOut, $splits[1]);
}
close(FILE);

$ref_to_listOut = shuffle(@listOut);

my $refListSize = scalar @{ $ref_to_listOut };

if ($refListSize < $count) {
  print "$refListSize < $count\n";
  $count = $refListSize;
#  exit 9;
}

open (FILE, ">$outFile") || die("Cannot Open Out File\n");
for (my $x = 0; $x < $count; $x++) { 
  print FILE $ref_to_listOut->[$x];
}
close(FILE);

###############################################################
exit 0;

sub shuffle {
    my @a = splice @_;
    for my $i (0 .. $#a) {
        my $j = int rand @a;
        @a[$i, $j] = @a[$j, $i];
    }
    return \@a;
}

#############################################################

