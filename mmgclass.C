#define mmgclass_cxx
#include "mmgclass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void mmgclass::Loop()
{
//   In a ROOT session, you can do:
//      root> .L mmgclass.C
//      root> mmgclass t(RunNum)
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//   This is the loop skeleton where:
//      jentry is the global entry number in the chain
//      ientry is the entry number in the current Tree
//   Note that the argument to GetEntry must be:
//      jentry for TChain::GetEntry
//      ientry for TTree::GetEntry and TBranch::GetEntry
//
//   To read only selected branches, Insert statements like:
//      METHOD1:
//      fChain->SetBranchStatus("*",0);  // disable all branches
//      fChain->SetBranchStatus("branchname",1);  // activate branchname
//      METHOD2: replace line
//      fChain->GetEntry(jentry);       //read all branches
//      b_branchname->GetEntry(ientry); //read only this branch
   
   if (fChain == 0) return;
   
   // MMG first slot/ch definition
   #define mmg_x_slot 3
   #define mmg_x_ch0 24
   
   // RWELL first slot/ch definition
   #define rwell_x_slot 9
   #define rwell_x_ch0 48
   
   // GEM first slot/ch definition
   #define gem_x_slot 6
   #define gem_x_ch0 0
   
   // GEMTRD mapping
   int GetGEMSlot(int ch) {
      
   }
   int GetGEMChan(int ch) {
      
   }
   
   // MMG-1 mapping
   int GetMMG1Slot(int ch, int runNum) {
      
      if (runNum<3148) { // -- Map #1
         
      } else if (runNum>3147 && runNum<3262) { // -- Map #2
         
      } else if (runNum>3261 && runNum<3279) { // -- Map #3
         
      } else if (runNum>3278) { // -- Map #4
         
      }
   }
   int GetMMG1Chan(int ch, int runNum) {
      int cardNumber = ch/24;
      int cardChannel = ch-cardNumber*24;
      int invCardChannel = 23 - cardChannel;
      
      if (runNum<3148) { // -- Map #1
         return inv_card_ch+card*24+(slot-3)*72.;
      } else if (runNum>3147 && runNum<3262) { // -- Map #2
         
      } else if (runNum>3261 && runNum<3279) { // -- Map #3
         
      } else if (runNum>3278) { // -- Map #4
         
      }
   }
   
   // MMG-2 mapping
   int GetMMG2Slot(int ch, int runNum) {
      if (runNum<3148) { // -- Map #1
         
      } else if (runNum>3147 && runNum<3262) { // -- Map #2
         
      } else if (runNum>3261 && runNum<3279) { // -- Map #3
         
      } else if (runNum>3278) { // -- Map #4
         
      }
   }
   int GetMMG2Chan(int ch, int runNum) {
      if (runNum<3148) { // -- Map #1
         
      } else if (runNum>3147 && runNum<3262) { // -- Map #2
         
      } else if (runNum>3261 && runNum<3279) { // -- Map #3
         
      } else if (runNum>3278) { // -- Map #4
         
      }
   }
   
   // RWELLTRD mapping
   int GetRWELLSlot(int ch, int runNum) {
      if (runNum<3148) { // -- Map #1
         
      } else if (runNum>3147 && runNum<3262) { // -- Map #2
         
      } else if (runNum>3261 && runNum<3279) { // -- Map #3
         
      } else if (runNum>3278) { // -- Map #4
         
      }
   }
   int GetRWELLChan(int ch, int runNum) {
      if (runNum<3148) { // -- Map #1
         
      } else if (runNum>3147 && runNum<3262) { // -- Map #2
         
      } else if (runNum>3261 && runNum<3279) { // -- Map #3
         
      } else if (runNum>3278) { // -- Map #4
         
      }
   }
   
   //========= Book Histograms =============
   
   hCal_occ = new TH1F("hCal_occ"," Calorimeter Occupancy",9,-0.5,8.5);
   hCal_sum = new TH1F("hCal_sum"," Calorimeter Sum",4096,0.5,4095.5);
   for (int cc=0; cc<NCAL; cc++) {
     char hName[128];  sprintf(hName,"hCal_adc%d",cc);
     char hTitle[128]; sprintf(hTitle,"Calorimeter ADC, cell%d",cc);
     hCal_adc[cc] = new TH1F(hName,hTitle,4096,-0.5,4095.5);
   }
   hCher_u_adc = new TH1F("hCher_u_adc"," Cherenkov Upstream ADC ",4096,-0.5,4095.5);
   hCher_din_adc = new TH1F("hCher_din_adc"," Cherenkov Downstream in ADC ",4096,-0.5,4095.5);
   hCher_dout_adc = new TH1F("hCher_dout_adc"," Cherenkov Downstream out ADC ",4096,-0.5,4095.5);
   //
   hCher_u_time = new TH1F("hCher_u_time"," Cherenkov Upstream time ",300,-0.5,299.5);
   hCher_din_time = new TH1F("hCher_din_time"," Cherenkov Downstream in time ",300,-0.5,299.5);
   hCher_dout_time = new TH1F("hCher_dou_time"," Cherenkov Downstream out time ",300,-0.5,299.5);
   //
   hCCCor_u = new TH2F("hCCCor_u"," Cherenkov Calorimeter Corr ; Ustream ; Cal ",400,0.5,4095.5,400,0.5,4095.5);
   hCCCor_dout = new TH2F("hCCCor_dout"," Cherenkov Calorimeter Corr ; Downstr. out ; Cal ",400,0.5,4095.5,400,0.5,4095.5);
   hCCor_ud = new TH2F("hCCor_ud"," Cherenkov ud Corr ; upstream ; Downstr. out ",400,-0.5,4095.5,400,0.5,4095.5);
   //
   f125_el = new TH1F("f125_el","f125 peak amp for electrons",100,0.,4096);
   f125_pi = new TH1F("f125_pi","f125 peak amp for pions",100,0.,4096);
   mmg_f125_el = new TH1F("mmg_f125_el","f125 peak amp for electrons",100,0.,4096);
   mmg_f125_pi = new TH1F("mmg_f125_pi","f125 peak amp for pions",100,0.,4096);

   f125_el_amp2d = new TH2F("f125_el_amp2d","amp (ch,time) for electrons",240,0.5,240.5,200,0.5,200.5);
   f125_pi_amp2d = new TH2F("f125_pi_amp2d","amp (ch,time) for pions",240,0.5,240.5,200,0.5,200.5);
   mmg_f125_el_amp2d = new TH2F("mmg_f125_el_amp2d","amp (ch,time) for electrons",240,0.5,240.5,200,0.5,200.5);
   mmg_f125_pi_amp2d = new TH2F("mmg_f125_pi_amp2d","amp (ch,time) for pions",240,0.5,240.5,200,0.5,200.5);

   //=========================================

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   //nentries=100;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     // if (Cut(ientry) < 0) continue;


     if (jentry<10 || !(jentry%1000) )
       printf("------- evt=%llu  f125_raw_count=%llu f125_pulse_count=%llu f250_wraw_count=%llu, srs_raw_count=%llu \n"
	      ,jentry,f125_wraw_count, f125_pulse_count, f250_wraw_count,srs_raw_count);

     if (jentry<10) printf("-------------------- Fadc125 ---------------------------\n");

     for (int i=0;i<f125_pulse_count; i++) {
       if (jentry<10) printf("F125:: i=%d  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n"
			     ,i,f125_pulse_slot->at(i),f125_pulse_channel->at(i),f125_pulse_npk->at(i)
			     ,f125_pulse_peak_time->at(i),f125_pulse_peak_amp->at(i),f125_pulse_pedestal->at(i));
     }


     if (jentry<10) printf("------------------ Fadc250  wraw_count = %llu ---------\n", f250_wraw_count);

     double CalSum=0;
     double Ch_u=0;
     double Ch_in=0;
     double Ch_out=0;

         bool electron=false;

     for (int i=0;i<f250_wraw_count; i++) { // --- fadc250 channels loop
       if (jentry<10) printf("F250:: i=%d  sl=%d, ch=%d, idx=%lu, cnt=%lu \n"
			     ,i,f250_wraw_slot->at(i),f250_wraw_channel->at(i)
			     ,f250_wraw_samples_index->at(i),f250_wraw_samples_count->at(i));

       int fadc_chan = f250_wraw_channel->at(i);
       int fadc_window = f250_wraw_samples_count->at(i);
       hCal_occ->Fill(fadc_chan+0.);

       int amax=0; int tmax=9;
       for (int si=0; si<fadc_window; si++) {
	 int adc = f250_wraw_samples->at(f250_wraw_samples_index->at(i)+si); // printf(" sample=%d adc=%d \n",si,adc);
	 if (adc>amax) { amax=adc; tmax=si; }
       } // --  end of samples loop
       if (fadc_chan<NCAL) { hCal_adc[fadc_chan]->Fill(amax); CalSum+=amax; }
       else { // Cherenkov
	 if (fadc_chan==13) { hCher_u_adc->Fill(amax);   hCher_u_time->Fill(tmax); Ch_u=amax; }
	 if (fadc_chan==14) { hCher_din_adc->Fill(amax);  hCher_din_time->Fill(tmax); Ch_in=amax; }
	 if (fadc_chan==15) { if(amax>300)electron=true; hCher_dout_adc->Fill(amax);  hCher_dout_time->Fill(tmax);Ch_out=amax; }
       }
     } // -- end of fadc250 channels loop

     if (jentry<10) printf("-------------------- again  Fadc125 ---------------------------\n");

     float f125_amp_max=0.;
     for (int i=0;i<f125_pulse_count; i++) {
       if (jentry<10) printf("F125:: i=%d  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n"
			     ,i,f125_pulse_slot->at(i),f125_pulse_channel->at(i),f125_pulse_npk->at(i)
			     ,f125_pulse_peak_time->at(i),f125_pulse_peak_amp->at(i),f125_pulse_pedestal->at(i));
	
     int slot = f125_pulse_slot->at(i);
     float peak_amp = f125_pulse_peak_amp->at(i);
     float ped = f125_pulse_pedestal->at(i);
     float amp=peak_amp-ped;
     float time=f125_pulse_peak_time->at(i);
     int chan=f125_pulse_channel->at(i);
     int card=chan/24;
     int card_ch=chan-card*24;
     int inv_card_ch = 23-card_ch;
     float dchan=inv_card_ch+card*24+(slot-3)*72.;
     
     
     
       if(electron){
        if((slot<6)||(slot==7&&chan<24))f125_el_amp2d->Fill(time,dchan,amp);
        if((slot==6&&chan>23)||(slot>6&&slot<9)||(slot==9&&chan<48))mmg_f125_el_amp2d->Fill(time,dchan-240.,amp);
     } else {
        if((slot<6)||(slot==7&&chan<24))f125_pi_amp2d->Fill(time,dchan,amp);
        if((slot==6&&chan>23)||(slot>6&&slot<9)||(slot==9&&chan<48))mmg_f125_pi_amp2d->Fill(time,dchan-240.,amp);
     }

     if(peak_amp-ped>f125_amp_max)f125_amp_max=peak_amp-ped;
     hCal_sum->Fill(CalSum/7.);
     hCCor_ud->Fill(Ch_u,Ch_out);
     hCCCor_u->Fill(Ch_u,CalSum/7.);
     hCCCor_dout->Fill(Ch_out,CalSum/7.);

   }
/*       if(electron){
        f((slot<6)||(slot==7&&chan<24))f125_el->Fill(f125_amp_max);
        f((slot==6&&chan>23)||(slot>6&&slot<9)||(slot==9&&chan<48))mmg_f125_el->Fill(f125_amp_max);
     } else {
        if((slot<6)||(slot==7&&chan<24))f125_pi->Fill(f125_amp_max);
        if((slot==6&&chan>23)||(slot>6&&slot<9)||(slot==9&&chan<48))mmg_f125_pi->Fill(f125_amp_max);
     }
*/
   } // -- end of event loop
   
   
   //==================  P L O T ===========================

   char c1Title[256]; sprintf(c1Title,"Calorimeter, Run=%d ",RunNum);
   TCanvas *c1 = new TCanvas("CAL",c1Title,1,1,1300,900);

   c1->Divide(3,3);

   int n=1;
   c1->cd(n++); hCal_occ->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[6]->Draw();
   c1->cd(n++); hCal_sum->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[3]->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[4]->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[5]->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[0]->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[1]->Draw();
   c1->cd(n++);  gPad->SetLogy(); hCal_adc[2]->Draw();

   c1->Modified();
   c1->Update();

   char c2Title[256]; sprintf(c2Title,"Cherenkov det,  Run=%d ",RunNum);
   TCanvas *c2 = new TCanvas("CHER",c2Title,100,100,1300,900);
   c2->Divide(3,3);
   n=1;
   c2->cd(n++);  gPad->SetLogy();  hCher_u_adc->Draw();
   c2->cd(n++);  gPad->SetLogy();  hCher_din_adc->Draw();
   c2->cd(n++);  gPad->SetLogy();  hCher_dout_adc->Draw();
   c2->cd(n++);   hCher_u_time->Draw();
   c2->cd(n++);   hCher_din_time->Draw();
   c2->cd(n++);   hCher_dout_time->Draw();

   c2->cd(n++);  hCCor_ud->Draw("colz");
   c2->cd(n++);  hCCCor_u->Draw("colz");
   c2->cd(n++);  hCCCor_dout->Draw("colz");

   c2->Modified();
   c2->Update();

}
