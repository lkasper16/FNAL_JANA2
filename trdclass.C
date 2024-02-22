#define trdclass_cxx
#include "trdclass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "PlotLib.C"
#include <TError.h>

#define NPRT 1000
#define MAX_PRINT 1
//#define SHOW_EVT_DISPLAY
//#define USE_250_PULSE
//#define USE_125_RAW

//=================================================
//            Prototype DAQ Mapping
//=================================================

// -- GEMTRD mapping --
int GetGEMChan(int ch, int slot) {
  int cardNumber = ch/24;
  int cardChannel = ch-cardNumber*24;
  int invCardChannel = 23-cardChannel;
  if (slot<6 || (slot==6 && ch<24)) {
    //--Remove noisy channels before returning the rest
    if (slot==3 && ch==8) {return -1;} else if (slot==3 && ch==10) {return -1;} else if (slot==6 && ch==16) {return -1;}
    else {return invCardChannel+cardNumber*24+(slot-3)*72.;}
  }
  return -1;
}

// -- MMG-1 mapping --
int GetMMG1Chan(int ch, int slot, int runNum) {
  int cardNumber = ch/24;
  int cardChannel = ch-cardNumber*24;
  int invCardChannel = 23-cardChannel;
  float dchan = invCardChannel+cardNumber*24+(slot-3)*72.;
  
  if (runNum<3148) { // -- Map #1
    if (slot==7 || slot==8 || (slot==6&&ch>23) || (slot==9&&ch<48)) {
      return dchan - 240.;
    }
  } else if (runNum>3147 && runNum<3262) { // -- Map #2
    if (slot==8 || (slot==9&&ch<48)) {
      if (slot==9 && ch==40) {return -1;}
      else {return dchan - 240.;}
    }
  } else if (runNum>3261 && runNum<3279) { // -- Map #3
    if (slot==7&&ch>23) {
      if (slot==7 && ch==58) {return -1;}
      else {return dchan - 312.;}
    }
    if (slot==8 || (slot==9&&ch<48)) {
      if (slot==9 && ch==40) {return -1;}
      else {return dchan - 240.;}
    }
  } else if (runNum>3278) { // -- Map #4
    if (slot==8 || (slot==7&&ch>23) || (slot==9&&ch<48)) {
      return dchan - 264.;
    }
  }
  return -1;
}

// -- MMG-2 mapping --
int GetMMG2Chan(int ch, int slot, int runNum) {
  int cardNumber = ch/24;
  int cardChannel = ch-cardNumber*24;
  int invCardChannel = 23 - cardChannel;
  float dchan = invCardChannel+cardNumber*24+(slot-3)*72.;
  
  if (runNum>3261 && runNum<3279) { // -- Map #3
    if (slot==6&&ch>23&&ch<48) {
      return dchan - 144.;
    }
    if (slot==6&&ch>47) {
      return dchan - 192.;
    }
    if (slot==7&&ch<24) {
      return dchan - 240.;
    }
  } else if (runNum>3278) { // -- Map #4
    if (slot==6&&ch>23&&ch<48) {
      return dchan - 144.;
    }
    if (slot==6&&ch>47) {
      return dchan - 192.;
    }
    if (slot==7&&ch<24) {
      return dchan - 240.;
    }
  }
  return -1;
}

// -- uRWELLTRD mapping --
int GetRWELLChan(int ch, int slot, int runNum) {
  int cardNumber = ch/24;
  int cardChannel = ch-cardNumber*24;
  int invCardChannel = 23 - cardChannel;
  float dchan = invCardChannel+cardNumber*24+(slot-3)*72.;
  
  if (runNum<3148) { // -- Map #1
    return -1;
  } else if (runNum>3147 && runNum<3262) { // -- Map #2
    if (slot==7 || (slot==6&&ch>23)) {
      if (slot==7 && ch==56) {return -1;} else {
      if (slot==6 && ch<48){return dchan - 144.;} else if (slot==6 && ch>47) {return dchan - 192.;} else if (slot==7 && ch<24) {return dchan - 240.;} else if (slot==7 && ch>47) {return dchan - 336.;} else {return dchan - 288.;} }
    }
  }
  return -1;
}

