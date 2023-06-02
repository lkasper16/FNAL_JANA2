/
////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat May  6 17:30:16 2023 by ROOT version 6.28/00
// from TTree events/jana4ml4fpga_tree_v1
// found on file: Run_003078_000.root
//////////////////////////////////////////////////////////

#ifndef trdclass_h
#define trdclass_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class trdclass {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   ULong64_t       srs_raw_count;
   vector<unsigned int> *srs_raw_roc;
   vector<unsigned int> *srs_raw_slot;
   vector<unsigned int> *srs_raw_channel;
   vector<unsigned int> *srs_raw_apv_id;
   vector<unsigned int> *srs_raw_channel_apv;
   vector<unsigned long> *srs_raw_samples_index;
   vector<unsigned long> *srs_raw_samples_count;
   vector<unsigned short> *srs_raw_samples;
   ULong64_t       f125_wraw_count;
   vector<unsigned int> *f125_wraw_roc;
   vector<unsigned int> *f125_wraw_slot;
   vector<unsigned int> *f125_wraw_channel;
   vector<bool>    *f125_wraw_invalid_samples;
   vector<bool>    *f125_wraw_overflow;
   vector<unsigned int> *f125_wraw_itrigger;
   vector<unsigned long> *f125_wraw_samples_index;
   vector<unsigned long> *f125_wraw_samples_count;
   vector<unsigned short> *f125_wraw_samples;
   ULong64_t       f125_pulse_count;
   vector<unsigned int> *f125_pulse_roc;
   vector<unsigned int> *f125_pulse_slot;
   vector<unsigned int> *f125_pulse_channel;
   vector<unsigned int> *f125_pulse_npk;
   vector<unsigned int> *f125_pulse_le_time;
   vector<unsigned int> *f125_pulse_time_quality_bit;
   vector<unsigned int> *f125_pulse_overflow_count;
   vector<unsigned int> *f125_pulse_pedestal;
   vector<unsigned int> *f125_pulse_integral;
   vector<unsigned int> *f125_pulse_peak_amp;
   vector<unsigned int> *f125_pulse_peak_time;
   vector<unsigned int> *f125_pulse_word1;
   vector<unsigned int> *f125_pulse_word2;
   vector<unsigned int> *f125_pulse_nsamples_pedestal;
   vector<unsigned int> *f125_pulse_nsamples_integral;
   vector<bool>    *f125_pulse_emulated;
   vector<unsigned int> *f125_pulse_le_time_emulated;
   vector<unsigned int> *f125_pulse_time_quality_bit_emulated;
   vector<unsigned int> *f125_pulse_overflow_count_emulated;
   vector<unsigned int> *f125_pulse_pedestal_emulated;
   vector<unsigned int> *f125_pulse_integral_emulated;
   vector<unsigned int> *f125_pulse_peak_amp_emulated;
   vector<unsigned int> *f125_pulse_peak_time_emulated;
   ULong64_t       f250_wraw_count;
   vector<unsigned int> *f250_wraw_roc;
   vector<unsigned int> *f250_wraw_slot;
   vector<unsigned int> *f250_wraw_channel;
   vector<bool>    *f250_wraw_invalid_samples;
   vector<bool>    *f250_wraw_overflow;
   vector<unsigned int> *f250_wraw_itrigger;
   vector<unsigned long> *f250_wraw_samples_index;
   vector<unsigned long> *f250_wraw_samples_count;
   vector<unsigned short> *f250_wraw_samples;

   // List of branches
   TBranch        *b_srs_raw_count;   //!
   TBranch        *b_srs_raw_roc;   //!
   TBranch        *b_srs_raw_slot;   //!
   TBranch        *b_srs_raw_channel;   //!
   TBranch        *b_srs_raw_apv_id;   //!
   TBranch        *b_srs_raw_channel_apv;   //!
   TBranch        *b_srs_raw_samples_index;   //!
   TBranch        *b_srs_raw_samples_count;   //!
   TBranch        *b_srs_raw_samples;   //!
   TBranch        *b_f125_wraw_count;   //!
   TBranch        *b_f125_wraw_roc;   //!
   TBranch        *b_f125_wraw_slot;   //!
   TBranch        *b_f125_wraw_channel;   //!
   TBranch        *b_f125_wraw_invalid_samples;   //!
   TBranch        *b_f125_wraw_overflow;   //!
   TBranch        *b_f125_wraw_itrigger;   //!
   TBranch        *b_f125_wraw_samples_index;   //!
   TBranch        *b_f125_wraw_samples_count;   //!
   TBranch        *b_f125_wraw_samples;   //!
   TBranch        *b_f125_pulse_count;   //!
   TBranch        *b_f125_pulse_roc;   //!
   TBranch        *b_f125_pulse_slot;   //!
   TBranch        *b_f125_pulse_channel;   //!
   TBranch        *b_f125_pulse_npk;   //!
   TBranch        *b_f125_pulse_le_time;   //!
   TBranch        *b_f125_pulse_time_quality_bit;   //!
   TBranch        *b_f125_pulse_overflow_count;   //!
   TBranch        *b_f125_pulse_pedestal;   //!
   TBranch        *b_f125_pulse_integral;   //!
   TBranch        *b_f125_pulse_peak_amp;   //!
   TBranch        *b_f125_pulse_peak_time;   //!
   TBranch        *b_f125_pulse_word1;   //!
   TBranch        *b_f125_pulse_word2;   //!
   TBranch        *b_f125_pulse_nsamples_pedestal;   //!
   TBranch        *b_f125_pulse_nsamples_integral;   //!
   TBranch        *b_f125_pulse_emulated;   //!
   TBranch        *b_f125_pulse_le_time_emulated;   //!
   TBranch        *b_f125_pulse_time_quality_bit_emulated;   //!
   TBranch        *b_f125_pulse_overflow_count_emulated;   //!
   TBranch        *b_f125_pulse_pedestal_emulated;   //!
   TBranch        *b_f125_pulse_integral_emulated;   //!
   TBranch        *b_f125_pulse_peak_amp_emulated;   //!
   TBranch        *b_f125_pulse_peak_time_emulated;   //!
   TBranch        *b_f250_wraw_count;   //!
   TBranch        *b_f250_wraw_roc;   //!
   TBranch        *b_f250_wraw_slot;   //!
   TBranch        *b_f250_wraw_channel;   //!
   TBranch        *b_f250_wraw_invalid_samples;   //!
   TBranch        *b_f250_wraw_overflow;   //!
   TBranch        *b_f250_wraw_itrigger;   //!
   TBranch        *b_f250_wraw_samples_index;   //!
   TBranch        *b_f250_wraw_samples_count;   //!
   TBranch        *b_f250_wraw_samples;   //!

   trdclass(int RunNum);
   virtual ~trdclass();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   //==================  histograms ========================

   int RunNum;
 
   TH1F *hCal_occ;
   TH1F *hCal_sum;
   TH1F *hCal_sum_el;
   TH1F *hCal_sum_pi;
   const int NCAL=7;
   TH1F *hCal_adc[7];  //---  FADC250 channles 0 - 8
   TH1F *hCal_time[7];  //---  FADC250 channles 0 - 8
   const int NCHER=3;
   //TH1F *hCher_adc[3]; //-- FADC250 channels 13,14,15
   TH1F *hCher_u_adc;
   TH1F *hCher_din_adc;
   TH1F *hCher_dout_adc;
   TH1F *hCher_u_time;
   TH1F *hCher_din_time;
   TH1F *hCher_dout_time;

   TH2F *hCCor_ud;
   TH2F *hCCCor_u;
   TH2F *hCCCor_dout;

   TH1F *f125_el;
   TH1F *f125_pi;
   TH2F *f125_el_amp2d;
   TH2F *f125_pi_amp2d;
   TH2F *f125_el_clu2d;
   TH2F *f125_pi_clu2d;

   TH1F *mmg_f125_el;
   TH1F *mmg_f125_pi;
   TH2F *mmg_f125_el_amp2d;
   TH2F *mmg_f125_pi_amp2d;
   TH2F *mmg_f125_el_clu2d;
   TH2F *mmg_f125_pi_clu2d;
   
   TH2F *urw_f125_el_amp2d;
   TH2F *urw_f125_pi_amp2d;
   TH2F *urw_f125_el_clu2d;
   TH2F *urw_f125_pi_clu2d;

   //=============================================
};

