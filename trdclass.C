#define trdclass_cxx
#include "trdclass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void trdclass::Loop()
{
//   In a ROOT session, you can do:
//      root> .L trdclass.C
//      root> trdclass t(RunNum)
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//    b_branchname->GetEntry(ientry); //read only this branch
	if (fChain == 0) return;

   //========= Book Histograms =============

  	//-----------------  canvas 0 Event Display ----------
   char c0Title[256]; sprintf(c0Title,"Event_Display_Run=%d",RunNum);
   TCanvas *c0 = new TCanvas("DISP",c0Title,200,200,1300,900);
   c0->Divide(2,1); c0->cd(1);

   hCal_occ = new TH1F("hCal_occ"," Calorimeter Occupancy",9,-0.5,8.5);
   hCal_sum = new TH1F("hCal_sum"," Calorimeter Sum",4096,0.5,4095.5);
   for (int cc=0; cc<NCAL; cc++) {
   	char hName[128];  sprintf(hName,"hCal_adc%d",cc);
     	char hTitle[128]; sprintf(hTitle,"Calorimeter ADC, cell%d",cc);   
     	hCal_adc[cc] = new TH1F(hName,hTitle,4096,-0.5,4095.5);
   }
	
   hCal_sum_el = new TH1F("hCal_sum_el"," Calorimeter Sum for electrons",4096,0.5,4095.5);
   hCal_sum_pi = new TH1F("hCal_sum_pi"," Calorimeter Sum for pions",4096,0.5,4095.5);
   hCher_u_adc = new TH1F("hCher_u_adc"," Cherenkov Upstream ADC ",4096,-0.5,4095.5);
   hCher_din_adc = new TH1F("hCher_din_adc"," Cherenkov Downstream in ADC ",4096,-0.5,4095.5);
   hCher_dout_adc = new TH1F("hCher_dout_adc"," Cherenkov Downstream out ADC ",4096,-0.5,4095.5);
   //
   hCher_u_time = new TH1F("hCher_u_time"," Cherenkov Upstream time ",300,-0.5,299.5);
   hCher_din_time = new TH1F("hCher_din_time"," Cherenkov Downstream in time ",300,-0.5,299.5);
   hCher_dout_time = new TH1F("hCher_dout_time"," Cherenkov Downstream out time ",300,-0.5,299.5);
   //
   hCCCor_u = new TH2F("hCCCor_u"," Cherenkov Calorimeter Corr ; Ustream ; Cal ",400,0.5,4095.5,400,0.5,4095.5);
   hCCCor_dout = new TH2F("hCCCor_dout"," Cherenkov Calorimeter Corr ; Downstr. out ; Cal ",400,0.5,4095.5,400,0.5,4095.5);
   hCCor_ud = new TH2F("hCCor_ud"," Cherenkov ud Corr ; upstream ; Downstr. out ",400,-0.5,4095.5,400,0.5,4095.5);
   //
   f125_el = new TH1F("f125_el","f125 peak amp for electrons",100,0.,4096);
   f125_pi = new TH1F("f125_pi","f125 peak amp for pions",100,0.,4096);
   mmg_f125_el = new TH1F("mmg_f125_el","f125 peak amp for electrons",100,0.,4096);
   mmg_f125_pi = new TH1F("mmg_f125_pi","f125 peak amp for pions",100,0.,4096);

   f125_el_amp2d = new TH2F("f125_el_amp2d","amp (ch,time) for electrons",200,0.5,200.5,240,0.5,240.5);
   f125_pi_amp2d = new TH2F("f125_pi_amp2d","amp (ch,time) for pions",200,0.5,200.5,240,0.5,240.5);
   mmg_f125_el_amp2d = new TH2F("mmg_f125_el_amp2d","amp (ch,time) for electrons",200,0.5,200.5,240,0.5,240.5);
   mmg_f125_pi_amp2d = new TH2F("mmg_f125_pi_amp2d","amp (ch,time) for pions",200,0.5,200.5,240,0.5,240.5);
   urw_f125_el_amp2d = new TH2F("urw_f125_el_amp2d","amp (ch,time) for electrons",200,0.5,200.5,240,0.5,240.5);
   urw_f125_pi_amp2d = new TH2F("urw_f125_pi_amp2d","amp (ch,time) for pions",200,0.5,200.5,240,0.5,240.5);
   f125_el_clu2d = new TH2F("f125_el_clu2d","amp (ch,time) for electrons",200,0.5,200.5,240,0.5,240.5);
   f125_pi_clu2d = new TH2F("f125_pi_clu2d","amp (ch,time) for pions",200,0.5,200.5,240,0.5,240.5);
   mmg_f125_el_clu2d = new TH2F("mmg_f125_el_clu2d","amp (ch,time) for electrons",200,0.5,200.5,240,0.5,240.5);
   mmg_f125_pi_clu2d = new TH2F("mmg_f125_pi_clu2d","amp (ch,time) for pions",200,0.5,200.5,240,0.5,240.5);
   urw_f125_el_clu2d = new TH2F("urw_f125_el_clu2d","amp (ch,time) for electrons",200,0.5,200.5,240,0.5,240.5);
   urw_f125_pi_clu2d = new TH2F("urw_f125_pi_clu2d","amp (ch,time) for pions",200,0.5,200.5,240,0.5,240.5);

   //=========================================

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;  

   //nentries=200;  //-- limit number of events for test 
   //================ Begin Event Loop ==============
	Long64_t jentry=0;
	for (jentry=0; jentry<nentries;jentry++) {
   	Long64_t ientry = LoadTree(jentry);
     	if (ientry < 0) break;
     	nb = fChain->GetEntry(jentry);   nbytes += nb;
     	// if (Cut(ientry) < 0) continue;
		
     	if (jentry<1 || !(jentry%1000) ) 
       printf("------- evt=%llu  f125_raw_count=%llu f125_pulse_count=%llu f250_wraw_count=%llu, srs_raw_count=%llu \n"
	      ,jentry,f125_wraw_count, f125_pulse_count, f250_wraw_count,srs_raw_count);
		
     	if (jentry<1) printf("-------------------- Fadc125 ---------------------------\n");
		
     	for (int i=0;i<f125_pulse_count; i++) {
      	if (jentry<1) printf("F125:: i=%d  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n"
			     ,i,f125_pulse_slot->at(i),f125_pulse_channel->at(i),f125_pulse_npk->at(i)
			     ,f125_pulse_peak_time->at(i),f125_pulse_peak_amp->at(i),f125_pulse_pedestal->at(i));
     		}
		
		
     		if (jentry<1) printf("------------------ Fadc250  wraw_count = %llu ---------\n", f250_wraw_count);

     		double CalSum=0;
     		double Ch_u=0;
     		double Ch_in=0;
     		double Ch_out=0;
         bool electron=false;
			
     		for (int i=0;i<f250_wraw_count; i++) { // --- fadc250 channels loop 
       		if (jentry<1) printf("F250:: i=%d  sl=%d, ch=%d, idx=%lu, cnt=%lu \n"
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

     		if(electron){
      		hCal_sum_el->Fill(CalSum);
     		} else {
      		hCal_sum_pi->Fill(CalSum);
     		}
			
     		if (jentry<1) printf("-------------------- again  Fadc125 ---------------------------\n");
			
     		float f125_amp_max=0.;
     		for (int i=0;i<f125_pulse_count; i++) {
       		if (jentry<1) printf("F125:: i=%d  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n"
			     ,i,f125_pulse_slot->at(i),f125_pulse_channel->at(i),f125_pulse_npk->at(i)
			     ,f125_pulse_peak_time->at(i),f125_pulse_peak_amp->at(i),f125_pulse_pedestal->at(i));
		//cout<<" ++++++++++++++++++++ f125_pulse_npk= "<<f125_pulse_npk->at(i)<<endl;
       		int slot = f125_pulse_slot->at(i);
       		float peak_amp = f125_pulse_peak_amp->at(i);
       		float ped = f125_pulse_pedestal->at(i);
       		if (0 > ped || ped > 200 ) ped = 100;
       		float amp=peak_amp-ped;
       		float time=f125_pulse_peak_time->at(i);   int itime=f125_pulse_peak_time->at(i);
       		int chan=f125_pulse_channel->at(i);
       		int card=chan/24;
       		int card_ch=chan-card*24;
       		int inv_card_ch = 23-card_ch;
       		float dchan=inv_card_ch+card*24+(slot-3)*72.;     int idchan = inv_card_ch+card*24+(slot-3)*72.;
				
       		if (amp<0) amp=0;
       		int MM_THR=150;
				
       		if(electron){
	 				if ( (slot<6) || (slot==6 && chan<24) ) { f125_el_amp2d->Fill(time,dchan,amp); } 
	 				if ( (slot<6) || (slot==6 && chan<24) ) { f125_el_clu2d->Fill(time,dchan,1.); 
					//f125_el->Fill(amp); 
				}
	 			if (amp>MM_THR) {
	   			if ((slot==8)||(slot==9 && chan<48)) { 
	     				mmg_f125_el_amp2d->Fill(time,dchan-360.,amp); mmg_f125_el->Fill(amp);
	     				mmg_f125_el_clu2d->Fill(time,dchan-360.,1.); mmg_f125_el->Fill(amp);
	   			}
	 			}
	   			if ((slot==6&&chan>23)||(slot==7)) { 
	     				urw_f125_el_amp2d->Fill(time,dchan-240.,amp);
	     				urw_f125_el_clu2d->Fill(time,dchan-240.,1.);
	   			}
       	} else {
	 			if ((slot<6)||(slot==6&&chan<24)) { f125_pi_amp2d->Fill(time,dchan,amp);} 
	 			if ((slot<6)||(slot==6&&chan<24)) { f125_pi_clu2d->Fill(time,dchan,1.); 
				//f125_pi->Fill(amp); 
	   		/*
	     		if (itime==23 && idchan==15 && )
	     		printf("***************  fill f125_pi_amp2d:: %f %f %f %f \n",time,dchan,amp, peak_amp);
	     		if (amp < 0.) printf("***************  AAAAMMMMMPPPP = %f %f \n",amp, peak_amp);
	   		*/
	 		}
	 		if (amp>MM_THR) {
	   			if ((slot==8)||(slot==9 && chan<48)) { 
	     			mmg_f125_pi_amp2d->Fill(time,dchan-360.,amp);
	     			mmg_f125_pi_clu2d->Fill(time,dchan-360.,1.);
	     			mmg_f125_pi->Fill(amp); 
	   		}
	 		}
	   			if ((slot==6&&chan>23)||(slot==7)) { 
	     			urw_f125_pi_amp2d->Fill(time,dchan-240.,amp);
	     			urw_f125_pi_clu2d->Fill(time,dchan-240.,1.);
                }
      }
		
      if (peak_amp-ped>f125_amp_max) f125_amp_max=peak_amp-ped;
		
      hCal_sum->Fill(CalSum/7.);
      hCCor_ud->Fill(Ch_u,Ch_out);
      hCCCor_u->Fill(Ch_u,CalSum/7.);
      hCCCor_dout->Fill(Ch_out,CalSum/7.);
		
	}
   if(electron){
		f125_el->Fill(f125_amp_max);
	 //    if((slot<6)||(slot==7&&chan<24))f125_el->Fill(f125_amp_max);
	 //    if((slot==6&&chan>23)||(slot>6&&slot<9)||(slot==9&&chan<48))mmg_f125_el->Fill(f125_amp_max);
	   } else {
	   	f125_pi->Fill(f125_amp_max);
	 //     if((slot<6)||(slot==7&&chan<24))f125_pi->Fill(f125_amp_max);
	 //     if((slot==6&&chan>23)||(slot>6&&slot<9)||(slot==9&&chan<48))mmg_f125_pi->Fill(f125_amp_max);
	   }
      
		
     	//--- Event Display -----
		
     	if (jentry<10 || !(jentry%1000) )  { 
       	c0->cd(1); f125_el_amp2d->Draw("colz");  
       	c0->cd(2); f125_pi_amp2d->Draw("colz");   c0->Modified();   c0->Update(); //sleep(1); 
    	}
		
   } // -- end of event loop 
   
   cout<<" jentry= "<<jentry<<endl;
   
   //==================  S A V E  ===========================

   //open
   TFile* fOut;
   char rootFileName[256]; sprintf(rootFileName, "RunOutput/Run%d_Output.root", RunNum);
   fOut = new TFile(rootFileName, "RECREATE");
   fOut->cd();
   //write hists
   hCher_u_adc->Write(0, TObject::kOverwrite);
   hCher_din_adc->Write(0, TObject::kOverwrite);
   hCher_dout_adc->Write(0, TObject::kOverwrite);
   hCher_u_time->Write(0, TObject::kOverwrite);
   hCher_din_time->Write(0, TObject::kOverwrite);
   hCher_dout_time->Write(0, TObject::kOverwrite);
   hCCCor_u->Write(0, TObject::kOverwrite);
   hCCor_ud->Write(0, TObject::kOverwrite);
   hCCCor_dout->Write(0, TObject::kOverwrite);
   hCal_occ->Write(0, TObject::kOverwrite);
   hCal_sum_el->Write(0, TObject::kOverwrite);
   hCal_sum_pi->Write(0, TObject::kOverwrite);
   f125_el->Write(0, TObject::kOverwrite);
   f125_pi->Write(0, TObject::kOverwrite);
   f125_el_amp2d->Write(0, TObject::kOverwrite);
   f125_pi_amp2d->Write(0, TObject::kOverwrite);
   f125_el_clu2d->Write(0, TObject::kOverwrite);
   f125_pi_clu2d->Write(0, TObject::kOverwrite);
   mmg_f125_el->Write(0, TObject::kOverwrite);
   mmg_f125_pi->Write(0, TObject::kOverwrite);
   mmg_f125_el_amp2d->Write(0, TObject::kOverwrite);
   mmg_f125_pi_amp2d->Write(0, TObject::kOverwrite);
   mmg_f125_el_clu2d->Write(0, TObject::kOverwrite);
   mmg_f125_pi_clu2d->Write(0, TObject::kOverwrite);
   urw_f125_el_amp2d->Write(0, TObject::kOverwrite);
   urw_f125_pi_amp2d->Write(0, TObject::kOverwrite);
   urw_f125_el_clu2d->Write(0, TObject::kOverwrite);
   urw_f125_pi_clu2d->Write(0, TObject::kOverwrite);
   fOut->Write(0, TObject::kOverwrite);
	
   //close
   fOut->Close();
	delete fOut;

   //==================  P L O T ===========================

   //-----------------  canvas 1 calorimeter (fa250) ----------
   char c1Title[256]; sprintf(c1Title,"RunOutput/Calorimeter_Run=%d.pdf",RunNum);
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
   c1->Print(c1Title);

   //-----------------  canvas 2 Cherenkov (fa250) ----------
   char c2Title[256]; sprintf(c2Title,"RunOutput/Cherenkov_det_Run=%d.pdf",RunNum);
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
   c2->Print(c2Title);

   //-----------------  canvas 3 GEMTRD (fa125) ----------
   char c3Title[256]; sprintf(c3Title,"RunOutput/GEMTRD_det_Run=%d.pdf",RunNum);
   TCanvas *c3 = new TCanvas("GEMTRD",c3Title,100,100,1300,900);
   c3->Divide(3,3);
   n=1;

   c3->cd(n++);   f125_el->Draw();
   c3->cd(n++);   f125_pi->Draw();
   c3->cd(n++);   mmg_f125_el->Draw();
   c3->cd(n++);   mmg_f125_pi->Draw();

   c3->cd(n++);   f125_el_amp2d->Draw("colz");
   c3->cd(n++);   f125_pi_amp2d->Draw("colz");
   c3->cd(n++);   mmg_f125_el_amp2d->Draw("colz");
   c3->cd(n++);   mmg_f125_pi_amp2d->Draw("colz");

   c3->Modified();
   c3->Update();
   c3->Print(c3Title);

}
