#!/bin/bash

NODE=$(hostname -s)

#SCRATCH=/scratch_local/phwilliams/gp
SCRATCH=./

cd /home/phwilliams/baseLineGP

#./trimSeedPopRand.pl $NODE.pop.log 20000 wildType.pop.dat
# Establish the wild type population for the first run

if [ ! -f wildType.pop.dat ]; then
  ls wildType.pop.dat
  exit
fi

./trimSeedPopRand.pl wildType.pop.dat 15000 $SCRATCH/$NODE.seedSource.pop.log

#exit


##########################################################################
for QFILE in 3X^2+2X-3.dat X^3+X^2+X+5.dat 2X^3+3X^2+2X+5.dat X^4-X^3+X^2+X-3.dat X^4-2X^3+3X^2+2X-3.dat X^5+2X^4-3X^3+2X^2+4X+5.dat
do
  if [ ! -f $QFILE ]; then
    echo Qfile not found
    exit
  fi

#  echo run set $QFILE
  START=$(date +"%s")
  echo run set $QFILE start $(date)

  STEM=$(basename $QFILE .dat)

  rm -f $SCRATCH/$NODE.$STEM.data
  touch $SCRATCH/$NODE.$STEM.data
  STATUS=0
  while [ $STATUS -eq 0 ]
  do
    ./buildNewRandSetAndRunReturnSuccessOrNot.sh $NODE $QFILE
    STATUS=$?
  done

  if [ $STATUS -eq 1 ]; then
    echo $START,$(date +"%s"),$NODE,$STEM,rand
  fi
  if [ $STATUS -eq 2 ]; then
    echo $START,$(date +"%s"),$NODE,$STEM,cubist
  fi
  if [ $STATUS -gt 2 ]; then
    echo ERROR return $STATUS $NODE $STEM
  fi

  echo " ----------------------------------------"

done

echo "node $NODE done"

#############################################################################################
exit


