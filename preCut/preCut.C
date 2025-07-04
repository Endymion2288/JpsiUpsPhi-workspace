#define preCut_cxx
// #define SHOW_DEBUG
#ifdef SHOW_DEBUG
#define VERBOSE
#endif

//#define DRAW_RAW

//#define CUT_GLOBAL_VTX_PROB
#define CUT_FROM_3J

//#define CUT_MUON_ID_LOOSE

#define CUT_MUON_ID_SOFT

#define CUT_UPS_TRY

#define CUT_PHI_TRY

#define ALLOW_OVERLAP
#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/includes/ParticleCand.C"
#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/preCut/preCut.h"
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

void preCut::Loop()
{
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();

    printf("Entries: %lld\n", nentries);

    const unsigned int nBin = 40;
    const unsigned int nBin_cut = 20;
    const unsigned int nCandsAllowed = 20;

    // Use Roofit to draw the plot with proper error bars.
    // Define mass histograms for Jpsi, Ups and Pri passing the cut. Using Roofit.
    RooRealVar Jpsi_mass_cut_var("Jpsi_mass_cut", "Jpsi_mass_cut", 2.5, 3.5);
    RooRealVar Ups_mass_cut_var("Ups_mass_cut", "Ups_mass_cut", 8.0, 12.0);
    RooRealVar Phi_mass_cut_var("Phi_mass_cut","Phi_mass_cut", 0.99, 1.07);
    RooRealVar Pri_mass_cut_var("Pri_mass_cut","Pri_mass_cut", 0.0, 100.0);

    // Define dataset for Jpsi, Ups and Pri passing the cut. Using Roofit.
    RooDataSet Jpsi_mass_cut_data("Jpsi_mass_cut_data", "Jpsi_mass_cut_data", RooArgList(Jpsi_mass_cut_var));
    RooDataSet Ups_mass_cut_data("Ups_mass_cut_data", "Ups_mass_cut_data", RooArgList(Ups_mass_cut_var));
    RooDataSet Phi_mass_cut_data("Phi_mass_cut_data", "Phi_mass_cut_data", RooArgList(Phi_mass_cut_var));
    RooDataSet Pri_mass_cut_data("Pri_mass_cut_data", "Pri_mass_cut_data", RooArgList(Pri_mass_cut_var));

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;

        // Initialize the total set of candidates.
        std::vector<std::shared_ptr<ParticleCand> > CandList;
        ParticleCand tempCand;
        ParticleCand::PartIdxList_t tempList;
        double temp_massChi2;
        const double vtxProb_Thr = 0.01;

        // Marker
        #ifdef SHOW_DEBUG
        printf("\n>>>>> Begin new event %lld <<<<<\n", jentry);
        #endif

        // Display event number once 500 events.
        if(jentry % 500 == 0){
            printf("Processing event %lld\n", jentry);
        }
        // Loop over all candidates.
        size_t nCands = Jpsi_mass->size();

        #ifdef VERBOSE
        printf("number of candidate = %lld\n", nCands);
        #endif

        if(nCands == 0) continue;

        // Calculate muon pT, eta, phi from px, py and pz.
        std::vector<double> mu_pT;
        std::vector<double> mu_eta;
        std::vector<double> mu_phi;
        
        for(Long64_t iMuon=0; iMuon < nMu; iMuon++){
            // Calculate pT, eta, phi from px, py and pz.
            double pT  = sqrt(muPx->at(iMuon) * muPx->at(iMuon) + muPy->at(iMuon) * muPy->at(iMuon));
            double eta = 0.5 * log((pT + muPz->at(iMuon)) / (pT - muPz->at(iMuon)));
            double phi = atan2(muPy->at(iMuon), muPx->at(iMuon));
            mu_pT.push_back(pT);
            mu_eta.push_back(eta);
            mu_phi.push_back(phi);
        }

        #ifdef SHOW_DEBUG
        std::cout << "finished calculating muon pT, eta, phi" << std::endl;
        #endif

        for (Long64_t iCand=0; iCand < nCands; iCand++) {
            // Exclude failed fitting.
            tempList.clear();
            tempCand.Clear();

            #ifdef SHOW_DEBUG
            std::cout << "Processing candidate " << iCand << std::endl;
            #endif

            if(Pri_mass->at(iCand) <= 0.0) continue;

            #ifdef CUT_GLOBAL_VTX_PROB
            // Cut from vertex probability
            if(Pri_VtxProb->at(iCand) < vtxProb_Thr) continue;
            if(Jpsi_VtxProb->at(iCand) < vtxProb_Thr) continue;
            if(Ups_VtxProb->at(iCand) < vtxProb_Thr) continue;
            if(Phi_VtxProb->at(iCand) < vtxProb_Thr) continue;

            #ifdef SHOW_DEBUG
            std::cout << "passed vtxProb cut" << std::endl;
            #endif
            #endif

            // Further cuts are applied here.
            bool passCut = true;

            // Prevent underflow or overflow of masses.
            if(Jpsi_mass->at(iCand) < 2.9 || Jpsi_mass->at(iCand) > 3.3){
                continue;
            }
            if(Ups_mass->at(iCand)  < 8.00 || Ups_mass->at(iCand)  > 12.0){
                continue;
            }
            if(Phi_mass->at(iCand)  < 0.8 || Phi_mass->at(iCand)  > 1.2){
                continue;
            }

            #ifdef SHOW_DEBUG
            std::cout << "passed mass cut" << std::endl;
            #endif

            // Register the candidate final state muons.
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

            #ifdef SHOW_DEBUG
            std::cout << tempCand.ToString() << std::endl;
            #endif

            #ifdef CUT_MUON_ID_LOOSE
            // Check by muIsPatLooseMuon
            for (unsigned int iMuon=0; iMuon < 4; iMuon++){
                if(!muIsPatLooseMuon->at(tempCand.GetParticleIdx(ParticleCand::PartType::Muon)->at(iMuon))){
                    passCut = false;
                    break;
                }
            }

            #endif

            #ifdef CUT_MUON_ID_SOFT
            // Check by muIsPatSoftMuon
            for (unsigned int iMuon=0; iMuon < 4; iMuon++){
                #ifdef SHOW_DEBUG
                printf("Inspecting muon %d\n", tempCand.GetParticleIdx(ParticleCand::PartType::Muon)->at(iMuon));
                #endif
                if(!muIsPatSoftMuon->at(tempCand.GetParticleIdx(ParticleCand::PartType::Muon)->at(iMuon))){
                    passCut = false;
                    break;
                }
            }
            #endif

            #ifdef SHOW_DEBUG
            std::cout << "passed 3J cut" << std::endl;
            #endif

            #ifdef CUT_FROM_3J
            // Apply cut using conditions from 3J analysis.
            // Muons:
            // - For 1.2 < abs(eta) < 2.5, require pT > 3.5 GeV/c
            // - For 0.0 < abs(eta) < 1.2, require pT > 2.5 GeV/c
            // Jpsi:
            // - Require abs(eta) < 2.5, which has been applied in previous steps.
            for(unsigned int iMuon=0; iMuon < 4; iMuon++){
                unsigned int muonIdx = tempCand.GetParticleIdx(ParticleCand::PartType::Muon)->at(iMuon);
                if(fabs(mu_eta.at(muonIdx)) > 2.5){
                    passCut = false;
                    break;
                }
                if(fabs(mu_eta.at(muonIdx)) > 1.2){
                    if(mu_pT.at(muonIdx) < 3.5){
                        passCut = false;
                        break;
                    }
                }
                else{
                    if(mu_pT.at(muonIdx) < 2.5){
                        passCut = false;
                        break;
                    }
                }
            }
            #endif

            if(!passCut){
                continue;
            }

            #ifdef CUT_UPS_TRY
            // Upsilon:
            // - Require pT > 2 GeV/c as a crude cut.
            if(Ups_pt->at(iCand) < 2.0){
                continue;
            }
            #endif

            #ifdef CUT_PHI_TRY
            // Phi:
            // - Require pT > 2.0 GeV/c as a crude cut.
            // Kaon tracks:
            // - Require pT > 0.5 GeV/c as a crude cut.
            if(Phi_pt->at(iCand) < 2.0){
                continue;
            }
            if(Phi_K_1_pt->at(iCand) < 2 || Phi_K_2_pt->at(iCand) < 2){
                continue;
            }
            if(fabs(Phi_K_1_eta->at(iCand)) > 2.5 || fabs(Phi_K_2_eta->at(iCand)) > 2.5){
                continue;
            }
            #endif


            
            // Saving result to the candidate list.
            temp_massChi2 =   (Jpsi_massDiff->at(iCand) / Jpsi_massErr->at(iCand))
                            * (Jpsi_massDiff->at(iCand) / Jpsi_massErr->at(iCand))
                            + (Ups_massDiff->at(iCand) / Ups_massErr->at(iCand))
                            * (Ups_massDiff->at(iCand) / Ups_massErr->at(iCand))
                            + (Phi_massDiff->at(iCand) / Phi_massErr->at(iCand))
                            * (Phi_massDiff->at(iCand) / Phi_massErr->at(iCand));
            double temp_pt_abs = sqrt(Jpsi_pt->at(iCand) * Jpsi_pt->at(iCand) + Ups_pt->at(iCand) * Ups_pt->at(iCand) + Phi_pt->at(iCand) * Phi_pt->at(iCand));
            tempCand.SetScore(temp_pt_abs);
            tempCand.SetId(iCand);
            CandList.push_back(std::make_shared<ParticleCand>(tempCand));
            tempCand.Clear();
        }
        // Sort all candidates for later use.
        std::sort(CandList.begin(), CandList.end(), [](const std::shared_ptr<ParticleCand>& a, 
                                                       const std::shared_ptr<ParticleCand>& b){
            return a->GetScore() < b->GetScore();
        });
        // Print out the sorted candidates.
        #ifdef VERBOSE
        for(auto& cand : CandList){
            printf("[ %d ] %.4f ",cand->GetId() ,cand->GetScore());
            puts(cand->ToString().c_str());
        }
        #endif

        #ifdef ALLOW_OVERLAP
        std::vector<std::shared_ptr<ParticleCand> > SelectedCands_cut;
        for(auto& cand : CandList){
            if(SelectedCands_cut.size() < nCandsAllowed){
                SelectedCands_cut.push_back(cand);
            }
            else{
                break;
            }
        }

        #else
        std::vector<std::shared_ptr<ParticleCand> > SelectedCands_cut;
        for(auto& cand : CandList){
            if(SelectedCands_cut.empty()){
                SelectedCands_cut.push_back(cand);
            }
            else{
                bool isOverlap = false;
                for(auto& selCand : SelectedCands_cut){
                    if(cand->Overlap(*selCand)){
                        isOverlap = true;
                        break;
                    }
                }
                if(!isOverlap){
                    SelectedCands_cut.push_back(cand);
                }
            }
        }
        #endif

        #ifdef SHOW_DEBUG
        std::cout << "Finished selecting candidates" << std::endl;
        #endif

        // Print out the selected candidates with the highest score.
        // #ifdef VERBOSE
        // printf("\n Selected candidates: %lld\n", SelectedCands_raw.size());
        // for(auto& cand : SelectedCands_raw){
        //     printf("[ %d ] %.4f ",cand->GetId() ,cand->GetScore());
        //     puts(cand->ToString().c_str());
        // }
        // #endif

        // Draw the selected candidates passing the cut.
        for(auto& cand : SelectedCands_cut){
            // Set the mass.
            Jpsi_mass_cut_var.setVal(Jpsi_mass->at(cand->GetId()));
            Ups_mass_cut_var.setVal(Ups_mass->at(cand->GetId()));
            Phi_mass_cut_var.setVal(Phi_mass->at(cand->GetId()));
            Pri_mass_cut_var.setVal(Pri_mass->at(cand->GetId()));
            // Set the mass error.
            Jpsi_mass_cut_var.setError(Jpsi_massErr->at(cand->GetId()));
            Ups_mass_cut_var.setError(Ups_massErr->at(cand->GetId()));
            Phi_mass_cut_var.setError(Phi_massErr->at(cand->GetId()));
            Pri_mass_cut_var.setError(Pri_massErr->at(cand->GetId()));
            // Add the data to the dataset.
            Jpsi_mass_cut_data.add(RooArgSet(Jpsi_mass_cut_var));
            Ups_mass_cut_data.add(RooArgSet(Ups_mass_cut_var));
            Phi_mass_cut_data.add(RooArgSet(Phi_mass_cut_var));
            Pri_mass_cut_data.add(RooArgSet(Pri_mass_cut_var));
        }

        #ifdef SHOW_DEBUG
        std::cout << "Finished adding mass to the dataset" << std::endl;
        #endif

        // Use the pre-defined outputTree to store the filtered data.
        for(auto& cand : SelectedCands_cut){
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

            #ifdef SHOW_DEBUG
            std::cout << "Finished adding Jpsi 1 to the dataset" << std::endl;
            #endif

            filtered_Ups_mass->push_back(Ups_mass->at(cand->GetId()));
            filtered_Ups_massErr->push_back(Ups_massErr->at(cand->GetId()));
            filtered_Ups_massDiff->push_back(Ups_massDiff->at(cand->GetId()));
            filtered_Phi_ctau->push_back(Phi_ctau->at(cand->GetId()));
            filtered_Phi_ctauErr->push_back(Phi_ctauErr->at(cand->GetId()));
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

            #ifdef SHOW_DEBUG
            std::cout << "Finished adding Jpsi 2 to the dataset" << std::endl;
            #endif

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

            #ifdef SHOW_DEBUG
            std::cout << "Finished adding Pri to the dataset" << std::endl;
            #endif

            filtered_Phi_mass->push_back(Phi_mass->at(cand->GetId()));
            filtered_Phi_massErr->push_back(Phi_massErr->at(cand->GetId()));
            filtered_Phi_massDiff->push_back(Phi_massDiff->at(cand->GetId()));
            filtered_Phi_Chi2->push_back(Phi_Chi2->at(cand->GetId()));
            filtered_Phi_ndof->push_back(Phi_ndof->at(cand->GetId()));
            filtered_Phi_VtxProb->push_back(Phi_VtxProb->at(cand->GetId()));
            filtered_Phi_px->push_back(Phi_px->at(cand->GetId()));
            filtered_Phi_py->push_back(Phi_py->at(cand->GetId()));
            filtered_Phi_pz->push_back(Phi_pz->at(cand->GetId()));
            filtered_Phi_phi->push_back(Phi_phi->at(cand->GetId()));
            filtered_Phi_eta->push_back(Phi_eta->at(cand->GetId()));
            filtered_Phi_pt->push_back(Phi_pt->at(cand->GetId()));
            filtered_Phi_ctau->push_back(Phi_ctau->at(cand->GetId()));
            filtered_Phi_ctauErr->push_back(Phi_ctauErr->at(cand->GetId()));
            filtered_Phi_K_1_Idx->push_back(Phi_K_1_Idx->at(cand->GetId()));
            filtered_Phi_K_2_Idx->push_back(Phi_K_2_Idx->at(cand->GetId()));

            #ifdef SHOW_DEBUG
            std::cout << "Finished adding Phi to the dataset" << std::endl;
            #endif

            // For the muons
            filtered_Jpsi_mu_1_px->push_back(muPx->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_py->push_back(muPy->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_pz->push_back(muPz->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_eta->push_back(mu_eta[Jpsi_mu_1_Idx->at(cand->GetId())]);
            filtered_Jpsi_mu_1_phi->push_back(mu_phi[Jpsi_mu_1_Idx->at(cand->GetId())]);
            filtered_Jpsi_mu_1_pt->push_back(mu_pT[Jpsi_mu_1_Idx->at(cand->GetId())]);
            filtered_Jpsi_mu_1_isPatLooseMuon->push_back(muIsPatLooseMuon->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_isPatSoftMuon->push_back(muIsPatSoftMuon->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_isPatMediumMuon->push_back(muIsPatMediumMuon->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_isPatTightMuon->push_back(muIsPatTightMuon->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_isJpsiFilterMatch->push_back(muIsJpsiFilterMatch->at(Jpsi_mu_1_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_1_isUpsFilterMatch->push_back(muIsUpsFilterMatch->at(Jpsi_mu_1_Idx->at(cand->GetId())));

            filtered_Jpsi_mu_2_px->push_back(muPx->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_py->push_back(muPy->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_pz->push_back(muPz->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_eta->push_back(mu_eta[Jpsi_mu_2_Idx->at(cand->GetId())]);
            filtered_Jpsi_mu_2_phi->push_back(mu_phi[Jpsi_mu_2_Idx->at(cand->GetId())]);
            filtered_Jpsi_mu_2_pt->push_back(mu_pT[Jpsi_mu_2_Idx->at(cand->GetId())]);
            filtered_Jpsi_mu_2_isPatLooseMuon->push_back(muIsPatLooseMuon->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_isPatSoftMuon->push_back(muIsPatSoftMuon->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_isPatMediumMuon->push_back(muIsPatMediumMuon->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_isPatTightMuon->push_back(muIsPatTightMuon->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_isJpsiFilterMatch->push_back(muIsJpsiFilterMatch->at(Jpsi_mu_2_Idx->at(cand->GetId())));
            filtered_Jpsi_mu_2_isUpsFilterMatch->push_back(muIsUpsFilterMatch->at(Jpsi_mu_2_Idx->at(cand->GetId())));

            filtered_Ups_mu_1_px->push_back(muPx->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_py->push_back(muPy->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_pz->push_back(muPz->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_eta->push_back(mu_eta[Ups_mu_1_Idx->at(cand->GetId())]);
            filtered_Ups_mu_1_phi->push_back(mu_phi[Ups_mu_1_Idx->at(cand->GetId())]);
            filtered_Ups_mu_1_pt->push_back(mu_pT[Ups_mu_1_Idx->at(cand->GetId())]);
            filtered_Ups_mu_1_isPatLooseMuon->push_back(muIsPatLooseMuon->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_isPatSoftMuon->push_back(muIsPatSoftMuon->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_isPatMediumMuon->push_back(muIsPatMediumMuon->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_isPatTightMuon->push_back(muIsPatTightMuon->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_isUpsFilterMatch->push_back(muIsUpsFilterMatch->at(Ups_mu_1_Idx->at(cand->GetId())));
            filtered_Ups_mu_1_isJpsiFilterMatch->push_back(muIsJpsiFilterMatch->at(Ups_mu_1_Idx->at(cand->GetId())));

            filtered_Ups_mu_2_px->push_back(muPx->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_py->push_back(muPy->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_pz->push_back(muPz->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_eta->push_back(mu_eta[Ups_mu_2_Idx->at(cand->GetId())]);
            filtered_Ups_mu_2_phi->push_back(mu_phi[Ups_mu_2_Idx->at(cand->GetId())]);
            filtered_Ups_mu_2_pt->push_back(mu_pT[Ups_mu_2_Idx->at(cand->GetId())]);
            filtered_Ups_mu_2_isPatLooseMuon->push_back(muIsPatLooseMuon->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_isPatSoftMuon->push_back(muIsPatSoftMuon->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_isPatMediumMuon->push_back(muIsPatMediumMuon->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_isPatTightMuon->push_back(muIsPatTightMuon->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_isUpsFilterMatch->push_back(muIsUpsFilterMatch->at(Ups_mu_2_Idx->at(cand->GetId())));
            filtered_Ups_mu_2_isJpsiFilterMatch->push_back(muIsJpsiFilterMatch->at(Ups_mu_2_Idx->at(cand->GetId())));

            #ifdef SHOW_DEBUG
            std::cout << "Finished adding Jpsi muons to the dataset" << std::endl;
            #endif

            filtered_Phi_K_1_px->push_back(Phi_K_1_px->at(cand->GetId()));
            filtered_Phi_K_1_py->push_back(Phi_K_1_py->at(cand->GetId()));
            filtered_Phi_K_1_pz->push_back(Phi_K_1_pz->at(cand->GetId()));
            filtered_Phi_K_1_eta->push_back(Phi_K_1_eta->at(cand->GetId()));
            filtered_Phi_K_1_phi->push_back(Phi_K_1_phi->at(cand->GetId()));
            filtered_Phi_K_1_pt->push_back(Phi_K_1_pt->at(cand->GetId()));

            filtered_Phi_K_2_px->push_back(Phi_K_2_px->at(cand->GetId()));
            filtered_Phi_K_2_py->push_back(Phi_K_2_py->at(cand->GetId()));
            filtered_Phi_K_2_pz->push_back(Phi_K_2_pz->at(cand->GetId()));
            filtered_Phi_K_2_eta->push_back(Phi_K_2_eta->at(cand->GetId()));
            filtered_Phi_K_2_phi->push_back(Phi_K_2_phi->at(cand->GetId()));
            filtered_Phi_K_2_pt->push_back(Phi_K_2_pt->at(cand->GetId()));

            // #ifdef SHOW_DEBUG
            // std::cout << "Finished adding Phi tracks to the dataset" << std::endl;
            // #endif
        }

        #ifdef SHOW_DEBUG
        std::cout << "Finished filling the output tree" << std::endl;
        #endif
        // Fill into the output tree.
        if(filtered_Jpsi_mass->size() > 0){
            filtered_isJpsiTrigEvt = muIsJpsiTrigMatch->at(0);
            filtered_isUpsTrigEvt  = muIsUpsTrigMatch->at(0);
            outputTree->Fill();
        }

        // Clear the vectors.
        filtered_Jpsi_mass->clear();
        filtered_Jpsi_massErr->clear();
        filtered_Jpsi_massDiff->clear();
        filtered_Jpsi_ctau->clear();
        filtered_Jpsi_ctauErr->clear();
        filtered_Jpsi_Chi2->clear();
        filtered_Jpsi_ndof->clear();
        filtered_Jpsi_VtxProb->clear();
        filtered_Jpsi_px->clear();
        filtered_Jpsi_py->clear();
        filtered_Jpsi_pz->clear();
        filtered_Jpsi_phi->clear();
        filtered_Jpsi_eta->clear();
        filtered_Jpsi_pt->clear();
        filtered_Jpsi_mu_1_Idx->clear();
        filtered_Jpsi_mu_2_Idx->clear();

        filtered_Ups_mass->clear();
        filtered_Ups_massErr->clear();
        filtered_Ups_massDiff->clear();
        filtered_Phi_ctau->clear();
        filtered_Phi_ctauErr->clear();
        filtered_Ups_Chi2->clear();
        filtered_Ups_ndof->clear();
        filtered_Ups_VtxProb->clear();
        filtered_Ups_px->clear();
        filtered_Ups_py->clear();
        filtered_Ups_pz->clear();
        filtered_Ups_phi->clear();
        filtered_Ups_eta->clear();
        filtered_Ups_pt->clear();
        filtered_Ups_mu_1_Idx->clear();
        filtered_Ups_mu_2_Idx->clear();

        filtered_Pri_mass->clear();
        filtered_Pri_massErr->clear();
        filtered_Pri_ctau->clear();
        filtered_Pri_ctauErr->clear();
        filtered_Pri_Chi2->clear();
        filtered_Pri_ndof->clear();
        filtered_Pri_VtxProb->clear();
        filtered_Pri_px->clear();
        filtered_Pri_py->clear();
        filtered_Pri_pz->clear();
        filtered_Pri_phi->clear();
        filtered_Pri_eta->clear();
        filtered_Pri_pt->clear();

        filtered_Phi_mass->clear();
        filtered_Phi_massErr->clear();
        filtered_Phi_massDiff->clear();
        filtered_Phi_Chi2->clear();
        filtered_Phi_ndof->clear();
        filtered_Phi_VtxProb->clear();
        filtered_Phi_px->clear();
        filtered_Phi_py->clear();
        filtered_Phi_pz->clear();
        filtered_Phi_phi->clear();
        filtered_Phi_eta->clear();
        filtered_Phi_pt->clear();
        filtered_Phi_K_1_Idx->clear();
        filtered_Phi_K_2_Idx->clear();
        filtered_Phi_ctau->clear();
        filtered_Phi_ctauErr->clear();

        // For the muons
        filtered_Jpsi_mu_1_px->clear();
        filtered_Jpsi_mu_1_py->clear();
        filtered_Jpsi_mu_1_pz->clear();
        filtered_Jpsi_mu_1_eta->clear();
        filtered_Jpsi_mu_1_phi->clear();
        filtered_Jpsi_mu_1_pt->clear();
        filtered_Jpsi_mu_1_isPatLooseMuon->clear();
        filtered_Jpsi_mu_1_isPatSoftMuon->clear();
        filtered_Jpsi_mu_1_isPatMediumMuon->clear();
        filtered_Jpsi_mu_1_isPatTightMuon->clear();
        filtered_Jpsi_mu_1_isJpsiFilterMatch->clear();
        filtered_Jpsi_mu_1_isUpsFilterMatch->clear();

        filtered_Jpsi_mu_2_px->clear();
        filtered_Jpsi_mu_2_py->clear();
        filtered_Jpsi_mu_2_pz->clear();
        filtered_Jpsi_mu_2_eta->clear();
        filtered_Jpsi_mu_2_phi->clear();
        filtered_Jpsi_mu_2_pt->clear();
        filtered_Jpsi_mu_2_isPatLooseMuon->clear();
        filtered_Jpsi_mu_2_isPatSoftMuon->clear();
        filtered_Jpsi_mu_2_isPatMediumMuon->clear();
        filtered_Jpsi_mu_2_isPatTightMuon->clear();
        filtered_Jpsi_mu_2_isJpsiFilterMatch->clear();
        filtered_Jpsi_mu_2_isUpsFilterMatch->clear();

        filtered_Ups_mu_1_px->clear();
        filtered_Ups_mu_1_py->clear();
        filtered_Ups_mu_1_pz->clear();
        filtered_Ups_mu_1_eta->clear();
        filtered_Ups_mu_1_phi->clear();
        filtered_Ups_mu_1_pt->clear();
        filtered_Ups_mu_1_isPatLooseMuon->clear();
        filtered_Ups_mu_1_isPatSoftMuon->clear();
        filtered_Ups_mu_1_isPatMediumMuon->clear();
        filtered_Ups_mu_1_isPatTightMuon->clear();
        filtered_Ups_mu_1_isUpsFilterMatch->clear();
        filtered_Ups_mu_1_isJpsiFilterMatch->clear();

        filtered_Ups_mu_2_px->clear();
        filtered_Ups_mu_2_py->clear();
        filtered_Ups_mu_2_pz->clear();
        filtered_Ups_mu_2_eta->clear();
        filtered_Ups_mu_2_phi->clear();
        filtered_Ups_mu_2_pt->clear();
        filtered_Ups_mu_2_isPatLooseMuon->clear();
        filtered_Ups_mu_2_isPatSoftMuon->clear();
        filtered_Ups_mu_2_isPatMediumMuon->clear();
        filtered_Ups_mu_2_isPatTightMuon->clear();
        filtered_Ups_mu_2_isUpsFilterMatch->clear();
        filtered_Ups_mu_2_isJpsiFilterMatch->clear();

        filtered_Phi_K_1_px->clear();
        filtered_Phi_K_1_py->clear();
        filtered_Phi_K_1_pz->clear();
        filtered_Phi_K_1_eta->clear();
        filtered_Phi_K_1_phi->clear();
        filtered_Phi_K_1_pt->clear();

        filtered_Phi_K_2_px->clear();
        filtered_Phi_K_2_py->clear();
        filtered_Phi_K_2_pz->clear();
        filtered_Phi_K_2_eta->clear();
        filtered_Phi_K_2_phi->clear();
        filtered_Phi_K_2_pt->clear();
        #ifdef SHOW_DEBUG
        puts(">>>>> End of event <<<<<");
        #endif
    }
    #ifdef DRAW_RAW
    // Display the histograms.
    // Mass histograms passing the cut are drawn using Roofit.
    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 1200);
    c2->Divide(2,2);
    RooPlot* frame5 = Jpsi_mass_cut_var.frame(nBin);
    RooPlot* frame6 = Ups_mass_cut_var.frame(nBin);
    RooPlot* frame7 = Phi_mass_cut_var.frame(nBin);
    RooPlot* frame8 = Pri_mass_cut_var.frame(nBin);
    Jpsi_mass_cut_data.plotOn(frame5);
    Ups_mass_cut_data.plotOn(frame6);
    Phi_mass_cut_data.plotOn(frame7);
    Pri_mass_cut_data.plotOn(frame8);
    // Draw the histograms.
    c2->cd(1); frame5->Draw();
    c2->cd(2); frame6->Draw();
    c2->cd(3); frame7->Draw();
    c2->cd(4); frame8->Draw();
    // Save png file.
    c2->SaveAs("mass_cut.png");
    #endif

    // Storing the filtered candidates.
    // Create a new ROOT file to save the filtered data.
    TFile* outputFile = new TFile("filtered_data_preCut.root", "RECREATE");
    // Write the output file.
    outputTree->Write();
    outputFile->Close();
}

