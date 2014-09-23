#!/bin/bash

##

cd /home/phwilliams/optimizeParametersGP

if [ ! -f currentTargetSize.log ]; then
  echo 7 > currentTargetSize.log
fi

TARGETSIZE=$(cat currentTargetSize.log)

echo target size $TARGETSIZE

#cat compute*.*.data > allCPUsMaxFitness.tmp 2> /dev/null
for QFILE in 3X^2+2X-3.dat X^3+X^2+X+5.dat 2X^3+3X^2+2X+5.dat X^4-X^3+X^2+X-3.dat X^4-2X^3+3X^2+2X-3.dat X^5+2X^4-3X^3+2X^2+4X+5.dat
do
  STEM=$(basename $QFILE .dat)

  echo $STEM
  cat compute-*.$STEM.data >> allCPUsMaxFitness.tmp 2> /dev/null
done

grep ^150, allCPUsMaxFitness.tmp | tail -$TARGETSIZE > allCPUsMaxFitness.csv

wc -l allCPUsMaxFitness.csv
FILESIZE=$(cat allCPUsMaxFitness.csv | wc -l)

if [ $FILESIZE -lt $TARGETSIZE ]; then
  echo $FILESIZE -lt $TARGETSIZE
  exit
fi

#exit


  ###############################################################

  while [ -f ./lockmaxAndMinConf ]
  do
    sleep 30
  done

  DATESTR=$(date)

  MONTH=$(echo $DATESTR | cut -d" " -f2)
  DAY=$(echo $DATESTR | cut -d" " -f3)
  HOUR=$(echo $DATESTR | cut -d":" -f1 | cut -d" " -f4)
  MIN=$(echo $DATESTR | cut -d":" -f2)

  echo date string $MONTH.$DAY.$HOUR.$MIN

  cp maxAndMin.conf $MONTH.$DAY.$HOUR.$MIN-maxAndMin.conf

  ./writeNewMaxAndMinConfFile.pl allCPUsMaxFitness.csv $MONTH.$DAY.$HOUR.$MIN-maxAndMin.conf

  diff $MONTH.$DAY.$HOUR.$MIN-maxAndMin.conf maxAndMin.conf
  if [ $? -ne 0 ]; then
    cp $MONTH.$DAY.$HOUR.$MIN-maxAndMin.conf maxAndMin.conf
  else
    rm $MONTH.$DAY.$HOUR.$MIN-maxAndMin.conf
  fi

  (( TARGETSIZE++ ))

echo $TARGETSIZE > currentTargetSize.log



#######################################################################################
exit


