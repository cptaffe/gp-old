#!/bin/bash

##
# nohup ./parameterOptimizer-V2.sh > outParameterOptimizer-V2.log &
###

cd /home/phwilliams/optimizeParametersGP

#TARGETSIZE=10
TARGETSIZE=7

while true
do

  echo target size $TARGETSIZE

  FILESIZE=0
  while [ $FILESIZE -lt $TARGETSIZE ]
  do
    sleep 3600
    rm allCPUsMaxFitness.tmp
    for QFILE in 3X^2+2X-3.dat X^3+X^2+X+5.dat 2X^3+3X^2+2X+5.dat X^4-X^3+X^2+X-3.dat X^4-2X^3+3X^2+2X-3.dat X^5+2X^4-3X^3+2X^2+4X+5.dat
    do
      STEM=$(basename $QFILE .dat)

#      echo $STEM
      cat compute*.$STEM.data >> allCPUsMaxFitness.tmp 2> /dev/null
    done

    grep ^150, allCPUsMaxFitness.tmp | tail -$TARGETSIZE > allCPUsMaxFitness.csv

    wc -l allCPUsMaxFitness.csv
    FILESIZE=$(cat allCPUsMaxFitness.csv | wc -l)

#exit

  done

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

  OLDSIZE=$(cat allCPUsMaxFitness.csv | wc -l)
#  (( TARGETSIZE = TARGETSIZE + OLDSIZE ))
  (( TARGETSIZE++ ))

    sleep 3600

done

#######################################################################################
exit


