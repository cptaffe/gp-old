#!/bin/bash
#    setBuilderRandomOrTrainedByProbRun.sh $RANDPROB $RANDSETSIZE $QFILE $CC
#    ./setBuilderRandomOrTrainedByProbRun.sh 5 15 2X^2+3.dat 85
# nohup ./setBuilderRandomOrTrainedByProbRun-V3.sh 5 15 2X^2+3.dat 85 > outSetBuilderRandomOrTrainedByProbRun.log &

# Must have current names file, $NODE.$STEM.names

if [ $# -lt 5 ]; then
  echo need RANDPROB RANDSETSIZE QFILE CC CPU0
  exit
fi

RANDPROB=$1
RANDSETSIZE=$2
QFILE=$3
CC=$4
#NODE=$(hostname)
NODE=$5
STEM=$(basename $QFILE .dat)

if [ ! -f $NODE.seedSource.pop.log ]; then
  ls $NODE.seedSource.pop.log
  exit
fi

STEM=$(basename $QFILE .dat)
DIR=$NODE.$STEM
MODELNAMESTEM=$NODE.$STEM
MODELNAMEDIR=$NODE.$STEM.models

# Exit this script when a solution is found
# The max fitness/parameter-set will be concatenated to the cubist data file for each run.
# The max fitness/parameter-set comes from each current NODE.CURVE.pop.log
# Delete each max fitness/parameter-set (NODE.CURVE.pop.log) after adding to cubist data file

# What is the richness of the cubist data set?
# The cubist data set may have to be filtered to remove an overabundance of low fitness values.

# If data file is at target size ($RANDSETSIZE) then train the model

# Check probability rand value to see if the rand set or the trained set is used.
# If the trained set is to be used first test if CC is >= $CC, if not then use rand set.

# If the CC is >= $CC then the model is used to predict the best parameters and run them.
# Else run the random set

wc -l $MODELNAMESTEM.data

CURRENTCOUNT=0
NUM=1
while [ $CURRENTCOUNT -lt $RANDSETSIZE ]
do
  echo run $NUM counts $CURRENTCOUNT
  (( NUM++ ))
  # Build new rand set and run. Return success or not.
  ./buildNewRandSetAndRunReturnSuccessOrNot.sh $NODE $QFILE
  if [ $? -eq 1 ]; then
#    echo "found approximate solutions in loop"
    exit 1
  fi
# The max fitness/parameter-set will be concatenated to the cubist data file for each run.
# The max fitness/parameter-set comes from each current NODE.CURVE.pop.log
# Delete each max fitness/parameter-set (NODE.CURVE.pop.log) before new run
#  CURRENTCOUNT=$(ls ./$DIR/* | wc -l)
  CURRENTCOUNT=$(cat $MODELNAMESTEM.data | wc -l)
done

MODELNAMEDIR=$NODE.$STEM.models
MODELNAMESTEM=$NODE.$STEM

cp model.names $NODE.$STEM.names

for (( X = 1; X <= $RANDPROB; X++ ))
do 
  ./loopThroughExclusionOfOnes.sh $NODE $QFILE
#  ./buildNewCubistModelRunReturnCCtestTrimOnes.sh $QFILE
  # The above script test several combinations and saves models
  #  in the following format ./node.Qfile.models/atribCount.CC.model.
  # The above directory content will be sorted and the top models used.

#  for MODEL in $(ls $MODELNAMEDIR/*.model | sort -t. -k1,1n -k2,2n)
  for MODEL in $(ls -Xr $MODELNAMEDIR/*.model)
  do
    echo check model $MODEL
#exit
    STEM=$(basename $MODEL .model)
    NEWCC=$(echo $STEM | cut -d. -f2)

    # If the newCC is >= $CC then the model is used to predict the best parameters and run them.
    # Else run the random set

    if [ $NEWCC -ge $CC ]; then
      # fix then use this model
      grep -v ^id= $MODEL > $MODELNAMESTEM.model
      # Build and run parameter set from current cubist model. Return success or not.
#      echo start ./buildAndRunParameterSetCurrentCubistModel.sh $QFILE from $0
      ./buildAndRunParameterSetCurrentCubistModel.sh $NODE $QFILE
      STATUS=$?
#      echo "exit status from buildAndRunParameterSetCurrentCubistModel.sh $QFILE is $STATUS"
      if [ $STATUS -eq 1 ]; then
        echo "found approximate solution from cubist set"
        exit 2
      fi
    else
#      echo start buildNewRandSetAndRunReturnSuccessOrNot.sh $NODE $QFILE from $0
      ./buildNewRandSetAndRunReturnSuccessOrNot.sh $NODE $QFILE
      STATUS=$?
#      echo "exit status $STATUS for buildNewRandSetAndRunReturnSuccessOrNot.sh $NODE $QFILE from $0"
      if [ $STATUS -eq 1 ]; then
        echo "found approximate solution from rand set"
        exit 1
      fi
    fi
  done
done

#echo "bottom of script $0 exit 0 next"

exit 0
#########################################################################################


