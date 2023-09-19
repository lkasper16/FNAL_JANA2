#!/bin/bash

#RUNLIST=" 3287 3288 3203 3201 3202 3131 3134 3213 3211 "
RUNLIST=" 3216 3288 3248 3277 3211 3213 "
#RUNLIST=" 3200 3202 3131 3132 3135 3196"

for run in ${RUNLIST} ; do 

echo " start run = $run "
sleep 1

./trdclass.sh $run

done

