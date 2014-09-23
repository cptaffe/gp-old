#!/bin/bash

./randgen4 100000 wildType

mv wildType.pop.dat wildType.pop.tmp

while read LINE
do
  echo "  0, $LINE" >> wildType.pop.dat
done < wildType.pop.tmp

#############################################################################
exit


