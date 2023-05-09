#!/bin/bash

RUN=${1-help}
MAXEVT=${2-0}
MODE=${3-ADC}
SRSBIN=${4-3}


if [[ $RUN == "help" ]] ; then
    echo -e "\n Usage:  $0 <run_num> [max events] [ADC,SRS] \n"
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

#  jana4ml4fpga -Pplugins=log,root_output,flat_tree,CDAQfile DATA/hd_rawdata_${RUNNUM}_${FILENUM}.evio  -Pnthreads=1 -Pjana:nevents=${MAXEVT} -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root  
  jana4ml4fpga -Pplugins=log,root_output,flat_tree,CDAQfile   -Pnthreads=1 -Pjana:nevents=${MAXEVT} -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root  $FILELIST

elif [[ $MODE ==  "DUMP" ]] ; then

   echo " MODE = DUMP "
   sleep 1

#  jana4ml4fpga -Pplugins=log,root_output,flat_tree,CDAQfile  DATA/hd_rawdata_${RUNNUM}_${FILENUM}.evio -Pnthreads=1 -Pjana:nevents=${MAXEVT} -PEVIO:output_file=${RUNNUM}.evio -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root
jana4ml4fpga -Pplugins=log,root_output,flat_tree,CDAQfile -Pnthreads=1 -Pjana:nevents=${MAXEVT} -PEVIO:output_file=${RUNNUM}.evio -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root $FILELIST

else
   echo " MODE = SRS "
   sleep 1

#   jana4ml4fpga -Pplugins=CDAQfile,root_output,flat_tree,log,gemrecon -Pjana:nevents=${MAXEVT} -Pgemrecon:mapping=db/2023_mapping_GEMTRK.cfg \
#-Pgemrecon:config=db/Config_GEM_TRD_JANA.cfg -Pcdaqfile:srs_nsamples=${SRSBIN} -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root DATA/hd_rawdata_${RUNNUM}_${FILENUM}.evio

   jana4ml4fpga -Pplugins=CDAQfile,root_output,flat_tree,log,gemrecon -Pjana:nevents=${MAXEVT} -Pgemrecon:mapping=db/2023_mapping_GEMTRK.cfg \
-Pgemrecon:config=db/Config_GEM_TRD_JANA.cfg -Pcdaqfile:srs_nsamples=${SRSBIN} -Phistsfile=ROOT/Run_${RUNNUM}_${FILENUM}.root  $FILELIST

fi



#
#jana4ml4fpga
#-Pplugins=CDAQfile,root_output,flat_tree,log,gemrecon
#-Pjana:nevents=100
#-Pgemrecon:mapping=/tmp/JANA4ML4FPGA/src/plugins/gemrecon/db/2019_mapping_GEMTRK.cfg
#-Pgemrecon:config=/tmp/JANA4ML4FPGA/src/plugins/gemrecon/db/Config.cfg
#-Phistsfile=/mnt/data/data_1068.root
#/mnt/data/hd_rawdata_001068_001.evio
#