#endif

#ifdef trdclass_cxx
trdclass::trdclass(int RunNum_in) : fChain(0)
{
  RunNum=RunNum_in;
  TTree *tree=NULL;
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
     char FileName[128];
     sprintf(FileName,"ROOT/Run_%06d.root",RunNum);
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(FileName);
      if (!f || !f->IsOpen()) {
         f = new TFile(FileName);
      }
      f->GetObject("events",tree);

   }
   Init(tree);
}

trdclass::~trdclass()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t trdclass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t trdclass::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void trdclass::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   srs_raw_roc = 0;
   srs_raw_slot = 0;
   srs_raw_channel = 0;
   srs_raw_apv_id = 0;
   srs_raw_channel_apv = 0;
   srs_raw_samples_index = 0;
   srs_raw_samples_count = 0;
   srs_raw_samples = 0;
   f125_wraw_roc = 0;
   f125_wraw_slot = 0;
   f125_wraw_channel = 0;
   f125_wraw_invalid_samples = 0;
   f125_wraw_overflow = 0;
   f125_wraw_itrigger = 0;
   f125_wraw_samples_index = 0;
   f125_wraw_samples_count = 0;
   f125_wraw_samples = 0;
   f125_pulse_roc = 0;
   f125_pulse_slot = 0;
   f125_pulse_channel = 0;
   f125_pulse_npk = 0;
   f125_pulse_le_time = 0;
   f125_pulse_time_quality_bit = 0;
   f125_pulse_overflow_count = 0;
   f125_pulse_pedestal = 0;
   f125_pulse_integral = 0;
   f125_pulse_peak_amp = 0;
   f125_pulse_peak_time = 0;
   f125_pulse_word1 = 0;
   f125_pulse_word2 = 0;
   f125_pulse_nsamples_pedestal = 0;
   f125_pulse_nsamples_integral = 0;
   f125_pulse_emulated = 0;
   f125_pulse_le_time_emulated = 0;
   f125_pulse_time_quality_bit_emulated = 0;
   f125_pulse_overflow_count_emulated = 0;
   f125_pulse_pedestal_emulated = 0;
   f125_pulse_integral_emulated = 0;
   f125_pulse_peak_amp_emulated = 0;
   f125_pulse_peak_time_emulated = 0;
   f250_wraw_roc = 0;
   f250_wraw_slot = 0;
   f250_wraw_channel = 0;
   f250_wraw_invalid_samples = 0;
   f250_wraw_overflow = 0;
   f250_wraw_itrigger = 0;
   f250_wraw_samples_index = 0;
   f250_wraw_samples_count = 0;
   f250_wraw_samples = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("srs_raw_count", &srs_raw_count, &b_srs_raw_count);
   fChain->SetBranchAddress("srs_raw_roc", &srs_raw_roc, &b_srs_raw_roc);
   fChain->SetBranchAddress("srs_raw_slot", &srs_raw_slot, &b_srs_raw_slot);
   fChain->SetBranchAddress("srs_raw_channel", &srs_raw_channel, &b_srs_raw_channel);
   fChain->SetBranchAddress("srs_raw_apv_id", &srs_raw_apv_id, &b_srs_raw_apv_id);
   fChain->SetBranchAddress("srs_raw_channel_apv", &srs_raw_channel_apv, &b_srs_raw_channel_apv);
   fChain->SetBranchAddress("srs_raw_samples_index", &srs_raw_samples_index, &b_srs_raw_samples_index);
   fChain->SetBranchAddress("srs_raw_samples_count", &srs_raw_samples_count, &b_srs_raw_samples_count);
   fChain->SetBranchAddress("srs_raw_samples", &srs_raw_samples, &b_srs_raw_samples);
   fChain->SetBranchAddress("f125_wraw_count", &f125_wraw_count, &b_f125_wraw_count);
   fChain->SetBranchAddress("f125_wraw_roc", &f125_wraw_roc, &b_f125_wraw_roc);
   fChain->SetBranchAddress("f125_wraw_slot", &f125_wraw_slot, &b_f125_wraw_slot);
   fChain->SetBranchAddress("f125_wraw_channel", &f125_wraw_channel, &b_f125_wraw_channel);
   fChain->SetBranchAddress("f125_wraw_invalid_samples", &f125_wraw_invalid_samples, &b_f125_wraw_invalid_samples);
   fChain->SetBranchAddress("f125_wraw_overflow", &f125_wraw_overflow, &b_f125_wraw_overflow);
   fChain->SetBranchAddress("f125_wraw_itrigger", &f125_wraw_itrigger, &b_f125_wraw_itrigger);
   fChain->SetBranchAddress("f125_wraw_samples_index", &f125_wraw_samples_index, &b_f125_wraw_samples_index);
   fChain->SetBranchAddress("f125_wraw_samples_count", &f125_wraw_samples_count, &b_f125_wraw_samples_count);
   fChain->SetBranchAddress("f125_wraw_samples", &f125_wraw_samples, &b_f125_wraw_samples);
   fChain->SetBranchAddress("f125_pulse_count", &f125_pulse_count, &b_f125_pulse_count);
   fChain->SetBranchAddress("f125_pulse_roc", &f125_pulse_roc, &b_f125_pulse_roc);
   fChain->SetBranchAddress("f125_pulse_slot", &f125_pulse_slot, &b_f125_pulse_slot);
   fChain->SetBranchAddress("f125_pulse_channel", &f125_pulse_channel, &b_f125_pulse_channel);
   fChain->SetBranchAddress("f125_pulse_npk", &f125_pulse_npk, &b_f125_pulse_npk);
   fChain->SetBranchAddress("f125_pulse_le_time", &f125_pulse_le_time, &b_f125_pulse_le_time);
   fChain->SetBranchAddress("f125_pulse_time_quality_bit", &f125_pulse_time_quality_bit, &b_f125_pulse_time_quality_bit);
   fChain->SetBranchAddress("f125_pulse_overflow_count", &f125_pulse_overflow_count, &b_f125_pulse_overflow_count);
   fChain->SetBranchAddress("f125_pulse_pedestal", &f125_pulse_pedestal, &b_f125_pulse_pedestal);
   fChain->SetBranchAddress("f125_pulse_integral", &f125_pulse_integral, &b_f125_pulse_integral);
   fChain->SetBranchAddress("f125_pulse_peak_amp", &f125_pulse_peak_amp, &b_f125_pulse_peak_amp);
   fChain->SetBranchAddress("f125_pulse_peak_time", &f125_pulse_peak_time, &b_f125_pulse_peak_time);
   fChain->SetBranchAddress("f125_pulse_word1", &f125_pulse_word1, &b_f125_pulse_word1);
   fChain->SetBranchAddress("f125_pulse_word2", &f125_pulse_word2, &b_f125_pulse_word2);
   fChain->SetBranchAddress("f125_pulse_nsamples_pedestal", &f125_pulse_nsamples_pedestal, &b_f125_pulse_nsamples_pedestal);
   fChain->SetBranchAddress("f125_pulse_nsamples_integral", &f125_pulse_nsamples_integral, &b_f125_pulse_nsamples_integral);
   fChain->SetBranchAddress("f125_pulse_emulated", &f125_pulse_emulated, &b_f125_pulse_emulated);
   fChain->SetBranchAddress("f125_pulse_le_time_emulated", &f125_pulse_le_time_emulated, &b_f125_pulse_le_time_emulated);
   fChain->SetBranchAddress("f125_pulse_time_quality_bit_emulated", &f125_pulse_time_quality_bit_emulated, &b_f125_pulse_time_quality_bit_emulated);
   fChain->SetBranchAddress("f125_pulse_overflow_count_emulated", &f125_pulse_overflow_count_emulated, &b_f125_pulse_overflow_count_emulated);
   fChain->SetBranchAddress("f125_pulse_pedestal_emulated", &f125_pulse_pedestal_emulated, &b_f125_pulse_pedestal_emulated);
   fChain->SetBranchAddress("f125_pulse_integral_emulated", &f125_pulse_integral_emulated, &b_f125_pulse_integral_emulated);
   fChain->SetBranchAddress("f125_pulse_peak_amp_emulated", &f125_pulse_peak_amp_emulated, &b_f125_pulse_peak_amp_emulated);
   fChain->SetBranchAddress("f125_pulse_peak_time_emulated", &f125_pulse_peak_time_emulated, &b_f125_pulse_peak_time_emulated);
   fChain->SetBranchAddress("f250_wraw_count", &f250_wraw_count, &b_f250_wraw_count);
   fChain->SetBranchAddress("f250_wraw_roc", &f250_wraw_roc, &b_f250_wraw_roc);
   fChain->SetBranchAddress("f250_wraw_slot", &f250_wraw_slot, &b_f250_wraw_slot);
   fChain->SetBranchAddress("f250_wraw_channel", &f250_wraw_channel, &b_f250_wraw_channel);
   fChain->SetBranchAddress("f250_wraw_invalid_samples", &f250_wraw_invalid_samples, &b_f250_wraw_invalid_samples);
   fChain->SetBranchAddress("f250_wraw_overflow", &f250_wraw_overflow, &b_f250_wraw_overflow);
   fChain->SetBranchAddress("f250_wraw_itrigger", &f250_wraw_itrigger, &b_f250_wraw_itrigger);
   fChain->SetBranchAddress("f250_wraw_samples_index", &f250_wraw_samples_index, &b_f250_wraw_samples_index);
   fChain->SetBranchAddress("f250_wraw_samples_count", &f250_wraw_samples_count, &b_f250_wraw_samples_count);
   fChain->SetBranchAddress("f250_wraw_samples", &f250_wraw_samples, &b_f250_wraw_samples);
   Notify();
}

Bool_t trdclass::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void trdclass::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t trdclass::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef trdclass_cxx
