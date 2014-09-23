#!/usr/bin/perl
# Phil Williams
#

use warnings;
use strict;

my $numArgs = $#ARGV + 1;
if ($numArgs < 2) { die("\nmust have input\n");}
# ./writeNewMaxAndMinConfFile.pl allCPUs.3X^2+2X-3.csv maxAndMin.conf.bak
# cp maxAndMin.conf.bak maxAndMin.conf.tmp
# ./writeNewMaxAndMinConfFile.pl allCPUsMaxFitness.csv maxAndMin.conf.tmp

# Get command line args. 
my $Inputfilecsv = $ARGV[0];
my $confFile = $ARGV[1];

#my @lineList;
my $line;
my @splits;
my @attribTable;
my @confTable;
my @attribRangeList;
my @maxAndMinList;

open (FILE, "<$Inputfilecsv") || die("Cannot Open input File $Inputfilecsv\n");
# write the header on the first line by itself
while ($line = <FILE>) {
  chomp $line;
  @splits = split(/,/,$line);
  shift @splits;  
  push(@attribTable, [@splits]);
}
close(FILE);

open (FILE, "<$confFile") || die("Cannot Open Out File\n");
while ($line = <FILE>) {
  chomp $line;
  @splits = split(/,/,$line);
  push(@confTable, [@splits]); 
}
close(FILE);

for (my $j = 0; $j < scalar(@confTable); $j++) {
#  print "$confTable[$j][0],$confTable[$j][1],$confTable[$j][2]\n";
  for (my $x = 0; $x < scalar(@attribTable); $x++) {
    push(@attribRangeList, $attribTable[$x][$j]);
  }
#  foreach my $val(@attribRangeList) {
#    print "$val,"; 
#  }
#  print "\n";
  @maxAndMinList = maxAndMin(@attribRangeList);
  if ($confTable[$j][0] =~ /threshold/) {
#    print "$confTable[$j][0]\n";
    if ($confTable[$j][0] eq 'threshold') {
#      $confTable[$j][1] = substr($maxAndMinList[0], 1); 
#      $confTable[$j][2] = substr($maxAndMinList[1], 1);
      $confTable[$j][1] = 9; 
      $confTable[$j][2] = 6;
#      if ($confTable[$j][1] > 9) { $confTable[$j][1] = 1; }
#      if ($confTable[$j][2] > 9) { $confTable[$j][2] = 1; }
#      if ($confTable[$j][1] > 9) { $confTable[$j][1] = substr($maxAndMinList[0], 2); }
#      if ($confTable[$j][2] > 9) { $confTable[$j][2] = substr($maxAndMinList[1], 2); }
    } 
    if ($confTable[$j][0] eq 'thresholdDiviser') {
      $confTable[$j][1] = substr($maxAndMinList[1], 4); 
      $confTable[$j][2] = substr($maxAndMinList[0], 4);
    } 
  }
  else {
    $confTable[$j][1] = $maxAndMinList[0]; 
    $confTable[$j][2] = $maxAndMinList[1];
  }
  if ($confTable[$j][0] eq 'minKeepFitness') {
    if ($maxAndMinList[1] == -1) { # if min = -1
      # max = 3
      $confTable[$j][1] = 3; 
      $confTable[$j][2] = $maxAndMinList[0];
    }
  }
  if ($confTable[$j][0] eq 'multByNewSize') {
    $confTable[$j][1] = substr($maxAndMinList[1], 0, 1); 
#    $confTable[$j][1] = 2; 
    $confTable[$j][2] = substr($maxAndMinList[0], 0, 1);
#    $confTable[$j][2] = 1;
  }
  @attribRangeList = ();
}

open (FILE, ">$confFile") || die("Cannot Open Out File\n");
for (my $j = 0; $j < scalar(@confTable); $j++) {
  print FILE "$confTable[$j][0],$confTable[$j][1],$confTable[$j][2]\n";
#  print "$confTable[$j][0],$confTable[$j][1],$confTable[$j][2]\n";
}
close(FILE);


###############################################################
exit;

sub maxAndMin {
  my (@listIn) = @_;
  my @maxMin;

  my @sortList = sort { $a <=> $b } @listIn;

#  foreach my $val(@sortList)  {
#    print "$val ";
#  }
#  print "\n";

  push(@maxMin, $sortList[scalar(@sortList) -1]);

  # push on the min value
  push(@maxMin, $sortList[0]);

#  print "max $maxMin[0], min $maxMin[1]\n";

  return @maxMin;
}

#############################################################

