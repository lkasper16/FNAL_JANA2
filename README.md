# FNAL_JANA2

## Repository for Fermilab May 2023 TRD Test Beam

Workflow (assuming JANA2 installation is complete):  
```
source env.csh  
./run.sh [RUNNUM] [EVENT_OPT]  
root -l  
.L trdclass.C  
trdclass t([RUNNUM])  
t.Loop()  
```
The `[EVENT_OPT]` indicates how many events from the specified run you wish to run over.  
The default option runs over all events in the specified run, i.e. all data files.  
To run over only 10k events in run 3100, the command is:  
`./run.sh 3100 10000`  
While to run over all events present, the command is:  
`./run.sh 3100`  

Raw data files (.evio) should be linked in the `DATA/` directory  
ROOT files that are processed from `./run.sh` should appear in the `ROOT/` directory  
Output files (.root, .pdf, etc.) generated from `trdclass.C` should go in the `RunOutput/` directory  
