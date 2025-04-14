#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/includes/ParticleCand.C"
#include "UpsFit.C"

//#define RUN_JOB

void runUpsFit(){
    TChain *chain = new TChain("outputTree","");
    #ifdef RUN_JOB
    chain->Add("JOB_DATA");
    #else
    chain->Add("/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/preCut/preCut_Run3all_first.root");
    #endif
    UpsFit myUpsFit(chain);
    myUpsFit.Loop();
}
