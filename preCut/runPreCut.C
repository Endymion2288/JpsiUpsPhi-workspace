#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/includes/ParticleCand.C"
#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/preCut/preCut.C"

#define RUN_MINOR
// #define RUN_ALL
// #define RUN_JOB

void runPreCut(){
    TChain *chain = new TChain("mkcands/X_data","");
    // chain->Add("/home/storage0/users/chiwang/storage2/CMS-Analysis/JJP/Analysis/MultiCandChecker/TripleMeson-ntuple.root");
    // chain->Add("/home/storage0/users/chiwang/storage2/CMS-Analysis/JJP/TripleMeson-ntuple.root");
    #ifdef RUN_JOB
    chain->Add("JOB_DATA");
    #else

    #ifdef RUN_MINOR
    //chain->Add("/home/storage0/users/chiwang/storage2/CMS-Analysis/JJP/Analysis/MultiCandChecker/JJU-ntuple.root");
    //chain->Add("/home/storage0/users/chiwang/storage2/CMS-Analysis/JpsiUpsPhi/rootNtuple/sample_JUP_2023.root");
    //chain->Add("/afs/cern.ch/user/c/chiw/cernbox/JpsiJpsiUps/rootNtuple/P_Run2024/P0_Run2024Cv1_0000.root");
    chain->Add("/home/storage0/users/chiwang/storage2/CMS-Analysis/JpsiUpsPhi/rootNtuple/P_Run2023_half/*.root");
    #endif

    

    #ifdef RUN_ALL
    // chain->Add("~/storage2/CMS-Analysis/JpsiJpsiUps/rootNtuple/P_Run2022/*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Cv1_MINIAOD/250204_140228/0000/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Cv2_MINIAOD/250204_140238/0000/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Cv3_MINIAOD/250204_140250/0000/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Cv4_MINIAOD/250204_140300/0000/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Cv4_MINIAOD/250204_140300/0001/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Dv1_MINIAOD/250204_140310/0000/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Dv1_MINIAOD/250204_140310/0001/mymultilep_*.root");
    chain->Add("/home/storage0/users/xingcheng/storage2/CMS-Analysis/Data/JpsiJpsiPhi/JpsiJpsiPhi/rootNtuple/ParkingDoubleMuonLowMass0/crab3_0_Run2023Dv2_MINIAOD/250204_140321/0000/mymultilep_*.root");
    // chain->Add("~/storage2/CMS-Analysis/JpsiJpsiUps/rootNtuple/P_Run2024/*.root");
    #endif
    #endif

    preCut *preCutObj = new preCut(chain);
    preCutObj->Loop();
}
