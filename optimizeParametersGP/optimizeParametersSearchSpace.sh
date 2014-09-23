#!/bin/bash

if [ $# -lt 1 ]; then
  echo need CPUx
  exit
fi

NODE=$1

#./trimSeedPopRand.pl $NODE.pop.log 20000 wildType.pop.dat
# Establish the wild type population for the first run

if [ ! -f wildType.pop.dat ]; then
  ls wildType.pop.dat
  exit
fi

# Trim the *.pop.log down to only 10,000 lines
#./trimSeedPopRand.pl CPU1.pop.log 10000 $NODE.seedSource.pop.log
./trimSeedPopRand.pl wildType.pop.dat 10000 $NODE.seedSource.pop.log
#cp CPU1.pop.log $NODE.seedSource.pop.log

##########################################################################
for QFILE in 3X^2+2X-3.dat X^3+X^2+X+5.dat 2X^3+3X^2+2X+5.dat X^4-X^3+X^2+X-3.dat X^4-2X^3+3X^2+2X-3.dat X^5+2X^4-3X^3+2X^2+4X+5.dat
do
  if [ ! -f $QFILE ]; then
    echo Qfile not found
    exit
  fi

#  QFILESTEM=$(basename $QFILE .dat)
#  echo $QFILESTEM > ./currentQfile.log

#  echo run set $QFILE
  START=$(date +"%s")
  echo run set $QFILE start $(date)

  # Build directory by curve filename
  STEM=$(basename $QFILE .dat)
  DIR=$NODE.$STEM

#  cat $NODE.$STEM.data >> $STEM.data.all

  rm $NODE.$STEM.data
  touch $NODE.$STEM.data
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

#  mv ./currentQfile.log ./previousQfile.log

done

echo "node $NODE done"

#############################################################################################
exit


