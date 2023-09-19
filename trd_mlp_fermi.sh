#!/bin/bash


source setup_env.sh
# only root needed

RUNNUM=${1-none}
#MAXEVT=${2-0}

if [[ ${RUNNUM} == "none" ]] ; then
    echo "================================="
    echo " Usage: ./$0 <RunNum> [Max_Events] "
    echo "================================="
    exit 0;
fi

echo "====>  Process RUN=$RUNNUM <=========="

root --web=off -l <<EOC
.x trd_mlp_fermi.C++(${RUNNUM})
EOC
