#define secCut_cxx
#include "/home/storage2/users/xingcheng/CMSSW_14_0_18/src/JpsiUpsPhi-workspace/secCut/secCut.h"
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
#define Jpsi_CUT
#define Ups_CUT
#define Phi_CUT

void secCut::Loop()
{
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();\
    
    printf("Entries: %lld\n", nentries);

    const unsigned int nBins = 20;
    const unsigned int nBin_cut = 20;
    const unsigned int nCandsAllowed = 20;

    double Jpsi_mass_min = 2.9;
    double Jpsi_mass_max = 3.3;
    double Ups_mass_min  = 8.5;
    double Ups_mass_max  = 11.4;
    double Phi_mass_min  = 0.99;
    double Phi_mass_max  = 1.1;

    double Jpsi_Ups_DR_max = 10.0;
    double Jpsi_Phi_DR_max = 10.0;
    double Ups_Phi_DR_max  = 10.0;

    double Jpsi_Ups_DR_min = 0.0;
    double Jpsi_Phi_DR_min = 0.0;
    double Ups_Phi_DR_min  = 0.0;

    double Jpsi_pT_min = 3.0;
    double Jpsi_mu_pT_min = 3.0;
    double Jpsi_VtxProb_min = 0.05;
    bool require_Jpsi_trig = false;
    bool Jpsi_mu_require_medium = false;
    bool Jpsi_mu_require_loose  = false;
    bool Jpsi_mu_require_tight  = false;

    double Ups_pT_min = 4.0;
    double Ups_mu_pT_min = 3.0;
    double Ups_VtxProb_min = 0.1;
    bool require_Ups_trig  = false;
    bool Ups_mu_require_medium = false;
    bool Ups_mu_require_loose  = false;
    bool Ups_mu_require_tight  = true;

    double Phi_pT_min = 2.0;
    double Phi_K_pT_min = 2.0;
    double Phi_VtxProb_min = 0.05;

    // Use Roofit to draw the plot with proper error bars.
    // Define mass histograms for Jpsi, Phi and Pri passing the cut. Using Roofit.
    RooRealVar Jpsi_mass_var("Jpsi_mass_cut", "Jpsi_mass_cut", Jpsi_mass_min, Jpsi_mass_max);
    RooRealVar Ups_mass_var("Ups_mass_cut", "Ups_mass_cut", Ups_mass_min, Ups_mass_max);
    RooRealVar Phi_mass_var("Phi_mass_cut","Phi_mass_cut", Phi_mass_min, Phi_mass_max);
    RooRealVar Pri_mass_var("Pri_mass_cut","Pri_mass_cut", 0.0, 100.0);

    RooRealVar Jpsi_ctau_var("Jpsi_ctau_cut", "Jpsi_ctau_cut", -0.05, 0.1);
    RooRealVar Phi_ctau_var("Phi_ctau_cut","Phi_ctau_cut", -0.1, 0.1);
    RooRealVar Pri_ctau_var("Pri_ctau_cut","Pri_ctau_cut", -0.1, 0.1);

    // Define dataset for Jpsi, Phi and Pri passing the cut. Using Roofit.
    RooDataSet Jpsi_mass_set_multi("Jpsi_mass_set_multi", "Jpsi_mass_set_multi", RooArgList(Jpsi_mass_var));
    RooDataSet Ups_mass_set_multi("Ups_mass_set_multi", "Ups_mass_set_multi", RooArgList(Ups_mass_var));
    RooDataSet Phi_mass_set_multi("Phi_mass_set_multi", "Phi_mass_set_multi", RooArgList(Phi_mass_var));
    RooDataSet Pri_mass_set_multi("Pri_mass_set_multi", "Pri_mass_set_multi", RooArgList(Pri_mass_var));

    // Define dataset for Jpsi, Phi and Pri passing the cut. Using Roofit. Removed the multiple candidates.
    RooDataSet Jpsi_mass_set("Jpsi_mass_set", "Jpsi_mass_set", RooArgList(Jpsi_mass_var));
    RooDataSet Ups_mass_set("Ups_mass_set", "Ups_mass_set", RooArgList(Ups_mass_var));
    RooDataSet Phi_mass_set("Phi_mass_set", "Phi_mass_set", RooArgList(Phi_mass_var));
    RooDataSet Pri_mass_set("Pri_mass_set", "Pri_mass_set", RooArgList(Pri_mass_var));

    RooDataSet Jpsi_ctau_set("Jpsi_ctau_set", "Jpsi_ctau_set", RooArgList(Jpsi_ctau_var));
    RooDataSet Phi_ctau_set("Phi_ctau_set", "Phi_ctau_set", RooArgList(Phi_ctau_var));
    RooDataSet Pri_ctau_set("Pri_ctau_set", "Pri_ctau_set", RooArgList(Pri_ctau_var));
   gStyle->SetOptStat(0);
   gStyle->SetCanvasColor(33);
   gStyle->SetFrameFillColor(18);
   Int_t cancolor = 17;
   auto h2 = new TH2F("h2", "Jpsi_mass and Ups_mass", 20, Jpsi_mass_min, Jpsi_mass_max, 20, Ups_mass_min, Ups_mass_max);
   h2->SetFillColor(46);
   auto h3 = new TH2F("h3", "Jpsi_mass and Phi_mass", 20, Jpsi_mass_min, Jpsi_mass_max, 20, Phi_mass_min, Phi_mass_max);
   h3->SetFillColor(46);
   auto h4 = new TH2F("h4", "Ups_mass and Phi_mass", 20, Ups_mass_min, Ups_mass_max, 20, Phi_mass_min, Phi_mass_max);
   h4->SetFillColor(46);

    RooRealVar m_Jpsi("m_Jpsi", "J/#psi invariant mass", 3.097, Jpsi_mass_min, Jpsi_mass_max);
    RooRealVar m_Ups("m_Ups", "Ups invariant mass", 9.460, Ups_mass_min, Ups_mass_max);
    RooRealVar m_Phi("m_Phi", "#Phi invariant mass", 1.019, Phi_mass_min, Phi_mass_max);

    RooArgSet normSet_Jpsi(m_Jpsi);
    RooArgSet normSet_Ups(m_Ups);
    // RooArgSet normSet_Phi(m_Phi);
    RooArgSet normSet_Total(m_Jpsi, m_Ups, m_Phi);

    RooDataSet data("data", "Dataset with m_Jpsi, m_Ups, m_Phi", RooArgSet(m_Jpsi, m_Ups, m_Phi));

   // J/psi 的信号模型（Crystal Ball + Gaussian）
   RooRealVar mean_Jpsi("mean_Jpsi", "Mean of J/#psi", 3.097, 3.05, 3.15);
   RooRealVar sigma_CB_1("sigma_CB_1", "CB Sigma of J/#psi", 0.02, 0.005, 0.05);
   RooRealVar alpha_1("alpha_1", "alpha of J/#psi", 1.5, 0.5, 5.0);
   RooRealVar n_1("n_1", "n of J/#psi", 2.0, 0, 10.0);
   RooCBShape cb_Jpsi("cb_Jpsi", "CB function for J/#psi", m_Jpsi, mean_Jpsi, sigma_CB_1, alpha_1, n_1);

   RooRealVar sigma_Gauss_1("sigma_Gauss_1", "Gaussian Sigma of J/#psi_{1}", 0.03, 0.01, 0.1);
   RooGaussian gauss_Jpsi("gauss_Jpsi", "Gaussian for J/#psi", m_Jpsi, mean_Jpsi, sigma_Gauss_1);

   RooRealVar frac_1("frac_1", "Fraction of CB in J/#psi", 0.7, 0.0, 1.0);
   RooAddPdf signal_Jpsi("signal_Jpsi", "Signal PDF for J/#psi", RooArgList(cb_Jpsi, gauss_Jpsi), frac_1);
   signal_Jpsi.fixCoefNormalization(normSet_Jpsi);

   // Jpsi 的背景模型（指数函数）
   RooRealVar lambda_1("lambda_1", "Background slope for J/#psi", -0.5, -10.0, 0.0);
   RooExponential bkg_Jpsi("bkg_Jpsi", "Background PDF for J/#psi", m_Jpsi, lambda_1);

   // Ups的信号模型（Crystal Ball + Gaussian）
//    RooRealVar mean_Ups("mean_Ups", "Mean of Ups", 9.460, 9.40, 9.50);
//    RooRealVar sigma_CB_2("sigma_CB_2", "CB Sigma of Ups", 0.02, 0.005, 0.05);
//    RooRealVar alpha_2("alpha_2", "alpha of Ups", 1.5, 0.5, 5.0);
//    RooRealVar n_2("n_2", "n of Ups", 2.0, 1.0, 10.0);
//    RooCBShape cb_Ups("cb_Ups", "CB function for Ups", m_Ups, mean_Ups, sigma_CB_2, alpha_2, n_2);

//    RooRealVar sigma_Gauss_2("sigma_Gauss_2", "Gaussian Sigma of Ups", 0.03, 0.01, 0.1);
//    RooGaussian gauss_Ups("gauss_Ups", "Gaussian for Ups", m_Ups, mean_Ups, sigma_Gauss_2);

//    RooRealVar frac_2("frac_2", "Fraction of CB in Ups", 0.7, 0.0, 1.0);
//    RooAddPdf signal_Ups("signal_Ups", "Signal PDF for Ups", RooArgList(cb_Ups, gauss_Ups), frac_2);
//    signal_Ups.fixCoefNormalization(normSet_Ups);

    // Ups 的信号模型（水晶球函数）考虑Ups的1S，2S，3S
    // 定义1S峰为参考峰
    RooRealVar mean_Ups_1S("mean_Ups_1S", "Mean of Ups_1S", 9.460, 9.40, 9.50);
    // RooRealVar sigma_Ups_1S_1("sigma_Ups_1S_1", "Sigma of Ups_1S_1", 0.02, 0.005, 0.5);
    // RooRealVar sigma_Ups_1S_2("sigma_Ups_1S_2", "Sigma of Ups_1S_2", 0.02, 0.005, 0.5);
    RooConstVar sigma_Ups_1S_1("sigma_Ups_1S_1", "Sigma of Ups_1S_1", 0.081172);
    RooConstVar sigma_Ups_1S_2("sigma_Ups_1S_2", "Sigma of Ups_1S_2", 0.05);

    // 定义峰间距固定常量 - 使用PDG标准值
    RooConstVar mean_diff_2S_1S("mean_diff_2S_1S", "Mass difference 2S-1S", 10.023 - 9.460);
    RooConstVar mean_diff_3S_1S("mean_diff_3S_1S", "Mass difference 3S-1S", 10.355 - 9.460);

    // 计算2S和3S的中心值 - 基于1S的浮动值加上固定差距
    RooFormulaVar mean_Ups_2S("mean_Ups_2S", "mean_Ups_1S + mean_diff_2S_1S", 
                            RooArgList(mean_Ups_1S, mean_diff_2S_1S));
    RooFormulaVar mean_Ups_3S("mean_Ups_3S", "mean_Ups_1S + mean_diff_3S_1S", 
                            RooArgList(mean_Ups_1S, mean_diff_3S_1S));

    // 计算2S和3S的宽度 - 基于1S的宽度按中心值比例缩放
    RooFormulaVar sigma_Ups_2S_1("sigma_Ups_2S_1", "sigma_Ups_1S_1 * mean_Ups_2S / mean_Ups_1S", 
                            RooArgList(sigma_Ups_1S_1, mean_Ups_2S, mean_Ups_1S));
    RooFormulaVar sigma_Ups_2S_2("sigma_Ups_2S_2", "sigma_Ups_1S_2 * mean_Ups_2S / mean_Ups_1S", 
                            RooArgList(sigma_Ups_1S_2, mean_Ups_2S, mean_Ups_1S));
    RooFormulaVar sigma_Ups_3S_1("sigma_Ups_3S_1", "sigma_Ups_1S_1 * mean_Ups_3S / mean_Ups_1S", 
                            RooArgList(sigma_Ups_1S_1, mean_Ups_3S, mean_Ups_1S));
    RooFormulaVar sigma_Ups_3S_2("sigma_Ups_3S_2", "sigma_Ups_1S_2 * mean_Ups_3S / mean_Ups_1S", 
                            RooArgList(sigma_Ups_1S_2, mean_Ups_3S, mean_Ups_1S));

    // // CrystalBall共享相同的形状参数
    // RooRealVar alpha_Ups("alpha_Ups", "alpha of Ups", 1.5, 0.5, 5.0);
    // RooRealVar n_Ups("n_Ups", "n of Ups", 2.0, 1.0, 10.0);
    RooConstVar alpha_1_Ups("alpha_1_Ups", "alpha_1 of Ups", 2.8762);
    RooConstVar n_1_Ups("n_1_Ups", "n_1 of Ups", 0.061663);
    RooConstVar alpha_2_Ups("alpha_2_Ups", "alpha_2 of Ups", 4.8121e-01);
    RooConstVar n_2_Ups("n_2_Ups", "n_2 of Ups", 18.676);

    // // 创建三个峰的CrystalBall PDF
    // RooCBShape cb_Ups_1S("cb_Ups_1S", "CB function for Ups_1S", 
    //                     m_Ups, mean_Ups_1S, sigma_Ups_1S, alpha_Ups, n_Ups);
    // RooCBShape cb_Ups_2S("cb_Ups_2S", "CB function for Ups_2S", 
    //                     m_Ups, mean_Ups_2S, sigma_Ups_2S, alpha_Ups, n_Ups);
    // RooCBShape cb_Ups_3S("cb_Ups_3S", "CB function for Ups_3S", 
    //                     m_Ups, mean_Ups_3S, sigma_Ups_3S, alpha_Ups, n_Ups);
    // 使用双侧CB函数拟合Ups
    // 左侧尾巴的CB函数
    RooCBShape cb_Ups_1S_1("cb_Ups_1S_1", "CB function for Ups_1S (left tail)", 
        m_Ups, mean_Ups_1S, sigma_Ups_1S_1, alpha_1_Ups, n_1_Ups);

    // 右侧尾巴的CB函数 (注意alpha取负值)
    RooCBShape cb_Ups_1S_2("cb_Ups_1S_2", "CB function for Ups_1S (right tail)", 
        m_Ups, mean_Ups_1S, sigma_Ups_1S_2, alpha_2_Ups, n_2_Ups);

    // 加权合并两个CB函数
    RooConstVar frac_cb_Ups("frac_cb_Ups_1S", "Fraction of left CB in Ups_1S", 7.2778e-01);
    RooAddPdf cb_Ups_1S("cb_Ups_1S", "Double-sided CB for Ups_1S", 
                        RooArgList(cb_Ups_1S_1, cb_Ups_1S_2), frac_cb_Ups);
    // 2S和3S的CB函数
    RooCBShape cb_Ups_2S_1("cb_Ups_2S_1", "CB function for Ups_2S", 
        m_Ups, mean_Ups_2S, sigma_Ups_2S_1, alpha_1_Ups, n_1_Ups);
    RooCBShape cb_Ups_2S_2("cb_Ups_2S_2", "CB function for Ups_2S", 
        m_Ups, mean_Ups_2S, sigma_Ups_2S_2, alpha_2_Ups, n_2_Ups);
    RooAddPdf cb_Ups_2S("cb_Ups_2S", "Double-sided CB for Ups_2S",
                        RooArgList(cb_Ups_2S_1, cb_Ups_2S_2), frac_cb_Ups);
    
    RooCBShape cb_Ups_3S_1("cb_Ups_3S_1", "CB function for Ups_3S", 
        m_Ups, mean_Ups_3S, sigma_Ups_3S_1, alpha_1_Ups, n_1_Ups);
    RooCBShape cb_Ups_3S_2("cb_Ups_3S_2", "CB function for Ups_3S", 
        m_Ups, mean_Ups_3S, sigma_Ups_3S_2, alpha_2_Ups, n_2_Ups);
    RooAddPdf cb_Ups_3S("cb_Ups_3S", "Double-sided CB for Ups_3S",
                        RooArgList(cb_Ups_3S_1, cb_Ups_3S_2), frac_cb_Ups);
    

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

//    // Ups 的背景模型（指数函数）
//    RooRealVar lambda_2("lambda_2", "Background slope for Ups", -0.5, -10.0, 0.0);
//    RooExponential bkg_Ups("bkg_Ups", "Background PDF for Ups", m_Ups, lambda_2);
    // Ups 的背景模型（4阶多项式）
    RooRealVar c0_Ups("c0_Ups", "Ups bkg c0", 1, -100.0, 100.0);
    RooRealVar c1_Ups("c1_Ups", "Ups bkg c1", 0.1, -100.0, 100.0);
    RooRealVar c2_Ups("c2_Ups", "Ups bkg c2", 0.01, -100.0, 100.0);
    // RooRealVar c3_Ups("c3", "c3", 0.001, -1000.0, 1000.0);
    // RooRealVar c4_Ups("c4", "c4", 0.0001, -10000.0, 10000.0);
    // RooPolynomial bkg_Ups("bkg_Ups", "Background PDF for Ups", m_Ups, RooArgList(c0_Ups, c1_Ups, c2_Ups, c3_Ups, c4_Ups));
    // RooPolynomial bkg_Ups("bkg_Ups", "Background PDF for Ups", m_Ups, RooArgList(c0_Ups, c1_Ups));
    RooChebychev bkg_Ups("bkg_Ups", "Background PDF for Ups", m_Ups, RooArgList(c0_Ups, c1_Ups, c2_Ups));

   // Phi 的信号模型（高斯分布）
   RooRealVar mean_Phi("mean_Phi", "Mean of #Phi", 1.020, 1.01, 1.03); // MeV单位
   RooRealVar sigma_Phi("sigma_Phi", "Sigma of #Phi", 0.003, 0.001, 0.01); // 3.1 MeV宽度
   RooGaussian signal_Phi("signal_Phi", "Signal PDF for #Phi", m_Phi, mean_Phi, sigma_Phi);

   // Phi 的背景模型（4阶多项式）
//    RooRealVar c0_Phi("c0_Phi", "Phi bkg c0", 0.1, -10.0, 10.0);
//    RooRealVar c1_Phi("c1_Phi", "Phi bkg c1", 0.01, -10.0, 10.0);
//    RooRealVar c2_Phi("c2_Phi", "Phi bkg c2", 0.001, -10.0, 10.0);
//    RooChebychev bkg_Phi("bkg_Phi", "Background PDF for #Phi", m_Phi, RooArgList(c0_Phi, c1_Phi, c2_Phi));
   RooRealVar c0_Phi("c0_Phi", "Phi bkg c0", 0.1, -100.0, 100.0);
   RooRealVar c1_Phi("c1_Phi", "Phi bkg c1", 0.01, -100.0, 100.0);
   RooRealVar c2_Phi("c2_Phi", "Phi bkg c2", 0.001, -100.0, 100.0);
   RooRealVar c3_Phi("c3_Phi", "Phi bkg c3", 0.001, -100.0, 100.0);
   RooRealVar c4_Phi("c4_Phi", "Phi bkg c4", 0.0001, -100.0, 100.0);
   RooPolynomial bkg_Phi("bkg_Phi", "Background PDF for #Phi", m_Phi, RooArgList(c0_Phi, c1_Phi, c2_Phi, c3_Phi, c4_Phi));
   
//    // Phi 的背景模型（误差函数）
//    RooRealVar erf_c0("erf_c0", "erf_c0", 1.0, 0.0, 10.0);         // 整体归一化参数
//    RooRealVar erf_c1("erf_c1", "erf_c1", 1.02, 0.98, 1.05);       // 位置参数（阈值）
//    RooRealVar erf_c2("erf_c2", "erf_c2", 0.005, 0.001, 0.02);     // 宽度参数
//    RooRealVar erf_c3("erf_c3", "erf_c3", 0.0, -0.5, 0.5);         // 线性分量的斜率
//    RooRealVar erf_c4("erf_c4", "erf_c4", 1.0, 0.0, 10.0);         // 误差函数的比例因子
//    RooRealVar erf_c5("erf_c5", "二次项系数", 0.0, -5.0, 5.0);
//    // 背景模型：p0 + p1*(m-m0) + p2*erf((m-m0)/p3)
//    // 使用线性项和误差函数的组合
//    RooGenericPdf bkg_Phi("bkg_Phi", "Phi介子背景PDF", 
//       "erf_c0 + erf_c3*(m_Phi - erf_c1) + erf_c5*(m_Phi - erf_c1)*(m_Phi - erf_c1) + erf_c4*TMath::Erf((m_Phi - erf_c1)/erf_c2)",
//       RooArgSet(m_Phi, erf_c0, erf_c1, erf_c2, erf_c3, erf_c4, erf_c5));

    // SSS: signal+signal+signal
   RooRealVar yield_SSS("yield_SSS", "Yield of SSS", 100, 0, 10000);
   RooProdPdf pdf_SSS("pdf_SSS", "Signal+Signal+Signal PDF", 
                  RooArgList(signal_Jpsi, signal_Ups, signal_Phi));

    // SSB: signal+signal+background
    RooRealVar yield_SSB("yield_SSB", "Yield of SSB", 50, 0, 10000);
    RooProdPdf pdf_SSB("pdf_SSB", "Signal+Signal+Background PDF", 
                   RooArgList(signal_Jpsi, signal_Ups, bkg_Phi));

    // SBS: signal+background+signal
    RooRealVar yield_SBS("yield_SBS", "Yield of SBS", 50, 0, 10000);
    RooProdPdf pdf_SBS("pdf_SBS", "Signal+Background+Signal PDF", 
                   RooArgList(signal_Jpsi, bkg_Ups, signal_Phi));
    
    // BSS: background+signal+signal
    RooRealVar yield_BSS("yield_BSS", "Yield of BSS", 50, 0, 10000);
    RooProdPdf pdf_BSS("pdf_BSS", "Background+Signal+Signal PDF", 
                   RooArgList(bkg_Jpsi, signal_Ups, signal_Phi));
    // SBB: signal+background+background
    RooRealVar yield_SBB("yield_SBB", "Yield of SBB", 25, 0, 10000);
    RooProdPdf pdf_SBB("pdf_SBB", "Signal+Background+Background PDF", 
                   RooArgList(signal_Jpsi, bkg_Ups, bkg_Phi));
    // BSB: background+signal+background
    RooRealVar yield_BSB("yield_BSB", "Yield of BSB", 25, 0, 1000000);
    RooProdPdf pdf_BSB("pdf_BSB", "Background+Signal+Background PDF", 
                   RooArgList(bkg_Jpsi, signal_Ups, bkg_Phi));
    // BBS: background+background+signal
    RooRealVar yield_BBS("yield_BBS", "Yield of BBS", 25, 0, 10000);
    RooProdPdf pdf_BBS("pdf_BBS", "Background+Background+Signal PDF", 
                   RooArgList(bkg_Jpsi, bkg_Ups, signal_Phi));
    // BBB: background+background+background
    RooRealVar yield_BBB("yield_BBB", "Yield of BBB", 10, 0, 100000);
    RooProdPdf pdf_BBB("pdf_BBB", "Background+Background+Background PDF", 
                   RooArgList(bkg_Jpsi, bkg_Ups, bkg_Phi));
    // 组合8种可能性为一个完整模型
    RooAddPdf totalModel("totalModel", "Total PDF", 
                      RooArgList(pdf_SSS, pdf_SSB, pdf_SBS, pdf_BSS, pdf_SBB, pdf_BSB, pdf_BBS, pdf_BBB),
                      RooArgList(yield_SSS, yield_SSB, yield_SBS, yield_BSS, yield_SBB, yield_BSB, yield_BBS, yield_BBB));
    totalModel.fixCoefNormalization(normSet_Total);


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

            #ifdef Jpsi_CUT
            if (Jpsi_pt->at(iCand) < Jpsi_pT_min){
                passCut = false;
            }
            if (Jpsi_mu_1_pt->at(iCand) < Jpsi_mu_pT_min || Jpsi_mu_2_pt->at(iCand) < Jpsi_mu_pT_min){
                passCut = false;
            }
            if (Jpsi_VtxProb->at(iCand) < Jpsi_VtxProb_min){
                passCut = false;
            }
            if (Jpsi_mu_require_medium && (!Jpsi_mu_1_isPatMediumMuon->at(iCand) || !Jpsi_mu_2_isPatMediumMuon->at(iCand))){
                passCut = false;
            }
            if (Jpsi_mu_require_loose && (!Jpsi_mu_1_isPatLooseMuon->at(iCand) || !Jpsi_mu_2_isPatLooseMuon->at(iCand))){
                passCut = false;
            }
            if (Jpsi_mu_require_tight && (!Jpsi_mu_1_isPatTightMuon->at(iCand) || !Jpsi_mu_2_isPatTightMuon->at(iCand))){
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

            if (Ups_VtxProb->at(iCand) < Ups_VtxProb_min){
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

            #ifdef Phi_CUT
            if (Phi_pt->at(iCand) < Phi_pT_min){
                passCut = false;
            }
            if (Phi_K_1_pt->at(iCand) < Phi_K_pT_min || Phi_K_2_pt->at(iCand) < Phi_K_pT_min){
                passCut = false;
            }
            if (Phi_VtxProb->at(iCand) < Phi_VtxProb_min){
                passCut = false;
            }
            #endif

            #ifdef CUT_MASS
            if (Jpsi_mass->at(iCand) < Jpsi_mass_min || Jpsi_mass->at(iCand) > Jpsi_mass_max){
                passCut = false;
            }
            if (Ups_mass->at(iCand) < Ups_mass_min || Ups_mass->at(iCand) > Ups_mass_max){
                passCut = false;
            }
            if (Phi_mass->at(iCand) < Phi_mass_min || Phi_mass->at(iCand) > Phi_mass_max){
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
        for (auto cand: CandList){
            Jpsi_mass_var.setVal(Jpsi_mass->at(cand->GetId()));
            Ups_mass_var.setVal(Ups_mass->at(cand->GetId()));
            Phi_mass_var.setVal(Phi_mass->at(cand->GetId()));
            Pri_mass_var.setVal(Pri_mass->at(cand->GetId()));

            Jpsi_mass_var.setError(Jpsi_massErr->at(cand->GetId()));
            Ups_mass_var.setError(Ups_massErr->at(cand->GetId()));
            Phi_mass_var.setError(Phi_massErr->at(cand->GetId()));
            Pri_mass_var.setError(Pri_massErr->at(cand->GetId()));

            Jpsi_mass_set_multi.add(RooArgSet(Jpsi_mass_var));
            Ups_mass_set_multi.add(RooArgSet(Ups_mass_var));
            Phi_mass_set_multi.add(RooArgSet(Phi_mass_var));
            Pri_mass_set_multi.add(RooArgSet(Pri_mass_var));
        }
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
            Jpsi_mass_var.setVal(Jpsi_mass->at(cand->GetId()));
            Ups_mass_var.setVal(Ups_mass->at(cand->GetId()));
            Phi_mass_var.setVal(Phi_mass->at(cand->GetId()));
            Pri_mass_var.setVal(Pri_mass->at(cand->GetId()));

            Jpsi_mass_var.setError(Jpsi_massErr->at(cand->GetId()));
            Ups_mass_var.setError(Ups_massErr->at(cand->GetId()));
            Phi_mass_var.setError(Phi_massErr->at(cand->GetId()));
            Pri_mass_var.setError(Pri_massErr->at(cand->GetId()));

            Jpsi_mass_set.add(RooArgSet(Jpsi_mass_var));
            Ups_mass_set.add(RooArgSet(Ups_mass_var));
            Phi_mass_set.add(RooArgSet(Phi_mass_var));
            Pri_mass_set.add(RooArgSet(Pri_mass_var));

            Jpsi_ctau_var.setVal(Jpsi_ctau->at(cand->GetId()));
         Phi_ctau_var.setVal(Phi_ctau->at(cand->GetId()));
         Pri_ctau_var.setVal(Pri_ctau->at(cand->GetId()));

         double jpsi_mass = Jpsi_mass->at(cand->GetId()); // 获取 Jpsi_1_mass 的值
         double ups_mass = Ups_mass->at(cand->GetId()); // 获取 Jpsi_2_mass 的值
         double phi_mass = Phi_mass->at(cand->GetId());      // 获取 Phi_mass 的值
         h2->Fill(jpsi_mass, ups_mass);      // 填充到二维直方图
         h3->Fill(jpsi_mass, phi_mass);         // 填充到二维直方图
         h4->Fill(ups_mass, phi_mass);         // 填充到二维直方图

         m_Jpsi.setVal(Jpsi_mass->at(cand->GetId()));
         m_Ups.setVal(Ups_mass->at(cand->GetId()));
         m_Phi.setVal(Phi_mass->at(cand->GetId()));
         data.add(RooArgSet(m_Jpsi, m_Ups, m_Phi));
        }
        ClearBranches();
        CandList.clear();
    }

    // Draw the histograms.
    TCanvas *MassCanvas1 = new TCanvas("MassCanvas1", "MassCanvas1", 1600, 1200);
    MassCanvas1->Divide(2,2);
    RooPlot* Jpsi_mass_frame = Jpsi_mass_var.frame(nBins);
    RooPlot* Ups_mass_frame = Ups_mass_var.frame(nBins);
    RooPlot* Phi_mass_frame = Phi_mass_var.frame(nBins);
    RooPlot* Pri_mass_frame = Pri_mass_var.frame(nBins);
    Jpsi_mass_set_multi.plotOn(Jpsi_mass_frame);
    Ups_mass_set_multi.plotOn(Ups_mass_frame);
    Phi_mass_set_multi.plotOn(Phi_mass_frame);
    Pri_mass_set_multi.plotOn(Pri_mass_frame);
    MassCanvas1->cd(1);
    Jpsi_mass_frame->Draw();
    MassCanvas1->cd(2);
    Ups_mass_frame->Draw();
    MassCanvas1->cd(3);
    Phi_mass_frame->Draw();
    MassCanvas1->cd(4);
    Pri_mass_frame->Draw();
    MassCanvas1->SaveAs("secCut_multi_Mass_by_pT.pdf");
    MassCanvas1->SaveAs("secCut_multi_Mass_by_pT.png");

    // Draw the histograms for the non-overlap candidates.
    TCanvas *MassCanvas2 = new TCanvas("MassCanvas2", "MassCanvas2", 1600, 1200);
    MassCanvas2->Divide(2,2);
    RooPlot* Jpsi_mass_frame_nonOverlap = Jpsi_mass_var.frame(nBins);
    RooPlot* Ups_mass_frame_nonOverlap = Ups_mass_var.frame(nBins);
    RooPlot* Phi_mass_frame_nonOverlap = Phi_mass_var.frame(nBins);
    RooPlot* Pri_mass_frame_nonOverlap = Pri_mass_var.frame(nBins);
    Jpsi_mass_set.plotOn(Jpsi_mass_frame_nonOverlap);
    Ups_mass_set.plotOn(Ups_mass_frame_nonOverlap);
    Phi_mass_set.plotOn(Phi_mass_frame_nonOverlap);
    Pri_mass_set.plotOn(Pri_mass_frame_nonOverlap);
    MassCanvas2->cd(1);
    Jpsi_mass_frame_nonOverlap->Draw();
    MassCanvas2->cd(2);
    Ups_mass_frame_nonOverlap->Draw();
    MassCanvas2->cd(3);
    Phi_mass_frame_nonOverlap->Draw();
    MassCanvas2->cd(4);
    Pri_mass_frame_nonOverlap->Draw();
    MassCanvas2->SaveAs("secCut_nonOverlap_Mass_by_pT.pdf");
    MassCanvas2->SaveAs("secCut_nonOverlap_Mass_by_pT.png");

    // Draw the histograms for the ctau
   TCanvas *CTauCanvas = new TCanvas("CTauCanvas", "CTauCanvas", 1600, 1200);
   CTauCanvas->Divide(2,2);
   RooPlot* Jpsi_ctau_frame = Jpsi_ctau_var.frame(nBins);
   RooPlot* Phi_ctau_frame = Phi_ctau_var.frame(nBins);
   RooPlot* Pri_ctau_frame = Pri_ctau_var.frame(nBins);
   Jpsi_ctau_set.plotOn(Jpsi_ctau_frame);
   Phi_ctau_set.plotOn(Phi_ctau_frame);
   Pri_ctau_set.plotOn(Pri_ctau_frame);
   CTauCanvas->cd(1);
   gPad->SetLogy();
   Jpsi_ctau_frame->Draw();
   CTauCanvas->cd(2);
   gPad->SetLogy();
   Phi_ctau_frame->Draw();
   CTauCanvas->cd(3);
   gPad->SetLogy();
   Pri_ctau_frame->Draw();
   CTauCanvas->SaveAs("secCut_ctau_by_pT.pdf");
   CTauCanvas->SaveAs("secCut_ctau_by_pT.png");

   auto surf = new TCanvas("surfopt", "surface options", 200, 200, 800, 600);
   surf->Divide(2, 2);
   surf->SetFillColor(cancolor);
   surf->cd(1);
   h2->Draw("surf3");
   surf->cd(2);
   h3->Draw("surf3");
   surf->cd(3);
   h4->Draw("surf3");
   surf->SaveAs("secCut_Mass_surf.pdf");
   surf->SaveAs("secCut_Mass_surf.png");

   // 执行三维拟合
   //RooFitResult* fitResult = model.fitTo(data, RooFit::Extended(true), RooFit::Save(), RooFit::PrintLevel(1));
   //RooFitResult* fitResult = extendedModel.fitTo(data, RooFit::Save(), RooFit::PrintLevel(1));
   RooFitResult* fitResult = totalModel.fitTo(data, RooFit::Extended(true), RooFit::Save(), RooFit::PrintLevel(1));

   // 创建新的画布展示拟合结果
   TCanvas *fitCanvas = new TCanvas("fitCanvas", "Fit Results", 1800, 600);
   fitCanvas->Divide(3, 1);

   // 创建分布的帧
   RooPlot* frameMJpsi = m_Jpsi.frame(RooFit::Title("J/#psi Mass Distribution"), RooFit::Bins(40));
   RooPlot* frameMUps = m_Ups.frame(RooFit::Title("Ups Mass Distribution"), RooFit::Bins(40));
   RooPlot* frameMPhi = m_Phi.frame(RooFit::Title("#Phi Mass Distribution"), RooFit::Bins(40));

   // 绘制数据
   data.plotOn(frameMJpsi);
   data.plotOn(frameMUps);
   data.plotOn(frameMPhi);

   // 在每个变量上投影PDF
   
   totalModel.plotOn(frameMJpsi, RooFit::LineColor(kRed));
   totalModel.plotOn(frameMJpsi, RooFit::Components("pdf_SSS"), 
                  RooFit::LineColor(kRed), RooFit::FillColor(kRed-4), 
                  RooFit::FillStyle(3004), RooFit::DrawOption("F"));
   totalModel.plotOn(frameMJpsi, RooFit::Components("pdf_SSS,pdf_SSB,pdf_SBS,pdf_SBB"), 
                  RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
   totalModel.plotOn(frameMJpsi, RooFit::Components("pdf_BSS,pdf_BSB,pdf_BBS,pdf_BBB"), 
                  RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));

   totalModel.plotOn(frameMUps, RooFit::LineColor(kRed));
   totalModel.plotOn(frameMUps, RooFit::Components("pdf_SSS"), 
                  RooFit::LineColor(kRed), RooFit::FillColor(kRed-4), 
                  RooFit::FillStyle(3004), RooFit::DrawOption("F"));
   totalModel.plotOn(frameMUps, RooFit::Components("pdf_SSS,pdf_SSB,pdf_BSS,pdf_BSB"), 
                  RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
   totalModel.plotOn(frameMUps, RooFit::Components("pdf_SBS,pdf_SBB,pdf_BBS,pdf_BBB"), 
                  RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));

   totalModel.plotOn(frameMPhi, RooFit::LineColor(kRed));
   totalModel.plotOn(frameMPhi, RooFit::Components("pdf_SSS"), 
                 RooFit::LineColor(kRed), RooFit::FillColor(kRed-4), 
                 RooFit::FillStyle(3004), RooFit::DrawOption("F"));
   totalModel.plotOn(frameMPhi, RooFit::Components("pdf_SSS,pdf_SBS,pdf_BSS,pdf_BBS"), 
                  RooFit::LineColor(kBlue), RooFit::LineStyle(kDashed));
   totalModel.plotOn(frameMPhi, RooFit::Components("pdf_SSB,pdf_SBB,pdf_BSB,pdf_BBB"), 
                  RooFit::LineColor(kGreen), RooFit::LineStyle(kDashed));

   // 添加拟合结果文本框
   fitCanvas->cd(1);
   frameMJpsi->Draw();
   TPaveText *textJpsi = new TPaveText(0.65, 0.7, 0.9, 0.9, "NDC");
   textJpsi->AddText(Form("Mean = %.3f #pm %.3f GeV", mean_Jpsi.getVal(), mean_Jpsi.getError()));
   textJpsi->AddText(Form("CB #sigma = %.3f #pm %.3f GeV", sigma_CB_1.getVal(), sigma_CB_1.getError()));
   textJpsi->AddText(Form("Gauss #sigma = %.3f #pm %.3f GeV", sigma_Gauss_1.getVal(), sigma_Gauss_1.getError()));
   //textJpsi1->AddText(Form("N_{sss} = %.0f #pm %.0f", nsig_Jpsi_1_SSS.getVal(), nsig_Jpsi_1_SSS.getError()));
   textJpsi->AddText(Form("SSS events = %.0f #pm %.0f", 
      yield_SSS.getVal(), sqrt(pow(yield_SSS.getError(),2))));
   textJpsi->SetFillColor(0);
   textJpsi->SetBorderSize(1);
   textJpsi->Draw();

   fitCanvas->cd(2);
   frameMUps->Draw();
   TPaveText *textUps = new TPaveText(0.65, 0.7, 0.9, 0.9, "NDC");
    textUps->AddText(Form("1S Mean = %.3f #pm %.3f GeV", mean_Ups_1S.getVal(), mean_Ups_1S.getError()));
    // textUps->AddText(Form("1S CB_1 #sigma = %.3f #pm %.3f GeV", sigma_Ups_1S_1.getVal(), sigma_Ups_1S_1.getError()));
    // textUps->AddText(Form("1S CB_2 #sigma = %.3f #pm %.3f GeV", sigma_Ups_1S_2.getVal(), sigma_Ups_1S_2.getError()));
    textUps->AddText(Form("frac of 1S = %.3f #pm %.3f", frac_1S.getVal(), frac_1S.getError()));
    textUps->AddText(Form("frac of 2S = %.3f #pm %.3f", frac_2S.getVal(), frac_2S.getError()));
   textUps->AddText(Form("frac of 3S = %.3f #pm %.3f", frac_3S.getVal(), frac_3S.getError()));
   textUps->AddText(Form("SSS events = %.0f #pm %.0f", 
      yield_SSS.getVal(), sqrt(pow(yield_SSS.getError(),2))));
   textUps->SetFillColor(0);
   textUps->SetBorderSize(1);
   textUps->Draw();

   fitCanvas->cd(3);
   frameMPhi->Draw();
   TPaveText *textPhi = new TPaveText(0.65, 0.7, 0.9, 0.9, "NDC");
   textPhi->AddText(Form("Mean = %.3f #pm %.3f GeV", mean_Phi.getVal(), mean_Phi.getError()));
   textPhi->AddText(Form("Sigma = %.3f #pm %.3f GeV", sigma_Phi.getVal(), sigma_Phi.getError()));
   //textPhi->AddText(Form("N_{sss} = %.0f #pm %.0f", nsig_Phi_SSS.getVal(), nsig_Phi_SSS.getError()));
   textPhi->AddText(Form("SSS events = %.0f #pm %.0f", 
      yield_SSS.getVal(), sqrt(pow(yield_SSS.getError(),2))));
   textPhi->SetFillColor(0);
   textPhi->SetBorderSize(1);
   textPhi->Draw();

   // 保存拟合结果
   fitCanvas->SaveAs("secCut_Fit_Results.pdf");
   fitCanvas->SaveAs("secCut_Fit_Results.png");

   // 打印拟合参数
   // std::cout << "\n===== Fit Results =====" << std::endl;
   // std::cout << "J/psi_1 Mean: " << mean_Jpsi_1.getVal() << " ± " << mean_Jpsi_1.getError() << " GeV" << std::endl;
   // std::cout << "J/psi_2 Mean: " << mean_Jpsi_2.getVal() << " ± " << mean_Jpsi_2.getError() << " GeV" << std::endl;
   // std::cout << "Phi Mean: " << mean_Phi.getVal() << " ± " << mean_Phi.getError() << " GeV" << std::endl;
   // std::cout << "Phi Width: " << sigma_Phi.getVal()*1000 << " ± " << sigma_Phi.getError()*1000 << " MeV" << std::endl;

   // 打印每个model的参数
   // RooArgSet* components = model.getComponents();
   // std::cout << "\n===== Model Components =====" << std::endl;
   // components->Print("v");

   // 1. 首先保存包含信号+背景的拟合结果
   RooFitResult* fitResult_SB = totalModel.fitTo(data, RooFit::Extended(true), 
   RooFit::Save(), RooFit::PrintLevel(1));
   double nll_SB = fitResult_SB->minNll(); // 带信号+背景的负对数似然值

   // 2. 强制信号产率为零，只拟合背景
   RooConstVar zero_SSS("zero_SSS", "Constrained zero for SSS", 0.0);
   RooAbsReal* yield_SSS_orig = (RooAbsReal*)yield_SSS.clone("yield_SSS_orig");
   yield_SSS.setAttribute("Constant");
   yield_SSS.setVal(0);

   // 3. 用零信号假设再次拟合
   RooFitResult* fitResult_B = totalModel.fitTo(data, RooFit::Extended(true), 
   RooFit::Save(), RooFit::PrintLevel(1));
   double nll_B = fitResult_B->minNll(); // 仅背景的负对数似然值

   // 4. 恢复原始状态
   yield_SSS.setAttribute("Constant", false);
   yield_SSS.setVal(yield_SSS_orig->getVal());
   delete yield_SSS_orig;

   // 5. 计算似然比检验统计量
   double deltaLL = nll_B - nll_SB;
   double significance = sqrt(2 * deltaLL);

   std::cout << "\n===== Component Yields =====" << std::endl;
   std::cout << "SSS: " << yield_SSS.getVal() << " ± " << yield_SSS.getError() << std::endl;
   std::cout << "SSB: " << yield_SSB.getVal() << " ± " << yield_SSB.getError() << std::endl;
   std::cout << "SBS: " << yield_SBS.getVal() << " ± " << yield_SBS.getError() << std::endl;
   std::cout << "BSS: " << yield_BSS.getVal() << " ± " << yield_BSS.getError() << std::endl;
   std::cout << "SBB: " << yield_SBB.getVal() << " ± " << yield_SBB.getError() << std::endl;
   std::cout << "BSB: " << yield_BSB.getVal() << " ± " << yield_BSB.getError() << std::endl;
   std::cout << "BBS: " << yield_BBS.getVal() << " ± " << yield_BBS.getError() << std::endl;
   std::cout << "BBB: " << yield_BBB.getVal() << " ± " << yield_BBB.getError() << std::endl;

   // 计算总信号数和背景数
   double total_events = yield_SSS.getVal() + yield_SSB.getVal() + yield_SBS.getVal() + yield_BSS.getVal() +
                     yield_SBB.getVal() + yield_BSB.getVal() + yield_BBS.getVal() + yield_BBB.getVal();
                     
   double pure_signal_events = yield_SSS.getVal();
   double signal_err = yield_SSS.getError();

   std::cout << "\n===== Summary =====" << std::endl;
   std::cout << "Total events: " << total_events << std::endl;
   std::cout << "Pure signal events (SSS): " << pure_signal_events << " ± " << signal_err << std::endl;
   std::cout << "Pure signal fraction: " << pure_signal_events/total_events*100 << "%" << std::endl;

   std::cout << "\n===== Significance Calculation =====" << std::endl;
   std::cout << "Log-likelihood (S+B): " << -nll_SB << std::endl;
   std::cout << "Log-likelihood (B only): " << -nll_B << std::endl;
   std::cout << "Delta Log-likelihood: " << deltaLL << std::endl;
   std::cout << "Significance: " << significance << " sigma" << std::endl;

    // Save the output tree.
    TFile *outputFile = new TFile("filtered_data_secCut.root", "RECREATE");
    filteredTree->Write();
    outputFile->Close();
}
