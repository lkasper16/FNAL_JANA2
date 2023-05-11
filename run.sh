#!/bin/bash

RUN=${1-help}
MAXEVT=${2-0}
MODE=${3-SRS}
SRSBIN=${4-3}


if [[ $RUN == "help" ]] ; then
    echo -e "\n Usage:  $0 <run_num> [max events] [SRS,ADC] \n"
    exit 0
fi


RUNNUM=$(printf '%06d' ${RUN} ) 

FILELIST="`/bin/ls DATA/hd_rawdata_${RUNNUM}_*.evio `"

echo "FILELIST = $FILELIST "
echo "RUN = $RUN "
echo "SRSBIN = $SRSBIN "
if [[ $MAXEVT == 0 ]] ; then
    echo "MAXEVT = ALL "
else
    echo "MAXEVT = $MAXEVT "
fi

#exit 0

if [[ $MODE   == "ADC" ]] ; then

    echo " MODE = ADC "
    sleep 1

    jana4ml4fpga -Pplugins=log,root_output,flat_tree,CDAQfile   -Pnthreads=1 -Pjana:nevents=${MAXEVT} \
	-Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root  $FILELIST

elif [[ $MODE ==  "DUMP" ]] ; then

    echo " MODE = DUMP "
    sleep 1

    jana4ml4fpga -Pplugins=log,root_output,flat_tree,CDAQfile -Pnthreads=1 -Pjana:nevents=${MAXEVT} \
	-PEVIO:output_file=${RUNNUM}.evio -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root $FILELIST

else
    echo " MODE = SRS "
    sleep 1

    jana4ml4fpga -Pplugins=CDAQfile,flat_tree,root_output,gemrecon -Pjana:nevents=${MAXEVT} \
	-Pdaq:srs_window_raw:ntsamples=${SRSBIN} \
	-Pevio:LogLevel=trace -Pgemrecon:LogLevel=trace \
	-Pgemrecon:mapping=db/2023_mapping_fermilab.cfg \
	-Phistsfile=ROOT/Run_${RUNNUM}.root  $FILELIST
fi
