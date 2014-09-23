#!/bin/bash

cd /home/phwilliams/optimizeParametersGP

rm ./optimize_gp.jobID.lst

# clear old data for new run
./clearOldData.sh

echo 7 > currentTargetSize.log
cp maxAndMin.conf.bak maxAndMin.conf

for NUM in {0..8}
do

  echo $NUM
  /opt/torque/bin/qsub ./submitScriptByCron.pbs
  sleep 10

done

echo $0 done

#########################################################################
exit


