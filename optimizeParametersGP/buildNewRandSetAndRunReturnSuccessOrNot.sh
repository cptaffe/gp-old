#!/bin/bash

if [ $# -lt 2 ]; then
  echo need NODE Qfile
  exit
fi

if [ ! -f maxAndMin.conf ]; then
  echo maxAndMin.conf not found
  exit
fi

cd /home/phwilliams/optimizeParametersGP

NODE=$1
#FILEOUT=$1
QFILE=$2
PROG=gp_onDaughterCell-V10

#SCRATCH=/scratch_local/phwilliams/gp
SCRATCH=./

STEM=$(basename $QFILE .dat)

touch ./lockmaxAndMinConf

for LINE in $(cat maxAndMin.conf)
do
#  echo $LINE
  ATTRIB=$(echo $LINE | cut -d, -f1)
  MAX=$(echo $LINE | cut -d, -f2)
  MIN=$(echo $LINE | cut -d, -f3)
  RANDVAL=$[ ( $RANDOM % $MAX )  + $MIN ]
  (( $ATTRIB=$RANDVAL ))
#  if [ $ATTRIB == thresholdDiviser ]; then
#    thresholdDiviser=-0.0$RANDVAL
#  fi
#  echo $ATTRIB = $MAX  + $MIN 
#  echo $ATTRIB is $RANDVAL
done

rm ./lockmaxAndMinConf

if [ $minKeepFitness -eq 3 ]; then
  minKeepFitness=-1
fi
threshold=0.$threshold
thresholdDiviser=-0.0$thresholdDiviser

#MIN=1
#MAX=2
#tmpValInt=$[ ( $RANDOM % $MAX )]
tmpValInt=$multByNewSize
if [ $tmpValInt -eq 0 ]; then
  MIN=7
  MAX=3
  tmpValFloat=$[ ( $RANDOM % $MAX ) + $MIN ]
else
  MIN=1
  MAX=9
  tmpValFloat=$[ ( $RANDOM % $MAX ) + $MIN ]
fi
multByNewSize="$tmpValInt.$tmpValFloat"


#echo    ./$PROG $g $popsize $randpopsize $threshold $thresholdDiviser $intronProb $genejumpProb $founderMinFitness $ratioOfXoverMainadf $exitMultValue $popTargetSize $minKeepFitness $scaleFitness $pickForInsertionProb $pickForSelfCrossProb $gDiff $deltaPunkInk $multByNewSize $maxChroLen $pickForSpliceOutGene $minGeneSizeOut $maxGeneSizeOut $NODE $QFILE


#exit

  rm -f $SCRATCH/$NODE.results.dat

  for ((X = 1; X <= 1000; X++))
  do
    ./$PROG $g $popsize $randpopsize $threshold $thresholdDiviser $intronProb $genejumpProb $founderMinFitness $ratioOfXoverMainadf $exitMultValue $popTargetSize $minKeepFitness $scaleFitness $pickForInsertionProb $pickForSelfCrossProb $gDiff $deltaPunkInk $multByNewSize $maxChroLen $pickForSpliceOutGene $minGeneSizeOut $maxGeneSizeOut $NODE $QFILE > $SCRATCH/lastRun.$NODE.log
    STATUS=$?

    if [ $STATUS -gt 1 ]; then
      echo exit status of $STATUS ERROR
      cp $SCRATCH/lastRun.$NODE.log $SCRATCH/lastRun.$NODE.error
      echo ./$PROG $g $popsize $randpopsize $threshold $thresholdDiviser $intronProb $genejumpProb $founderMinFitness $ratioOfXoverMainadf $exitMultValue $popTargetSize $minKeepFitness $scaleFitness $pickForInsertionProb $pickForSelfCrossProb $gDiff $deltaPunkInk $multByNewSize $maxChroLen $pickForSpliceOutGene $minGeneSizeOut $maxGeneSizeOut $NODE $QFILE >> $SCRATCH/lastRun.$NODE.error
      exit 1
    fi
  
    mv $SCRATCH/$NODE.pop.log $SCRATCH/$NODE.pop.log.tmp
    ./trimSeedPopRand.pl $SCRATCH/$NODE.pop.log.tmp 15000 $SCRATCH/$NODE.pop.log

    if [ $STATUS -eq 1 ]; then
      break
    fi
  done

CASE=$g,$popsize,$randpopsize,$threshold,$thresholdDiviser,$intronProb,$genejumpProb,$founderMinFitness,$ratioOfXoverMainadf,$exitMultValue,$popTargetSize,$minKeepFitness,$scaleFitness,$pickForInsertionProb,$pickForSelfCrossProb,$gDiff,$deltaPunkInk,$multByNewSize,$maxChroLen,$pickForSpliceOutGene,$minGeneSizeOut,$maxGeneSizeOut
MAX=$(cut -d"," -f1 $SCRATCH/$NODE.results.dat | sort -k1,1n | tail -1)
echo $MAX,$CASE >> $SCRATCH/$NODE.$STEM.data

if [ $STATUS -eq 1 ]; then
  cp $SCRATCH/$NODE.pop.log.tmp $SCRATCH/$NODE.$STEM.txt
  grep ^150, $SCRATCH/$NODE.pop.log.tmp > $SCRATCH/$NODE.$STEM.150
  exit 1
# Stop when the first solution is found
fi


exit 0
####################################################################################################