void trdclass::Loop() {
  //   In a ROOT session, you can do:
  //      root> .L trdclass.C
  //      root> trdclass t(RunNum)
  //      root> t.GetEntry(12); // Fill t data members with entry number 12
  //      root> t.Show();       // Show values of entry 12
  //      root> t.Show(16);     // Read and show values of entry 16
  //      root> t.Loop();       // Loop on all entries
  
  if (fChain == 0) return;
  
  //==================================================================================================
  //            B o o k    H i s t o g r a m s
  //==================================================================================================

  TList *HistList = new TList();

//============= Event Display (canvas 0) =============
#ifdef SHOW_EVT_DISPLAY
  char c0Title[256];
  sprintf(c0Title,"Event_Display_Run=%d",RunNum);
  TCanvas *c0 = new TCanvas("DISP",c0Title,200,200,1500,1300);
  c0->Divide(4,3); c0->cd(1);
  TLine peak_line[100];
  
  f125_el_evt_display = new TH2F("f125_el_evt_display","GEM-TRD Track for Electrons;Time Response (8ns);GEMTRD Channel (X)",100,100.5,200.5,200,20.5,220.5);      HistList->Add(f125_el_evt_display);
  f125_el_evt_display->SetStats(0); f125_el_evt_display->SetMinimum(THRESH); f125_el_evt_display->SetMaximum(1000.);
  f125_pi_evt_display = new TH2F("f125_pi_evt_display","GEM-TRD Track for Pions;Time Response (8ns);GEMTRD Channel (X)",100,100.5,200.5,200,20.5,220.5);      HistList->Add(f125_pi_evt_display);
  f125_pi_evt_display->SetStats(0); f125_pi_evt_display->SetMinimum(THRESH); f125_pi_evt_display->SetMaximum(1000.);
  f125_el_raw = new TH2F("f125_el_raw","GEM-TRD Raw fADC Response for Electrons;Time Response (8ns);GEMTRD Channel (X)",100,100.5,200.5,200,20.5,220.5);      HistList->Add(f125_el_raw);
  f125_el_raw->SetStats(0); f125_el_raw->SetMinimum(THRESH); f125_el_raw->SetMaximum(1000.);
  f125_pi_raw = new TH2F("f125_pi_raw","GEM-TRD Raw fADC Response for Pions;Time Response (8ns);GEMTRD Channel (X)",100,100.5,200.5,200,20.5,220.5);        HistList->Add(f125_pi_raw);
  f125_pi_raw->SetStats(0); f125_pi_raw->SetMinimum(THRESH); f125_pi_raw->SetMaximum(1000.);
#endif

  hcount= new TH1D("hcount","Count",3,0,3);     HistList->Add(hcount);
  hcount->SetStats(0); hcount->SetFillColor(38); hcount->SetMinimum(1.);
#if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
  hcount->SetCanExtend(TH1::kXaxis);
#else
  hcount->SetBit(TH1::kCanRebin);
#endif
  
  //h250_size = new TH1F("h250_size"," fa250 Raw data size",4096,0.5,4095.5);      HistList->Add(h250_size);
  
  //============ Calorimeter & Cherenkovs Plots ===============
/* //////////////////////////////////
  hCal_occ = new TH1F("hCal_occ"," Calorimeter Occupancy ; Cal. Cell Number ",8,-0.5,7.5);         HistList->Add(hCal_occ);
  hCal_sum = new TH1F("hCal_sum"," Calorimeter Sum (GeV)",100.,0.,25.);        HistList->Add(hCal_sum);
  for (int cc=0; cc<NCAL; cc++) {
    char hName[128];  sprintf(hName,"hCal_adc%d",cc);
    char hTitle[128]; sprintf(hTitle,"Calorimeter ADC, cell%d",cc);
    hCal_adc[cc] = new TH1F(hName,hTitle,4096,-0.5,4095.5);                    HistList->Add(hCal_adc[cc]);
    //sprintf(hName,"hCal_cor%d",cc);  sprintf(hTitle,"Calorimeter ADC corr GemTrd X, cell%d",cc);
    //hCal_cor[cc] =  new TH2F(hName,hTitle,25,0.5,25.5,100,-55.,55.);          HistList->Add(hCal_cor[cc]);
    //sprintf(hName,"hCal_trk%d",cc);  sprintf(hTitle,"Calorimeter corr GemTRK, cell%d",cc);
    //hCal_trk[cc] =  new TH2F(hName,hTitle,100,-55.,55.,100,-55.,55.);          HistList->Add(hCal_trk[cc]);
    sprintf(hName,"hCal_cal%d",cc);  sprintf(hTitle,"Calorimeter ADC Calib, cell%d",cc);
    hCal_cal[cc] = new TH2F(hName,hTitle,10,-0.5,4095.5,10,-5.,15.);           HistList->Add(hCal_cal[cc]);
  }
  //cal_el_evt = new TH2F("cal_el_evt"," Calorimeter Electron Event ; X ; Y ",3,-0.5,2.5,3,-0.5,2.5);   HistList->Add(cal_el_evt);
  //cal_el_evt->SetMinimum(-2.); cal_el_evt->SetMaximum(10.); cal_el_evt->SetStats(0);
  //cal_pi_evt = new TH2F("cal_pi_evt"," Calorimeter Pion Event ; X ; Y ",3,-0.5,2.5,3,-0.5,2.5);   HistList->Add(cal_pi_evt);
  //cal_pi_evt->SetMinimum(-2.); cal_pi_evt->SetMaximum(10.); cal_pi_evt->SetStats(0);
  ///////////hCal_sum_el = new TH1F("hCal_sum_el"," Calorimeter Sum for electrons",100,0.,25.);                               HistList->Add(hCal_sum_el);
  ///////////hCal_sum_pi = new TH1F("hCal_sum_pi"," Calorimeter Sum for pions",100,0.,25.);                                   HistList->Add(hCal_sum_pi);
  */
  hCher_u_adc = new TH1F("hCher_u_adc"," Cherenkov Upstream ADC ; ADC Amplitude ",4096,-0.5,4095.5);               HistList->Add(hCher_u_adc);
  //hCher_din_adc = new TH1F("hCher_din_adc"," Cherenkov Downstream (in) ADC ; ADC Amplitude ",4096,-0.5,4095.5);       HistList->Add(hCher_din_adc);
  hCher_dout_adc = new TH1F("hCher_dout_adc"," Cherenkov Downstream (out) ADC ; ADC Amplitude ",4096,-0.5,4095.5);    HistList->Add(hCher_dout_adc);
  hCher_u_time = new TH1F("hCher_u_time"," Cherenkov Upstream Time ; Time Response (8ns)",300,-0.5,299.5);                         HistList->Add(hCher_u_time);
  //hCher_din_time = new TH1F("hCher_din_time"," Cherenkov Downstream (in) Time ; Time Response (8ns)",300,-0.5,299.5);              HistList->Add(hCher_din_time);
  hCher_dout_time = new TH1F("hCher_dout_time"," Cherenkov Downstream (out) Time ; Time Response (8ns)",300,-0.5,299.5);           HistList->Add(hCher_dout_time);
  
  // ============= Detector Correlation Plots =============
  //hCCCor_u = new TH2F("hCCCor_u"," Cherenkov Calorimeter Corr ; Upstream ; Calorimeter ",400,0.5,4095.5,400,0.5,4095.5);                 HistList->Add(hCCCor_u);
  //hCCCor_dout = new TH2F("hCCCor_dout"," Cherenkov Calorimeter Corr ; Downstream (out) ; Calorimeter ",400,0.5,4095.5,400,0.5,4095.5);   HistList->Add(hCCCor_dout);
  hCCor_ud = new TH2F("hCCor_ud"," Cherenkov Upstr./Downstr. Corr ; Upstream ; Downstream (out) ",400,-0.5,4095.5,400,0.5,4095.5);   HistList->Add(hCCor_ud);
  
  //-- GEM-TRKR & Prototype Correlations
  srs_gem_dx = new TH2F("srs_gem_dx","Correlation GEMTRD X & GEMTRKR X (Peaks) ; GEMTRD X [mm]; GEMTRKR Peak X [mm]",240,-60.,60.,256,-64.,64.);    HistList->Add(srs_gem_dx);
  srs_gem_dy = new TH2F("srs_gem_dy","Correlation GEMTRD X & GEMTRKR Y (Peaks) ; GEMTRD X [mm]; GEMTRKR Peak Y [mm]",240,-60.,60.,256,-64.,64.);    HistList->Add(srs_gem_dy);
  hgemtrkr_peak_xy_chi2 = new TH2F("hgemtrkr_peak_xy_chi2","Correlation GEMTRD X & GEMTRKR Y (chi^2 condition); GEMTRD X [mm] ; GEMTRKR Peak Y [mm]",240,-60.,60.,256,-64.,64.);  HistList->Add(hgemtrkr_peak_xy_chi2);
  srs_mmg1_x = new TH2F("srs_mmg1_x","Correlation MMG1TRD Y & GEMTRKR X (Peaks); GEMTRKR Peak X [mm] ; MMG-1 Peak(SRS) Y [mm]",256,-64.,64.,256,-64.,64.);    HistList->Add(srs_mmg1_x);
  srs_mmg1_dx = new TH2F("srs_mmg1_dx","Correlation MMG1TRD X & GEMTRKR X (Peaks); MMG-1 X [mm]; GEMTRKR Peak X [mm]",240,-60.,60.,256,-64.,64.);    HistList->Add(srs_mmg1_dx);
  srs_mmg1_dy = new TH2F("srs_mmg1_dy","Correlation MMG1TRD X & GEMTRKR Y ; MMG-1 X [mm]; GEMTRKR Peak Y [mm]",240,-60.,60.,256,-64.,64.);    HistList->Add(srs_mmg1_dy);
  srs_mmg1_y = new TH2F("srs_mmg1_y","Correlation MMG1TRD Y & GEMTRKR Y (Peaks); GEMTRKR Peak Y [mm]; MMG-1 Peak(SRS) Y [mm]",256,-64.,64.,256,-64.,64.);    HistList->Add(srs_mmg1_y);
  srs_urw_x = new TH2F("srs_urw_x","Correlation uRWellTRD Y & GEMTRKR X (Peaks); GEMTRKR Peak X; uRWell Peak(SRS) Y [mm]",256,-64.,64.,128,-64.,64.);    HistList->Add(srs_urw_x);
  srs_urw_dx = new TH2F("srs_urw_dx","Correlation uRWellTRD X & GEMTRKR X (Peaks); uRWell X [mm]; GEMTRKR Peak X [mm]",120,-60.,60.,256,-64.,64.);    HistList->Add(srs_urw_dx);
  srs_urw_dy = new TH2F("srs_urw_dy","Correlation uRWellTRD X & GEMTRKR Y (Peaks) ; uRWell X [mm] ; GEMTRKR Peak Y [mm]",120,-60.,60.,256,-64.,64.);    HistList->Add(srs_urw_dy);
  srs_urw_y = new TH2F("srs_urw_y","Correlation uRWellTRD Y & GEMTRKR Y (Peaks); GEMTRKR Peak Y [mm]; uRWell Peak(SRS) Y [mm]",256,-64.,64.,128,-64.,64.);    HistList->Add(srs_urw_y);
  srs_mmg2_x = new TH2F("srs_mmg2_x","Correlation MMG2TRD Y & GEMTRKR X (Peaks); GEMTRKR Peak X [mm]; MMG-2 Peak(SRS) Y [mm]",256,-64.,64.,128,-64.,64.);    HistList->Add(srs_mmg2_x);
  srs_mmg2_dx = new TH2F("srs_mmg2_dx","Correlation MMG2TRD X & GEMTRKR X (Peaks) ; MMG-2 X [mm]; GEMTRKR Peak X [mm]",72,-72.,72.,256,-64.,64.);    HistList->Add(srs_mmg2_dx);
  srs_mmg2_dy = new TH2F("srs_mmg2_dy","Correlation MMG2TRD & GEMTRKR X&Y; MMG-2 X [mm] ; GEMTRKR Peak Y [mm]",72,-72.,72.,256,-64.,64.);    HistList->Add(srs_mmg2_dy);
  srs_mmg2_y = new TH2F("srs_mmg2_y","Correlation MMG2TRD & GEMTRKR Y (Peaks); GEMTRKR Peak Y [mm]; MMG-2 Y [mm]",256,-64.,64.,128,-64.,64.);    HistList->Add(srs_mmg2_y);
  
  //-- GEM-TRD & Other TRD Prototypes fADC Correlations
  gem_mmg1_x = new TH2F("gem_mmg1_x","Correlation GEMTRD X & MMG1 X ; GEMTRD X [mm]; MMG-1 X [mm]",240,-60.,60.,240,-60.,60.);    HistList->Add(gem_mmg1_x);
  gem_urw_x = new TH2F("gem_urw_x","Correlation GEMTRD X & uRWell X ; GEMTRD X [mm]; uRWell X [mm]",240,-60.,60.,120,-60.,60.);    HistList->Add(gem_urw_x);
  gem_mmg2_x = new TH2F("gem_mmg2_x","Correlation GEMTRD X & MMG2 X ; GEMTRD X [mm]; MMG-2 X [mm]",240,-60.,60.,72,-72.,72.);    HistList->Add(gem_mmg2_x);
  gem_mmg1_y = new TH2F("gem_mmg1_y","Correlation GEMTRD X & MMG1 Y ; GEMTRD X [mm] ; MMG-1 Y(SRS) [mm]",240,-60.,60.,256,-64.,64.);    HistList->Add(gem_mmg1_y);
  gem_urw_y = new TH2F("gem_urw_y","Correlation GEMTRD X & uRWell Y ; GEMTRD X [mm] ; uRWell Y(SRS) [mm]",240,-60.,60.,128,-64.,64.);    HistList->Add(gem_urw_y);
  gem_mmg2_y = new TH2F("gem_mmg2_y","Correlation GEMTRD X & MMG2 Y ; GEMTRD X [mm] ; MMG-2 Y(SRS) [mm]",240,-60.,60.,128,-64.,64.);    HistList->Add(gem_mmg2_y);
  mmg1_urw_y = new TH2F("mmg1_urw_y","Correlation MMG1 Y & uRWell Y ; uRWell Y(SRS) [mm]; MMG-1 Y(SRS) [mm]",128,-64.,64.,256,-64.,64.);    HistList->Add(mmg1_urw_y);
  mmg1_xy = new TH2F("mmg1_xy","Correlation MMG1TRD X&Y ; MMG-1 X [mm] ; MMG-1 Y(SRS) [mm]",240,-60.,60.,256,-64.,64.);    HistList->Add(mmg1_xy);
  urw_xy = new TH2F("urw_xy","Correlation uRWellTRD X&Y ; uRWell X [mm] ; uRWell Y(SRS) [mm]",120,-60.,60.,128,-64.,64.);    HistList->Add(urw_xy);
  mmg2_xy = new TH2F("mmg2_xy","Correlation MMG2TRD X&Y ; MMG-2 X [mm] ; MMG-2 Y(SRS) [mm]",72,-72.,72.,128,-64.,64.);    HistList->Add(mmg2_xy);
  
  //-- GEM-TRKR & PID/Beam Correlations
  //srs_cal_corr = new TH2F("srs_cal_corr","Correlation GEMTRKR & CAL; X ; Y ",100,-55.,55.,100,-55.,55.);            HistList->Add(srs_cal_corr);
  srs_gemtrd_el = new TH2F("srs_gemtrd_el","Correlation GEMTRKR & GEMTRD Electron Hit; GEMTRKR X [mm] ; GEMTRKR Y (Peaks) [mm] ",110,-55.,55.,110,-55.,55.);   HistList->Add(srs_gemtrd_el);
  srs_etrd_beam = new TH2F("srs_etrd_beam","Correlation GEMTRKR & Beam; GEMTRKR Peak X [mm] ; GEMTRKR Peak Y [mm] ",100,-55.,55.,100,-55.,55.);         HistList->Add(srs_etrd_beam);
  srs_gemtrd_pion = new TH2F("srs_gemtrd_pion","Correlation GEMTRKR & GEMTRD Pion Hit; GEMTRKR X [mm] ; GEMTRKR Y [mm] ",110,-55.,55.,110,-55.,55.);   HistList->Add(srs_gemtrd_pion);
  //srs_etrd_ratio = new TH2F("srs_etrd_ratio","Correlation TRK ratio; X ; Y ",100,-55.,55.,100,-55.,55.);         HistList->Add(srs_etrd_ratio);
  
  //=============== Track Fitting & chi^2 ==================
  gErrorIgnoreLevel = kBreak; // Suppress warning messages from empty chi^2 fit data
  TF1 fx("fx","pol1",100,190); //-- Linear function fit (ax+b) over time response window
  TF1 fx_mmg1("fx_mmg1","pol1",80,190);
  TF1 fx_urw("fx_urw","pol1",80,190);
  TF1 fx_mmg2("fx_mmg2","pol1",80,190);
  
  //-- GEMTRD & GEMTRKR alignment
  //double gemtrkr_x2ch=-999.;
  double x_1=-48., y_1=-55., x_2=54., y_2=51.;
  double a_slope=(y_2-y_1)/(x_2-x_1);
  double b_intercept=y_1-a_slope*x_1;
  double x_boxcut1=-50., x_boxcut2=+50., y_boxcut1=-50., y_boxcut2=+50.;
  TF1 ftrk("ftrk","[0]*x+[1]",-55.,55.);
  ftrk.SetParameter(0,a_slope);
  ftrk.SetParameter(1,b_intercept);
  ////TF1 ftrkr("ftrk","(x-[1])/[0]",0.,255.);
  ////ftrkr.SetParameter(0,a_slope);
  ////ftrkr.SetParameter(1,b_intercept);
  
  //-- Prototype Chi^2 Fits
  f125_fit = new TH2F("f125_fit","GEM-TRD Track Fit; Time Response (8ns) ; Channel ",250,0.5,250.5,240,0.5,240.5);     HistList->Add(f125_fit);
  mmg1_f125_fit = new TH2F("mmg1_f125_fit","MMG1-TRD Track Fit; Time Response (8ns) ; Channel ",250,0.5,250.5,240,0.5,240.5);     HistList->Add(mmg1_f125_fit);
  urw_f125_fit = new TH2F("urw_f125_fit","uRWell-TRD Track Fit; Time Response (8ns) ; Channel ",250,0.5,250.5,120,0.5,120.5);     HistList->Add(urw_f125_fit);
  mmg2_f125_fit = new TH2F("mmg2_f125_fit","MMG2-TRD Track Fit; Time Response (8ns) ; Channel ",250,0.5,250.5,64,48.5,122.5);     HistList->Add(mmg2_f125_fit);
  //-- Prototype Chi^2 Track Fitting Distributions
  f125_el_chi2 = new  TH1F("f125_el_chi2","GEM-TRD Electron Chi2 Values",100,0.,10000.);                            HistList->Add(f125_el_chi2);
  f125_pi_chi2 = new  TH1F("f125_pi_chi2","GEM-TRD Pion Chi2 Values",100,0.,10000.);                            HistList->Add(f125_pi_chi2);
  f125_el_fita = new  TH1F("f125_el_fita","GEM-TRD Electron Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(f125_el_fita);
  f125_pi_fita = new  TH1F("f125_pi_fita","GEM-TRD Pion Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(f125_pi_fita);
  mmg1_f125_el_chi2 = new  TH1F("mmg1_f125_el_chi2","MMG1 Electron Chi2 Values",100,0.,10000.);                            HistList->Add(mmg1_f125_el_chi2);
  mmg1_f125_pi_chi2 = new  TH1F("mmg1_f125_pi_chi2","MMG1 Pion Chi2 Values",100,0.,10000.);                            HistList->Add(mmg1_f125_pi_chi2);
  mmg1_f125_el_fita = new  TH1F("mmg1_f125_el_fita","MMG1 Electron Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(mmg1_f125_el_fita);
  mmg1_f125_pi_fita = new  TH1F("mmg1_f125_pi_fita","MMG1 Pion Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(mmg1_f125_pi_fita);
  urw_f125_el_chi2 = new  TH1F("urw_f125_el_chi2","uRWell Electron Chi2 Values",100,0.,10000.);                            HistList->Add(urw_f125_el_chi2);
  urw_f125_pi_chi2 = new  TH1F("urw_f125_pi_chi2","uRWell Pion Chi2 Values",100,0.,10000.);                            HistList->Add(urw_f125_pi_chi2);
  urw_f125_el_fita = new  TH1F("urw_f125_el_fita","uRWell Electron Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(urw_f125_el_fita);
  urw_f125_pi_fita = new  TH1F("urw_f125_pi_fita","uRWell Pion Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(urw_f125_pi_fita);
  mmg2_f125_el_chi2 = new  TH1F("mmg2_f125_el_chi2","MMG2 Electron Chi2 Values",100,0.,10000.);                            HistList->Add(mmg2_f125_el_chi2);
  mmg2_f125_pi_chi2 = new  TH1F("mmg2_f125_pi_chi2","MMG2 Pion Chi2 Values",100,0.,10000.);                            HistList->Add(mmg2_f125_pi_chi2);
  mmg2_f125_el_fita = new  TH1F("mmg2_f125_el_fita","MMG2 Electron Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(mmg2_f125_el_fita);
  mmg2_f125_pi_fita = new  TH1F("mmg2_f125_pi_fita","MMG2 Pion Track (Linear Fit Coefficient)",100,-0.1,+0.1);                            HistList->Add(mmg2_f125_pi_fita);
  
  //======== GEM-TRKR ========
  singleTrackIndex = new TH2F("singleTrackIndex","GEMTracker Correlated Hits with Chi^2; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);      HistList->Add(singleTrackIndex);
  multiTrackIndex = new TH2F("multiTrackIndex","GEMTracker Correlated Hits Failed Chi^2; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);    HistList->Add(multiTrackIndex);
  hgemtrkr_peak_xy = new TH2F("hgemtrkr_peak_xy","GEM-TRKR Peak X-Y Correlation (mm); Peak X [mm]; Peak Y [mm] ",256,-64.,64.,256,-64.,64.);    HistList->Add(hgemtrkr_peak_xy);
  hgemtrkr_ch_xy = new TH2F("hgemtrkr_ch_xy","GEM-TRKR Peak X-Y Correlation (CHANNELS); Peak X Channel Number; Peak Y Channel Number",256,-0.5,255.5,256,-0.5,255.5);    HistList->Add(hgemtrkr_ch_xy);
  //srs_trk_pi = new TH2F("srs_trk_pi","GEM-TRKR , Pions ; X ; Y ",110,-55.,55.,110,-55.,55.);        HistList->Add(srs_trk_pi);
  hgemtrkr_peak_x = new TH1F("hgemtrkr_peak_x"," GEM-TRKR Peak X ; X [mm] ",256,-64.,64.);                HistList->Add(hgemtrkr_peak_x);
  hgemtrkr_peak_y = new TH1F("hgemtrkr_peak_y"," GEM-TRKR Peak Y ; Y [mm] ",256,-64.,64.);              HistList->Add(hgemtrkr_peak_y);
  
  //============= Prototype ADC Amplitude Distributions ============
  f125_el = new TH1F("f125_el","GEM-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096);                  HistList->Add(f125_el);
  f125_pi = new TH1F("f125_pi","GEM-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096);                      HistList->Add(f125_pi);
  mmg1_f125_el = new TH1F("mmg1_f125_el","MMG1-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096);       HistList->Add(mmg1_f125_el);
  mmg1_f125_pi = new TH1F("mmg1_f125_pi","MMG1-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096);           HistList->Add(mmg1_f125_pi);
  urw_f125_el = new TH1F("urw_f125_el","uRWell-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096);          HistList->Add(urw_f125_el);
  urw_f125_pi = new TH1F("urw_f125_pi","uRWell-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096);              HistList->Add(urw_f125_pi);
  mmg2_f125_el = new TH1F("mmg2_f125_el","MMG2-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096);       HistList->Add(mmg2_f125_el);
  mmg2_f125_pi = new TH1F("mmg2_f125_pi","MMG2-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096);           HistList->Add(mmg2_f125_pi);
  
  //-- Prototype ADC Amplitudes in Time (2D)
  f125_el_amp2d = new TH2F("f125_el_amp2d","GEM-TRD Electron Amp. in Time ; Time Response (8ns) ; GEMTRD Channel (X) ",250,0.5,250.5,240,0.5,240.5);           HistList->Add(f125_el_amp2d);
  f125_pi_amp2d = new TH2F("f125_pi_amp2d","GEM-TRD Pion Amp. in Time ; Time Response (8ns) ; GEMTRD Channel (X)",250,0.5,250.5,240,0.5,240.5);               HistList->Add(f125_pi_amp2d);
  mmg1_f125_el_amp2d = new TH2F("mmg1_f125_el_amp2d","MMG1-TRD Electron Amp. in Time; Time Response (8ns) ; MMG1 Channel (X)",250,0.5,250.5,240,0.5,240.5);   HistList->Add(mmg1_f125_el_amp2d);
  mmg1_f125_pi_amp2d = new TH2F("mmg1_f125_pi_amp2d","MMG1-TRD Pion Amp. in Time; Time Response (8ns) ; MMG1 Channel (X)",250,0.5,250.5,240,0.5,240.5);       HistList->Add(mmg1_f125_pi_amp2d);
  mmg2_f125_el_amp2d = new TH2F("mmg2_f125_el_amp2d","MMG2-TRD Electron Amp. in Time; Time Response (8ns) ; MMG2 Channel (X)",250,0.5,250.5,64,48.5,112.5);   HistList->Add(mmg2_f125_el_amp2d);
  mmg2_f125_pi_amp2d = new TH2F("mmg2_f125_pi_amp2d","MMG2-TRD Pion Amp. in Time; Time Response (8ns) ; MMG2 Channel (X)",250,0.5,250.5,64,48.5,112.5);       HistList->Add(mmg2_f125_pi_amp2d);
  urw_f125_el_amp2d = new TH2F("urw_f125_el_amp2d","uRWell-TRD Amp Electron Amp. in Time; Time Response (8ns) ; uRWell Channel (X)",250,0.5,250.5,120,0.5,120.5);      HistList->Add(urw_f125_el_amp2d);
  urw_f125_pi_amp2d = new TH2F("urw_f125_pi_amp2d","uRWell-TRD Pion Amp. in Time; Time Response (8ns) ; uRWell Channel (X)",250,0.5,250.5,120,0.5,120.5);          HistList->Add(urw_f125_pi_amp2d);
  
  //-- Prototype ADC Amplitudes in Time (2D) for Single-Track Evts Only
  f125_el_amp2ds = new TH2F("f125_el_amp2ds","GEM-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; GEMTRD Channel (X)",250,0.5,250.5,240,0.5,240.5);  HistList->Add(f125_el_amp2ds);
  f125_pi_amp2ds = new TH2F("f125_pi_amp2ds","GEM-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; GEMTRD Channel (X)",250,0.5,250.5,240,0.5,240.5);      HistList->Add(f125_pi_amp2ds);
  mmg1_f125_el_amp2ds = new TH2F("mmg1_f125_el_amp2ds","MMG1-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; MMG1 Channel (X)",250,0.5,250.5,240,0.5,240.5);  HistList->Add(mmg1_f125_el_amp2ds);
  mmg1_f125_pi_amp2ds = new TH2F("mmg1_f125_pi_amp2ds","MMG1-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; MMG1 Channel (X)",250,0.5,250.5,240,0.5,240.5);      HistList->Add(mmg1_f125_pi_amp2ds);
  urw_f125_el_amp2ds = new TH2F("urw_f125_el_amp2ds","uRWell-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; uRWell Channel (X)",250,0.5,250.5,120,0.5,120.5);  HistList->Add(urw_f125_el_amp2ds);
  urw_f125_pi_amp2ds = new TH2F("urw_f125_pi_amp2ds","uRWell-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; uRWell Channel (X)",250,0.5,250.5,120,0.5,120.5);      HistList->Add(urw_f125_pi_amp2ds);
  mmg2_f125_el_amp2ds = new TH2F("mmg2_f125_el_amp2ds","MMG2-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; MMG2 Channel (X)",250,0.5,250.5,64,48.5,112.5);  HistList->Add(mmg2_f125_el_amp2ds);
  mmg2_f125_pi_amp2ds = new TH2F("mmg2_f125_pi_amp2ds","MMG2-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; MMG2 Channel (X)",250,0.5,250.5,64,48.5,112.5);      HistList->Add(mmg2_f125_pi_amp2ds);
  
  //-- Prototype Single-Track Amplitudes in Time (for NN)
  gem_zHist = new  TH1F("gem_zHist", "gem_zHist", 20, 80., 200.);
  mmg1_zHist = new  TH1F("mmg1_zHist", "mmg1_zHist", 20, 80., 200.);
  mmg2_zHist = new  TH1F("mmg2_zHist", "mmg2_zHist", 20, 80., 200.);
  urw_zHist = new  TH1F("urw_zHist", "urw_zHist", 20, 80., 200.);
  
  // ============================ End Histogram Booking =====================================
  
  //--- Calorimeters Calibration ----
/* //////////////////////////
  //-------Cal Cell        0      1      2      3      4      5      6
  double CalCal10[NCAL]={ 4096., 3300., 3700., 2100., 2350., 2400., 1760. };
  double CalCal3[NCAL] ={ 1700., 1180., 1340.,  820.,  820.,  860.,  660. };
  double ac[NCAL],ab[NCAL];
  TF1 *fcal[NCAL];
  for (int i=0; i<NCAL; i++) {
    ac[i]=(10.-3.)/(CalCal10[i]-CalCal3[i]);
    ab[i]=3.-ac[i]*CalCal3[i];
    char fnam[64]; sprintf(fnam,"fcal%d",i);
    fcal[i] = new TF1(fnam,"[0]*x+[1]",-5.,4000.);
    fcal[i]->SetParameter(0,ac[i]);
    fcal[i]->SetParameter(1,ab[i]);
  }
*/ ////////////////////////
  // ----- Declare Beam Energy -----
  double Ebeam=10.; // GeV
  if  (3131 <= RunNum && RunNum <= 3171) {    Ebeam=10.;   }   // NEG 10 GeV
  if  (3172 <= RunNum && RunNum <= 3176) {    Ebeam=120.;   }   // protons
  if  (3177 <= RunNum && RunNum <= 3210) {    Ebeam=10.;   }   //  NEG 10 GeV
  if  (3211 <= RunNum && RunNum <= 3219) {    Ebeam=3.;    }   // NEG 3 GeV
  if  (3220 <= RunNum && RunNum <= 3226) {    Ebeam=120.;  }   // protons
  if  (3227 <= RunNum && RunNum <= 3227) {    Ebeam=10.;   }   // POS 10 GeV
  if  (3228 <= RunNum && RunNum <= 3240) {    Ebeam=120.;  }   // protons
  if  (3241 <= RunNum && RunNum <= 3253) {    Ebeam=3.;    }   // NEG 3GeV
  if  (3255 <= RunNum && RunNum <= 3261) {    Ebeam=120.;  }   // protons
  if  (3272 <= RunNum && RunNum <= 3288) {    Ebeam=10.;   }   // NEG 10 GeV
  if  (3289 <= RunNum && RunNum <= 3290) {    Ebeam=120.;  }   // protons
  double Ebeam_el=0.2*Ebeam;
  double Ebeam_pi=0.1*Ebeam;
  
  //=================================================
  //        Create TTrees of Hit Info for NN
  //=================================================
  
  TFile* fHits;
  int save_hits_root = 1;
  if (save_hits_root) {
    char hitsFileName[256]; sprintf(hitsFileName, "RootOutput/trd_singleTrackHits_Run_%06d.root", RunNum);
    fHits = new TFile(hitsFileName, "RECREATE");
    //-- GEM-TRD
    EVENT_VECT_GEM = new TTree("gem_hits","GEM TTree with single track hit info");
    EVENT_VECT_GEM->Branch("event_num",&event_num,"event_num/I");
    EVENT_VECT_GEM->Branch("nhit",&gem_nhit,"gem_nhit/I");
    EVENT_VECT_GEM->Branch("xpos",&gem_xpos);
    EVENT_VECT_GEM->Branch("zpos",&gem_zpos);
    EVENT_VECT_GEM->Branch("dedx",&gem_dedx);
    EVENT_VECT_GEM->Branch("parID",&gem_parID);
    EVENT_VECT_GEM->Branch("zHist",&gem_zHist_vect);
    //-- MMG1-TRD
    EVENT_VECT_MMG1 = new TTree("mmg1_hits","MMG1 TTree with single track hit info");
    EVENT_VECT_MMG1->Branch("event_num",&event_num,"event_num/I");
    EVENT_VECT_MMG1->Branch("nhit",&mmg1_nhit,"mmg1_nhit/I");
    EVENT_VECT_MMG1->Branch("xpos",&mmg1_xpos);
    EVENT_VECT_MMG1->Branch("zpos",&mmg1_zpos);
    EVENT_VECT_MMG1->Branch("dedx",&mmg1_dedx);
    EVENT_VECT_MMG1->Branch("parID",&mmg1_parID);
    EVENT_VECT_MMG1->Branch("zHist",&mmg1_zHist_vect);
    if (RunNum>3261) {
    //-- MMG2-TRD
      EVENT_VECT_MMG2 = new TTree("mmg2_hits","MMG2 TTree with single track hit info");
      EVENT_VECT_MMG2->Branch("event_num",&event_num,"event_num/I");
      EVENT_VECT_MMG2->Branch("nhit",&mmg2_nhit,"mmg2_nhit/I");
      EVENT_VECT_MMG2->Branch("xpos",&mmg2_xpos);
      EVENT_VECT_MMG2->Branch("zpos",&mmg2_zpos);
      EVENT_VECT_MMG2->Branch("dedx",&mmg2_dedx);
      EVENT_VECT_MMG2->Branch("parID",&mmg2_parID);
      EVENT_VECT_MMG2->Branch("zHist",&mmg2_zHist_vect);
    } else {
    //-- uRWell-TRD
      EVENT_VECT_URW = new TTree("urw_hits","uRWELL TTree with single track hit info");
      EVENT_VECT_URW->Branch("event_num",&event_num,"event_num/I");
      EVENT_VECT_URW->Branch("nhit",&urw_nhit,"urw_nhit/I");
      EVENT_VECT_URW->Branch("xpos",&urw_xpos);
      EVENT_VECT_URW->Branch("zpos",&urw_zpos);
      EVENT_VECT_URW->Branch("dedx",&urw_dedx);
      EVENT_VECT_URW->Branch("parID",&urw_parID);
      EVENT_VECT_URW->Branch("zHist",&urw_zHist_vect);
    }
  }

//==================================================================================================
//                      E v e n t    L o o p
//==================================================================================================
  
  int pre_n_trk_el=0;
  int pre_n_trk_pi=0;
  int N_trk_el=0;
  int N_trk_pi=0;
  int e_1trk=0;
  int pi_1trk=0;
  int _1TRK=0;
  int n_clSRS=0;
  int pi_CC=0;
  int el_CC=0;
  int e_CHR=0;
  int e_CHR_Up=0;
  int _calsum=0.0;
  int _calsum_ecut=0.0;
  int _calsum_pcut=0.0;
  
  int THRESH=100;
  int MM_THR=65;
  if (RunNum>3250) MM_THR=80;
  int URW_THR=105;
  
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  if (MaxEvt>0) nentries=MaxEvt;  //-- limit number of events for test
  Long64_t jentry=0;
  
  for (jentry=0; jentry<nentries; jentry++) {
    
    event_num = jentry;
    Count("EVT");
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);
    nbytes += nb;
    if (jentry<MAX_PRINT || !(jentry%NPRT)) {
      printf("------- evt=%llu  f125_raw_count=%llu f125_pulse_count=%llu f250_wraw_count=%llu, srs_raw_count=%llu, gem_scluster_count=%llu, srs_prerecon_count=%llu, gem_peak_count=%llu \n", jentry, f125_wraw_count, f125_pulse_count, f250_wraw_count, srs_raw_count, gem_scluster_count, srs_prerecon_count, gem_peak_count);
    }
    
    //-- Reset TTree Hit Info for each event
    gem_nhit=0;
    gem_xpos.clear();
    gem_zpos.clear();
    gem_dedx.clear();
    gem_parID.clear();
    gem_zHist->Reset();
    gem_zHist_vect.clear();
  
    mmg1_nhit=0;
    mmg1_xpos.clear();
    mmg1_zpos.clear();
    mmg1_dedx.clear();
    mmg1_parID.clear();
    mmg1_zHist->Reset();
    mmg1_zHist_vect.clear();
   
    mmg2_nhit=0;
    mmg2_xpos.clear();
    mmg2_zpos.clear();
    mmg2_dedx.clear();
    mmg2_parID.clear();
    mmg2_zHist->Reset();
    mmg2_zHist_vect.clear();
  
    urw_nhit=0;
    urw_xpos.clear();
    urw_zpos.clear();
    urw_dedx.clear();
    urw_parID.clear();
    urw_zHist->Reset();
    urw_zHist_vect.clear();
    
    //==================================================================================================
    //                      E v e n t    D i s p l a y
    //==================================================================================================
#ifdef SHOW_EVT_DISPLAY
    if (jentry<MAX_PRINT) {
      printf("-------------------- Pulse  125  ---------------------------\n");
      for (ULong64_t i=0;i<f125_pulse_count; i++) {
        printf("F125:pulse: i=%lld  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n"
         ,i,f125_pulse_slot->at(i),f125_pulse_channel->at(i),f125_pulse_npk->at(i)
         ,f125_pulse_peak_time->at(i),f125_pulse_peak_amp->at(i),f125_pulse_pedestal->at(i));
      }
      printf("-------------------- Pulse  250  n=%lld ---------------------------\n",f250_pulse_count);
      for (ULong64_t i=0;i<f250_pulse_count; i++) {
        printf("F250:: i=%lld  sl=%d, ch=%d,  npk=%d  time=%d amp=%d ped=%f \n"
         ,i,f250_pulse_slot->at(i),f250_pulse_channel->at(i),f250_pulse_pulse_number->at(i)
         ,f250_pulse_course_time->at(i),f250_pulse_pulse_peak->at(i),f250_pulse_pedestal->at(i)/4.);
      }
      printf("-------------------- Raw  125  ---------------------------\n");
      for (ULong64_t i=0;i<f125_wraw_count; i++) { // --- fadc125 channels loop
        printf("F125:raw: i=%lld  sl=%d, ch=%d, idx=%d, cnt=%d \n"
         ,i,f125_wraw_slot->at(i),f125_wraw_channel->at(i)
         ,f125_wraw_samples_index->at(i),f125_wraw_samples_count->at(i));
      }
      printf("-------------------- SRS   ev=%lld   ---------------------------\n",jentry);
      printf("SRS:: gem_scluster:  cnt=%lld \n",gem_scluster_count);
      for (ULong64_t i=0;i<gem_scluster_count; i++) { // --- SRS cluster loop
        printf("SRS:clusters:  i=%lld  X=%f Y=%f  \n",i,gem_scluster_x->at(i), gem_scluster_y->at(i));
      }
      printf("SRS:: srs_prerecon:  cnt=%lld cnt_x=%ld cnt_y=%ld \n",srs_prerecon_count, srs_prerecon_x->size(), srs_prerecon_y->size() );
      for (ULong64_t i=0;i<srs_prerecon_count; i++) {
        printf("SRS:: srs_prerecon: i=%lld %f %f \n",i,srs_prerecon_x->at(i),srs_prerecon_y->at(i));
      }
      printf("SRS:0: srs_peak:  cnt=%lld \n",gem_peak_count);
      for (ULong64_t i=0;i<gem_peak_count; i++) {
  	    printf("SRS:: srs_peak: i=%lld id=%d name=%s idx=%d apv=%d Amp=%f wid=%f E=%f Pos=%f \n"
         ,i,gem_peak_plane_id->at(i),gem_peak_plane_name->at(i).c_str(),gem_peak_index->at(i), gem_peak_apv_id->at(i), gem_peak_height->at(i)
         ,gem_peak_width->at(i), gem_peak_area->at(i), gem_peak_real_pos->at(i));
      }
    }
#endif
    
    //=============================================
    //   Process   Fa250 Pulse data (Cal. & Cher.)
    //=============================================

#ifdef USE_250_PULSE
    printf("-------------------- Pulse  250  n=%lld ---------------------------\n",f250_pulse_count);
    for (int i=0;i<f250_pulse_count; i++) {
      printf("F250:: i=%d  sl=%d, ch=%d,  npk=%d  time=%d amp=%d ped=%f \n", i, f250_pulse_slot->at(i), f250_pulse_channel->at(i), f250_pulse_pulse_number->at(i), f250_pulse_course_time->at(i), f250_pulse_pulse_peak->at(i), f250_pulse_pedestal->at(i)/4.);
    }
    if (jentry<MAX_PRINT) printf("------------------ Fadc250  wraw_count = %llu ---------\n", f250_wraw_count);
    h250_size->Fill(f250_wraw_count);
#endif
    
    double CalSum=0;
    double Ch_u=0;
    double Ch_in=0;
    double Ch_out=0;
    bool electron_chUp=false;
    bool electron_ch=false;
    bool electron=false;
    bool pion=false;
    double Ecal[NCAL]; for (int i=0; i<NCAL; i++) Ecal[i]=0;
    
    //==================================================================================================
    //                FADC250 Channel Loop (Cal. & Cher.)
    //==================================================================================================
    
    for (ULong64_t i=0; i<f250_wraw_count; i++) {
      int fadc_chan = f250_wraw_channel->at(i);
      int fadc_window = f250_wraw_samples_count->at(i);
      /////////hCal_occ->Fill(fadc_chan+0.);
      int amax=0;
      int tmax=9;
      
      for (int si=0; si<fadc_window; si++) {
        int adc = f250_wraw_samples->at(f250_wraw_samples_index->at(i)+si);
        if (adc>amax) {
          amax=adc;
          tmax=si;
        }
      } // --  end of fADC 250 samples loop
      
      if (fadc_chan<NCAL) { //-- Cal Energy Sum
        //////////Ecal[fadc_chan]=fcal[fadc_chan]->Eval(amax);
        //////////hCal_adc[fadc_chan]->Fill(amax);
        //////////CalSum+=Ecal[fadc_chan];
      } else { // Cherenkov
        if (fadc_chan==13) { if(amax>130)electron_chUp=true; hCher_u_adc->Fill(amax);  hCher_u_time->Fill(tmax); Ch_u=amax; Count("eCHR_Up"); e_CHR_Up++;}
        if (fadc_chan==15) { if(amax>300)electron_ch=true; hCher_dout_adc->Fill(amax);  hCher_dout_time->Fill(tmax); Ch_out=amax; Count("eCHR"); e_CHR++;}
      }
    } // ==== End of Fadc250 Channel Loop =====
    
    
    //=======================================================
    //                   S e t    P I D
    //=======================================================
    
    if (CalSum>0.) {Count("calSum"); _calsum++;}
    if (CalSum>Ebeam_el) {Count("calSumEl"); _calsum_ecut++;}
    if (CalSum<Ebeam_pi && CalSum>0.) {Count("calSumPi"); _calsum_pcut++;}
    if (electron_ch  && electron_chUp) { electron=true;  Count("elCC"); el_CC++;}
    if (!electron_ch && !electron_chUp) { pion=true;  Count("piCC"); pi_CC++;}
/* ////////////////////
    hCal_sum->Fill(CalSum);
    if (electron) {
      hCal_sum_el->Fill(CalSum);
    } else if (pion) {
      hCal_sum_pi->Fill(CalSum);
    }
*/ ///////////////
    //================================================================
    //                   Process GEM-TRKR Data (SRS)
    //================================================================
    
    //double gemtrkr_peak_x=-999., gemtrkr_peak_y=-999., gemtrkr_E=-999., delta_x=1000.;
    
    //==================================================================================================
    //                    Process  Fa125 Pulse Data (TRDs)
    //==================================================================================================
#ifdef SHOW_EVT_DISPLAY
    if (!(jentry%NPRT)) {
      if(electron) {
        f125_el_evt_display->Reset();
        f125_el_raw->Reset();
        cal_el_evt->Reset();
        for (int cc=0; cc<NCAL; cc++) {
          int ix=cc%3; int iy=cc/3;
          if (cc<6) cal_el_evt->Fill(ix,iy,Ecal[cc]); else cal_el_evt->Fill(1,2,Ecal[cc]);
        }
      } else if (pion) {
        f125_pi_evt_display->Reset();
        f125_pi_raw->Reset();
        cal_pi_evt->Reset();
        for (int cc=0; cc<NCAL; cc++) {
          if (cc<6) cal_pi_evt->Fill(cc%3,cc/3,Ecal[cc]); else cal_pi_evt->Fill(1,2,Ecal[cc]);
        }
      }
    }
#endif
    
    f125_fit->Reset();
    mmg1_f125_fit->Reset();
    urw_f125_fit->Reset();
    mmg2_f125_fit->Reset();
    if (electron) {
      f125_el_amp2d->Reset();
      mmg1_f125_el_amp2d->Reset();
      urw_f125_el_amp2d->Reset();
      mmg2_f125_el_amp2d->Reset();
    } else if (pion) {
      f125_pi_amp2d->Reset();
      mmg1_f125_pi_amp2d->Reset();
      urw_f125_pi_amp2d->Reset();
      mmg2_f125_pi_amp2d->Reset();
    }
    
    double x0_urw=-999;
    double x0_mmg1=-999;
    double x0_mmg2=-999;
    double x0_gem=-999;
    double chi2_max=20000;
    
    double gemtrkr_peak_pos_y[gem_peak_count];
    double gemtrkr_peak_pos_x[gem_peak_count];
    double mmg1_peak_pos_y[gem_peak_count];
    double urw_peak_pos_y[gem_peak_count];
    double mmg2_peak_pos_y[gem_peak_count];
    int gemtrkr_peak_ch_y[gem_peak_count];
    int gemtrkr_peak_ch_x[gem_peak_count];
    
    for (ULong64_t i=0;i<f125_pulse_count; i++) {
    
      //if (jentry<MAX_PRINT) printf("F125:: i=%lld  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n", i, f125_pulse_slot->at(i), f125_pulse_channel->at(i), f125_pulse_npk->at(i), f125_pulse_peak_time->at(i), f125_pulse_peak_amp->at(i), f125_pulse_pedestal->at(i));
      //cout<<" ++++++++++++++++++++ f125_pulse_npk= "<<f125_pulse_npk->at(i)<<endl;
      
      //===== Fill Histos to Perform Chi^2 Track Fitting On =====
      
      float peak_amp = f125_pulse_peak_amp->at(i);
      float ped = f125_pulse_pedestal->at(i);
      if (0 > ped || ped > 200 ) ped = 100;
      float amp = peak_amp-ped;
      if (amp<0) amp=0;
      float time = f125_pulse_peak_time->at(i); //int itime=f125_pulse_peak_time->at(i);
      int fADCSlot = f125_pulse_slot->at(i);
      int fADCChan = f125_pulse_channel->at(i);
      int gemChan = GetGEMChan(fADCChan, fADCSlot);
      int mmg1Chan = GetMMG1Chan(fADCChan, fADCSlot, RunNum);
      int mmg2Chan = GetMMG2Chan(fADCChan, fADCSlot, RunNum);
      int rwellChan = GetRWELLChan(fADCChan, fADCSlot, RunNum);
      
      if (gemChan>-1 && amp>THRESH) {
#ifdef SHOW_EVT_DISPLAY
        if (!(jentry%NPRT)) {
          f125_el_evt_display->Fill(time, gemChan, amp);
        }
#endif
        f125_fit->Fill(time,gemChan,amp);
        if (electron) {
          //if (100. < time && time < 185.) {srs_gemtrd_el->Fill(gemtrkr_peak_x, gemtrkr_peak_y, amp);}
          f125_el_amp2d->Fill(time,gemChan,amp);
        } else if (pion) {
#ifdef SHOW_EVT_DISPLAY
          if (!(jentry%NPRT)) {
            f125_el_evt_display->Fill(time, gemChan, amp);
          }
#endif
          //if (100. < time && time < 185.) {srs_gemtrd_pion->Fill(gemtrkr_x,gemtrkr_y,amp);}
          f125_pi_amp2d->Fill(time,gemChan,amp);
        }
      }
      if (mmg1Chan>-1 && amp>MM_THR) {
        mmg1_f125_fit->Fill(time,mmg1Chan,amp);
        if (electron) {
          mmg1_f125_el_amp2d->Fill(time,mmg1Chan,amp);
        } else if (pion) {
          mmg1_f125_pi_amp2d->Fill(time,mmg1Chan,amp);
        }
      }
      if (rwellChan>-1 && amp>URW_THR) {
        urw_f125_fit->Fill(time,rwellChan,amp);
        if (electron) {
          urw_f125_el_amp2d->Fill(time,rwellChan,amp);
        } else if (pion) {
          urw_f125_pi_amp2d->Fill(time,rwellChan,amp);
        }
      }
      if (mmg2Chan>-1 && amp>MM_THR) {
        mmg2_f125_fit->Fill(time,mmg2Chan,amp);
        if (electron) {
          mmg2_f125_el_amp2d->Fill(time,mmg2Chan,amp);
        } else if (pion) {
          mmg2_f125_pi_amp2d->Fill(time,mmg2Chan,amp);
        }
      }
    } //---- End Fadc125 Pulse Loop ----
    
    //==============================================================
    //        TRD Prototypes - Chi^2 Calculation
    //==============================================================
    
    double chi2cc_gem = TrkFit(f125_fit,fx,"fx",1);
    // ?? What does the 0 or 1 mean in the TrkFit argument?
    double a_gem = fx.GetParameter(1);
    double chi2cc_mmg1 = TrkFit(mmg1_f125_fit,fx_mmg1,"fx_mmg1",1);
    double a_mmg1 = fx_mmg1.GetParameter(1);
    double chi2cc_urw = -999.;
    double a_urw = -999.;
    if (RunNum<3262 && RunNum>3147) {
      chi2cc_urw = TrkFit(urw_f125_fit,fx_urw,"fx_urw",1);
      a_urw = fx_urw.GetParameter(1);
    }
    double chi2cc_mmg2 = -999.;
    double a_mmg2 = -999.;
    if (RunNum>3261) {
      chi2cc_mmg2 = TrkFit(mmg2_f125_fit,fx_mmg2,"fx_mmg2",1);
      a_mmg2 = fx_mmg2.GetParameter(1);
    }
    
    if (chi2cc_gem>0. && chi2cc_gem<chi2_max) x0_gem=fx.Eval(100.)*0.4-50.;         //-- Convert channels (strips) to [mm] -- 400u pitch
    if (chi2cc_mmg1>0. && chi2cc_mmg1<chi2_max) x0_mmg1=fx_mmg1.Eval(80.)*0.4-50.;  //-- Convert channels (strips) to [mm] -- 400u pitch
    if (chi2cc_urw>0. && chi2cc_urw<chi2_max) x0_urw=fx_urw.Eval(80.)*0.8-50.;      //-- Convert channels (strips) to [mm] -- 800u pitch
    if (chi2cc_mmg2>0. && chi2cc_mmg2<chi2_max) x0_mmg2=fx_mmg2.Eval(80.)*1.6-50.;  //-- Convert channels (strips) to [mm] -- 1600u pitch
    
    if (x0_gem>-55 && x0_gem<55.) {  
      if(x0_mmg1>-55 && x0_mmg1<55.) gem_mmg1_x->Fill(x0_gem, x0_mmg1);
      if(x0_urw>-55 && x0_urw<55.) gem_urw_x->Fill(x0_gem, x0_urw);
      if(x0_mmg2>-55 && x0_mmg2<55.) gem_mmg2_x->Fill(x0_gem, x0_mmg2);
    }
    
    //==============================================================
    //  Chi^2 Track Fitting for TRDs : Determine Single-Track Evts
    //==============================================================
    bool isSingleTrack=false;
    
    if (electron) {
      f125_el_chi2->Fill(chi2cc_gem);
      mmg1_f125_el_chi2->Fill(chi2cc_mmg1);
      if (RunNum<3262 && RunNum>3147) {urw_f125_el_chi2->Fill(chi2cc_urw);} else if (RunNum>3261) {mmg2_f125_el_chi2->Fill(chi2cc_mmg2);}
      
      double chi2el_gem = TrkFit(f125_el_amp2d, fx, "fx", 0);
      Double_t ax_gem = fx.GetParameter(1);
      if (chi2el_gem>0. && chi2el_gem<chi2_max) {
        f125_el_amp2ds->Add(f125_el_amp2d);
        f125_el_fita->Fill(ax_gem);
        Count("pre_n_trk_el");
        pre_n_trk_el++;
        if ( -0.04 < ax_gem && ax_gem < 0.02) {
          Count("n_trk_el");
          N_trk_el++;
          isSingleTrack=true;
        }
      }
      
      double chi2el_mmg1 = TrkFit(mmg1_f125_el_amp2d, fx_mmg1, "fx_mmg1", 0);
      Double_t ax_mmg1 = fx_mmg1.GetParameter(1);
      if (chi2el_mmg1>0. && chi2el_mmg1<chi2_max) {
        mmg1_f125_el_amp2ds->Add(mmg1_f125_el_amp2d);
        mmg1_f125_el_fita->Fill(ax_mmg1);
        if ( -0.04 < ax_mmg1 && ax_mmg1 < 0.02) {
          isSingleTrack=true;
        }
      }
      
      double chi2el_urw = TrkFit(urw_f125_el_amp2d, fx_urw, "fx_urw", 0);
      Double_t ax_urw = fx_urw.GetParameter(1);
      if (chi2el_urw>0. && chi2el_urw<chi2_max) {
        urw_f125_el_amp2ds->Add(urw_f125_el_amp2d);
        urw_f125_el_fita->Fill(ax_urw);
        if ( -0.04 < ax_urw && ax_urw < 0.02) {
          isSingleTrack=true;
        }
      }
      
      double chi2el_mmg2 = TrkFit(mmg2_f125_el_amp2d, fx_mmg2, "fx_mmg2", 0);
      Double_t ax_mmg2 = fx_mmg2.GetParameter(1);
      if (chi2el_mmg2>0. && chi2el_mmg2<chi2_max) {
        mmg2_f125_el_amp2ds->Add(mmg2_f125_el_amp2d);
        mmg2_f125_el_fita->Fill(ax_mmg2);
        if ( -0.04 < ax_mmg2 && ax_mmg2 < 0.02) {
          isSingleTrack=true;
        }
      }
      
    } else if (pion) {
      f125_pi_chi2->Fill(chi2cc_gem);
      mmg1_f125_pi_chi2->Fill(chi2cc_mmg1);
      if (RunNum<3262 && RunNum>3147) {urw_f125_pi_chi2->Fill(chi2cc_urw);} else if (RunNum>3261) {mmg2_f125_pi_chi2->Fill(chi2cc_mmg2);}
      
      double chi2pi_gem = TrkFit(f125_pi_amp2d, fx, "fx", 0);
      Double_t ax_gem = fx.GetParameter(1);
      if (chi2pi_gem>0. && chi2pi_gem<chi2_max) {
        f125_pi_amp2ds->Add(f125_pi_amp2d);
        f125_pi_fita->Fill(ax_gem);
        Count("pre_n_trk_pi");
        pre_n_trk_pi++;
        if ( -0.04 < ax_gem && ax_gem < 0.02) {
          Count("n_trk_pi");
          N_trk_pi++;
          isSingleTrack=true;
        }
      }
      
      double chi2pi_mmg1 = TrkFit(mmg1_f125_pi_amp2d, fx_mmg1, "fx_mmg1", 0);
      Double_t ax_mmg1 = fx_mmg1.GetParameter(1);
      if (chi2pi_mmg1>0. && chi2pi_mmg1<chi2_max) {
        mmg1_f125_pi_amp2ds->Add(mmg1_f125_pi_amp2d);
        mmg1_f125_pi_fita->Fill(ax_mmg1);
        if ( -0.04 < ax_mmg1 && ax_mmg1 < 0.02) {
          isSingleTrack=true;
        }
      }
      
      double chi2pi_urw = TrkFit(urw_f125_pi_amp2d, fx_urw, "fx_urw", 0);
      Double_t ax_urw = fx_urw.GetParameter(1);
      if (chi2pi_urw>0. && chi2pi_urw<chi2_max) {
        urw_f125_pi_amp2ds->Add(urw_f125_pi_amp2d);
        urw_f125_pi_fita->Fill(ax_urw);
        if ( -0.04 < ax_urw && ax_urw < 0.02) {
          isSingleTrack=true;
        }
      }
      
      double chi2pi_mmg2 = TrkFit(mmg2_f125_pi_amp2d, fx_mmg2, "fx_mmg2", 0);
      Double_t ax_mmg2 = fx_mmg2.GetParameter(1);
      if (chi2pi_mmg2>0. && chi2pi_mmg2<chi2_max) {
        mmg2_f125_pi_amp2ds->Add(mmg2_f125_pi_amp2d);
        mmg2_f125_pi_fita->Fill(ax_mmg2);
        if ( -0.04 < ax_mmg2 && ax_mmg2 < 0.02) {
          isSingleTrack=true;
        }
      }  
    }
    
    //===========================================================
    //  GEMTracker (SRS) Correlation with TRD Prototypes
    //===========================================================
    
    ULong64_t gt_idx_x = 0, gt_idx_y=0, mmg1_idx_y=0, urw_idx_y=0, mmg2_idx_y=0;
    
    for (ULong64_t i=0; i<gem_peak_count; i++) { //-- SRS Peaks Loop
      
      if (gem_peak_plane_name->at(i) == "GEMTRKX") {
        gemtrkr_peak_pos_y[gt_idx_y] = gem_peak_real_pos->at(i);
        if (gemtrkr_peak_pos_y[gt_idx_y]<=0) gemtrkr_peak_pos_y[gt_idx_y]+=50.; else gemtrkr_peak_pos_y[gt_idx_y]-=50.;  gemtrkr_peak_pos_y[gt_idx_y]*=-1.;
        gemtrkr_peak_ch_y[gt_idx_y] = gem_peak_index->at(i);
        if (gemtrkr_peak_ch_y[gt_idx_y]<128) gemtrkr_peak_ch_y[gt_idx_y]+=128; else gemtrkr_peak_ch_y[gt_idx_y]-=128;
        gt_idx_y++;
      } if (gem_peak_plane_name->at(i) == "GEMTRKY") {
          gemtrkr_peak_pos_x[gt_idx_x] = gem_peak_real_pos->at(i);
          if (gemtrkr_peak_pos_x[gt_idx_x]<=0) gemtrkr_peak_pos_x[gt_idx_x]+=50.; else gemtrkr_peak_pos_x[gt_idx_x]-=50.;  gemtrkr_peak_pos_x[gt_idx_x]*=-1.;
        gemtrkr_peak_ch_x[gt_idx_x] = gem_peak_index->at(i);
        if (gemtrkr_peak_ch_x[gt_idx_x]<128) gemtrkr_peak_ch_x[gt_idx_x]+=128; else gemtrkr_peak_ch_x[gt_idx_x]-=128;  
        gt_idx_x++;
      } if (gem_peak_plane_name->at(i) == "MMG1Y") {
          mmg1_peak_pos_y[mmg1_idx_y] = gem_peak_real_pos->at(i);
          if (mmg1_peak_pos_y[mmg1_idx_y]<=0) mmg1_peak_pos_y[mmg1_idx_y]+=50.; else mmg1_peak_pos_y[mmg1_idx_y]-=50.;  mmg1_peak_pos_y[mmg1_idx_y]*=-1.;
          mmg1_idx_y++;
      } if (gem_peak_plane_name->at(i) == "URWELLY") {
          urw_peak_pos_y[urw_idx_y] = gem_peak_real_pos->at(i);
          urw_peak_pos_y[urw_idx_y]*=-1.;
          urw_idx_y++;
      } if (gem_peak_plane_name->at(i) == "MMG2Y") {
          mmg2_peak_pos_y[mmg2_idx_y] = gem_peak_real_pos->at(i);
          if (mmg2_peak_pos_y[mmg2_idx_y]<=0) mmg2_peak_pos_y[mmg2_idx_y]+=50.; else mmg2_peak_pos_y[mmg2_idx_y]-=50.;  mmg2_peak_pos_y[mmg2_idx_y]*=-1.;
          mmg2_idx_y++;
      }
    } //-- End SRS Peak Loop
    
    if (gt_idx_x>0 && gt_idx_y>0) {  
      for (ULong64_t j=0; j<sizeof(gemtrkr_peak_pos_y)/sizeof(gemtrkr_peak_pos_y[0]); j++) {
        hgemtrkr_peak_y->Fill(gemtrkr_peak_pos_y[j]);
        for (ULong64_t i=0; i<sizeof(mmg1_peak_pos_y)/sizeof(mmg1_peak_pos_y[0]); i++) {
          srs_mmg1_y->Fill(gemtrkr_peak_pos_y[j], mmg1_peak_pos_y[i]);
        }
        for (ULong64_t h=0; h<sizeof(urw_peak_pos_y)/sizeof(urw_peak_pos_y[0]); h++) {
          srs_urw_y->Fill(gemtrkr_peak_pos_y[j], urw_peak_pos_y[h]);
        }
        //srs_mmg2_y->Fill(gemtrkr_peak_pos_y[gt_idx_y], mmg2_peak_pos_y[i]);
        for (ULong64_t k=0; k<sizeof(gemtrkr_peak_pos_x)/sizeof(gemtrkr_peak_pos_x[0]); k++) {
          hgemtrkr_peak_xy->Fill(gemtrkr_peak_pos_x[k], gemtrkr_peak_pos_y[j]);
          srs_gem_dy->Fill(x0_gem, gemtrkr_peak_pos_y[j]);
          srs_gem_dx->Fill(x0_gem, gemtrkr_peak_pos_x[k]);
          srs_mmg1_dx->Fill(x0_mmg1, gemtrkr_peak_pos_x[k]);
          srs_mmg1_dy->Fill(x0_mmg1, gemtrkr_peak_pos_y[j]);
          srs_urw_dx->Fill(x0_urw, gemtrkr_peak_pos_x[k]);
          srs_urw_dy->Fill(x0_urw, gemtrkr_peak_pos_y[j]);
          if (chi2cc_gem>chi2_max) {multiTrackIndex->Fill(gt_idx_x, gt_idx_y);}
          if (chi2cc_gem>0. && chi2cc_gem<chi2_max) {
            hgemtrkr_peak_xy_chi2->Fill(x0_gem, gemtrkr_peak_pos_y[j]);
            singleTrackIndex->Fill(gt_idx_x, gt_idx_y);
          }
        }
      }
      for (ULong64_t k=0; k<sizeof(gemtrkr_peak_pos_x)/sizeof(gemtrkr_peak_pos_x[0]); k++) {
        hgemtrkr_peak_x->Fill(gemtrkr_peak_pos_x[k]);
        for (ULong64_t i=0; i<sizeof(mmg1_peak_pos_y)/sizeof(mmg1_peak_pos_y[0]); i++) {
          srs_mmg1_x->Fill(gemtrkr_peak_pos_x[k], mmg1_peak_pos_y[i]);
        }
        for (ULong64_t h=0; h<sizeof(urw_peak_pos_y)/sizeof(urw_peak_pos_y[0]); h++) {
          srs_urw_x->Fill(gemtrkr_peak_pos_x[k], urw_peak_pos_y[h]);
        }
        //srs_mmg2_x->Fill(gemtrkr_peak_pos_x[i], mmg2_peak_pos_y[i]);
      }
      //-- NEW
      for (ULong64_t j=0; j<sizeof(gemtrkr_peak_ch_y)/sizeof(gemtrkr_peak_ch_y[0]); j++) {
        for (ULong64_t k=0; k<sizeof(gemtrkr_peak_ch_x)/sizeof(gemtrkr_peak_ch_x[0]); k++) {
          hgemtrkr_ch_xy->Fill(gemtrkr_peak_ch_x[k], gemtrkr_peak_ch_y[j]);
        }
      }
    }
    for (ULong64_t i=0; i<sizeof(mmg1_peak_pos_y)/sizeof(mmg1_peak_pos_y[0]); i++) {
      mmg1_xy->Fill(x0_mmg1, mmg1_peak_pos_y[i]);
      gem_mmg1_y->Fill(x0_gem, mmg1_peak_pos_y[i]);
    }
    for (ULong64_t h=0; h<sizeof(urw_peak_pos_y)/sizeof(urw_peak_pos_y[0]); h++) {
      urw_xy->Fill(x0_urw, urw_peak_pos_y[h]);
      gem_urw_y->Fill(x0_gem, urw_peak_pos_y[h]);
      for (ULong64_t i=0; i<sizeof(mmg1_peak_pos_y)/sizeof(mmg1_peak_pos_y[0]); i++) {
        mmg1_urw_y->Fill(urw_peak_pos_y[h], mmg1_peak_pos_y[i]);
      }
    }
/*      srs_mmg2_dx->Fill(x0_mmg2, gemtrkr_peak_pos_x[i]);
        srs_mmg2_dy->Fill(x0_mmg2, gemtrkr_peak_pos_y[i]);
        mmg2_xy->Fill(x0_mmg2, mmg2_peak_pos_y[i]);
*/
    //-- NEW
    
    
    //==========================================================
    //  GEM TRKR Fuducial Area (Y-Direction) Selection (Box Cut)
    //==========================================================

    //delta_x=abs(ftrk.Eval(gemtrkr_x)-x0_gem);
    int BoxCut=1;
/*        
        if (RunNum > 3200 && RunNum < 3203) { //-- GEMTRD Double Fleece
          //if (x_boxcut1>gemtrkr_peak_x || gemtrkr_peak_x>x_boxcut2 || y_boxcut1 > gemtrkr_peak_y || gemtrkr_peak_y > y_boxcut2) BoxCut=0;)
          x_boxcut1=-50., x_boxcut2=50., y_boxcut1=-20., y_boxcut2=15.;
          if (x_boxcut1>x0_gem || x0_gem>x_boxcut2 || y_boxcut1>gemtrkr_peak_pos_y || gemtrkr_peak_pos_y>y_boxcut2) BoxCut=0;
        }
        if (RunNum > 3124 && RunNum < 3132) { //-- GEMTRD Single Fleece
          x_boxcut1=-50., x_boxcut2=50., y_boxcut1=-25., y_boxcut2=25.;
          //if (x_boxcut1>gemtrkr_peak_x || gemtrkr_peak_x>x_boxcut2 || y_boxcut1 > gemtrkr_peak_y || gemtrkr_peak_y > y_boxcut2) BoxCut=0;
          if (x_boxcut1>x0_gem || x0_gem>x_boxcut2 || y_boxcut1>gemtrkr_peak_pos_y || gemtrkr_peak_pos_y>y_boxcut2) BoxCut=0;
        }
        if (RunNum > 3131 && RunNum < 3135) { //-- GEMTRD Single Foil (VU)
          x_boxcut1=-50., x_boxcut2=50., y_boxcut1=-40., y_boxcut2=40.;
          //if (x_boxcut1>gemtrkr_peak_x || gemtrkr_peak_x>x_boxcut2 || y_boxcut1 > gemtrkr_peak_y || gemtrkr_peak_y > y_boxcut2) BoxCut=0;
          if (x_boxcut1>x0_gem || x0_gem>x_boxcut2 || y_boxcut1>gemtrkr_peak_pos_y || gemtrkr_peak_pos_y>y_boxcut2) BoxCut=0;
        }
        if (RunNum > 3274 && RunNum < 3289) { //-- GEMTRD Single Foil (TU)
          x_boxcut1=-50., x_boxcut2=50., y_boxcut1=-40., y_boxcut2=40.;
          //if (x_boxcut1>gemtrkr_peak_x || gemtrkr_peak_x>x_boxcut2 || y_boxcut1 > gemtrkr_peak_y || gemtrkr_peak_y > y_boxcut2) BoxCut=0;
          if (x_boxcut1>x0_gem || x0_gem>x_boxcut2 || y_boxcut1>gemtrkr_peak_pos_y || gemtrkr_peak_pos_y>y_boxcut2) BoxCut=0;
        }
        if (RunNum >  3195 && RunNum < 3201) { //-- GEMTRD Double Foil
          x_boxcut1=-50., x_boxcut2=50., y_boxcut1=-40., y_boxcut2=40.;
          //if (x_boxcut1>gemtrkr_peak_x || gemtrkr_peak_x>x_boxcut2 || y_boxcut1 > gemtrkr_peak_y || gemtrkr_peak_y > y_boxcut2) BoxCut=0;
          if (x_boxcut1>x0_gem || x0_gem>x_boxcut2 || y_boxcut1>gemtrkr_peak_pos_y || gemtrkr_peak_pos_y>y_boxcut2) BoxCut=0;
        }
        if (RunNum > 3202 && RunNum < 3205) { //-- GEMTRD No Radiator
          x_boxcut1=-50., x_boxcut2=50., y_boxcut1=-50., y_boxcut2=50.;
          //if (x_boxcut1>gemtrkr_peak_x || gemtrkr_peak_x>x_boxcut2 || y_boxcut1 > gemtrkr_peak_y || gemtrkr_peak_y > y_boxcut2) BoxCut=0;
          if (x_boxcut1>x0_gem || x0_gem>x_boxcut2 || y_boxcut1>gemtrkr_peak_pos_y || gemtrkr_peak_pos_y>y_boxcut2) BoxCut=0;
        }
*/    
    //============================================================================
    //                  Single Track Event fADC Processing
    //============================================================================
    
    //if (isSingleTrack && BoxCut) { //-- single track from TRDs & GEM TRKR Y-direction hit condition --
    if (isSingleTrack) {  
      
      float gem_amp_max = -1000.;
      Count("_1TRK");
      _1TRK++;
      for (ULong64_t i=0;i<f125_pulse_count; i++) { //-- Fadc125 Pulse Loop
    
        float peak_amp = f125_pulse_peak_amp->at(i);
        float ped = f125_pulse_pedestal->at(i);
        if (0 > ped || ped > 200 ) ped = 100;
        float amp=peak_amp-ped;
        if (amp<0) amp=0;
        float time=f125_pulse_peak_time->at(i); //int itime=f125_pulse_peak_time->at(i);
        int fADCSlot = f125_pulse_slot->at(i);
        int fADCChan = f125_pulse_channel->at(i);
        
        int gemChan = GetGEMChan(fADCChan, fADCSlot);
        int mmg1Chan = GetMMG1Chan(fADCChan, fADCSlot, RunNum);
        int mmg2Chan = GetMMG2Chan(fADCChan, fADCSlot, RunNum);
        int rwellChan = GetRWELLChan(fADCChan, fADCSlot, RunNum);
        
        if(electron) { //-- electron by: both Cherenkov detectors (Upstream & Downstream(Out))
          if (i==0) {
            Count("1eTRK");
            e_1trk++;
          }
          if (amp>THRESH && gemChan>-1) {
            f125_el->Fill(amp);
            gem_xpos.push_back(gemChan);
            gem_dedx.push_back(amp);
            gem_zpos.push_back(time);
            gem_parID.push_back(electron);
            gem_nhit++;
            gem_zHist->Fill(time, amp);
          }
          if (amp>MM_THR && mmg1Chan>-1) {
            mmg1_f125_el->Fill(amp);
            mmg1_xpos.push_back(mmg1Chan);
            mmg1_dedx.push_back(amp);
            mmg1_zpos.push_back(time);
            mmg1_parID.push_back(electron);
            mmg1_nhit++;
            mmg1_zHist->Fill(time, amp);
          }
          if (RunNum>3261) {
            if (amp>MM_THR && mmg2Chan>-1) {
              mmg2_f125_el->Fill(amp);
              mmg2_xpos.push_back(mmg2Chan);
              mmg2_dedx.push_back(amp);
              mmg2_zpos.push_back(time);
              mmg2_parID.push_back(electron);
              mmg2_nhit++;
              mmg2_zHist->Fill(time, amp);
            }
          } else {
            if (amp>URW_THR && rwellChan>-1) {
              urw_f125_el->Fill(amp);
              urw_xpos.push_back(rwellChan);
              urw_dedx.push_back(amp);
              urw_zpos.push_back(time);
              urw_parID.push_back(electron);
              urw_nhit++;
              urw_zHist->Fill(time, amp);
            }
          }
          
        } else if (pion) {
          if (i==0) {
            Count("1piTRK");
            pi_1trk++;
          }
          if (amp>THRESH && gemChan>-1) {
            f125_pi->Fill(amp);
            gem_xpos.push_back(gemChan);
            gem_dedx.push_back(amp);
            gem_zpos.push_back(time);
            gem_parID.push_back(electron);
            gem_nhit++;
          }
          if (amp>MM_THR && mmg1Chan>-1) {
            mmg1_f125_pi->Fill(amp);
            mmg1_xpos.push_back(mmg1Chan);
            mmg1_dedx.push_back(amp);
            mmg1_zpos.push_back(time);
            mmg1_parID.push_back(electron);
            mmg1_nhit++;
          }
          if (RunNum>3261) {
            if (amp>MM_THR && mmg2Chan>-1) {
              mmg2_f125_pi->Fill(amp);
              mmg2_xpos.push_back(mmg2Chan);
              mmg2_dedx.push_back(amp);
              mmg2_zpos.push_back(time);
              mmg2_parID.push_back(electron);
              mmg2_nhit++;
            }
          } else {
            if (amp>URW_THR && rwellChan>-1) {
              urw_f125_pi->Fill(amp);
              urw_xpos.push_back(rwellChan);
              urw_dedx.push_back(amp);
              urw_zpos.push_back(time);
              urw_parID.push_back(electron);
              urw_nhit++;
            }
          }
        }
        
        hCCor_ud->Fill(Ch_u,Ch_out);
      } //--- end Fa125 Pulse Loop ---
      
      for (int i=1; i<21; i++) {
        gem_zHist_vect.push_back(gem_zHist->GetBinContent(i));
        mmg1_zHist_vect.push_back(mmg1_zHist->GetBinContent(i));
        mmg2_zHist_vect.push_back(mmg2_zHist->GetBinContent(i));
        urw_zHist_vect.push_back(urw_zHist->GetBinContent(i));
      }
      
    } //-- End Single Track Condition Loop --
    
    //=====================================================
    //        Process   Fa125 RAW data
    //=====================================================
    
#ifdef USE_125_RAW
//  if (jentry<MAX_PRINT) printf("------------------ Fadc125  wraw_count = %llu ---------\n", f125_wraw_count);
    for (ULong64_t i=0;i<f125_wraw_count; i++) { // --- fadc125 channels loop
      //  if (jentry<MAX_PRINT) printf("F125:RAW: i=%lld  sl=%d, ch=%d, idx=%d, cnt=%d \n"
      //			   ,i,f125_wraw_slot->at(i),f125_wraw_channel->at(i)
      //			   ,f125_wraw_samples_index->at(i),f125_wraw_samples_count->at(i));
      
      int fadc_window = f125_wraw_samples_count->at(i);
      int fADCSlot = f125_wraw_slot->at(i);
      int fADCChan = f125_wraw_channel->at(i);
      int gemChan = GetGEMChan(fADCChan, fADCSlot);
      int amax=0;
      int tmax=0;
      
      for (int si=0; si<fadc_window; si++) {
        int time=si;
        int adc = f125_wraw_samples->at(f125_wraw_samples_index->at(i)+si);
        if (adc>amax) {
          amax=adc;
          tmax=si;
        }
        if (!(jentry%NPRT)) {
          if (electron) {
            f125_el_raw->Fill(time,gemChan,adc);
          } else if (pion) {
            f125_pi_raw->Fill(time,gemChan,adc);
          }
        }
      } // --  end of samples loop
    } // -- end of fadc125 channels loop
    
    if (!(jentry%NPRT)) {
      int nx = f125_el_raw->GetNbinsX();
      int ny = f125_el_raw->GetNbinsY();
      double pedestal=100.;
      for (int ii=0; ii<nx; ii++) {
        for (int jj=0; jj<ny; jj++) {
          if (electron) {
            double cc = f125_el_raw->GetBinContent(ii, jj);
            if (cc == 0.) f125_el_raw->Fill(ii,jj,pedestal);
          } else if (pion) {
            double cc = f125_pi_raw->GetBinContent(ii, jj);
            if (cc == 0.) f125_pi_raw->Fill(ii,jj,pedestal);
          }
        }
      }
    }
    //=============  End Fa125 RAW  process Loop  =================
    if(electron) {
      f125_el->Fill(gem_amp_max);
    } else if (pion) {
      f125_pi->Fill(gem_amp_max);
    }
#endif
    
    //=====================================================================================
    //                        E v e n t    D i s p l a y
    //=====================================================================================
    
#ifdef SHOW_EVT_DISPLAY
    if (jentry<MAX_PRINT || !(jentry%NPRT)) {
      c0->cd(1); f125_el_amp2d->Draw("colz");
      c0->cd(5); f125_pi_amp2d->Draw("colz");
      c0->cd(2); f125_el_evt_display->Draw("colz");         double chi2e = TrkFit(f125_el_evt_display,fx1,"fx1",0); if (chi2e>0. && chi2e < chi2_max ) fx1.Draw("same");
      c0->cd(6); f125_pi_evt_display->Draw("colz");         double chi2p = TrkFit(f125_pi_evt_display,fx2,"fx2",0); if (chi2p>0. && chi2p < chi2_max ) fx2.Draw("same");
      printf("========================>>>  Chi2 e=%f p=%f \n",chi2e,chi2p);
      //c0->cd(3); f125_el_chi2->Draw("colz");
      //c0->cd(6); f125_pi_chi2->Draw("colz");
      
      if (electron) {
        c0->cd(3); f125_el_raw->Draw("colz");  f125_el_evt_display->Draw("same");
        TLine lin1(110.,gemtrkr_x2ch,190.,gemtrkr_x2ch); lin1.Draw("same");   //--- draw  gemtrkr x
        printf("++++++++++++ Draw GEMTRK:: %f %f %f  \n",gemtrkr_x,ftrk.Eval(gemtrkr_x),gemtrkr_x2ch);
      }
      if (pion) {
        c0->cd(7); f125_pi_raw->Draw("colz");  f125_pi_evt_display->Draw("same");
        TLine lin2(110.,gemtrkr_x2ch,190.,gemtrkr_x2ch); lin2.SetLineColor(kRed); lin2.Draw("same");   //--- draw  gemtrkr x
        //c0->cd(12); gPad->WaitPrimitive();
      }
      if (electron || pion ) {
        printf("--------------->  SRS:: srs_peak:  cnt=%lld evt=%llu \n",gem_peak_count,jentry);
        int lc=0;
        for (int k=0; k<100; k++) {  peak_line[k].SetX1 (100.);	    peak_line[lc].SetY1 (-10.);	    peak_line[lc].SetX2 (101.);	    peak_line[lc].SetY2 (-10.); }
        for (ULong64_t i=0;i<gem_peak_count; i++) {
          printf("SRS:: srs_peak: i=%lld id=%d name=%s idx=%d apv=%d Amp=%f wid=%f E=%f Pos=%f \n"
     ,i,gem_peak_plane_id->at(i),gem_peak_plane_name->at(i).c_str(),gem_peak_index->at(i), gem_peak_apv_id->at(i), gem_peak_height->at(i)
     ,gem_peak_width->at(i), gem_peak_area->at(i), gem_peak_real_pos->at(i));
          double pos = gem_peak_real_pos->at(i);
          if (pos<=0) pos=pos+50.; else pos=pos-50.;  pos*=-1.;
          double pos2ch=(ftrk.Eval(pos)+50.)/0.4;  // -- to gemtrd coordinate system
          peak_line[lc].SetX1 (110.);	    peak_line[lc].SetY1 (pos2ch);	    peak_line[lc].SetX2 (190.);	    peak_line[lc].SetY2 (pos2ch);
          printf(" i=%llu pos=%f pos2ch=%f \n ",i,pos,pos2ch);
          if (gem_peak_plane_name->at(i) == "GEMTRKY" ) { if (lc<100) {  peak_line[lc].SetLineColor(kGreen);  peak_line[lc].Draw("same");  lc++;   }   }
        }  //--- peak Loop --
      }
      c0->cd(4); cal_el_evt->Draw("colz");
      c0->cd(8); cal_pi_evt->Draw("colz");
      c0->cd(9); hgemtrkr_peak_xy->Draw("colz");
      c0->cd(10); srs_gem_x->Draw("colz");  ftrk.Draw("same");
      c0->Modified();   c0->Update();
      
      //---------- fiducial area ---
      
      // srs_gemtrd_el->Divide(srs_etrd_beam);
      srs_etrd_ratio = (TH2F*)srs_gemtrd_el->Clone("srs_etrd_ratio");
      //srs_gemtrd_el->Copy((TH2F*)srs_etrd_ratio);
      srs_etrd_ratio->GetXaxis()->SetTitle(" "); srs_etrd_ratio->GetYaxis()->SetTitle(" ");
      srs_etrd_ratio->SetTitle("TR energy norm");  //srs_etrd_ratio->SetStats(1); srs_etrd_ratio->SetMinimum(0.8); srs_etrd_ratio->SetMaximum(1.35);
#if 0
      srs_etrd_ratio->Divide(srs_etrd_beam);
#else
      srs_etrd_ratio->Add(srs_etrd_beam,-1.);
#endif
      c0->cd(11); srs_etrd_ratio->DrawCopy("colz");
      c0->Modified();   c0->Update();
      //c0->cd(11); srs_gemtrd_el->Draw("colz");
      TBox fbox(x_boxcut1,y_boxcut1,x_boxcut2,y_boxcut2);  //---- draw box cut ---
      fbox.SetLineColor(kRed);
      fbox.SetFillStyle(0);
      fbox.SetLineWidth(2);
      fbox.DrawClone();
     //---------
      c0->cd(12); srs_gem_dx->Draw("colz");
      c0->Modified();   c0->Update();
      if (NPRT<1000) sleep(1);
    }
#endif
    //==== Fill Track Hit Info Trees ====
    if (gem_nhit>0) EVENT_VECT_GEM->Fill();
    if (mmg1_nhit>0) EVENT_VECT_MMG1->Fill();
    if (mmg2_nhit>0 && RunNum>3261) EVENT_VECT_MMG2->Fill();
    if (urw_nhit>0 && RunNum<3262 && RunNum>3147) EVENT_VECT_URW->Fill();
    
  } // ================== End of Event Loop  ====================
   
  cout<<"Total events= "<<jentry<<" pre_n_trk_el="<<pre_n_trk_el<<" pre_n_trk_pi="<<pre_n_trk_pi<< "  N_trk_el=" << N_trk_el << " N_trk_pi=" << N_trk_pi <<endl;
  cout<<"hcount values: 1_TRK="<<_1TRK<<" 1eTRK="<<e_1trk<<" 1piTRK="<<pi_1trk<<" eCHR="<<e_CHR<<" elCC="<<el_CC<<" piCC="<<pi_CC<<" nclSRS="<<n_clSRS<<" eCHR_Up="<<e_CHR_Up<<endl;

  //=====================================================================================
  //===                 S A V E   H I S T O G R A M S                                ====
  //=====================================================================================

  TFile* fOut;
  char rootFileName[256]; sprintf(rootFileName, "RootOutput/Run_%06d_Output.root", RunNum);
  fOut = new TFile(rootFileName, "RECREATE");
  fOut->cd();
  HistList->Write("HistDQM", TObject::kSingleKey);
  fOut->Close();
  delete fOut;
  
  //=====================================================================================
  //===                 S A V E   T R A C K   H I T   T T R E E S                    ====
  //=====================================================================================
  
  if (save_hits_root) {
    printf("Writing Hit Info TTree files... \n");
    fHits->cd();
    EVENT_VECT_GEM->Write();
    EVENT_VECT_MMG1->Write();
    if (RunNum>3261) EVENT_VECT_MMG2->Write();
    if (RunNum<3262 && RunNum>3147) EVENT_VECT_URW->Write();
    fHits->Close();
    printf("TTree files written & closed OK \n");
  }
  
  //=====================================================================================
  //===                 P L O T     H I S T O G R A M S                               ===
  //=====================================================================================
  
  const char *OutputDir="RootOutput";
  char ctit[120];
  sprintf(G_DIR,"%s/Run_%06d",OutputDir,RunNum);
  sprintf(ctit,"File=%s",G_DIR);
  bool COMPACT=false;
  TCanvas *cc;
  int nxd=3;
  int nyd=5;
  
  //======= Plot & Save Event Display ========
  //char pngname[120];  sprintf(pngname,"%s_evdisp.png",G_DIR);  //c0->Print(pngname);
  char pdfname[120];  sprintf(pdfname,"%s_evdisp.pdf",G_DIR);  //c0->Print(pdfname);
  
  //--------------------- new page --------------------
  htitle(" Cherenkov (Fadc250)  ");   //if (!COMPACT) cc=NextPlot(0,0);
  
  nxd=2; nyd=3;
  cc=NextPlot(nxd,nyd);  gPad->SetLogy(); hcount->Draw();
  cc=NextPlot(nxd,nyd);  gPad->SetLogy();  hCher_u_adc->Draw();
  cc=NextPlot(nxd,nyd);  gPad->SetLogy();  hCher_dout_adc->Draw();
  cc=NextPlot(nxd,nyd);  hCCor_ud->Draw("colz");

  //--------------------- new page --------------------
  htitle(" GEM-Tracker (SRS) ");   if (!COMPACT) cc=NextPlot(0,0);
  
  cc=NextPlot(nxd,nyd);  hgemtrkr_peak_xy->Draw("colz");
  cc=NextPlot(nxd,nyd);  hgemtrkr_ch_xy->Draw("colz");
  cc=NextPlot(nxd,nyd);  hgemtrkr_peak_x->Draw();
  cc=NextPlot(nxd,nyd);  hgemtrkr_peak_y->Draw();
  cc=NextPlot(nxd,nyd);  mmg1_xy->Draw("colz");
  if (RunNum<3262 && RunNum>3147) {urw_xy->Draw("colz");} else if (RunNum>3261) {mmg2_xy->Draw("colz");}
/*  cc=NextPlot(nxd,nyd);  srs_gemtrd_el->Draw("colz");
  TBox fbox(x_boxcut1,y_boxcut1,x_boxcut2,y_boxcut2);  //---- draw box cut ---
  fbox.Draw("same");
  fbox.SetLineColor(kRed);
  fbox.SetFillStyle(0);
  fbox.SetLineWidth(1);
  cc=NextPlot(nxd,nyd);  srs_etrd_beam->Draw("colz");
  cc=NextPlot(nxd,nyd);  srs_etrd_ratio->Draw("colz");
*/
 //--------------------- new page --------------------
  htitle("  TRD Prototype (Fadc125) Amplitudes ");    if (!COMPACT) cc=NextPlot(0,0);
  nxd=2; nyd=3;
  cc=NextPlot(nxd,nyd);   gPad->SetLogy();   f125_el->Draw();
  cc=NextPlot(nxd,nyd);   gPad->SetLogy();   f125_pi->Draw();
  cc=NextPlot(nxd,nyd);   gPad->SetLogy();   mmg1_f125_el->Draw();
  cc=NextPlot(nxd,nyd);   gPad->SetLogy();   mmg1_f125_pi->Draw();
  cc=NextPlot(nxd,nyd);   gPad->SetLogy();   if (RunNum<3262 && RunNum>3147) {urw_f125_el->Draw();} else if (RunNum>3261) {mmg2_f125_el->Draw();}
  cc=NextPlot(nxd,nyd);   gPad->SetLogy();  if (RunNum<3262 && RunNum>3147) {urw_f125_pi->Draw();} else if (RunNum>3261) {mmg2_f125_pi->Draw();}

 //--------------------- new page --------------------
  htitle("  TRD Prototype (Fadc125) Amplitudes - 2D");    if (!COMPACT) cc=NextPlot(0,0);
  nxd=2; nyd=3;
  cc=NextPlot(nxd,nyd);   f125_el_amp2ds->Draw("colz");
  cc=NextPlot(nxd,nyd);   f125_pi_amp2ds->Draw("colz");
  cc=NextPlot(nxd,nyd);   mmg1_f125_el_amp2ds->Draw("colz");
  cc=NextPlot(nxd,nyd);   mmg1_f125_pi_amp2ds->Draw("colz");
  cc=NextPlot(nxd,nyd);   if (RunNum<3262 && RunNum>3147) {urw_f125_el_amp2ds->Draw("colz");} else if (RunNum>3261) {mmg2_f125_el_amp2ds->Draw("colz");}
  cc=NextPlot(nxd,nyd);   if (RunNum<3262 && RunNum>3147) {urw_f125_pi_amp2ds->Draw("colz");} else if (RunNum>3261) {mmg2_f125_pi_amp2ds->Draw("colz");}
  
  //--------------------- new page --------------------
  htitle(" SRS & TRD Prototypes - GEMTRD Correlations");    if (!COMPACT) cc=NextPlot(0,0);
  
  cc=NextPlot(nxd,nyd);  gem_mmg1_x->Draw("colz");  ftrk.Draw("same");
  if (RunNum<3262 && RunNum>3147) {cc=NextPlot(nxd,nyd); gem_urw_x->Draw("colz"); ftrk.Draw("same");} else if (RunNum>3261) {cc=NextPlot(nxd,nyd); gem_mmg2_x->Draw("colz"); ftrk.Draw("same");}
  cc=NextPlot(nxd,nyd);  gem_mmg1_y->Draw("colz");
  if (RunNum<3262 && RunNum>3147) { cc=NextPlot(nxd,nyd);  gem_urw_y->Draw("colz");} else if (RunNum>3261) { cc=NextPlot(nxd,nyd);  gem_mmg2_y->Draw("colz");}
  cc=NextPlot(nxd,nyd);  hgemtrkr_peak_xy_chi2->Draw("colz");
  cc=NextPlot(nxd,nyd);  srs_gem_dy->Draw("colz");
  
  //--------------------- new page --------------------
  htitle(" SRS & TRD Prototypes - Tracking");   if (!COMPACT) cc=NextPlot(0,0);
  cc=NextPlot(nxd,nyd);  singleTrackIndex->Draw("colz text");
  cc=NextPlot(nxd,nyd);  multiTrackIndex->Draw("colz text");
  cc=NextPlot(nxd,nyd);  srs_gem_dx->Draw("colz"); ftrk.Draw("same");
  cc=NextPlot(nxd,nyd);  srs_mmg1_dx->Draw("colz"); ftrk.Draw("same");
  cc=NextPlot(nxd,nyd);  srs_mmg1_dy->Draw("colz");
  if (RunNum<3262 && RunNum>3147) { cc=NextPlot(nxd,nyd); srs_urw_dx->Draw("colz"); ftrk.Draw("same"); cc=NextPlot(nxd,nyd); srs_urw_dy->Draw("colz");}
  else if (RunNum>3261) { cc=NextPlot(nxd,nyd); srs_mmg2_dx->Draw("colz"); ftrk.Draw("same"); cc=NextPlot(nxd,nyd); srs_mmg2_dy->Draw("colz");}
  
  //--------------------- new page --------------------
  htitle(" SRS & TRD Prototypes - Y Correlations");   if (!COMPACT) cc=NextPlot(0,0);
  
  cc=NextPlot(nxd,nyd);  srs_mmg1_y->Draw("colz"); ftrk.Draw("same");
  cc=NextPlot(nxd,nyd);  srs_mmg1_x->Draw("colz");
  if (RunNum<3262 && RunNum>3147) {
  cc=NextPlot(nxd,nyd);  srs_urw_x->Draw("colz");
  cc=NextPlot(nxd,nyd);  srs_urw_y->Draw("colz"); ftrk.Draw("same");
  cc=NextPlot(nxd,nyd);  mmg1_urw_y->Draw("colz"); ftrk.Draw("same");

  } else if (RunNum>3261) {
  cc=NextPlot(nxd,nyd);  srs_mmg2_x->Draw("colz");
  cc=NextPlot(nxd,nyd);  srs_mmg2_y->Draw("colz"); ftrk.Draw("same");
  }
  //--- close PDF file ----
  cc=NextPlot(-1,-1);
}
//=========== The End ===================
