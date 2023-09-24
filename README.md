# FNAL_JANA2

## Repository for Fermilab May 2023 TRD Test Beam

Workflow on JLab Gluon compute nodes:   
Log in to Gluon nodes with JLab computing account (must have 2FA)
```
ssh -XY [$USERNAME]@scilogin.jlab.org  
Password: [Pin+OTP]  
ssh hallgw  
Password: [Pin+OTP]  
ssh gluon[100-150]  
Password: [JLab CUE]  
```
After cloning repo, make directory links   
```
./make_gluon_links.sh  
```
And use executable files for run analysis   
```
./trdclass.sh [$RUNNUMBER]
./trd_mlp_fermi.sh [$RUNNUMBER]  
```
With this workflow, raw .evio data files have already been processed using `./run.sh [$RUNNUMBER]` and the readable .root files that result now live in the `ROOT/` directory. These root files are processed with the `trdclass.C` analysis macro. Output from there in the form of a .root file is passed on to `trd_mlp_fermi.C` as input where a rejection factor calculation is done for different particle efficiencies.  

See JLab's [2FA documentation](https://jlab.servicenowservices.com/sp?id=kb_article_view&sysparm_article=KB0012313&sys_kb_id=a8caee091b990910a552ed3ce54bcbe3&spa=1.)  
See JLab analysis code repository: [trd_root](https://github.com/JeffersonLab/trd_root/tree/main)  
See JLab JANA repository: [JANA4ML4FPGA](https://github.com/JeffersonLab/JANA4ML4FPGA/tree/main)  


###################################################  
## Historical -- defunct  
> :warning: The following workflow usage is outdated for this project & should no longer be used.


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
