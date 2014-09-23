#!/bin/bash

RUNTYPE=optimize
ARCHIVEDIR=/home/phwilliams/archiveGP

# The allotted time has passed so kill all processes and write data to archive

cd /home/phwilliams/optimizeParametersGP

#pkill -f ./parameterOptimizer-V2.sh

for JOBID in $(cut -d. -f1 optimize_gp.jobID.lst)
do
  /opt/torque/bin/qdel $JOBID 
done

# Write data to archive
DATESTR=$(date)

echo date string $DATESTR

MONTH=$(echo $DATESTR | cut -d" " -f2)
DAY=$(echo $DATESTR | cut -d" " -f3)
HOUR=$(echo $DATESTR | cut -d":" -f1 | cut -d" " -f4)
MIN=$(echo $DATESTR | cut -d":" -f2)

echo date string $MONTH.$DAY.$HOUR.$MIN

OUTDIR=$ARCHIVEDIR/$RUNTYPE.$MONTH.$DAY.$HOUR.$MIN
mkdir -p $OUTDIR

OUTFILE=$OUTDIR/$RUNTYPE.$MONTH.$DAY.$HOUR.$MIN.csv

cat ./*Testing.compute-*.log | grep ,compute > $OUTFILE
grep ERROR ./*Testing.compute-*.log > $OUTDIR/errorFile.out
cp *.conf $OUTDIR
cp *.150 $OUTDIR

for QFILE in 3X^2+2X-3.dat X^3+X^2+X+5.dat 2X^3+3X^2+2X+5.dat X^4-X^3+X^2+X-3.dat X^4-2X^3+3X^2+2X-3.dat X^5+2X^4-3X^3+2X^2+4X+5.dat
do
  STEM=$(basename $QFILE .dat)
  cat compute-*$STEM.data > byQfile.tmp
  grep ^150, byQfile.tmp > 150.$STEM.par
done

cp 150.*.par $OUTDIR

#################################################################
exit


