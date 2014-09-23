#!/bin/bash

NODE=$(hostname -s)

echo ./trimSeedPopRand.pl wildType.pop.dat 10000 $NODE.seedSource.pop.log

echo ./seedPopRand.pl $NODE.seedSource.pop.log 1000 $NODE

echo ./baselineSearchSpace.sh $NODE

##############################################################################
exit


