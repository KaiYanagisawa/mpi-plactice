#!/bin/bash
#
#
BIN=./sw-sample
TIMECMD="/usr/bin/time -p"

DIR=${1:-p0}
SCOREMATRIX=$DIR/score-matrix
QUERY=$DIR/query
DATABASE=$DIR/database

DATE=`date +%y%m%d-%H%M%S`
OUTPUT=$DIR/output.$DATE
LOG=$DIR/log.$DATE

echo "# testset $DIR"
$TIMECMD -o $LOG $BIN $SCOREMATRIX $QUERY $DATABASE > $OUTPUT
cat $LOG

P0SAMPLE=$DIR/output.sample
DIFF=`diff $P0SAMPLE $OUTPUT`
if [ -n "$DIFF" ]; then
  echo "# not matches sample output"
else
  echo "# matches sample output"
fi
