#define UpsFit_cxx
#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/secCut/UpsFit.h"
#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/includes/ParticleCand.C"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <memory>
#include <cstdio>

// Include the header file for the roofit.
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooPlot.h"
#include "RooGaussian.h"
#include "RooArgList.h"
#include "RooFitResult.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"

#define CUT_DR
#define CUT_MASS
// #define SHOW_DEBUG
// #define Stefanos_CUT
#define Ups_CUT

void UpsFit::Loop()
{
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();\
    
    printf("Entries: %lld\n", nentries);

    const unsigned int nBins = 20;
    const unsigned int nBin_cut = 20;
    const unsigned int nCandsAllowed = 20;

    // Use Roofit to draw the plot with proper error bars.
    
    RooRealVar m_Ups("m_Ups", "Ups invariant mass", 9.460, 8.7, 11.2);



    RooArgSet normSet_Ups(m_Ups);


    RooDataSet data("data", "Dataset with m_Jpsi, m_Ups, m_Phi", RooArgSet(m_Ups));


    // Ups 的信号模型（水晶球函数）考虑Ups的1S，2S，3S
    // 定义1S峰为参考峰
    RooRealVar mean_Ups_1S("mean_Ups_1S", "Mean of Ups_1S", 9.460, 9.40, 9.50);
    RooRealVar sigma_Ups_1S("sigma_Ups_1S", "Sigma of Ups_1S", 0.02, 0.005, 0.05);

    // 定义峰间距固定常量 - 使用PDG标准值
    RooConstVar mean_diff_2S_1S("mean_diff_2S_1S", "Mass difference 2S-1S", 10.023 - 9.460);
    RooConstVar mean_diff_3S_1S("mean_diff_3S_1S", "Mass difference 3S-1S", 10.355 - 9.460);

    // 计算2S和3S的中心值 - 基于1S的浮动值加上固定差距
    RooFormulaVar mean_Ups_2S("mean_Ups_2S", "mean_Ups_1S + mean_diff_2S_1S", 
                            RooArgList(mean_Ups_1S, mean_diff_2S_1S));
    RooFormulaVar mean_Ups_3S("mean_Ups_3S", "mean_Ups_1S + mean_diff_3S_1S", 
                            RooArgList(mean_Ups_1S, mean_diff_3S_1S));

    // 计算2S和3S的宽度 - 基于1S的宽度按中心值比例缩放
    RooFormulaVar sigma_Ups_2S("sigma_Ups_2S", "sigma_Ups_1S * mean_Ups_2S / mean_Ups_1S", 
                            RooArgList(sigma_Ups_1S, mean_Ups_2S, mean_Ups_1S));
    RooFormulaVar sigma_Ups_3S("sigma_Ups_3S", "sigma_Ups_1S * mean_Ups_3S / mean_Ups_1S", 
                            RooArgList(sigma_Ups_1S, mean_Ups_3S, mean_Ups_1S));

    // CrystalBall共享相同的形状参数
    RooRealVar alpha_Ups("alpha_Ups", "alpha of Ups", 1.5, 0.5, 5.0);
    RooRealVar n_Ups("n_Ups", "n of Ups", 2.0, 1.0, 10.0);

    // 创建三个峰的CrystalBall PDF
    RooCBShape cb_Ups_1S("cb_Ups_1S", "CB function for Ups_1S", 
                        m_Ups, mean_Ups_1S, sigma_Ups_1S, alpha_Ups, n_Ups);
    RooCBShape cb_Ups_2S("cb_Ups_2S", "CB function for Ups_2S", 
                        m_Ups, mean_Ups_2S, sigma_Ups_2S, alpha_Ups, n_Ups);
    RooCBShape cb_Ups_3S("cb_Ups_3S", "CB function for Ups_3S", 
                        m_Ups, mean_Ups_3S, sigma_Ups_3S, alpha_Ups, n_Ups);

    // 设定三个峰的相对产额比例
    RooRealVar frac_1S("frac_1S", "Fraction of Ups_1S", 0.7, 0.0, 1.0);
    RooRealVar frac_2S("frac_2S", "Fraction of Ups_2S", 0.2, 0.0, 1.0);
    RooRealVar frac_3S("frac_3S", "Fraction of Ups_3S", 0.1, 0.0, 1.0);

    // 将三个峰组合为总的信号PDF
    RooAddPdf signal_Ups("signal_Ups", "Signal PDF for Ups", 
                        RooArgList(cb_Ups_1S, cb_Ups_2S, cb_Ups_3S), 
                        RooArgList(frac_1S, frac_2S, frac_3S));

    // 指定归一化集合
    signal_Ups.fixCoefNormalization(normSet_Ups);


    // Ups 的背景模型（多项式）
    RooRealVar c0_Ups("c0", "c0", 1.0, -1000.0, 10000.0);
    RooRealVar c1_Ups("c1", "c1", 0.1, -1000.0, 10000.0);
    RooRealVar c2_Ups("c2", "c2", 0.01, -1000.0, 10000.0);
    // RooRealVar c3_Ups("c3", "c3", 0.001, -1000.0, 1000.0);
    // RooRealVar c4_Ups("c4", "c4", 0.0001, -10000.0, 10000.0);
    // RooPolynomial bkg_Ups("bkg_Ups", "Background PDF for Ups", m_Ups, RooArgList(c0_Ups, c1_Ups, c2_Ups, c3_Ups, c4_Ups));
    RooPolynomial bkg_Ups("bkg_Ups", "Background PDF for Ups", m_Ups, RooArgList(c0_Ups, c1_Ups, c2_Ups));

   
    // 组合信号和背景PDF得到总的拟合模型
    RooRealVar frac_bkg_Ups("frac_bkg_Ups", "Fraction of background", 0.5, 0.0, 1.0);
    RooRealVar frac_sig_Ups("frac_sig_Ups", "Fraction of signal", 0.5, 0.0, 1.0);
    RooAddPdf total_Ups("total_Ups", "Total PDF for Ups", 
                        RooArgList(signal_Ups, bkg_Ups), 
                        RooArgList(frac_sig_Ups, frac_bkg_Ups));


    double Jpsi_Ups_DR_max = 10.0;
    double Jpsi_Phi_DR_max    = 10.0;
    double Ups_Phi_DR_max    = 10.0;

    double Jpsi_Ups_DR_min = 0.0;
    double Jpsi_Phi_DR_min = 0.0;
    double Ups_Phi_DR_min  = 0.0;

    double Ups_pT_min = 0.0;
    double Ups_mu_pT_min = 0.0;

    bool Ups_mu_require_medium = false;
    bool Ups_mu_require_loose  = false;
    bool Ups_mu_require_tight  = false;

    bool require_Jpsi_trig = false;
    bool require_Ups_trig  = true;


    // --- End of cut parameters registration ---

    Long64_t nbytes = 0, nb = 0;
    for(Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;


        // Marker set to show the progress.
        if(jentry % 500 == 0){
            printf(">>> Processing entry %lld <<<\n", jentry);
        }

        #ifdef SHOW_DEBUG
        printf("Processing entry %ld\n", jentry);
        #endif

        #ifdef TRY_E4

        if(jentry == 10000){
            break;
        }

        #endif

        // Loop over all candidates.

        std::vector<std::shared_ptr<ParticleCand> > CandList;
        ParticleCand tempCand;
        ParticleCand::PartIdxList_t tempList;
        double temp_massChi2;

        if(require_Jpsi_trig && !isJpsiTrigEvt){
            continue;
        }
        if(require_Ups_trig && !isUpsTrigEvt){
            continue;
        }


        for (unsigned int iCand = 0; iCand < Jpsi_mass->size(); iCand++){
            bool passCut = true;

            #ifdef SHOW_DEBUG
            printf("Processing candidate %d\n", iCand);
            #endif

            // Calculate the DRs
            double Jpsi_Ups_DR = sqrt((Jpsi_eta->at(iCand) - Ups_eta->at(iCand)) * (Jpsi_eta->at(iCand) - Ups_eta->at(iCand))
                                    + (Jpsi_phi->at(iCand) - Ups_phi->at(iCand)) * (Jpsi_phi->at(iCand) - Ups_phi->at(iCand)));
            double Jpsi_Phi_DR = sqrt((Jpsi_eta->at(iCand) - Phi_eta->at(iCand)) * (Jpsi_eta->at(iCand) - Phi_eta->at(iCand)) 
                                    + (Jpsi_phi->at(iCand) - Phi_phi->at(iCand)) * (Jpsi_phi->at(iCand) - Phi_phi->at(iCand)));
            double Ups_Phi_DR  = sqrt(( Ups_eta->at(iCand) - Phi_eta->at(iCand)) * ( Ups_eta->at(iCand) - Phi_eta->at(iCand))
                                     +( Ups_phi->at(iCand) - Phi_phi->at(iCand)) * ( Ups_phi->at(iCand) - Phi_phi->at(iCand)));

            // Apply DR cuts

            #ifdef CUT_DR

            if (Jpsi_Ups_DR < Jpsi_Ups_DR_min || Jpsi_Ups_DR > Jpsi_Ups_DR_max){
                passCut = false;
            }

            if (Jpsi_Phi_DR < Jpsi_Phi_DR_min || Jpsi_Phi_DR > Jpsi_Phi_DR_max){
                passCut = false;
            }

            if (Ups_Phi_DR < Ups_Phi_DR_min || Ups_Phi_DR > Ups_Phi_DR_max){
                passCut = false;
            }

            #endif

            // Apply Upsilon cuts
            #ifdef Ups_CUT
            if (Ups_pt->at(iCand) < Ups_pT_min){
                passCut = false;
            }

            // Apply muon cuts
            if (Ups_mu_1_pt->at(iCand) < Ups_mu_pT_min || Ups_mu_2_pt->at(iCand) < Ups_mu_pT_min){
                passCut = false;
            }

            if (Ups_mu_require_medium && (!Ups_mu_1_isPatMediumMuon->at(iCand) || !Ups_mu_2_isPatMediumMuon->at(iCand))){
                passCut = false;
            }

            if (Ups_mu_require_loose && (!Ups_mu_1_isPatLooseMuon->at(iCand) || !Ups_mu_2_isPatLooseMuon->at(iCand))){
                passCut = false;
            }

            if (Ups_mu_require_tight && (!Ups_mu_1_isPatTightMuon->at(iCand) || !Ups_mu_2_isPatTightMuon->at(iCand))){
                passCut = false;
            }
            #endif

            #ifdef CUT_MASS
            // if (Jpsi_mass->at(iCand) < 3.0 || Jpsi_mass->at(iCand) > 3.2){
            //     passCut = false;
            // }
            if (Ups_mass->at(iCand) < 8.7 || Ups_mass->at(iCand) > 11.2){
                passCut = false;
            }
            #endif

            #ifdef Stefanos_CUT
            if(Jpsi_pt->at(iCand) < 6.0){
               passCut = false;
            }

            if(Jpsi_VtxProb->at(iCand) < 0.1){
               passCut = false;
            }

            double Jpsi_E = sqrt(Jpsi_mass->at(iCand) * Jpsi_mass->at(iCand) + Jpsi_px->at(iCand) * Jpsi_px->at(iCand) + Jpsi_py->at(iCand) * Jpsi_py->at(iCand) + Jpsi_pz->at(iCand) * Jpsi_pz->at(iCand));
            double Jpsi_gamma = 0.5 * log((Jpsi_E + Jpsi_pz->at(iCand)) / (Jpsi_E - Jpsi_pz->at(iCand)));
            if(fabs(Jpsi_gamma) > 2.4){
               passCut = false;
            }
            
            //JpsiUps四muon顶点拟合>0.01在ntuple里做过了

            
            // Jpsi_1_Lxy->push_back(Jpsi_1_ctau->at(iCand) * Jpsi_1_pt->at(iCand) / Jpsi_1_mass->at(iCand));
            // Jpsi_2_Lxy->push_back(Jpsi_2_ctau->at(iCand) * Jpsi_2_pt->at(iCand) / Jpsi_2_mass->at(iCand));
            // Phi_Lxy->push_back(Phi_ctau->at(iCand) * Phi_pt->at(iCand) / Phi_mass->at(iCand));
            // if(Jpsi_1_Lxy->at(iCand) > 0.1 || Jpsi_2_Lxy->at(iCand) > 0.1 || Phi_Lxy->at(iCand) > 0.1){
            //    passCut = false;
            // }

            // double Jpsi_1_Lxy = Jpsi_1_ctau->at(iCand) * Jpsi_1_pt->at(iCand) / Jpsi_1_mass->at(iCand);
            // double Jpsi_2_Lxy = Jpsi_2_ctau->at(iCand) * Jpsi_2_pt->at(iCand) / Jpsi_2_mass->at(iCand);
            // double Phi_Lxy = Phi_ctau->at(iCand) * Phi_pt->at(iCand) / Phi_mass->at(iCand);
            // if(Jpsi_1_Lxy > 0.01 || Jpsi_2_Lxy > 0.01 || Phi_Lxy > 0.01){
            //    passCut = false;
            // }
            #endif

            if (!passCut){
                continue;
            }

            // Fill the temporary candidate.
            tempList.clear();
            tempCand.Clear();
            tempCand.SetId(iCand);

            tempList.push_back(Jpsi_mu_1_Idx->at(iCand));
            tempList.push_back(Jpsi_mu_2_Idx->at(iCand));
            tempList.push_back(Ups_mu_1_Idx->at(iCand));
            tempList.push_back(Ups_mu_2_Idx->at(iCand));
            tempCand.AddParticle(ParticleCand::PartType::Muon, tempList);
            tempList.clear();
            tempList.push_back(Phi_K_1_Idx->at(iCand));
            tempList.push_back(Phi_K_2_Idx->at(iCand));
            tempCand.AddParticle(ParticleCand::PartType::Track, tempList);
            tempList.clear();

            temp_massChi2 = (Jpsi_massDiff->at(iCand) / Jpsi_massErr->at(iCand)) * (Jpsi_massDiff->at(iCand) / Jpsi_massErr->at(iCand)) +
                           (Ups_massDiff->at(iCand) /  Ups_massErr->at(iCand)) *  (Ups_massDiff->at(iCand) /  Ups_massErr->at(iCand)) +
                           (Phi_massDiff->at(iCand) /  Phi_massErr->at(iCand)) *  (Phi_massDiff->at(iCand) /  Phi_massErr->at(iCand));
            
            double temp_pt_abs = sqrt(Jpsi_pt->at(iCand) * Jpsi_pt->at(iCand) + Ups_pt->at(iCand) * Ups_pt->at(iCand) + Phi_pt->at(iCand) * Phi_pt->at(iCand));

            double temp_VtxProb = Jpsi_VtxProb->at(iCand) * Ups_VtxProb->at(iCand) * Phi_VtxProb->at(iCand);

            tempCand.SetScore(temp_pt_abs);
            CandList.push_back(std::make_shared<ParticleCand>(tempCand));
        }
        if(CandList.size() == 0){
            continue;
        }
        #ifdef SHOW_DEBUG
        printf("Number of candidates: %d\n", CandList.size());
        #endif
        // Save all filtered candidates to the tree.
        for (auto cand: CandList){
            #ifdef SHOW_DEBUG
            printf("Adding %d\n", cand->GetId());
            #endif
            filtered_Jpsi_mass->push_back(Jpsi_mass->at(cand->GetId()));
            filtered_Jpsi_massErr->push_back(Jpsi_massErr->at(cand->GetId()));
            filtered_Jpsi_massDiff->push_back(Jpsi_massDiff->at(cand->GetId()));
            filtered_Jpsi_ctau->push_back(Jpsi_ctau->at(cand->GetId()));
            filtered_Jpsi_ctauErr->push_back(Jpsi_ctauErr->at(cand->GetId()));
            filtered_Jpsi_Chi2->push_back(Jpsi_Chi2->at(cand->GetId()));
            filtered_Jpsi_ndof->push_back(Jpsi_ndof->at(cand->GetId()));
            filtered_Jpsi_VtxProb->push_back(Jpsi_VtxProb->at(cand->GetId()));
            filtered_Jpsi_px->push_back(Jpsi_px->at(cand->GetId()));
            filtered_Jpsi_py->push_back(Jpsi_py->at(cand->GetId()));
            filtered_Jpsi_pz->push_back(Jpsi_pz->at(cand->GetId()));
            filtered_Jpsi_phi->push_back(Jpsi_phi->at(cand->GetId()));
            filtered_Jpsi_eta->push_back(Jpsi_eta->at(cand->GetId()));
            filtered_Jpsi_pt->push_back(Jpsi_pt->at(cand->GetId()));
            filtered_Jpsi_mu_1_Idx->push_back(Jpsi_mu_1_Idx->at(cand->GetId()));
            filtered_Jpsi_mu_2_Idx->push_back(Jpsi_mu_2_Idx->at(cand->GetId()));

            filtered_Phi_mass->push_back(Phi_mass->at(cand->GetId()));
            filtered_Phi_massErr->push_back(Phi_massErr->at(cand->GetId()));
            filtered_Phi_massDiff->push_back(Phi_massDiff->at(cand->GetId()));
            filtered_Phi_ctau->push_back(Phi_ctau->at(cand->GetId()));
            filtered_Phi_ctauErr->push_back(Phi_ctauErr->at(cand->GetId()));
            filtered_Phi_Chi2->push_back(Phi_Chi2->at(cand->GetId()));
            filtered_Phi_ndof->push_back(Phi_ndof->at(cand->GetId()));
            filtered_Phi_VtxProb->push_back(Phi_VtxProb->at(cand->GetId()));
            filtered_Phi_px->push_back(Phi_px->at(cand->GetId()));
            filtered_Phi_py->push_back(Phi_py->at(cand->GetId()));
            filtered_Phi_pz->push_back(Phi_pz->at(cand->GetId()));
            filtered_Phi_phi->push_back(Phi_phi->at(cand->GetId()));
            filtered_Phi_eta->push_back(Phi_eta->at(cand->GetId()));
            filtered_Phi_pt->push_back(Phi_pt->at(cand->GetId()));
            filtered_Phi_K_1_Idx->push_back(Phi_K_1_Idx->at(cand->GetId()));
            filtered_Phi_K_2_Idx->push_back(Phi_K_2_Idx->at(cand->GetId()));

            filtered_Pri_mass->push_back(Pri_mass->at(cand->GetId()));
            filtered_Pri_massErr->push_back(Pri_massErr->at(cand->GetId()));
            filtered_Pri_ctau->push_back(Pri_ctau->at(cand->GetId()));
            filtered_Pri_ctauErr->push_back(Pri_ctauErr->at(cand->GetId()));
            filtered_Pri_Chi2->push_back(Pri_Chi2->at(cand->GetId()));
            filtered_Pri_ndof->push_back(Pri_ndof->at(cand->GetId()));
            filtered_Pri_VtxProb->push_back(Pri_VtxProb->at(cand->GetId()));
            filtered_Pri_px->push_back(Pri_px->at(cand->GetId()));
            filtered_Pri_py->push_back(Pri_py->at(cand->GetId()));
            filtered_Pri_pz->push_back(Pri_pz->at(cand->GetId()));
            filtered_Pri_phi->push_back(Pri_phi->at(cand->GetId()));
            filtered_Pri_eta->push_back(Pri_eta->at(cand->GetId()));
            filtered_Pri_pt->push_back(Pri_pt->at(cand->GetId()));

            filtered_Ups_mass->push_back(Ups_mass->at(cand->GetId()));
            filtered_Ups_massErr->push_back(Ups_massErr->at(cand->GetId()));
            filtered_Ups_massDiff->push_back(Ups_massDiff->at(cand->GetId()));
            filtered_Ups_Chi2->push_back(Ups_Chi2->at(cand->GetId()));
            filtered_Ups_ndof->push_back(Ups_ndof->at(cand->GetId()));
            filtered_Ups_VtxProb->push_back(Ups_VtxProb->at(cand->GetId()));
            filtered_Ups_px->push_back(Ups_px->at(cand->GetId()));
            filtered_Ups_py->push_back(Ups_py->at(cand->GetId()));
            filtered_Ups_pz->push_back(Ups_pz->at(cand->GetId()));
            filtered_Ups_phi->push_back(Ups_phi->at(cand->GetId()));
            filtered_Ups_eta->push_back(Ups_eta->at(cand->GetId()));
            filtered_Ups_pt->push_back(Ups_pt->at(cand->GetId()));
            filtered_Ups_mu_1_Idx->push_back(Ups_mu_1_Idx->at(cand->GetId()));
            filtered_Ups_mu_2_Idx->push_back(Ups_mu_2_Idx->at(cand->GetId()));

            // For the muons: copy the corresponding muon information.
            #ifdef SHOW_DEBUG
            printf("Adding muon information\n");
            #endif
            filtered_Jpsi_mu_1_px->push_back(Jpsi_mu_1_px->at(cand->GetId()));
            filtered_Jpsi_mu_1_py->push_back(Jpsi_mu_1_py->at(cand->GetId()));
            filtered_Jpsi_mu_1_pz->push_back(Jpsi_mu_1_pz->at(cand->GetId()));
            filtered_Jpsi_mu_1_pt->push_back(Jpsi_mu_1_pt->at(cand->GetId()));
            filtered_Jpsi_mu_1_eta->push_back(Jpsi_mu_1_eta->at(cand->GetId()));
            filtered_Jpsi_mu_1_phi->push_back(Jpsi_mu_1_phi->at(cand->GetId()));
            filtered_Jpsi_mu_1_isPatSoftMuon->push_back(Jpsi_mu_1_isPatSoftMuon->at(cand->GetId()));
            filtered_Jpsi_mu_1_isPatLooseMuon->push_back(Jpsi_mu_1_isPatLooseMuon->at(cand->GetId()));
            filtered_Jpsi_mu_1_isPatMediumMuon->push_back(Jpsi_mu_1_isPatMediumMuon->at(cand->GetId()));
            filtered_Jpsi_mu_1_isPatTightMuon->push_back(Jpsi_mu_1_isPatTightMuon->at(cand->GetId()));
            filtered_Jpsi_mu_1_isJpsiFilterMatch->push_back(Jpsi_mu_1_isJpsiFilterMatch->at(cand->GetId()));
            filtered_Jpsi_mu_1_isUpsFilterMatch->push_back(Jpsi_mu_1_isUpsFilterMatch->at(cand->GetId()));

            filtered_Jpsi_mu_2_px->push_back(Jpsi_mu_2_px->at(cand->GetId()));
            filtered_Jpsi_mu_2_py->push_back(Jpsi_mu_2_py->at(cand->GetId()));
            filtered_Jpsi_mu_2_pz->push_back(Jpsi_mu_2_pz->at(cand->GetId()));
            filtered_Jpsi_mu_2_pt->push_back(Jpsi_mu_2_pt->at(cand->GetId()));
            filtered_Jpsi_mu_2_eta->push_back(Jpsi_mu_2_eta->at(cand->GetId()));
            filtered_Jpsi_mu_2_phi->push_back(Jpsi_mu_2_phi->at(cand->GetId()));
            filtered_Jpsi_mu_2_isPatSoftMuon->push_back(Jpsi_mu_2_isPatSoftMuon->at(cand->GetId()));
            filtered_Jpsi_mu_2_isPatLooseMuon->push_back(Jpsi_mu_2_isPatLooseMuon->at(cand->GetId()));
            filtered_Jpsi_mu_2_isPatMediumMuon->push_back(Jpsi_mu_2_isPatMediumMuon->at(cand->GetId()));
            filtered_Jpsi_mu_2_isPatTightMuon->push_back(Jpsi_mu_2_isPatTightMuon->at(cand->GetId()));
            filtered_Jpsi_mu_2_isJpsiFilterMatch->push_back(Jpsi_mu_2_isJpsiFilterMatch->at(cand->GetId()));
            filtered_Jpsi_mu_2_isUpsFilterMatch->push_back(Jpsi_mu_2_isUpsFilterMatch->at(cand->GetId()));

            filtered_Ups_mu_1_px->push_back(Ups_mu_1_px->at(cand->GetId()));
            filtered_Ups_mu_1_py->push_back(Ups_mu_1_py->at(cand->GetId()));
            filtered_Ups_mu_1_pz->push_back(Ups_mu_1_pz->at(cand->GetId()));
            filtered_Ups_mu_1_pt->push_back(Ups_mu_1_pt->at(cand->GetId()));
            filtered_Ups_mu_1_eta->push_back(Ups_mu_1_eta->at(cand->GetId()));
            filtered_Ups_mu_1_phi->push_back(Ups_mu_1_phi->at(cand->GetId()));
            filtered_Ups_mu_1_isPatSoftMuon->push_back(Ups_mu_1_isPatSoftMuon->at(cand->GetId()));
            filtered_Ups_mu_1_isPatLooseMuon->push_back(Ups_mu_1_isPatLooseMuon->at(cand->GetId()));
            filtered_Ups_mu_1_isPatMediumMuon->push_back(Ups_mu_1_isPatMediumMuon->at(cand->GetId()));
            filtered_Ups_mu_1_isPatTightMuon->push_back(Ups_mu_1_isPatTightMuon->at(cand->GetId()));
            filtered_Ups_mu_1_isUpsFilterMatch->push_back(Ups_mu_1_isUpsFilterMatch->at(cand->GetId()));
            filtered_Ups_mu_1_isJpsiFilterMatch->push_back(Ups_mu_1_isJpsiFilterMatch->at(cand->GetId()));

            filtered_Ups_mu_2_px->push_back(Ups_mu_2_px->at(cand->GetId()));
            filtered_Ups_mu_2_py->push_back(Ups_mu_2_py->at(cand->GetId()));
            filtered_Ups_mu_2_pz->push_back(Ups_mu_2_pz->at(cand->GetId()));
            filtered_Ups_mu_2_pt->push_back(Ups_mu_2_pt->at(cand->GetId()));
            filtered_Ups_mu_2_eta->push_back(Ups_mu_2_eta->at(cand->GetId()));
            filtered_Ups_mu_2_phi->push_back(Ups_mu_2_phi->at(cand->GetId()));
            filtered_Ups_mu_2_isPatSoftMuon->push_back(Ups_mu_2_isPatSoftMuon->at(cand->GetId()));
            filtered_Ups_mu_2_isPatLooseMuon->push_back(Ups_mu_2_isPatLooseMuon->at(cand->GetId()));
            filtered_Ups_mu_2_isPatMediumMuon->push_back(Ups_mu_2_isPatMediumMuon->at(cand->GetId()));
            filtered_Ups_mu_2_isPatTightMuon->push_back(Ups_mu_2_isPatTightMuon->at(cand->GetId()));
            filtered_Ups_mu_2_isUpsFilterMatch->push_back(Ups_mu_2_isUpsFilterMatch->at(cand->GetId()));
            filtered_Ups_mu_2_isJpsiFilterMatch->push_back(Ups_mu_2_isJpsiFilterMatch->at(cand->GetId()));

            // Kaons from phi
            #ifdef SHOW_DEBUG
            printf("Adding kaon information\n");
            #endif
            filtered_Phi_K_1_px->push_back(Phi_K_1_px->at(cand->GetId()));
            filtered_Phi_K_1_py->push_back(Phi_K_1_py->at(cand->GetId()));
            filtered_Phi_K_1_pt->push_back(Phi_K_1_pt->at(cand->GetId()));
            filtered_Phi_K_1_eta->push_back(Phi_K_1_eta->at(cand->GetId()));
            filtered_Phi_K_1_phi->push_back(Phi_K_1_phi->at(cand->GetId()));

            filtered_Phi_K_2_px->push_back(Phi_K_2_px->at(cand->GetId()));
            filtered_Phi_K_2_py->push_back(Phi_K_2_py->at(cand->GetId()));
            filtered_Phi_K_2_pz->push_back(Phi_K_2_pz->at(cand->GetId()));
            filtered_Phi_K_2_pt->push_back(Phi_K_2_pt->at(cand->GetId()));
            filtered_Phi_K_2_eta->push_back(Phi_K_2_eta->at(cand->GetId()));
            filtered_Phi_K_2_phi->push_back(Phi_K_2_phi->at(cand->GetId()));    

            #ifdef SHOW_DEBUG
            printf("End with kaons information\n");
            #endif
        }
        #ifdef SHOW_DEBUG
        printf("Working to fill the tree\n");
        #endif
        filteredTree->Fill();
        // To plot in parallel: multi candidates allowed and non-overlap candidate only.
        // For all candidates, store them all.
        #ifdef SHOW_DEBUG
        printf("Working to store the candidates\n");
        #endif

        #ifdef SHOW_DEBUG
        printf("Working to resolve the overlap\n");
        #endif
        // For non-overlap candidates, store them.
        std::vector<std::shared_ptr<ParticleCand> > CandList_nonOverlap;
        std::sort(CandList.begin(), CandList.end(),
            [](std::shared_ptr<ParticleCand> cand1, std::shared_ptr<ParticleCand> cand2){
                return cand1->GetScore() > cand2->GetScore();
            }
        ); 
        for (auto& cand: CandList){
            if(CandList_nonOverlap.size() == 0){
                CandList_nonOverlap.push_back(cand);
            }
            else{
                bool isOverlap = false;
                for (auto& cand_nonOverlap: CandList_nonOverlap){
                    if(cand->Overlap(*cand_nonOverlap)){
                        isOverlap = true;
                        break;
                    }
                }
                if(!isOverlap){
                    CandList_nonOverlap.push_back(cand);
                }
            }
        }
        for( auto cand: CandList_nonOverlap){
            
         double ups_mass = Ups_mass->at(cand->GetId()); // 获取 Jpsi_2_mass 的值
         
         m_Ups.setVal(Ups_mass->at(cand->GetId()));

         data.add(RooArgSet(m_Ups));
        }
        ClearBranches();
        CandList.clear();
    }

    

   // 执行三维拟合
   //RooFitResult* fitResult = model.fitTo(data, RooFit::Extended(true), RooFit::Save(), RooFit::PrintLevel(1));
   //RooFitResult* fitResult = extendedModel.fitTo(data, RooFit::Save(), RooFit::PrintLevel(1));
   RooFitResult* fitResult = total_Ups.fitTo(data, RooFit::Extended(true), RooFit::Save(), RooFit::PrintLevel(1));

   // 创建新的画布展示拟合结果
   TCanvas *fitCanvas = new TCanvas("fitCanvas", "Fit Results", 800, 600);

   // 创建分布的帧

   RooPlot* frameMUps = m_Ups.frame(RooFit::Title("Ups Mass Distribution"), RooFit::Bins(40));


   // 绘制数据

   data.plotOn(frameMUps);


   // 在每个变量上投影PDF
   


   total_Ups.plotOn(frameMUps, RooFit::LineColor(kRed));
    total_Ups.plotOn(frameMUps, RooFit::Components("signal_Ups"), RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
    total_Ups.plotOn(frameMUps, RooFit::Components("bkg_Ups"), RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));



   // 添加拟合结果文本框
   
   frameMUps->Draw();
   TPaveText *textUps = new TPaveText(0.65, 0.7, 0.9, 0.9, "NDC");
    textUps->AddText(Form("Mean = %.3f #pm %.3f GeV", mean_Ups_1S.getVal(), mean_Ups_1S.getError()));
    textUps->AddText(Form("Sigma = %.3f #pm %.3f GeV", sigma_Ups_1S.getVal(), sigma_Ups_1S.getError()));
    textUps->AddText(Form("Alpha = %.3f #pm %.3f", alpha_Ups.getVal(), alpha_Ups.getError()));
    textUps->AddText(Form("n = %.3f #pm %.3f", n_Ups.getVal(), n_Ups.getError()));
   textUps->SetFillColor(0);
   textUps->SetBorderSize(1);
   textUps->Draw();

   

   // 保存拟合结果
   fitCanvas->SaveAs("UpsFit_Results.pdf");
   fitCanvas->SaveAs("UpsFit_Results.png");

   // 打印拟合参数
    fitResult->Print("v");

    // Save the output tree.
    TFile *outputFile = new TFile("filtered_data_UpsFit.root", "RECREATE");
    filteredTree->Write();
    outputFile->Close();
}
