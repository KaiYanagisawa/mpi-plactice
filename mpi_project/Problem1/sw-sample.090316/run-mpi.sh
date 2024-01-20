#!/bin/bash
#
#
CORE=${2:-2}
BIN="mpiexec -n $CORE ./sw-mpi"
SORT_BIN="mpiexec -n $CORE ./sw-sort"
TIMECMD="/usr/bin/time -p"

DIR=${1:-p0}
SCOREMATRIX=$DIR/score-matrix
QUERY=$DIR/query
DATABASE=$DIR/database

DATE=`date +%y%m%d-%H%M%S`
OUTPUT=$DIR/output.$DATE
LOG=$DIR/log.$DATE

echo "# testset $DIR"
$TIMECMD -o $LOG $BIN $SCOREMATRIX $QUERY $DATABASE $OUTPUT > $OUTPUT
$TIMECMD -ao $LOG $SORT_BIN $OUTPUT
TOTAL_REAL=0
TOTAL_USER=0
TOTAL_SYS=0
while read -r line; do
  case $line in
    "real "*) TOTAL_REAL=$(echo "$TOTAL_REAL + $(awk '{print $2}' <<< "$line")" | bc -l | sed -e 's/^\./0./g') ;;
    "user "*) TOTAL_USER=$(echo "$TOTAL_USER + $(awk '{print $2}' <<< "$line")" | bc -l | sed -e 's/^\./0./g') ;;
    "sys "*) TOTAL_SYS=$(echo "$TOTAL_SYS + $(awk '{print $2}' <<< "$line")" | bc -l | sed -e 's/^\./0./g') ;;
  esac
done < $LOG
echo "real $TOTAL_REAL" > $LOG
echo "user $TOTAL_USER" >> $LOG
echo "sys $TOTAL_SYS" >> $LOG
cat $LOG

P0SAMPLE=p0/output.sample
DIFF=`diff $P0SAMPLE $OUTPUT`
if [ -n "$DIFF" ]; then
  echo "# not matches sample output"
else
  echo "# matches sample output"
fi
