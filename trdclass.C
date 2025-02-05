#define trdclass_cxx
#include "trdclass.h"
#include "PlotLib.C"
#include "GNN/gnn_model.h"
#include "GNN/gnn_model.cpp"
#include "GNN/toGraph.cpp"

#define NPRT 1000
//#define VERBOSE
#define SAVE_TRACK_HITS
#define MAX_PRINT 10
#define SAVE_PDF
//#define USE_250_PULSE
#define USE_125_RAW
#define MAX_CLUST 500
#define MAX_NODES 100
#define USE_MAXPOS 1
#define USE_GNN  1
#define USE_FIT  1
#define USE_CLUST 1

//-- For single evt clustering display, uncomment BOTH:
//#define SHOW_EVTbyEVT
//#define SHOW_EVT_DISPLAY

//=================================================
//            TRD Prototype DAQ Mapping
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
/*
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
*/

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
//==========  END TRD Prototype DAQ Mapping ===========

void trdclass::Loop() {
  
  if (fChain == 0) return;
//==============================================================
//            B o o k    H i s t o g r a m s
//==============================================================

  TList *HistList = new TList();
  //--- Set TRD Prototype ADC thresholds
  int MM_THR=70;
  if (RunNum>3250) MM_THR=80;
  int URW_THR=100;
  int GEM_THR=100;
  //int GEM_THR=0;
  
//============= Event Display (canvas 0) =============
  #ifdef SHOW_EVT_DISPLAY
    char c0Title[256];
    sprintf(c0Title,"Event_Display_Run=%d",RunNum);
    TCanvas *c0 = new TCanvas("DISP",c0Title,100,100,1500,1300);
    c0->Divide(2,2); c0->cd(1);
    //TLine peak_line[100];
    //============== FPGA Display (canvas 1) ==========
    char c2Title[256];
    sprintf(c2Title,"FPGA_Event_Display_Run=%d",RunNum);
    TCanvas *c2 = new TCanvas("FPGA",c2Title,100,100,1500,1300);
    c2->Divide(5,2); c2->cd(1);
    
    
    //f125_el_evt_display = new TH2F("f125_el_evt_display","GEM-TRD track for Electrons ; Time Response (8ns) ; Channel ",100,100.5,200.5,200,-0.5,249.5);  //HistList->Add(f125_el_evt_display);
    //f125_el_evt_display->SetStats(0);  f125_el_evt_display->SetMinimum(GEM_THR);  f125_el_evt_display->SetMaximum(1000.);
    //f125_pi_evt_display = new TH2F("f125_pi_evt_display","GEM-TRD track for Pions ; Time Response (8ns) ; Channel ",100,100.5,200.5,200,-0.5,249.5);  //HistList->Add(f125_pi_evt_display);
    //f125_pi_evt_display->SetStats(0); f125_pi_evt_display->SetMinimum(GEM_THR); f125_pi_evt_display->SetMaximum(1000.);
  #endif
  hcount= new TH1D("hcount","Count",3,0,3); HistList->Add(hcount);
  hcount->SetStats(0); hcount->SetFillColor(38);  hcount->SetMinimum(1.);
  #if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
    hcount->SetCanExtend(TH1::kXaxis);
  #else
    hcount->SetBit(TH1::kCanRebin);
  #endif
  //TH1F *teff = new TH1F("teff","",528.,-0.5,527.5); HistList->Add(teff);
  hNTracks = new TH1D("hNTracks","Number of Tracks in GEMTRD",12,-0.5,11.5);  HistList->Add(hNTracks);
  hNTracks_e = new TH1D("hNTracks_e","Number of Electron Tracks in GEMTRD",12,-0.5,11.5); HistList->Add(hNTracks_e);
  hNTracks_pi = new TH1D("hNTracks_pi","Number of Pion Tracks in GEMTRD",12,-0.5,11.5); HistList->Add(hNTracks_pi);
  hNHits_el = new TH1D("hNHits_el","Number of Electron Hits in GEMTRD",128,-0.4,98.8);  HistList->Add(hNHits_el);
  hNHits_pi = new TH1D("hNHits_pi","Number of Pion Hits in GEMTRD",128,-0.4,98.8);  HistList->Add(hNHits_pi);
  hNExpected_el = new TH1D("hNExpected_el","Expected Electron Hits in GEMTRD",128,-0.4,98.8);  HistList->Add(hNExpected_el);
  hNExpected_pi = new TH1D("hNExpected_pi","Expected Pion Hits in GEMTRD",128,-0.4,98.8);  HistList->Add(hNExpected_pi);
  trdRatio_el = new TH1D("trdRatio_el","GEMTRD Electron Efficiency",128,-0.4,98.8);  HistList->Add(trdRatio_el);
  trdRatio_pi = new TH1D("trdRatio_pi","GEMTRD Pion Efficiency",128,-0.4,98.8);  HistList->Add(trdRatio_pi);
  
  f125_el_raw = new TH2F("f125_el_raw","GEM-TRD Raw Response for Electrons ; Time Response (8ns) ; X Channel",125,0.5,250.5,240,-0.5,239.5);
  f125_el_raw->SetStats(0);  f125_el_raw->SetMinimum(-1.);   f125_el_raw->SetMaximum(1000.);
  f125_pi_raw = new TH2F("f125_pi_raw","GEM-TRD Raw Response for Pions ; Time Response (8ns) ; X Channel",125,0.5,250.5,240,-0.5,239.5);
  f125_pi_raw->SetStats(0); f125_pi_raw->SetMinimum(-1.); f125_pi_raw->SetMaximum(1000.);
  
  //============ Calorimeter & Cherenkovs Plots ===============
  hCal_occ = new TH1F("hCal_occ","Calorimeter Occupancy; Cal. Cell Number",8,-0.5,7.5);  HistList->Add(hCal_occ);
  hCal_sum = new TH1F("hCal_sum","Calorimeter Sum (GeV); Total Deposited Energy [GeV]",125.,-0.5,24.5); HistList->Add(hCal_sum);
  hCal_sum_el = new TH1F("hCal_sum_el","Calorimeter Sum for electrons; Electron Energy Deposit [GeV]",125,-0.5,24.5); HistList->Add(hCal_sum_el);
  hCal_sum_pi = new TH1F("hCal_sum_pi","Calorimeter Sum for pions; Pion Energy Deposit [GeV]",125,-0.5,24.5); HistList->Add(hCal_sum_pi);
  for (int cc=0; cc<NCAL; cc++) {
    char hName[128];  sprintf(hName,"hCal_adc%d",cc);
    char hTitle[128]; sprintf(hTitle,"Calorimeter ADC Distribution, Cell %d",cc);
    hCal_adc[cc] = new TH1F(hName,hTitle,200,-0.5,4095.5); HistList->Add(hCal_adc[cc]);
    //sprintf(hName,"hCal_cor%d",cc);  sprintf(hTitle,"Correlation GEMTRD X & CAL, Cell %d",cc);
    //hCal_cor[cc] = new TH2F(hName,hTitle,128,-0.4,102.,25,-0.5,24.5);  hCal_cor[cc]->SetMaximum(12000.);  HistList->Add(hCal_cor[cc]);
    //sprintf(hName,"hCal_trk%d",cc);  sprintf(hTitle,"Correlation GEMTRKR & CAL, Cell %d",cc);
    //hCal_trk[cc] = new TH2F(hName,hTitle,128,-0.4,102.,128,-0.4,102.); hCal_trk[cc]->SetMaximum(4.);  HistList->Add(hCal_trk[cc]);
    sprintf(hName,"hCal_cal%d",cc);  sprintf(hTitle,"Calorimeter ADC Calib, Cell %d; Cell Energy; Distance from Cell Center [mm]",cc);
    hCal_cal[cc] = new TH2F(hName,hTitle,112,-1.5,12.5,40,-0.4,15.6);  HistList->Add(hCal_cal[cc]);
    sprintf(hName,"hCal_cell_sum%d",cc);  sprintf(hTitle,"Calorimeter Sum (GeV), Cell %d; Cell Total Energy [GeV]",cc);
    hCal_cell_sum[cc] = new TH1F(hName,hTitle,81,-2.5,24.5);  HistList->Add(hCal_cell_sum[cc]);
  }
  //h250_size = new TH1F("h250_size"," fa250 Raw data size",4096,0.5,4095.5); HistList->Add(h250_size);
  cal_el_evt = new TH2F("cal_el_evt","Calorimeter Electron Event ; X ; Y ",3,-0.5,2.5,3,-0.5,2.5); HistList->Add(cal_el_evt);
  cal_el_evt->SetMinimum(-2.); cal_el_evt->SetMaximum(10.); cal_el_evt->SetStats(0);
  cal_pi_evt = new TH2F("cal_pi_evt","Calorimeter Pion Event ; X ; Y ",3,-0.5,2.5,3,-0.5,2.5); HistList->Add(cal_pi_evt);
  cal_pi_evt->SetMinimum(-2.); cal_pi_evt->SetMaximum(10.); cal_pi_evt->SetStats(0);
/*
  //hCher_din_adc = new TH1F("hCher_din_adc"," Cherenkov Downstream (in) ADC ; ADC Amplitude ",4096,-0.5,4095.5); HistList->Add(hCher_din_adc);
  //hCher_din_time = new TH1F("hCher_din_time"," Cherenkov Downstream (in) Time ; Time Response (8ns)",300,-0.5,299.5); HistList->Add(hCher_din_time);
  //hCCCor_u = new TH2F("hCCCor_u"," Cherenkov Calorimeter Corr ; Upstream ; Calorimeter ",400,0.5,4095.5,400,0.5,4095.5);  HistList->Add(hCCCor_u);
  //hCCCor_dout = new TH2F("hCCCor_dout"," Cherenkov Calorimeter Corr ; Downstream (out) ; Calorimeter ",400,0.5,4095.5,400,0.5,4095.5);  HistList->Add(hCCCor_dout);
  */
  hCher_u_adc = new TH1F("hCher_u_adc"," Cherenkov Upstream ADC ; ADC Amplitude ",400,-0.5,4095.5);  HistList->Add(hCher_u_adc);
  hCher_dout_adc = new TH1F("hCher_dout_adc"," Cherenkov Downstream (out) ADC ; ADC Amplitude ",400,-0.5,4095.5);  HistList->Add(hCher_dout_adc);
  hCher_u_time = new TH1F("hCher_u_time"," Cherenkov Upstream Time ; Time Response (8ns)",300,-0.5,299.5);  HistList->Add(hCher_u_time);
  hCher_dout_time = new TH1F("hCher_dout_time"," Cherenkov Downstream (out) Time ; Time Response (8ns)",300,-0.5,299.5);  HistList->Add(hCher_dout_time);
  
  // ============= Detector Correlation Plots =============
  hCCor_ud = new TH2F("hCCor_ud"," Cherenkov Upstr./Downstr. Corr ; Upstream ; Downstream (out) ",400,-0.5,4095.5,400,0.5,4095.5);  HistList->Add(hCCor_ud);
  
  //-- GEM-TRKR & Prototype Correlations
  srs_mmg1_xy = new TH2F("srs_mmg1_xy","MMG1 Y & GEMTRKR X Hit Display; GEMTRKR Peak X [mm]; MMG-1 Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_mmg1_xy);
  srs_mmg1_y = new TH2F("srs_mmg1_y","Correlation MMG1 & GEMTRKR Y; GEMTRKR Peak Y [mm]; MMG-1 Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_mmg1_y);
  srs_mmg1_x = new TH2F("srs_mmg1_x","Correlation MMG1 & GEMTRKR X; GEMTRKR Peak X [mm]; MMG-1 X [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_mmg1_x);
  if (RunNum>3147 && RunNum<3262) {
    srs_urw_xy = new TH2F("srs_urw_xy","uRWell Y & GEMTRKR X Hit Display; GEMTRKR Peak X [mm]; uRWell Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_urw_xy);
    srs_urw_y = new TH2F("srs_urw_y","Correlation uRWell & GEMTRKR Y; GEMTRKR Peak Y [mm]; uRWell Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_urw_y);
    srs_urw_x = new TH2F("srs_urw_x","Correlation uRWell & GEMTRKR X; GEMTRKR Peak X [mm]; uRWell X [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_urw_x);
    //-- GEM-TRD & Other TRD Prototypes fADC Correlations
    gem_urw_x = new TH2F("gem_urw_x","Correlation GEMTRD & uRWell X ; GEMTRD X [mm]; uRWell X [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(gem_urw_x);
    mmg1_urw_x = new TH2F("mmg1_urw_x","Correlation MMG1 & uRWell X ; MMG-1 X [mm]; uRWell X [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(mmg1_urw_x);
    urw_xy = new TH2F("urw_xy","uRWellTRD X&Y Hit Display; uRWell X [mm] ; uRWell Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(urw_xy);
    mmg1_urw_y = new TH2F("mmg1_urw_y","Correlation MMG1 & uRWell Y ; MMG-1 Y(SRS) [mm]; uRWell Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(mmg1_urw_y);
  }
  gem_mmg1_x = new TH2F("gem_mmg1_x","Correlation GEMTRD & MMG1 X ; GEMTRD X [mm]; MMG-1 X [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(gem_mmg1_x);
  mmg1_xy = new TH2F("mmg1_xy","MMG1TRD X&Y Hit Display; MMG-1 X [mm] ; MMG-1 Y(SRS) [mm]",128,-0.4,102.,128,-0.4,102.);            HistList->Add(mmg1_xy);
  gem_urw_corr = new TH2F("gem_urw_corr","GEMTRD X & uRWell Y Hit Display; GEMTRD X [mm]; uRWell Y (SRS) [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(gem_urw_corr);
  srs_gem_x = new TH2F("srs_gem_x","Correlation GEMTRD & GEMTRKR X; GEMTRKR X [mm]; GEMTRD X [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_gem_x);
  srs_gem_xy = new TH2F("srs_gem_xy","GEMTRD X & GEMTRKR Y Hit Display; GEMTRD X [mm]; GEMTRKR Y [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(srs_gem_xy);
  mmg1_srs_xy = new TH2F("mmg1_srs_xy","MMG1 X & GEMTRKR Y Hit Display; MMG-1 X [mm]; GEMTRKR Y [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(mmg1_srs_xy);
  urw_srs_xy = new TH2F("urw_srs_xy","uRWell X & GEMTRKR Y Hit Display; uRWell X [mm]; GEMTRKR Y [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(urw_srs_xy);
  //-- GEM-TRKR & PID/Beam Correlations
  srs_cal_corr = new TH2F("srs_cal_corr","Correlation GEMTRKR & CAL, All Cells; GEMTRKR X [mm]; GEMTRKR Y [mm]",128,-0.4,102.,128,-0.4,102.); srs_cal_corr->SetMaximum(4.);  HistList->Add(srs_cal_corr);
  urw_cal_corr = new TH2F("urw_cal_corr","Correlation uRWell & CAL, All Cells; uRWell X [mm]; uRWell Y [mm]",128,-0.4,102.,128,-0.4,102.);   HistList->Add(urw_cal_corr);
  mmg1_cal_corr = new TH2F("mmg1_cal_corr","Correlation MMG1 & CAL, All Cells; MMG-1 X [mm]; MMG-1 Y [mm]",128,-0.4,102.,128,-0.4,102.);   HistList->Add(mmg1_cal_corr);
  srs_gemtrd_el = new TH2F("srs_gemtrd_el","Correlation uRWell & GEMTRD Electron Hit; uRWell X [mm]; uRWell Y [mm]",128,-0.4,102.,128,-0.4,102.);  HistList->Add(srs_gemtrd_el);
  srs_etrd_beam = new TH2F("srs_etrd_beam","Correlation uRWell & Beam; uRWell X [mm]; uRWell Y [mm]",128,-0.4,102.,128,-0.4,102.); HistList->Add(srs_etrd_beam);
  srs_gemtrd_pion = new TH2F("srs_gemtrd_pion","Correlation uRWell & GEMTRD Pion Hit; uRWell X [mm]; uRWell Y [mm]",128,-0.4,102.,128,-0.4,102.);  HistList->Add(srs_gemtrd_pion);
  hradshadow = new TH2F("hradshadow","GEMTRKR Y Hits & GEMTRD Time; GEMTRD Time(*8ns); GEMTRKR Y [mm]",140,79.5,219.5,128,-0.4,102.); HistList->Add(hradshadow);
  hradshadow_u = new TH2F("hradshadow_u","uRWell Y Hits & GEMTRD Time; GEMTRD Time(*8ns); uRWell Y [mm]",140,79.5,219.5,128,-0.4,102.); HistList->Add(hradshadow_u);
  hradshadow_m = new TH2F("hradshadow_m","MMG1TRD Y Hits & GEMTRD Time; GEMTRD Time(*8ns); MMG-1 Y [mm]",140,79.5,219.5,128,-0.4,102.); HistList->Add(hradshadow_m);
  hradshadow_ux = new TH2F("hradshadow_ux","uRWell X Hits & GEMTRD Time; GEMTRD Time(*8ns); uRWell X [mm]",140,79.5,219.5,128,-0.4,102.); HistList->Add(hradshadow_ux);
  //srs_etrd_ratio = new TH2F("srs_etrd_ratio","Correlation TRK ratio; X ; Y ",100,-55.,55.,100,-55.,55.);         HistList->Add(srs_etrd_ratio);
  
  //=============== Track Fitting & chi^2 ==================
  gErrorIgnoreLevel = kBreak; // Suppress warning messages from empty chi^2 fit data
  TF1 fx("fx","pol1",100,190); //-- Linear function fit (ax+b) over time response window
  TF1 fx1("fx1","pol1",100,190);
  TF1 fx2("fx2","pol1",100,190);
  TF1 fx_mmg1("fx_mmg1","pol1",80,190);
  TF1 fx_urw("fx_urw","pol1",80,190);
  f125_fit = new TH2F("f125_fit","GEM-TRD Track Fit; Time Response (8ns) ; X Channel",200,0.5,200.5,240,-0.5,239.5);
  //-- GEMTRD & GEMTRKR alignment
  double x_1=20., y_1=20., x_2=80., y_2=80.;
  double a_slope=(y_2-y_1)/(x_2-x_1);
  double b_intercept=y_1-a_slope*x_1;
  double x_cut1=-0.5, x_cut2=99.5, y_cut1=-0.5, y_cut2=99.5;
  TF1 ftrk("ftrk","[0]*x+[1]",1.,99.);
  ftrk.SetParameter(0,a_slope);
  ftrk.SetParameter(1,b_intercept);
  ftrk.SetLineWidth(1);
  
  //======== GEM-TRKR ========
  singleTrackIndex = new TH2F("singleTrackIndex","GEMTracker Correlated Hits with 1 Track; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);      HistList->Add(singleTrackIndex);
  singleTrackIndex_e = new TH2F("singleTrackIndex_e","GEMTracker Correlated Electrons with 1 Track; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);    HistList->Add(singleTrackIndex_e);
  singleTrackIndex_pi = new TH2F("singleTrackIndex_pi","GEMTracker Pions Hits with 1 Track; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);    HistList->Add(singleTrackIndex_pi);
  multiTrackIndex = new TH2F("multiTrackIndex","GEMTracker Correlated Hits with 2 Tracks; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);    HistList->Add(multiTrackIndex);
  multiTrackIndex_e = new TH2F("multiTrackIndex_e","GEMTracker Correlated Electrons with 2 Tracks; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);    HistList->Add(multiTrackIndex_e);
  multiTrackIndex_pi = new TH2F("multiTrackIndex_pi","GEMTracker Correlated Pions with 2 Tracks; GEMTRKR X Num Hits; GEMTRKR Y Num Hits",10,-0.5,9.5,10,-0.5,9.5);    HistList->Add(multiTrackIndex_pi);
  
  hgemtrkr_peak_xy = new TH2F("hgemtrkr_peak_xy","GEM-TRKR Peak X-Y Correlation (mm); Peak X [mm]; Peak Y [mm] ",128,-0.4,102.,128,-0.4,102.);    HistList->Add(hgemtrkr_peak_xy);
  hgemtrkr_peak_x = new TH1F("hgemtrkr_peak_x","GEM-TRKR Peak X; X [mm]",128,-0.4,102.);  HistList->Add(hgemtrkr_peak_x);
  hgemtrkr_peak_x_height = new TH1F("hgemtrkr_peak_x_height","GEM-TRKR Peak X Amp; ADC Value",100,0.,4096.);  HistList->Add(hgemtrkr_peak_x_height);
  hgemtrkr_peak_y = new TH1F("hgemtrkr_peak_y","GEM-TRKR Peak Y; Y [mm]",128,-0.4,102.);  HistList->Add(hgemtrkr_peak_y);
  hgemtrkr_peak_y_height = new TH1F("hgemtrkr_peak_y_height","GEM-TRKR Peak Y Amp; ADC Value",100,0.,4096.);  HistList->Add(hgemtrkr_peak_y_height);
  hgemtrkr_max_xch = new TH1F("hgemtrkr_max_xch","GEM-TRKR Max X Hit Position; X [mm]",128,-0.4,102.);  HistList->Add(hgemtrkr_max_xch);
  hgemtrkr_max_xamp = new TH1F("hgemtrkr_max_xamp","GEM-TRKR Max X Hit Distribution; ADC Value ",100,0.,4096.);  HistList->Add(hgemtrkr_max_xamp);
  hgemtrkr_max_ych = new TH1F("hgemtrkr_max_ych","GEM-TRKR Max Y Hit Position; Y [mm]",128,-0.4,102.);  HistList->Add(hgemtrkr_max_ych);
  hgemtrkr_max_yamp = new TH1F("hgemtrkr_max_yamp","GEM-TRKR Max Y Hit Distribution ; ADC Value ",100,0.,4096.);  HistList->Add(hgemtrkr_max_yamp);
  hgemtrkr_max_xy = new TH2F("hgemtrkr_max_xy","GEM-TRKR Max Pulse X-Y Correlation; Max X [mm]; Max Y [mm]",128,-0.4,102.,128,-0.4,102.);    HistList->Add(hgemtrkr_max_xy);
  hmmg1_peak_y = new TH1F("hmmg1_peak_y","MMG1 Peak Y Position (SRS); Y [mm]",128,-0.4,102.);  HistList->Add(hmmg1_peak_y);
  hmmg1_peak_y_height = new TH1F("hmmg1_peak_y_height","MMG1 Peak Y Amp (SRS); ADC Value",100,0.,4096.);  HistList->Add(hmmg1_peak_y_height);
  hmmg1_peak_y_height_el = new TH1F("hmmg1_peak_y_height_el","MMG1 Peak Y Amp for Electrons (SRS); ADC Value",100,0.,4096.);  HistList->Add(hmmg1_peak_y_height_el);
  hmmg1_peak_y_height_pi = new TH1F("hmmg1_peak_y_height_pi","MMG1 Peak Y Amp for Pions (SRS); ADC Value",100,0.,4096.);  HistList->Add(hmmg1_peak_y_height_pi);
  hurw_peak_y = new TH1F("hurw_peak_y","URW Peak Y Position (SRS); Y [mm]",128,-0.4,102.);  HistList->Add(hurw_peak_y);
  hurw_peak_y_height = new TH1F("hurw_peak_y_height","URW Peak Y Amp (SRS); ADC Value",100,0.,4096.);  HistList->Add(hurw_peak_y_height);
  hurw_peak_y_height_el = new TH1F("hurw_peak_y_height_el","URW Peak Y Amp for Electrons (SRS); ADC Value",100,0.,4096.);  HistList->Add(hurw_peak_y_height_el);
  hurw_peak_y_height_pi = new TH1F("hurw_peak_y_height_pi","URW Peak Y Amp for Pions (SRS); ADC Value",100,0.,4096.);  HistList->Add(hurw_peak_y_height_pi);
  hchan_u_el = new TH1F("hchan_u_el","URW Max X Position for Electrons; uRWell X Max [mm]",128,-0.4,102.);  HistList->Add(hchan_u_el);
  hchan_u_pi = new TH1F("hchan_u_pi","URW Max X Position for Pions; uRWell X Max [mm]",128,-0.4,102.);  HistList->Add(hchan_u_pi);
  
  //============= Prototype ADC Amplitude Distributions ============
  f125_el = new TH1F("f125_el","GEM-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096);  HistList->Add(f125_el);
  f125_pi = new TH1F("f125_pi","GEM-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096);  HistList->Add(f125_pi);
  f125_el_max = new TH1F("f125_el_max","GEM-TRD f125 Max Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(f125_el_max);
  f125_pi_max = new TH1F("f125_pi_max","GEM-TRD f125 Max Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(f125_pi_max);
  mmg1_f125_el = new TH1F("mmg1_f125_el","MMG1-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(mmg1_f125_el);
  mmg1_f125_pi = new TH1F("mmg1_f125_pi","MMG1-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(mmg1_f125_pi);
  mmg1_f125_el_max = new TH1F("mmg1_f125_el_max","MMG1-TRD f125 Max Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(mmg1_f125_el_max);
  mmg1_f125_pi_max = new TH1F("mmg1_f125_pi_max","MMG1-TRD f125 Max Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(mmg1_f125_pi_max);
  if (RunNum>3147 && RunNum<3262) {
    urw_f125_el = new TH1F("urw_f125_el","uRWell-TRD f125 Peak Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(urw_f125_el);
    urw_f125_pi = new TH1F("urw_f125_pi","uRWell-TRD f125 Peak Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(urw_f125_pi);
    urw_f125_el_max = new TH1F("urw_f125_el_max","uRWell-TRD f125 Max Amp for Electrons ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(urw_f125_el_max);
    urw_f125_pi_max = new TH1F("urw_f125_pi_max","uRWell-TRD f125 Max Amp for Pions ; ADC Amplitude ; Counts ",100,0.,4096); HistList->Add(urw_f125_pi_max);
  }
  
  //-- Prototype ADC Amplitudes in Time (2D) for Single-Track Evts Only
  f125_el_amp2ds = new TH2F("f125_el_amp2ds","GEM-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; GEMTRD Channel (X)",250,0.5,250.5,240,-0.5,239.5);  HistList->Add(f125_el_amp2ds);
  f125_pi_amp2ds = new TH2F("f125_pi_amp2ds","GEM-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; GEMTRD Channel (X)",250,0.5,250.5,240,-0.5,239.5);      HistList->Add(f125_pi_amp2ds);
  mmg1_f125_el_amp2ds = new TH2F("mmg1_f125_el_amp2ds","MMG1-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; MMG1 Channel (X)",250,0.5,250.5,240,-0.5,239.5);  HistList->Add(mmg1_f125_el_amp2ds);
  mmg1_f125_pi_amp2ds = new TH2F("mmg1_f125_pi_amp2ds","MMG1-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; MMG1 Channel (X)",250,0.5,250.5,240,-0.5,239.5);      HistList->Add(mmg1_f125_pi_amp2ds);
  if (RunNum>3147 && RunNum<3262) {
    urw_f125_el_amp2ds = new TH2F("urw_f125_el_amp2ds","uRWell-TRD Single-Trk Amp. in Time (Electrons); Time Response (8ns) ; uRWell Channel (X)",250,0.5,250.5,120,-0.5,119.5);  HistList->Add(urw_f125_el_amp2ds);
    urw_f125_pi_amp2ds = new TH2F("urw_f125_pi_amp2ds","uRWell-TRD Single-Trk Amp. in Time (Pions); Time Response (8ns) ; uRWell Channel (X)",250,0.5,250.5,120,-0.5,119.5);      HistList->Add(urw_f125_pi_amp2ds);
  }
  
  //-- Prototype Single-Track Amplitudes in Time (for NN)
  gem_zHist = new  TH1F("gem_zHist", "gem_zHist", 20, 80., 200.);
  mmg1_zHist = new  TH1F("mmg1_zHist", "mmg1_zHist", 20, 80., 200.);
  urw_zHist = new  TH1F("urw_zHist", "urw_zHist", 20, 80., 200.);
  
  //---  Clustering Histograms (Pattern Recognition Track-Finding) ---
  int nx0=100;    int ny0=256;
  hevt  = new TH2F("hevt","GEM Event Display; z pos [mm]; y pos [mm]",nx0,0.,+30.,ny0,-0.4,102.); hevt->SetStats(0); hevt->SetMaximum(10.);
  hevtc = new TH2F("hevtc","GEM Clustering; FADC bins; GEM X strips",nx0,-0.5,nx0-0.5,ny0,-0.5,ny0-0.5);  hevtc->SetStats(0);   hevtc->SetMinimum(0.07); hevtc->SetMaximum(40.);
  hevtf = new TH2F("hevtf","GEM Clusters for FPGA; z pos [mm]; y pos,mm ",nx0,0.,+30.,ny0,-0.4,102.);  hevtf->SetStats(0); hevtf->SetMaximum(10.);
  hClusterMaxdEdx_e = new TH1F("hClusterMaxdEdx_e","Maximum Cluster Energy for Single Track Events (Electrons); Cluster Energy ; Counts ",100,0.,4960); HistList->Add(hClusterMaxdEdx_e);
  hClusterMaxdEdx_pi = new TH1F("hClusterMaxdEdx_pi","Maximum Cluster Energy for Single Track Events (Pions); Cluster Energy ; Counts ",100,0.,4960); HistList->Add(hClusterMaxdEdx_pi);
  hClusterTotaldEdx_e = new TH1F("hClusterTotaldEdx_e","Total Cluster Energy for Single Track Events (Electrons); Total Cluster Energy ; Counts ",100,0.,4960); HistList->Add(hClusterTotaldEdx_e);
  hClusterTotaldEdx_pi = new TH1F("hClusterTotaldEdx_pi","Total Cluster Energy for Single Track Events (Pions); Total Cluster Energy ; Counts ",100,0.,4960); HistList->Add(hClusterTotaldEdx_pi);
  
  //============================ End Histogram Booking =====================================
  
  //--- Calorimeter Calibration
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
  
  // ----- Declare Beam Energy -----
  double Ebeam=10.; // GeV
  if  (3131 <= RunNum && RunNum <= 3171) {    Ebeam=10.;   }   // NEG 10 GeV
  if  (3172 <= RunNum && RunNum <= 3176) {    Ebeam=120.;  }   // protons
  if  (3177 <= RunNum && RunNum <= 3210) {    Ebeam=10.;   }   //  NEG 10 GeV
  if  (3211 <= RunNum && RunNum <= 3219) {    Ebeam=3.;    }   // NEG 3 GeV
  if  (3220 <= RunNum && RunNum <= 3226) {    Ebeam=120.;  }   // protons
  if  (3227 <= RunNum && RunNum <= 3227) {    Ebeam=10.;   }   // POS 10 GeV
  if  (3228 <= RunNum && RunNum <= 3240) {    Ebeam=120.;  }   // protons
  if  (3241 <= RunNum && RunNum <= 3253) {    Ebeam=3.;    }   // NEG 3GeV
  if  (3255 <= RunNum && RunNum <= 3261) {    Ebeam=120.;  }   // protons
  if  (3272 <= RunNum && RunNum <= 3288) {    Ebeam=10.;   }   // NEG 10 GeV
  if  (3289 <= RunNum && RunNum <= 3290) {    Ebeam=120.;  }   // protons
  double Ebeam_el= 0.35*Ebeam; /////0.2*Ebeam;
  double Ebeam_pi= 0.1*Ebeam; /////0.1*Ebeam;
  
  //=======================================================
  //     Create TTrees of Single Track Evt Info for NN
  //=======================================================
  
  TFile* fHits;
  #ifdef SAVE_TRACK_HITS
    #if ANALYZE_MERGED
      char hitsFileName[256]; sprintf(hitsFileName, "RootOutput/fermiMerged/trd_singleTrackHits_Run_%06d_%06dEntries.root",RunNum,nEntries);
    #else
      char hitsFileName[256]; sprintf(hitsFileName, "RootOutput/trd_singleTrackHits_Run_%06d.root", RunNum);
    #endif
    fHits = new TFile(hitsFileName, "RECREATE");
    //-- GEM-TRD
    EVENT_VECT_GEM = new TTree("gem_hits","GEM TTree with single track hit info");
    EVENT_VECT_GEM->Branch("event_num",&event_num,"event_num/I");
    EVENT_VECT_GEM->Branch("nhit",&gem_nhit,"gem_nhit/I");
    EVENT_VECT_GEM->Branch("nclu",&gem_nclu,"gem_nclu/I");
    EVENT_VECT_GEM->Branch("xpos",&gem_xpos);
    EVENT_VECT_GEM->Branch("zpos",&gem_zpos);
    EVENT_VECT_GEM->Branch("dedx",&gem_dedx);
    EVENT_VECT_GEM->Branch("parID",&gem_parID);
    EVENT_VECT_GEM->Branch("zHist",&gem_zHist_vect);
    EVENT_VECT_GEM->Branch("xposc",&clu_xpos);
    EVENT_VECT_GEM->Branch("zposc",&clu_zpos);
    EVENT_VECT_GEM->Branch("dedxc",&clu_dedx);
    EVENT_VECT_GEM->Branch("widthc",&clu_width);
    EVENT_VECT_GEM->Branch("xposc_max",&clu_xpos_max,"clu_xpos_max/f");
    EVENT_VECT_GEM->Branch("zposc_max",&clu_zpos_max,"clu_zpos_max/f");
    EVENT_VECT_GEM->Branch("dedxc_max",&clu_dedx_max,"clu_dedx_max/f");
    EVENT_VECT_GEM->Branch("widthc_max",&clu_width_max,"clu_width_max/f");
    
    //-- MMG1-TRD
    EVENT_VECT_MMG1 = new TTree("mmg1_hits","MMG1 TTree with single track hit info");
    EVENT_VECT_MMG1->Branch("event_num",&event_num,"event_num/I");
    EVENT_VECT_MMG1->Branch("nhit",&mmg1_nhit,"mmg1_nhit/I");
    //EVENT_VECT_MMG1->Branch("nclu",&mmg1_nclu,"mmg1_nclu/I");
    EVENT_VECT_MMG1->Branch("xpos",&mmg1_xpos);
    EVENT_VECT_MMG1->Branch("zpos",&mmg1_zpos);
    EVENT_VECT_MMG1->Branch("dedx",&mmg1_dedx);
    EVENT_VECT_MMG1->Branch("parID",&mmg1_parID);
    EVENT_VECT_MMG1->Branch("zHist",&mmg1_zHist_vect);
    
    
    if (RunNum>3147 && RunNum<3262) {
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
  #endif
  
  TStopwatch timer;
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes=0, nb=0;
  if (MaxEvt>0) nentries=MaxEvt;  //-- limit number of events for test
  Long64_t jentry=0;
  
  //===================================================================================
  //                      E v e n t    L o o p
  //===================================================================================
  
  int e_1TRK=0, pi_1TRK=0, _1TRK=0, pi_CC=0, el_CC=0, fid_OK=0;
  
  printf("=============== Begin Event Loop: 1st Evt=%lld, Last Evt=%lld ================ \n",FirstEvt,MaxEvt);
  timer.Start();
  
  for (jentry=FirstEvt; jentry<nentries; jentry++) {
    
    event_num = jentry;
    Count("EVT");
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);
    nbytes += nb;
    if (!(jentry%NPRT)) {
      printf("------- evt=%llu  f125_raw_count=%llu f125_pulse_count=%llu f250_wraw_count=%llu, srs_peak_count=%llu \n", jentry, f125_wraw_count, f125_pulse_count, f250_wraw_count, gem_peak_count);
    }
    
//==================================================================
//                  E v e n t    D i s p l a y
//==================================================================
    #ifdef SHOW_EVT_DISPLAY
      #ifdef VERBOSE
      if (jentry<MAX_PRINT) {
        printf("-------------------- Pulse  125  ---------------------------\n");
        for (ULong64_t i=0;i<f125_pulse_count; i++) {
          printf("F125:pulse: i=%lld  sl=%d, ch=%d, npk=%d time=%d amp=%d ped=%d \n"
           ,i,f125_pulse_slot->at(i),f125_pulse_channel->at(i),f125_pulse_npk->at(i)
           ,f125_pulse_peak_time->at(i),f125_pulse_peak_amp->at(i),f125_pulse_pedestal->at(i));
        }
        printf("-------------------- Pulse  250  n=%lld ---------------------------\n",f250_pulse_count);
        for (ULong64_t i=0;i<f250_pulse_count; i++) {
          printf("F250:pulse: i=%lld  sl=%d, ch=%d,  npk=%d  time=%d amp=%d ped=%f \n"
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
        printf("SRS:gem_scluster:  cnt=%lld \n",gem_scluster_count);
        for (ULong64_t i=0;i<gem_scluster_count; i++) { // --- SRS cluster loop
          printf("SRS:clusters:  i=%lld  X=%f Y=%f  \n",i,gem_scluster_x->at(i), gem_scluster_y->at(i));
        }
        printf("SRS:srs_prerecon:  cnt=%lld cnt_x=%ld cnt_y=%ld \n",srs_prerecon_count, srs_prerecon_x->size(), srs_prerecon_y->size() );
        for (ULong64_t i=0;i<srs_prerecon_count; i++) {
          printf("SRS:srs_prerecon: i=%lld %f %f \n",i,srs_prerecon_x->at(i),srs_prerecon_y->at(i));
        }
        printf("SRS:srs_peak:  cnt=%lld \n",gem_peak_count);
        for (ULong64_t i=0;i<gem_peak_count; i++) {
    	    printf("SRS:srs_peak: i=%lld id=%d name=%s idx=%d apv=%d Amp=%f wid=%f E=%f Pos=%f \n"
           ,i,gem_peak_plane_id->at(i),gem_peak_plane_name->at(i).c_str(),gem_peak_index->at(i), gem_peak_apv_id->at(i), gem_peak_height->at(i)
           ,gem_peak_width->at(i), gem_peak_area->at(i), gem_peak_real_pos->at(i));
        }
      }
      #endif
    #endif
    
//====================================================================
//       Process Fa250 Pulse data (Calorimeter & Cherenkovs)
//====================================================================
    
    #ifdef USE_250_PULSE
      #ifdef VERBOSE
      printf("-------------------- Pulse  250  n=%lld ---------------------------\n",f250_pulse_count);
      for (int i=0;i<f250_pulse_count; i++) {
        printf("F250:: i=%d  sl=%d, ch=%d,  npk=%d  time=%d amp=%d ped=%f \n", i, f250_pulse_slot->at(i), f250_pulse_channel->at(i), f250_pulse_pulse_number->at(i), f250_pulse_course_time->at(i), f250_pulse_pulse_peak->at(i), f250_pulse_pedestal->at(i)/4.);
      }
      #endif
      //h250_size->Fill(f250_wraw_count);
    #endif
    
    double CalSum=0.;
    double Ch_u=0.;
    double Ch_out=0.;
    bool electron_chUp=false;
    bool electron_ch=false;
    bool electron=false;
    bool pion=false;
    double Ecal[NCAL]; for (int i=0; i<NCAL; i++) Ecal[i]=0;
    double CellSum[NCAL]; for (int i=0; i<NCAL; i++) CellSum[i]=0;
    double maxDeposit=0.;
    
    for (ULong64_t i=0; i<f250_wraw_count; i++) {
      int fadc_chan = f250_wraw_channel->at(i);
      int fadc_window = f250_wraw_samples_count->at(i);
      hCal_occ->Fill(fadc_chan+0.);
      int amax=0;
      int tmax=9;
      
      //--ped calculation for f250 raw data
      int nped = 0, ped=100;
      double ped_sum = 0.;
      if (fadc_window > 15) {
        for (int si=20; si<35; si++) {
          int ped_samp = f250_wraw_samples->at(f250_wraw_samples_index->at(i)+si);
          ped_sum += ped_samp;
          nped++;
        }
      }
      if (nped>0.) ped = ped_sum / nped;
      if (0. > ped || ped > 200 ) ped = 100;
      ///////////Include ped subtraction here???
      for (int si=0; si<fadc_window; si++) {
        int adc = f250_wraw_samples->at(f250_wraw_samples_index->at(i)+si);
        if (adc>amax) {
          amax=adc;
          tmax=si;
        }
      } //--  end of fADC 250 samples loop
      double gaus_mean=1.;
      if (fadc_chan<NCAL) { //-- Cal Energy Sum
        Ecal[fadc_chan]=fcal[fadc_chan]->Eval(amax);
        //-- NEW FINER CALIBRATION
        switch (fadc_chan) {
        case (0): gaus_mean=9.66; break;
        case (1): gaus_mean=9.66; break;
        case (2): gaus_mean=9.66; break;
        case (3): gaus_mean=9.87; break;
        case (4): gaus_mean=8.83; break;
        case (5): gaus_mean=8.93; break;
        case (6): gaus_mean=9.66; break;
        }
        Ecal[fadc_chan] = Ecal[fadc_chan]*(9.66/gaus_mean);
        hCal_adc[fadc_chan]->Fill(amax);
        CalSum+=Ecal[fadc_chan];
        CellSum[fadc_chan]+=Ecal[fadc_chan];
        
      } else { // Cherenkov
        if (fadc_chan==13) { if(amax>130)electron_chUp=true; hCher_u_adc->Fill(amax);  hCher_u_time->Fill(tmax); Ch_u=amax; Count("eCHR_Up"); }
        if (fadc_chan==15) { if(amax>300)electron_ch=true; hCher_dout_adc->Fill(amax);  hCher_dout_time->Fill(tmax); Ch_out=amax; Count("eCHR"); }
      }
    } //--- End of Fadc250 Channel Loop
    
    //=======================================================
    //                   S e t    P I D
    //=======================================================
    for (int cc=0; cc<NCAL; cc++) {
      if (CellSum[cc]>0.) hCal_cell_sum[cc]->Fill(CellSum[cc]);
      if (Ecal[cc]>maxDeposit) maxDeposit=Ecal[cc];
    }
    if (maxDeposit==Ecal[4]) { Ebeam_el=0.5*Ebeam; } else { Ebeam_el=0.35*Ebeam; }
    if (CalSum>0.) {Count("calSum");}
    if (CalSum>=Ebeam_el) {Count("calSumEl");}
    if (CalSum<Ebeam_pi && CalSum>0.) {Count("calSumPi");}
    
    //--- CHERENKOV ONLY PID
    //if (electron_ch  && electron_chUp) { electron=true;  Count("el_CC"); el_CC++;}
    //if (!electron_ch && !electron_chUp) { pion=true;  Count("piCC"); pi_CC++;}

    //--- CHERENKOV & CALORIMETER PID
    if (electron_ch  && CalSum>Ebeam_el) { electron=true;  Count("elCC"); el_CC++;}
    if (!electron_ch && CalSum<=Ebeam_pi) { pion=true;  Count("piCC"); pi_CC++;}
    
    if (electron || pion) hCal_sum->Fill(CalSum);
    if (electron) {
      hCal_sum_el->Fill(CalSum);
    } else if (pion) {
      hCal_sum_pi->Fill(CalSum);
    }
    
    if (!electron && !pion) continue;
    Count("PID_OK");
    
    //============ END Process Fa250 Pulse data (Calorimeter & Cherenkovs) =============
    
    f125_fit->Reset();
    
    #ifdef SHOW_EVT_DISPLAY
      if(electron) {
        //f125_el_evt_display->Reset();
        //f125_el_raw->Reset();
        cal_el_evt->Reset();
        for (int cc=0; cc<NCAL; cc++) {
          int ix=cc%3; int iy=cc/3;
          if (cc<6) cal_el_evt->Fill(ix,iy,Ecal[cc]); else cal_el_evt->Fill(1,2,Ecal[cc]);
        }
      } else if (pion) {
        //f125_pi_evt_display->Reset();
        //f125_pi_raw->Reset();
        cal_pi_evt->Reset();
        for (int cc=0; cc<NCAL; cc++) {
          if (cc<6) cal_pi_evt->Fill(cc%3,cc/3,Ecal[cc]); else cal_pi_evt->Fill(1,2,Ecal[cc]);
        }
      }
    #endif
    
    //=====================================================
    //        Process Fa125 RAW data
    //=====================================================
    
    gem_nclu=0;
    clu_xpos.clear();
    clu_zpos.clear();
    clu_dedx.clear();
    clu_width.clear();
    clu_xpos_max=0;
    clu_zpos_max=0;
    clu_dedx_max=0;
    clu_width_max=0;
    
    #ifdef USE_125_RAW
      
      //==============================================================
      //           GEMTRD Pattern Recognition Tracking
      //==============================================================
      
      int DEDX_THR = GEM_THR;
      hevt->Reset();
      hevtc->Reset();
      hevtf->Reset();
      
      for (ULong64_t i=0; i<f125_wraw_count; i++) { // --- fadc125 channels loop
        
        int fadc_window = f125_wraw_samples_count->at(i);
        int fADCSlot = f125_wraw_slot->at(i);
        int fADCChan = f125_wraw_channel->at(i);
        int gemChan = GetGEMChan(fADCChan, fADCSlot);
        int TimeWindowStart = 95;
        //--ped calculation for f125 raw data
        int nped = 0, ped = 100;
        double ped_sum = 0.;
        for (int si=TimeWindowStart-15; si<TimeWindowStart; si++) {
          int ped_samp = f125_wraw_samples->at(f125_wraw_samples_index->at(i)+si);
          ped_sum += ped_samp;
          nped++;
        }
        ped = ped_sum / nped;
        if (0. > ped || ped > 200 ) ped = 100;
        
        for (int si=0; si<fadc_window; si++) {
          int time=si;
          int adc = f125_wraw_samples->at(f125_wraw_samples_index->at(i)+si);
          adc = adc - ped;
          if (adc>4090) printf("!!!!!!!!!!!!!!!!!!!!!! ADC 125 overflow: %d \n",adc);
          if (adc>DEDX_THR) {
            if (gemChan>-1) {
              //if (electron) {
              //  f125_el_raw->Fill(time,gemChan,adc);
              //} else if (pion) {
              //  f125_pi_raw->Fill(time,gemChan,adc);
              //}
              time-=TimeWindowStart;
          	  if ( 0 > time || time > 100 ) continue; // --- drop early and late hits ---
    	        hevtc->SetBinContent(100-time,gemChan,adc/100.); //Why 100- ???
    	        hevt->SetBinContent(100-time,gemChan,adc/100.);
            }
          }
        } // --  end of samples loop
      } // -- end of fadc125 raw channels loop
      
      #ifdef SHOW_EVTbyEVT
        c2->cd(1);  hevt->Draw("colz");
        c2->cd(2);  hevtf->Draw("text");
        c2->Modified(); c2->Update();
      #endif
      
      //==================================================================================================
      //            Begin NN Clustering & Track Fitting
      //==================================================================================================
      #if (USE_CLUST>0)
        // ------------------------   hist dist clustering         ------------------------
        float clust_Xmax[MAX_CLUST];
        float clust_Zmax[MAX_CLUST];
        float clust_Emax[MAX_CLUST];
        
        float clust_Xpos[MAX_CLUST];
        float clust_Zpos[MAX_CLUST];
        float clust_dEdx[MAX_CLUST];
        float clust_Size[MAX_CLUST];
        float clust_Width[MAX_CLUST][3];  // y1, y2, dy ; strips
        float clust_Length[MAX_CLUST][3]; // x1, x2, dx ; time
        
        float hits_Xpos[500];
        float hits_Zpos[500];
        float hits_dEdx[500];
        float hits_Size[MAX_CLUST];
        float hits_Width[MAX_CLUST];  // y1, y2, dy ; strips
        float hits_Length[MAX_CLUST]; // x1, x2, dx ; time
        
        for (int k=0; k<MAX_CLUST; k++) {
          clust_Xpos[k]=0; clust_Zpos[k]=0; clust_dEdx[k]=0;  clust_Size[k]=0;
          clust_Xmax[k]=0; clust_Zmax[k]=0; clust_Emax[k]=0;
          clust_Width[k][0]=999999;   	clust_Width[k][1]=-999999;   	clust_Width[k][2]=0;
          clust_Length[k][0]=999999;  	clust_Length[k][1]=-999999;  	clust_Length[k][2]=0;
        }
        float CL_DIST=2.7; // mm
        int nclust=0;
        
        TH2F* hp = hevt; // -- hevt and hevtc should be same bin size
        TH2F* hpc = hevtc;
        
        int nx=hp->GetNbinsX();    int ny=hp->GetNbinsY();
        double xmi=hp->GetXaxis()->GetBinLowEdge(1);     double xma=hp->GetXaxis()->GetBinUpEdge(nx);
        double ymi=hp->GetYaxis()->GetBinLowEdge(1);     double yma=hp->GetYaxis()->GetBinUpEdge(ny);
        double binx = (xma-xmi)/nx;      double biny = (yma-ymi)/ny;
        #ifdef VERBOSE
          printf("nx=%d,ny=%d,xmi=%f,xma=%f,ymi=%f,yma=%f\n",nx,ny,xmi,xma,ymi,yma);
        #endif
        double THR2 = 1.2; //?? 0.2??
        
        for (int iy=0; iy<ny; iy++) {
          for (int ix=0; ix<nx; ix++) {  //-------------------- clustering loop ------------------------------------
            double c1 = hpc->GetBinContent(ix,iy);                    // energy
            double x1=double(ix)/double(nx)*(xma-xmi)+xmi-binx/2.;    // drift time
            double y1=double(iy)/double(ny)*(yma-ymi)+ymi-biny/2.;    // X strip
            if (c1<THR2) continue;
            if (nclust==0) {
              clust_Xpos[nclust]=y1; clust_Zpos[nclust]=x1;  clust_dEdx[nclust]=c1;  clust_Size[nclust]=1;
              clust_Xmax[nclust]=y1; clust_Zmax[nclust]=x1;  clust_Emax[nclust]=c1;
              clust_Width[nclust][0]=y1;   	clust_Width[nclust][1]=y1;   	clust_Width[nclust][2]=0;
              clust_Length[nclust][0]=x1;  	clust_Length[nclust][1]=x1;  	clust_Length[nclust][2]=0;
              nclust++; continue;
            }
            int added=0;
            for (int k=0; k<nclust; k++) {
              double dist=sqrt(pow((y1-clust_Xpos[k]),2.)+pow((x1-clust_Zpos[k]),2.)); //--- dist hit to clusters
              if (dist<CL_DIST) {
                clust_Xpos[k]=(y1*c1+clust_Xpos[k]*clust_dEdx[k])/(c1+clust_dEdx[k]);  //--  new X pos
                clust_Zpos[k]=(x1*c1+clust_Zpos[k]*clust_dEdx[k])/(c1+clust_dEdx[k]);  //--  new Z pos
                if (c1>clust_Emax[k]) {
		              clust_Xmax[k]=y1;
		              clust_Zmax[k]=x1;
		              clust_Emax[k]=c1;
	              }
                clust_dEdx[k]=c1+clust_dEdx[k];  // new dEdx
                clust_Size[k]=1+clust_Size[k];  // clust size in pixels
                if (y1<clust_Width[k][0]) clust_Width[k][0]=y1; if (y1>clust_Width[k][1]) clust_Width[k][1]=y1; clust_Width[k][2]=clust_Width[k][1]-clust_Width[k][0];
                if (x1<clust_Length[k][0]) clust_Length[k][0]=x1;if (x1>clust_Length[k][1]) clust_Length[k][1]=x1;clust_Length[k][2]=clust_Length[k][1]-clust_Length[k][0];
                hpc->SetBinContent(ix,iy,k+1.);
                added=1; break;
              }
            }
            if (added==0) {
              if (nclust+1>=MAX_CLUST) continue;
              clust_Xpos[nclust]=y1; clust_Zpos[nclust]=x1;  clust_dEdx[nclust]=c1;  clust_Size[nclust]=1;
              clust_Width[nclust][0]=y1;   	clust_Width[nclust][1]=y1;   	clust_Width[nclust][2]=0;
              clust_Length[nclust][0]=x1;  	clust_Length[nclust][1]=x1;  	clust_Length[nclust][2]=0;
              nclust++;
            }
          }
        } //----------------------- end  clustering loop ------------------------------
        
        int MinClustSize=1; //?? 10??
        double MinClustWidth=0.001;
        double MinClustLength=0.01;
        double MaxClustLength=5.;
        double zStart = 0.;  // mm
        double zEnd = 30.; //29.; // mm
        int ii=0;
        #ifdef VERBOSE
          printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
          printf("                Xpos   Ypos   Zpos       E    Width  Length   Size \n");
        #endif
        
        double maxClust_dEdx=0., totalClust_dEdx=0.;
        for (int k=0; k<nclust; k++) {
          #ifdef VERBOSE
            printf("%2d Clust(%2d): %6.1f %6.1f %6.1f %8.1f %6.2f %6.2f %8.1f  ",k,k+1,clust_Xpos[k],clust_Ypos[k],clust_Zpos[k],clust_dEdx[k],clust_Width[k][2],clust_Length[k][2],clust_Size[k]);
            printf("                                  %6.1f %6.1f %6.1f %6.1f \n",clust_Width[k][0],clust_Width[k][1], clust_Length[k][0], clust_Length[k][1]);
          #endif
          //-------------  Cluster Filter -----------------
          if ((clust_Size[k] >= MinClustSize && zStart < clust_Zpos[k] && clust_Zpos[k] < zEnd && clust_Width[k][2]>=MinClustWidth)  || clust_Length[k][2]<=MaxClustLength) {
            #if (USE_MAXPOS>0)
	            hits_Xpos[ii]=clust_Xmax[k];
	            hits_Zpos[ii]=clust_Zmax[k];
            #else
              hits_Xpos[ii]=clust_Xpos[k];
        	    hits_Zpos[ii]=clust_Zpos[k];
            #endif
            hits_dEdx[ii]=clust_dEdx[k];
            hits_Width[ii]=clust_Width[k][2];
            hits_Length[ii]=clust_Length[k][2];
            ii++;
            if (clust_dEdx[k]>maxClust_dEdx) maxClust_dEdx=clust_dEdx[k];
            totalClust_dEdx+=clust_dEdx[k];
          } else {
            #ifdef VERBOSE
  	          printf(" <--- skip \n");
            #endif
  	      }
        }
        int nhits=ii;
        // -----  end hist dist clustering  -----
        
        //======================== Draw HITS and CLUST ==================================
        #ifdef SHOW_EVTbyEVT
          char hevtTitle[80]; sprintf(hevtTitle,"GEM-TRD: Evt=%lld Run=%d e=%d #pi=%d; z pos [mm]; y pos [mm]",jentry,RunNum,electron,pion);
          hevt->SetTitle(hevtTitle);
          #ifdef VERBOSE
            printf("hits_SIZE=%d  Clust size = %d \n",nhits,nclust);
          #endif
          c2->cd(1); gPad->Modified(); gPad->Update();
    	    int COLMAP[]={1,2,3,4,6,5};
    	    int pmt=22 ,pmt0=20; // PM type
    	    for (int i=0; i<nclust; i++) {
    	      #if (USE_MAXPOS>0)
	            TMarker m = TMarker(clust_Zmax[i],clust_Xmax[i],pmt);
            #else
          	  TMarker m = TMarker(clust_Zpos[i], clust_Xpos[i], pmt);
            #endif
    	      int tcol=2;
    	      if (clust_Size[i]<MinClustSize) pmt=22; else pmt=pmt0;
    	      int mcol = COLMAP[tcol-1];   m.SetMarkerColor(mcol);   m.SetMarkerStyle(pmt);
    	      m.SetMarkerSize(0.7+clust_dEdx[i]/300);
    	      m.DrawClone();
    	    }
    	    gPad->Modified(); gPad->Update();
        #endif
        
        #if (USE_GNN==1)   // GNN MC
          //==========================================================
          //           Send to Model simulation
          //==========================================================
          
          #ifdef VERBOSE
            printf("**> Start Model simulation nclust=%d nhits=%d \n",nclust,nhits);
          #endif
          std::vector<int> tracks(nhits, 0);
          std::vector<float> Xcl;
          std::vector<float> Zcl;
          Xcl.clear();
          Zcl.clear();
          for (int n=0; n<nhits; n++) {
            Xcl.push_back(hits_Xpos[n]);
            Zcl.push_back(hits_Zpos[n]);
          }
          doPattern(Xcl, Zcl, tracks);  //---- call GNN ---
          #ifdef VERBOSE
            printf("**> End Model simulation \n"); //===================================================
          #endif
          
          #ifdef SHOW_EVTbyEVT
            c2->cd(2); gPad->Modified(); gPad->Update();
            int COLMAP2[]={1,2,3,4,6,5};
            for (int i=0; i<(int)tracks.size(); i++) {
              #ifdef VERBOSE
        	      printf("i=%d trk=%d |  %8.2f,%8.2f\n",i, tracks[i], Xcl[i], Zcl[i]);
              #endif
              TMarker m = TMarker(hits_Zpos[i],hits_Xpos[i],24);
              int tcol=min(tracks[i],6);
          	  int mcol = COLMAP2[tcol-1];   m.SetMarkerColor(mcol);   m.SetMarkerStyle(41);     m.SetMarkerSize(1.5);
              m.DrawClone();  gPad->Modified(); gPad->Update();
            }
            #ifdef VERBOSE
              printf("\n\n");
              printf("**> End Cluster Plot \n");
            #endif
          #endif
          
          //==================================================
          //----           Track fitting                 -----
          //==================================================
          
          #ifdef VERBOSE
            printf("==> GNN: tracks sort  : trk_siz=%ld \r\n", tracks.size());
          #endif
          //----------------- tracks sorting -------------
          std::vector<std::vector<float>> TRACKS;
          TRACKS.resize(nhits);
          std::vector<int>  TRACKS_N(nhits, 0);
          for (int i=0; i<nhits; i++) { TRACKS_N[i] = 0; }
          for (int i2=0; i2<nhits; i2++) {
            int num =  tracks[i2];
            int num2 = std::max(0, std::min(num, nhits - 1));
            #ifdef VERBOSE
              printf("==> lstm3:track sort i=%d  : num=%d(%d) x=%f z=%f \n", i2, num, num2,  Xcl[i2],Zcl[i2]);
            #endif
            TRACKS[num2].push_back(Xcl[i2]);
            TRACKS[num2].push_back(Zcl[i2]);
            TRACKS_N[num2]++;
          }
          #if (DEBUG > 1)
            for (int i2=0; i2<nhits; i2++) {
              printf(" trdID=%d n_hits=%d v_size=%d \n",i2,TRACKS_N[i2],TRACKS[i2].size());
              for (int i3 = 0; i3 < TRACKS[i2].size(); i3+=2) {
                printf(" trkID=%d  hit=%d x=%f z=%f \n",i2,i3/2,TRACKS[i2].at(i3),TRACKS[i2].at(i3+1));
              }
              if (TRACKS_N[i2]>0) printf("\n");
            }
          #endif
          //------------- end tracks sorting ---------------
          
          #if (USE_FIT==1)
            //-----------------------------------
            //---       linear fitting        ---
            //-----------------------------------
            static TMultiGraph *mg;
            if (mg != NULL ) delete mg;
            mg = new TMultiGraph();
            int NTRACKS=0;
            int MIN_HITS=2;
            Double_t p0, p1;
            
            for (int i2=1; i2<nhits; i2++) {  // tracks loop; zero track -> noise
              if (TRACKS_N[i2]<MIN_HITS) continue;   //---- select 2 (x,z) and more hits on track ----
              #ifdef VERBOSE
                printf("==> fit: start trk: %d \r\n", i2);
              #endif
              std::vector<Double_t> x;
              std::vector<Double_t> y;
              for (int i3=0; i3<(int)TRACKS[i2].size(); i3+=2) {
                #ifdef VERBOSE
                  printf(" trkID=%d  hit=%d x=%f z=%f \n",i2,i3/2,TRACKS[i2].at(i3),TRACKS[i2].at(i3+1));
                #endif
                x.push_back(TRACKS[i2].at(i3+1));
                y.push_back(TRACKS[i2].at(i3));
              }
              #ifdef SHOW_EVTbyEVT
                gErrorIgnoreLevel = kBreak; // Suppress warning messages from empty fit data
                TGraph *g = new TGraph(TRACKS_N[i2], &x[0], &y[0]);  g->SetMarkerStyle(21); g->SetMarkerColor(i2);
                TF1 *f = new TF1("f", "[1] * x + [0]");
                g->Fit(f,"Q");
                //  --- get fit parameters ---
                TF1 *ffunc=g->GetFunction("f");
                p0=ffunc->GetParameter(0);
                p1=ffunc->GetParameter(1);
                Double_t chi2x_nn = ffunc->GetChisquare();
                Double_t Ndfx_nn = ffunc->GetNDF();
                double chi2nn=chi2x_nn/Ndfx_nn;
                mg->Add(g,"p");
              #endif
              NTRACKS++;
            }  //---  end tracks loop
            
            #ifdef SHOW_EVTbyEVT
              //if (NTRACKS<1 || NTRACKS>2) continue;  // --- skip event ----
              char mgTitle[80]; sprintf(mgTitle,"GEM ML-FPGA response, #Tracks=%d; z pos [mm]; y pos [mm]",NTRACKS);
              mg->SetTitle(mgTitle);
              c2->cd(3); mg->Draw("APsame"); //AP?
              mg->GetXaxis()->SetLimits(0.,30.);
              mg->SetMinimum(-0.4); //0?
              mg->SetMaximum(+102.); //96?
              gPad->Modified(); gPad->Update();
            #endif
          #endif // USE_FIT
        #endif // ============= End if (USE_GNN==1) (MC) ==============
        
        //******************************************************************************
        #ifdef SHOW_EVTbyEVT
          cout<<"Event#="<<event_num<<" Electron="<<electron<<"  Pion="<<pion<<" CalorimeterSum="<<CalSum<<" #ofTracks="<<NTRACKS<<endl;
          c2->cd(4);  f125_fit->Draw("box");          gPad->Modified(); gPad->Update();
          c2->cd(5);  hevt->Draw("colz");             gPad->Modified();   gPad->Update();
          
          c2->cd(6);  hCal_sum->Draw();               gPad->Modified();   gPad->Update();
          //c2->cd(7);  f125_el_raw->Draw("colz");      gPad->Modified();   gPad->Update();
          //c2->cd(8);  f125_pi_raw->Draw("colz");      gPad->Modified();  gPad->Update();
          c2->cd(9);  hCal_occ->Draw("hist");         gPad->Modified(); gPad->Update();
          c2->cd(10); urw_cal_corr->Draw("colz text"); gPad->Modified(); gPad->Update();
          
          printf("All done, click middle of canvas ...\n");
          if (electron || pion) c2->cd(1); gPad->WaitPrimitive();
        #endif
      #endif   // --- End USE_CLUST>0 ---
      
      //============ END GEMTRD Pattern Recognition Tracking ==================
      
      #endif //====================== END if (USE_125_RAW) =============================
      
      if (NTRACKS==1) {
        Count("singleTRK");
        _1TRK++;
        if (electron) {
          Count("snTRKel");
          e_1TRK++;
          if (maxClust_dEdx!=0.) hClusterMaxdEdx_e->Fill(maxClust_dEdx);
          if (totalClust_dEdx!=0.) hClusterTotaldEdx_e->Fill(totalClust_dEdx);
        } else if (pion) {
          Count("snTRKpi");
          pi_1TRK++;
          if (maxClust_dEdx!=0.) hClusterMaxdEdx_pi->Fill(maxClust_dEdx);
          if (totalClust_dEdx!=0.) hClusterTotaldEdx_pi->Fill(totalClust_dEdx);
        }
      }
      if (NTRACKS>1) Count("multTRK");
      
      //===========================================================
      //  GEMTracker (SRS) Correlation with TRD Prototypes
      //===========================================================
      
      ULong64_t gt_idx_x=0, gt_idx_y=0, mmg1_idx_y=0, urw_idx_y=0;
      int gt_nhit=0;
      
      double gemtrkr_peak_pos_y[gem_peak_count];
      double gemtrkr_peak_pos_x[gem_peak_count];
      double mmg1_peak_pos_y[gem_peak_count];
      double urw_peak_pos_y[gem_peak_count];
      double gemtrkr_peak_y_height[gem_peak_count];
      double gemtrkr_peak_x_height[gem_peak_count];
      double mmg1_peak_y_height[gem_peak_count];
      double urw_peak_y_height[gem_peak_count];
      
      double gemtrkr_xamp_max=-1., gemtrkr_xch_max=-1000.;
      double gemtrkr_yamp_max=-1., gemtrkr_ych_max=-1000.;
      double mmg1_el_yamp_max=-1., mmg1_pi_yamp_max=-1., mmg1_el_ych_max=-1000., mmg1_pi_ych_max=-1000.;
      double urw_pi_yamp_max=-1., urw_el_yamp_max=-1., urw_el_ych_max=-1000., urw_pi_ych_max=-1000.;
      
      for (ULong64_t i=0; i<gem_peak_count; i++) {
        gemtrkr_peak_pos_y[gem_peak_count] = -1000.;
        gemtrkr_peak_pos_x[gem_peak_count] = -1000.;
        mmg1_peak_pos_y[gem_peak_count] = -1000.;
        urw_peak_pos_y[gem_peak_count] = -1000.;
        gemtrkr_peak_y_height[gem_peak_count] = -1000.;
        gemtrkr_peak_x_height[gem_peak_count] = -1000.;
        mmg1_peak_y_height[gem_peak_count] = -1000.;
        urw_peak_y_height[gem_peak_count] = -1000.;
      }
      
      for (ULong64_t i=0; i<gem_peak_count; i++) { //-- SRS Peaks Loop
        
        if (gem_peak_plane_name->at(i) == "GEMTRKX") {
          gemtrkr_peak_pos_y[gt_idx_y] = gem_peak_real_pos->at(i);
          if (gemtrkr_peak_pos_y[gt_idx_y]<=0) gemtrkr_peak_pos_y[gt_idx_y]+=51.2; else gemtrkr_peak_pos_y[gt_idx_y]-=51.2;  gemtrkr_peak_pos_y[gt_idx_y]*=-1.;  gemtrkr_peak_pos_y[gt_idx_y]+=51.2;
          gemtrkr_peak_y_height[gt_idx_y] = gem_peak_height->at(i);
          gt_idx_y++;
        }
        if (gem_peak_plane_name->at(i) == "GEMTRKY") {
          gemtrkr_peak_pos_x[gt_idx_x] = gem_peak_real_pos->at(i);
          if (gemtrkr_peak_pos_x[gt_idx_x]<=0) gemtrkr_peak_pos_x[gt_idx_x]+=51.2; else gemtrkr_peak_pos_x[gt_idx_x]-=51.2;  gemtrkr_peak_pos_x[gt_idx_x]*=-1.; gemtrkr_peak_pos_x[gt_idx_x]+=51.2;
          gemtrkr_peak_x_height[gt_idx_x] = gem_peak_height->at(i);
          gt_idx_x++;
        }
        if (gem_peak_plane_name->at(i) == "MMG1Y") {
          mmg1_peak_pos_y[mmg1_idx_y] = gem_peak_real_pos->at(i);
          if (mmg1_peak_pos_y[mmg1_idx_y]<=0) mmg1_peak_pos_y[mmg1_idx_y]+=51.2; else mmg1_peak_pos_y[mmg1_idx_y]-=51.2;  mmg1_peak_pos_y[mmg1_idx_y]*=-1.; mmg1_peak_pos_y[mmg1_idx_y]+=51.2;
          mmg1_peak_y_height[mmg1_idx_y] = gem_peak_height->at(i);
          if (mmg1_peak_y_height[mmg1_idx_y]>1000.) mmg1_idx_y++;
          //mmg1_idx_y++;
        }
        if (gem_peak_plane_name->at(i) == "URWELLY") {
          urw_peak_pos_y[urw_idx_y] = gem_peak_real_pos->at(i);
          urw_peak_pos_y[urw_idx_y]*=-1.;  urw_peak_pos_y[urw_idx_y]+=51.2;
          urw_peak_y_height[urw_idx_y] = gem_peak_height->at(i);
          if (urw_peak_y_height[urw_idx_y]>1000.) urw_idx_y++;
          //urw_idx_y++;
        }
      } //--- End SRS Peaks Loop
      
      for (ULong64_t j=0; j<gt_idx_y; j++) {
        hgemtrkr_peak_y->Fill(gemtrkr_peak_pos_y[j]);
        hgemtrkr_peak_y_height->Fill(gemtrkr_peak_y_height[j]);
        if (gemtrkr_peak_y_height[j]>gemtrkr_yamp_max) {
          gemtrkr_yamp_max=gemtrkr_peak_y_height[j];
          gemtrkr_ych_max=gemtrkr_peak_pos_y[j];
        }
        for (ULong64_t i=0; i<mmg1_idx_y; i++) {
          srs_mmg1_y->Fill(gemtrkr_peak_pos_y[j], mmg1_peak_pos_y[i]);
        }
        for (ULong64_t h=0; h<urw_idx_y; h++) {
          srs_urw_y->Fill(gemtrkr_peak_pos_y[j], urw_peak_pos_y[h]);
        }
        for (ULong64_t k=0; k<gt_idx_x; k++) {
          if (NTRACKS==1) {
            singleTrackIndex->Fill(gt_idx_x, gt_idx_y);
            if (electron) singleTrackIndex_e->Fill(gt_idx_x, gt_idx_y);
            if (pion) singleTrackIndex_pi->Fill(gt_idx_x, gt_idx_y);
          }
          if (NTRACKS==2) {
            multiTrackIndex->Fill(gt_idx_x, gt_idx_y);
            if (electron) multiTrackIndex_e->Fill(gt_idx_x, gt_idx_y);
            if (pion) multiTrackIndex_pi->Fill(gt_idx_x, gt_idx_y);
          }
          hgemtrkr_peak_xy->Fill(gemtrkr_peak_pos_x[k], gemtrkr_peak_pos_y[j]);
          gt_nhit++; Count("trkr_hit");
        }
      }
      for (ULong64_t k=0; k<gt_idx_x; k++) {
        hgemtrkr_peak_x->Fill(gemtrkr_peak_pos_x[k]);
        hgemtrkr_peak_x_height->Fill(gemtrkr_peak_x_height[k]);
        if (gemtrkr_peak_x_height[k]>gemtrkr_xamp_max) {
          gemtrkr_xamp_max=gemtrkr_peak_x_height[k];
          gemtrkr_xch_max=gemtrkr_peak_pos_x[k];
        }
        for (ULong64_t i=0; i<mmg1_idx_y; i++) {
          srs_mmg1_xy->Fill(gemtrkr_peak_pos_x[k], mmg1_peak_pos_y[i]);
        }
        if (RunNum>3147 && RunNum<3262) {
          for (ULong64_t h=0; h<urw_idx_y; h++) {
            srs_urw_xy->Fill(gemtrkr_peak_pos_x[k], urw_peak_pos_y[h]);
          }
        }
      }
      for (ULong64_t i=0; i<mmg1_idx_y; i++) {
        hmmg1_peak_y->Fill(mmg1_peak_pos_y[i]);
        hmmg1_peak_y_height->Fill(mmg1_peak_y_height[i]);
        if (electron) hmmg1_peak_y_height_el->Fill(mmg1_peak_y_height[i]);
        if (pion) hmmg1_peak_y_height_pi->Fill(mmg1_peak_y_height[i]);
        if (electron && mmg1_peak_y_height[i]>mmg1_el_yamp_max) {
          mmg1_el_yamp_max=mmg1_peak_y_height[i];
          mmg1_el_ych_max=mmg1_peak_pos_y[i];
        } else if (pion && mmg1_peak_y_height[i]>mmg1_pi_yamp_max) {
          mmg1_pi_yamp_max=mmg1_peak_y_height[i];
          mmg1_pi_ych_max=mmg1_peak_pos_y[i];
        }
      }
      if (RunNum>3147 && RunNum<3262) {
        for (ULong64_t h=0; h<urw_idx_y; h++) {
          hurw_peak_y->Fill(urw_peak_pos_y[h]);
          hurw_peak_y_height->Fill(urw_peak_y_height[h]);
          if (electron) hurw_peak_y_height_el->Fill(urw_peak_y_height[h]);
          if (pion) hurw_peak_y_height_pi->Fill(urw_peak_y_height[h]);
          if (electron && urw_peak_y_height[h]>urw_el_yamp_max) {
            urw_el_yamp_max=urw_peak_y_height[h];
            urw_el_ych_max=urw_peak_pos_y[h];
          } else if (pion && urw_peak_y_height[h]>urw_pi_yamp_max) {
            urw_pi_yamp_max=urw_peak_y_height[h];
            urw_pi_ych_max=urw_peak_pos_y[h];
          }
          for (ULong64_t i=0; i<mmg1_idx_y; i++) {
            if (abs(urw_el_ych_max-mmg1_el_ych_max-3.5)<5) mmg1_urw_y->Fill(mmg1_peak_pos_y[i], urw_peak_pos_y[h]);
          }
        }
      }
      
      hNTracks->Fill(NTRACKS);
      if (electron) hNTracks_e->Fill(NTRACKS);
      if (pion) hNTracks_pi->Fill(NTRACKS);
      
      if (gemtrkr_xch_max>0.) hgemtrkr_max_xch->Fill(gemtrkr_xch_max);
      if (gemtrkr_xamp_max>0.) hgemtrkr_max_xamp->Fill(gemtrkr_xamp_max);
      if (gemtrkr_ych_max>0.) hgemtrkr_max_ych->Fill(gemtrkr_ych_max);
      if (gemtrkr_yamp_max>0.) hgemtrkr_max_yamp->Fill(gemtrkr_yamp_max);
      if (gemtrkr_xch_max>0. && gemtrkr_ych_max>0.) hgemtrkr_max_xy->Fill(gemtrkr_xch_max, gemtrkr_ych_max);
      
      //==========================================================
      //    GEM TRKR Fuducial Area (Y-Direction) Selection
      //==========================================================
      
      bool fiducialArea=false;
      
      if ((RunNum > 3200 && RunNum < 3203) || (RunNum >  3245 && RunNum < 3249)) { //-- GEMTRD Double Fleece
        x_cut1=44., x_cut2=69., y_cut1=28., y_cut2=65.;
        if (pion) { fiducialArea=true;
        } else {
          for (ULong64_t i=0; i<urw_idx_y; i++) {
            if (y_cut2>=urw_peak_pos_y[i] && urw_peak_pos_y[i]>=y_cut1) {
              fiducialArea=true;
              Count("fidOK"); fid_OK++;
              break;
            }
          }
        }
      }
      if (RunNum > 3124 && RunNum < 3132) { //-- GEMTRD Single Fleece
        x_cut1=38, x_cut2=64, y_cut1=25., y_cut2=75.;
        for (ULong64_t i=0; i<urw_idx_y; i++) {
          if (y_cut2>urw_peak_pos_y[i] && urw_peak_pos_y[i]>y_cut1) {
            fiducialArea=true;
            break;
          }
        }
      }
      if ((RunNum > 3131 && RunNum < 3135) || (RunNum >  3249 && RunNum < 3254)) { //-- GEMTRD Single Foil (VU)
        x_cut1=38, x_cut2=64, y_cut1=-0.5, y_cut2=99.5;
        fiducialArea=true;
      }
      if (RunNum > 3274 && RunNum < 3289) { //-- GEMTRD Single Foil (TU)
        x_cut1=38, x_cut2=64, y_cut1=10., y_cut2=90.;
        for (ULong64_t i=0; i<gt_idx_y; i++) {
          if (y_cut2>gemtrkr_peak_pos_y[i] && gemtrkr_peak_pos_y[i]>y_cut1) {
            fiducialArea=true;
            break;
          }
        }
      }
      if ((RunNum >  3195 && RunNum < 3201) || (RunNum >  3214 && RunNum < 3219)) { //-- GEMTRD Double Foil
        x_cut1=44., x_cut2=72, y_cut1=19., y_cut2=61.;
        if (pion) { fiducialArea=true;
        } else {
          for (ULong64_t i=0; i<urw_idx_y; i++) {
            if (y_cut2>urw_peak_pos_y[i] && urw_peak_pos_y[i]>y_cut1) {
              fiducialArea=true;
              break;
            }
          }
        }
      }
      if (RunNum > 3202 && RunNum < 3205) { //-- GEMTRD No Radiator
        x_cut1=44., x_cut2=72., y_cut1=16., y_cut2=80.;
        fiducialArea=true;
      }
      if ((RunNum > 3138 && RunNum < 3147) || (RunNum > 3162 && RunNum < 3183) || (RunNum > 3205 && RunNum < 3212)) { //-- ArCO2 (No Radiator)
        x_cut1=38, x_cut2=64, y_cut1=25, y_cut2=80;
        fiducialArea=true;
      }
      if ((RunNum == 3182) || (RunNum == 3219) || (RunNum == 3227)) { //-- ArCO2 (GEMTRD Double Foil)
        x_cut1=38, x_cut2=64, y_cut1=25, y_cut2=80;
        for (ULong64_t i=0; i<gt_idx_y; i++) {
          if (y_cut2>gemtrkr_peak_pos_y[i] && gemtrkr_peak_pos_y[i]>y_cut1) {
            fiducialArea=true;
            break;
          }
        }
      }
      if (RunNum == 3272) { //-- ArCO2 (GEMTRD Single Foil (TU))
        x_cut1=38, x_cut2=64, y_cut1=25., y_cut2=80.;
        for (ULong64_t i=0; i<gt_idx_y; i++) {
          if (y_cut2>gemtrkr_peak_pos_y[i] && gemtrkr_peak_pos_y[i]>y_cut1) {
            fiducialArea=true;
            break;
          }
        }
      }
      //======== END GEM TRKR Fuducial Area Selection =========
      
      //=====================================================================
      //                fADC125 Pulse Data Processing
      //=====================================================================
      
      double urw_el_amp_max=0., urw_el_chan_max=-1.;
      double urw_pi_amp_max=0., urw_pi_chan_max=-1.;
      double mmg1_el_amp_max=0., mmg1_el_chan_max=-1.;
      double mmg1_pi_amp_max=0., mmg1_pi_chan_max=-1.;
      ULong64_t urw_el_idx_x = 0, urw_pi_idx_x = 0, mmg1_el_idx_x = 0, mmg1_pi_idx_x = 0;
      double gem_amp_max = 0.;
      double mmg1_amp_max = 0.;
      double urw_amp_max = 0.;
      double gem_pos_x[f125_pulse_count], mmg1_pos_x[f125_pulse_count], urw_pos_x[f125_pulse_count];
      double gem_time[f125_pulse_count], gem_amp[f125_pulse_count];
      ULong64_t gem_idx_x = 0, mmg1_idx_x = 0, urw_idx_x = 0, gem_idx_t = 0;
      
      if (fiducialArea) {

        for (ULong64_t i=0; i<f125_pulse_count; i++) { //--- Fadc125 Pulse Loop

          float peak_amp = f125_pulse_peak_amp->at(i);
          float ped = f125_pulse_pedestal->at(i);
          if (0 > ped || ped > 200 ) ped = 100;
          float amp=peak_amp-ped;
          if (amp<0) amp=0;
          float time=f125_pulse_peak_time->at(i);
          int fADCSlot = f125_pulse_slot->at(i);
          int fADCChan = f125_pulse_channel->at(i);
          
          int gemChan = GetGEMChan(fADCChan, fADCSlot);
          int mmg1Chan = GetMMG1Chan(fADCChan, fADCSlot, RunNum);
          int rwellChan = GetRWELLChan(fADCChan, fADCSlot, RunNum);
          
          if (amp>GEM_THR && gemChan>-1 && 100.<=time && time<=185.) {
            f125_fit->Fill(time,gemChan,amp);
            gem_time[i] = time;
            gem_amp[i] = amp; 
            gem_idx_t++;
            gem_pos_x[i] = gemChan*0.4 + 3.2; //-- to [mm]
            gem_idx_x++;
          } else { gem_pos_x[i]=-1000.; gem_idx_x++; gem_time[i]=-1000.; gem_amp[i]=-1000.; gem_idx_t++; }
     
          if (amp>MM_THR && mmg1Chan>-1 && 80.<=time && time<=190.) {
            mmg1_pos_x[i] = mmg1Chan*0.4 + 3.2; //-- to [mm]
            mmg1_idx_x++;
          } else { mmg1_pos_x[i]=-1000.; mmg1_idx_x++; }
     
          if (amp>URW_THR && rwellChan>-1 && 80.<=time && time<=190.) {
            urw_pos_x[i] = rwellChan*0.8 + 3.2; //-- to [mm]
            urw_idx_x++;
          } else { urw_pos_x[i]=-1000.; urw_idx_x++; }
          
          if (electron) {
            if (amp>GEM_THR && gemChan>-1) {
              if (gem_amp_max<amp) gem_amp_max=amp;
            }
            if (RunNum<3262 && RunNum>3147) {
              if (amp>URW_THR && rwellChan>-1) {
                if (urw_amp_max<amp) urw_amp_max=amp;
                if (urw_el_amp_max<amp) {
                  urw_el_amp_max=amp;
                  urw_el_chan_max=rwellChan*0.8 + 3.2;
                }
              }
            }
            if (amp>MM_THR && mmg1Chan>-1) {
              if (mmg1_amp_max<amp) mmg1_amp_max=amp;
              if (mmg1_el_amp_max<amp) {
                mmg1_el_amp_max=amp;
                mmg1_el_chan_max=mmg1Chan*0.4 + 3.2;
              }
            }
          } else if (pion) {
            if (amp>GEM_THR && gemChan>-1) {
              if (gem_amp_max<amp) gem_amp_max=amp;
            }
            if (RunNum<3262 && RunNum>3147) {
              if (amp>URW_THR && rwellChan>-1) {
                if (urw_amp_max<amp) urw_amp_max=amp;
                if (urw_pi_amp_max<amp) {
                  urw_pi_amp_max=amp;
                  urw_pi_chan_max=rwellChan*0.8 + 3.2;
                }
              }
            }
            if (amp>MM_THR && mmg1Chan>-1) {
              if (mmg1_amp_max<amp) mmg1_amp_max=amp;
              if (mmg1_pi_amp_max<amp) {
                mmg1_pi_amp_max=amp;
                mmg1_pi_chan_max=mmg1Chan*0.4 + 3.2;
              }
            }
          }
        } //-- END first f125 pulse loop
        
        if (electron) {
          if (gem_amp_max>0.) f125_el_max->Fill(gem_amp_max);
          if (mmg1_amp_max>0.) mmg1_f125_el_max->Fill(mmg1_amp_max);
          if (urw_amp_max>0.) urw_f125_el_max->Fill(urw_amp_max);
          if (urw_el_amp_max>0. /*&& abs(urw_el_chan_max-mmg1_el_chan_max-0.5)<5*/) {
            urw_el_idx_x++;
          }
          if (mmg1_el_amp_max>0. /*&& abs(urw_el_chan_max-mmg1_el_chan_max-0.5)<5*/) {
            mmg1_el_idx_x++;
          }
        } else if (pion) {
          if (gem_amp_max>0.) f125_pi_max->Fill(gem_amp_max);
          if (mmg1_amp_max>0.) mmg1_f125_pi_max->Fill(mmg1_amp_max);
          if (urw_amp_max>0.) urw_f125_pi_max->Fill(urw_amp_max);
          if (urw_pi_amp_max>0. /*&& abs(urw_pi_chan_max-mmg1_pi_chan_max-0.5)<5*/) {
            urw_pi_idx_x++;
          }
          if (mmg1_pi_amp_max>0. /*&& abs(urw_pi_chan_max-mmg1_pi_chan_max-0.5)<5*/) {
            mmg1_pi_idx_x++;
          }
        }
      } //-- END Fiducial Area Condition
      
      bool trackFound=false, trackSeen=false;
      if (electron) {
        //if (urw_idx_y==1 && urw_el_idx_x==1 && urw_el_chan_max>=x_cut1 && urw_el_chan_max<=x_cut2 && mmg1_idx_y==1 && mmg1_el_idx_x<2 && abs(urw_el_ych_max-mmg1_el_ych_max-3.5)<5 && abs(urw_el_chan_max-mmg1_el_chan_max-0.5)<5) {
        if (urw_idx_y==1 && urw_el_idx_x==1 && urw_el_chan_max>=x_cut1 && urw_el_chan_max<=x_cut2) {
          hNExpected_el->Fill(urw_el_chan_max);
          trackFound=true;
        }
      } else if (pion) {
        //if (urw_idx_y==1 && urw_pi_idx_x==1 && urw_pi_chan_max>=x_cut1 && urw_pi_chan_max<=x_cut2 && mmg1_idx_y==1 && mmg1_pi_idx_x<2 && abs(urw_pi_ych_max-mmg1_pi_ych_max-3.5)<5 && abs(urw_pi_chan_max-mmg1_pi_chan_max-0.5)<5) { 
        if (urw_idx_y==1 && urw_pi_idx_x==1 && urw_pi_chan_max>=x_cut1 && urw_pi_chan_max<=x_cut2) {
          hNExpected_pi->Fill(urw_pi_chan_max);
          trackFound=true;
        }
      }
      
      //====== Second fADC125 Pulse Processing - Single Track Events
      
      bool isSingleTrack=false;
      if (NTRACKS==1) isSingleTrack=true;
      
      //--- Reset Single Track Hit Info for each event
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
      
      if (RunNum<3262 && RunNum>3147) {
        urw_nhit=0;
        urw_xpos.clear();
        urw_zpos.clear();
        urw_dedx.clear();
        urw_parID.clear();
        urw_zHist->Reset();
        urw_zHist_vect.clear();
      }
      
      f125_el_raw->Reset();
      f125_pi_raw->Reset();
      
      
      //if (isSingleTrack && fiducialArea) {
      if (fiducialArea) {
        
        for (ULong64_t i=0; i<f125_pulse_count; i++) { //--- Fadc125 Pulse Loop
          
          float peak_amp = f125_pulse_peak_amp->at(i);
          float ped = f125_pulse_pedestal->at(i);
          if (0 > ped || ped > 200 ) ped = 100;
          float amp=peak_amp-ped;
          if (amp<0) amp=0;
          float time=f125_pulse_peak_time->at(i);
          int fADCSlot = f125_pulse_slot->at(i);
          int fADCChan = f125_pulse_channel->at(i);
          
          int gemChan = GetGEMChan(fADCChan, fADCSlot);
          double gemChan_x = gemChan*0.4 + 3.2;
          int mmg1Chan = GetMMG1Chan(fADCChan, fADCSlot, RunNum);
          int rwellChan = GetRWELLChan(fADCChan, fADCSlot, RunNum);
          
          if (amp>GEM_THR && gemChan>-1 && 100.<=time && time<=185.) {
            for (ULong64_t j=0; j<urw_idx_y; j++) {
              for (ULong64_t k=0; k<urw_idx_x; k++) {
                srs_etrd_beam->Fill(urw_pos_x[k], urw_peak_pos_y[j], amp);
                if (electron) { srs_gemtrd_el->Fill(urw_pos_x[k], urw_peak_pos_y[j], amp); }
                else if (pion) { srs_gemtrd_pion->Fill(urw_pos_x[k], urw_peak_pos_y[j], amp); }
              }
            }
          }
          
          if (electron) {
            if (amp>GEM_THR && gemChan>-1) {
              if (trackFound && (!trackSeen) && abs(urw_el_chan_max-gemChan_x-5.5)<5 /*&& abs(urw_el_ych_max-mmg1_el_ych_max-3.5)<5*/) {
                hNHits_el->Fill(urw_el_chan_max);
                trackSeen=true;
              }
              if (trackFound && abs(urw_el_chan_max-gemChan_x-5.5)<5 /*&& abs(urw_el_ych_max-mmg1_el_ych_max-3.5)<5*/) {
                f125_el->Fill(amp);
                f125_el_amp2ds->Fill(time, gemChan, amp);
                gem_xpos.push_back(gemChan);
                gem_dedx.push_back(amp);
                gem_zpos.push_back(time);
                gem_parID.push_back(1);
                gem_nhit++;
                gem_zHist->Fill(time, amp);
              }
              if (urw_idx_y==1 && urw_el_idx_x==1 && urw_el_chan_max>=x_cut1 && urw_el_chan_max<=x_cut2 && mmg1_idx_y==1 && mmg1_el_idx_x<2 && abs(urw_el_ych_max-mmg1_el_ych_max-3.5)<5) {
                f125_el_raw->Fill(time,gemChan,amp);
              }
            }
            if (amp>MM_THR && mmg1Chan>-1) {
              mmg1_f125_el->Fill(amp);
              mmg1_f125_el_amp2ds->Fill(time, mmg1Chan, amp);
              mmg1_xpos.push_back(mmg1Chan);
              mmg1_dedx.push_back(amp);
              mmg1_zpos.push_back(time);
              mmg1_parID.push_back(1);
              mmg1_nhit++;
              mmg1_zHist->Fill(time, amp);
            }
            if (RunNum<3262 && RunNum>3147) {
              if (amp>URW_THR && rwellChan>-1) {
                urw_f125_el->Fill(amp);
                urw_f125_el_amp2ds->Fill(time, rwellChan, amp);
                urw_xpos.push_back(rwellChan);
                urw_dedx.push_back(amp);
                urw_zpos.push_back(time);
                urw_parID.push_back(1);
              }
            }
            
          } else if (pion) {
            if (amp>GEM_THR && gemChan>-1) {
              if (trackFound && (!trackSeen) && abs(urw_pi_chan_max-gemChan_x-5.5)<5 /*&& abs(urw_pi_ych_max-mmg1_pi_ych_max-3.5)<5*/) {
                hNHits_pi->Fill(urw_pi_chan_max);
                trackSeen=true;
              }
              if (trackFound && abs(urw_pi_chan_max-gemChan_x-5.5)<5 /*&& abs(urw_pi_ych_max-mmg1_pi_ych_max-3.5)<5*/) {
                f125_pi->Fill(amp);
                f125_pi_amp2ds->Fill(time, gemChan, amp);
                gem_xpos.push_back(gemChan);
                gem_dedx.push_back(amp);
                gem_zpos.push_back(time);
                gem_parID.push_back(0);
                gem_nhit++;
                gem_zHist->Fill(time, amp);
              }
              if (urw_idx_y==1 && urw_pi_idx_x==1 && urw_pi_chan_max>=x_cut1 && urw_pi_chan_max<=x_cut2 && mmg1_idx_y==1 && mmg1_pi_idx_x<2 && abs(urw_pi_ych_max-mmg1_pi_ych_max-3.5)<5) {
                f125_pi_raw->Fill(time,gemChan,amp);
              }
            }
            if (amp>MM_THR && mmg1Chan>-1) {
              mmg1_f125_pi_amp2ds->Fill(time, mmg1Chan, amp);
              mmg1_f125_pi->Fill(amp);
              mmg1_xpos.push_back(mmg1Chan);
              mmg1_dedx.push_back(amp);
              mmg1_zpos.push_back(time);
              mmg1_parID.push_back(0);
              mmg1_nhit++;
              mmg1_zHist->Fill(time, amp);
            }
            if (RunNum<3262 && RunNum>3147) {
              if (amp>URW_THR && rwellChan>-1) {
                urw_f125_pi->Fill(amp);
                urw_f125_pi_amp2ds->Fill(time, rwellChan, amp);
                urw_xpos.push_back(rwellChan);
                urw_dedx.push_back(amp);
                urw_zpos.push_back(time);
                urw_parID.push_back(0);
                urw_nhit++;
                urw_zHist->Fill(time, amp);
              }
            }
          }
          hCCor_ud->Fill(Ch_u,Ch_out);
        } //--- END Second fADC125 Pulse Loop ---
        
        double x_min=0., x_max=0., x_center=0., y_min=0., y_max=0., y_center=0.; 
        double cell_dx[NCAL]; for (int i=0; i<NCAL; i++) cell_dx[i]=0;
        for (int cc=0; cc<NCAL; cc++) {
          //-- NEW -- Calorimeter cell positions in SRS coord. sys
          switch(cc) {
          case 0: x_min=23; x_max=41; x_center=33; y_min=4; y_max=22; y_center=13;  break;
          case 1: x_min=44; x_max=64; x_center=54; y_min=4; y_max=22; y_center=13;  break;
          case 2: x_min=66; x_max=84; x_center=75; y_min=4; y_max=22; y_center=13;  break;
          case 3: x_min=23; x_max=41; x_center=33; y_min=23; y_max=43; y_center=33;  break;
          case 4: x_min=44; x_max=64; x_center=54; y_min=23; y_max=43; y_center=33;  break;
          case 5: x_min=66; x_max=84; x_center=75; y_min=24; y_max=42; y_center=33;  break;
          case 6: x_min=44; x_max=64; x_center=54; y_min=44; y_max=64; y_center=54;  break;
          }
          
          if (urw_el_chan_max>=0.) hchan_u_el->Fill(urw_el_chan_max);
          if (urw_pi_chan_max>=0.) hchan_u_pi->Fill(urw_pi_chan_max);
          
          for (ULong64_t j=0; j<urw_idx_y; j++) {
            if (urw_peak_pos_y[j]<=y_max && urw_peak_pos_y[j]>=y_min) {
              for (ULong64_t k=0; k<urw_idx_x; k++) {
                if (urw_pos_x[k]<=x_max && urw_pos_x[k]>=x_min) {
                  cell_dx[cc] = sqrt((urw_pos_x[k]-x_center)*(urw_pos_x[k]-x_center) + (urw_peak_pos_y[j]-y_center)*(urw_peak_pos_y[j]-y_center));
                  //hCal_cal[cc]->Fill(Ecal[cc], cell_dx[cc]);
                  hCal_cal[cc]->Fill(CalSum, cell_dx[cc]);
                }
              }
            }    
          }
          
          if (Ecal[cc]>0.8*Ebeam) {
            for (ULong64_t j=0; j<gt_idx_y; j++) {
              for (ULong64_t k=0; k<gt_idx_x; k++) {
                srs_cal_corr->Fill(gemtrkr_peak_pos_x[k], gemtrkr_peak_pos_y[j]);
                //hCal_trk[cc]->Fill(gemtrkr_peak_pos_x[k], gemtrkr_peak_pos_y[j]);
              }
            }
            for (ULong64_t j=0; j<urw_idx_y; j++) {
              for (ULong64_t k=0; k<urw_idx_x; k++) {
                urw_cal_corr->Fill(urw_pos_x[k], urw_peak_pos_y[j]);
              }
            }
            for (ULong64_t j=0; j<mmg1_idx_y; j++) {
              for (ULong64_t k=0; k<mmg1_idx_x; k++) {
                mmg1_cal_corr->Fill(mmg1_pos_x[k], mmg1_peak_pos_y[j]);
              }
            }
          }
        } //-- END NCAL Loop
        
        for (ULong64_t i=0; i<gem_idx_x; i++) {
          if (gem_pos_x[i]>=0.) {
            for (ULong64_t j=0; j<mmg1_idx_x; j++) {
              if (mmg1_pos_x[j]>=0.) gem_mmg1_x->Fill(gem_pos_x[i], mmg1_pos_x[j]);
            }
            for (ULong64_t j=0; j<urw_idx_x; j++) {
              if (urw_pos_x[j]>=0.) {
                if (abs(urw_pos_x[j]-gem_pos_x[i]-5.5)<5) gem_urw_x->Fill(gem_pos_x[i], urw_pos_x[j]);
              }
            }
            for (ULong64_t j=0; j<urw_idx_y; j++) {
               gem_urw_corr->Fill(gem_pos_x[i], urw_peak_pos_y[j]);
            }
            for (ULong64_t j=0; j<gt_idx_y; j++) {
              if (gemtrkr_peak_pos_y[j]>=0.) srs_gem_xy->Fill(gem_pos_x[i], gemtrkr_peak_pos_y[j]);
            }
            for (ULong64_t k=0; k<gt_idx_x; k++) {
              if (gemtrkr_peak_pos_x[k]>=0.) srs_gem_x->Fill(gemtrkr_peak_pos_x[k], gem_pos_x[i]);
            } 
          }
        }
        
        for (ULong64_t i=0; i<urw_idx_x; i++) {
          if (urw_pos_x[i]>=0.) {
            for (ULong64_t k=0; k<gt_idx_x; k++) {
              srs_urw_x->Fill(gemtrkr_peak_pos_x[k], urw_pos_x[i]);
            }
            for (ULong64_t k=0; k<gt_idx_y; k++) {
              if (gemtrkr_peak_pos_y[k]>=0.) urw_srs_xy->Fill(urw_pos_x[i], gemtrkr_peak_pos_y[k]);
            }
            for (ULong64_t j=0; j<mmg1_idx_x; j++) {
              if (mmg1_pos_x[j]>=0.) {
                 if (abs(urw_pos_x[i]-mmg1_pos_x[j]-0.5)<5) mmg1_urw_x->Fill(mmg1_pos_x[j], urw_pos_x[i]);
              }
            }
            for (ULong64_t j=0; j<urw_idx_y; j++) {
              if (urw_peak_pos_y[j]>=0.) urw_xy->Fill(urw_pos_x[i], urw_peak_pos_y[j]);
            }
          }
        }
        
        for (ULong64_t i=0; i<mmg1_idx_x; i++) {
          if (mmg1_pos_x[i]>=0.) {
            for (ULong64_t j=0; j<mmg1_idx_y; j++) {
              if (mmg1_peak_pos_y[j]>=0.) mmg1_xy->Fill(mmg1_pos_x[i], mmg1_peak_pos_y[j]);
            }
            for (ULong64_t j=0; j<gt_idx_y; j++) {
              if (gemtrkr_peak_pos_y[j]>=0.) mmg1_srs_xy->Fill(mmg1_pos_x[i], gemtrkr_peak_pos_y[j]);
            }
            for (ULong64_t k=0; k<gt_idx_x; k++) {
              srs_mmg1_x->Fill(gemtrkr_peak_pos_x[k], mmg1_pos_x[i]);
            } 
          }
        }
        
        for (ULong64_t i=0; i<gem_idx_t; i++) {
          if (gem_time[i]>0.) {
            for (ULong64_t j=0; j<gt_idx_y; j++) {
              if (gemtrkr_peak_pos_y[j]>=0.) hradshadow->Fill(gem_time[i], gemtrkr_peak_pos_y[j], gem_amp[i]);
            }
            for (ULong64_t j=0; j<urw_idx_y; j++) {
              if (urw_peak_pos_y[j]>=0.) hradshadow_u->Fill(gem_time[i], urw_peak_pos_y[j], gem_amp[i]);
            }
            for (ULong64_t j=0; j<mmg1_idx_y; j++) {
              if (mmg1_peak_pos_y[j]>=0.) hradshadow_m->Fill(gem_time[i], mmg1_peak_pos_y[j], gem_amp[i]);
            }
            for (ULong64_t j=0; j<urw_idx_x; j++) {
              if (urw_pos_x[j]>=0.) hradshadow_ux->Fill(gem_time[i], urw_pos_x[j], gem_amp[i]);
            }
          }
        }
        
        for (int i=1; i<21; i++) {
          gem_zHist_vect.push_back(gem_zHist->GetBinContent(i));
          mmg1_zHist_vect.push_back(mmg1_zHist->GetBinContent(i));
          urw_zHist_vect.push_back(urw_zHist->GetBinContent(i));
        }
      } //=============== END Single Track Event fADC Processing =================
      
      //=====================================================================================
      //                        E v e n t    D i s p l a y
      //=====================================================================================
      
    #ifdef SHOW_EVT_DISPLAY
      char rawelTitle[80]; sprintf(rawelTitle,"GEM-TRD: Evt=%lld e=%d #Trks=%d; Time; X Channel",jentry,electron,NTRACKS);
      f125_el_raw->SetTitle(rawelTitle);
      TLine eline(108.,(urw_el_chan_max-(3.2+5.5))/0.4,182.,(urw_el_chan_max-(3.2+5.5))/0.4); eline.SetLineColor(kRed);
      char rawpiTitle[80]; sprintf(rawpiTitle,"GEM-TRD: Evt=%lld #pi=%d #Trks=%d; Time; X Channel",jentry,pion,NTRACKS);
      f125_pi_raw->SetTitle(rawpiTitle);
      TLine piline(108.,(urw_pi_chan_max-(3.2+5.5))/0.4,182.,(urw_pi_chan_max-(3.2+5.5))/0.4); piline.SetLineColor(kRed);
      cout<<"***PULSE*** Event#="<<event_num<<" trackFound="<<trackFound<<" trackSeen="<<trackSeen<<" urw_x_el="<<(urw_el_chan_max-3.2)/0.4<<" urw_x_pi="<<(urw_pi_chan_max-3.2)/0.4<<" Electron="<<electron<<"  Pion="<<pion<<" CalorimeterSum="<<CalSum<<" #ofTracks="<<NTRACKS<<endl;
      c0->cd(3); f125_el_amp2ds->Draw("colz");  gPad->Modified(); gPad->Update();
      c0->cd(4); f125_pi_amp2ds->Draw("colz");  gPad->Modified(); gPad->Update();
      c0->cd(1); f125_el_raw->Draw("colz");  eline.Draw("same");  gPad->Modified(); gPad->Update();
      c0->cd(2); f125_pi_raw->Draw("colz");  piline.Draw("same"); gPad->Modified(); gPad->Update();
      
      if ((electron || pion) && (f125_el_raw->GetEntries()>0 || f125_pi_raw->GetEntries()>0)) { c0->cd(1); gPad->WaitPrimitive(); }
        //c0->cd(1); f125_el_amp2ds->Draw("colz");
        //c0->cd(5); f125_pi_amp2ds->Draw("colz");
        //c0->cd(2); f125_el_evt_display->Draw("colz");
        //c0->cd(6); f125_pi_evt_display->Draw("colz");
        
        //c0->cd(3); f125_el_raw->Draw("colz");  f125_el_evt_display->Draw("same");
        //TLine lin1(110.,gemtrkr_x2ch,190.,gemtrkr_x2ch); lin1.Draw("same");   //--- draw  gemtrkr x
        ////printf("++++++++++++ Draw GEMTRK:: %f %f %f  \n",gemtrkr_x,ftrk.Eval(gemtrkr_x),gemtrkr_x2ch);
        //c0->cd(7); f125_pi_raw->Draw("colz");  f125_pi_evt_display->Draw("same");
        //TLine lin2(110.,gemtrkr_x2ch,190.,gemtrkr_x2ch); lin2.SetLineColor(kRed); lin2.Draw("same");   //--- draw  gemtrkr x
        /*
        if (electron || pion ) {
          int lc=0;
          for (int k=0; k<100; k++) {
            peak_line[k].SetX1 (100.);
            peak_line[lc].SetY1 (-10.);
            peak_line[lc].SetX2 (101.);
            peak_line[lc].SetY2 (-10.);
          }
          for (ULong64_t i=0; i<gem_peak_count; i++) {
            double pos = gem_peak_real_pos->at(i);
            if (pos<=0) pos=pos+50.; else pos=pos-50.;  pos*=-1.;
            double pos2ch=(ftrk.Eval(pos)+50.)/0.4;  // -- to gemtrd coordinate system
            peak_line[lc].SetX1 (110.);	    peak_line[lc].SetY1 (pos2ch);	    peak_line[lc].SetX2 (190.);	    peak_line[lc].SetY2 (pos2ch);
            if (gem_peak_plane_name->at(i) == "GEMTRKY" ) {
              if (lc<100) {
                peak_line[lc].SetLineColor(kGreen);
                peak_line[lc].Draw("same");
                lc++;
              }
            }
          }  //--- SRS Peak Loop
        }
        */
        //c0->cd(9); hevt->Draw("colz");
        //c0->cd(10); hevtc->Draw("colz");
        //c0->Modified();   c0->Update();
        
        //---- Show Fiducial Area ----
        /*
        srs_gemtrd_el->Divide(srs_etrd_beam);
        //srs_etrd_ratio = (TH2F*)srs_gemtrd_el->Clone("srs_etrd_ratio");
        //srs_gemtrd_el->Copy((TH2F*)srs_etrd_ratio);
        //srs_etrd_ratio->GetXaxis()->SetTitle(" "); srs_etrd_ratio->GetYaxis()->SetTitle(" ");
        //srs_etrd_ratio->SetTitle("TR energy norm");  //srs_etrd_ratio->SetStats(1); srs_etrd_ratio->SetMinimum(0.8); srs_etrd_ratio->SetMaximum(1.35);
        #if 0
          srs_etrd_ratio->Divide(srs_etrd_beam);
        #else
          //srs_etrd_ratio->Add(srs_etrd_beam,-1.);
        #endif
        
        c0->cd(11); srs_etrd_ratio->DrawCopy("colz");
        c0->Modified();   c0->Update();
        //c0->cd(11); srs_gemtrd_el->Draw("colz");
        TBox fbox(x_cut1,y_cut1,x_cut2,y_cut2);  //---- draw box cut ---
        fbox.SetLineColor(kRed);
        fbox.SetFillStyle(0);
        fbox.SetLineWidth(2);
        fbox.DrawClone();
        */
        //c0->cd(12); srs_gem_dx->Draw("colz");
        //c0->Modified();   c0->Update();
        //c0->cd(12); gPad->WaitPrimitive();
    #endif //==================== END Event Display ========================
    
    #ifdef SAVE_TRACK_HITS
      
      float gem_max_clu_dEdx=-1.;
      float gem_max_clu_Xpos=-1.;
      float gem_max_clu_Zpos=-1.;
      float gem_max_clu_Width=-1.;
      gem_nclu=nhits;
      for (int n=0; n<nhits; n++) {
        clu_xpos.push_back(hits_Xpos[n]);
        clu_zpos.push_back(hits_Zpos[n]);
        clu_dedx.push_back(hits_dEdx[n]);
        clu_width.push_back(hits_Width[n]);
        if (hits_dEdx[n] > gem_max_clu_dEdx) {
          gem_max_clu_dEdx=hits_dEdx[n];
          gem_max_clu_Xpos=hits_Xpos[n];
          gem_max_clu_Zpos=hits_Zpos[n];
          gem_max_clu_Width=hits_Width[n];
        }
      }
      if (gem_max_clu_dEdx>0.) {
        clu_dedx_max=gem_max_clu_dEdx;
        clu_xpos_max=gem_max_clu_Xpos;
        clu_zpos_max=gem_max_clu_Zpos;
        clu_width_max=gem_max_clu_Width;
      }
      
      //==== Fill Track Hit Info Trees ====
      if (gem_nhit>0) EVENT_VECT_GEM->Fill();
      if (mmg1_nhit>0) EVENT_VECT_MMG1->Fill();
      if (urw_nhit>0 && RunNum<3262 && RunNum>3147) EVENT_VECT_URW->Fill();
    #endif
  } //=================================== End of Event Loop  ======================================
  
  timer.Stop();
  cout<<"***>>> End Event Loop, Elapsed Time:"<<endl; timer.Print();
  cout<<"Total events= "<<jentry<<endl;
  cout<<"hcount values: 1_TRK="<<_1TRK<<" 1eTRK="<<e_1TRK<<" 1piTRK="<<pi_1TRK<<" fid_OK="<<fid_OK<<" elCC="<<el_CC<<" piCC="<<pi_CC<<endl;
  
  //=====================================================================================
  //===                 S A V E   H I S T O G R A M S                                ====
  //=====================================================================================

  printf("Writing .root output file... \n");
  TFile* fOut;
  #if ANALYZE_MERGED
    char rootFileName[256]; sprintf(rootFileName, "RootOutput/fermiMerged/Run_%06d_%0dEntries_Output.root",RunNum,nEntries);
  #else 
    char rootFileName[256]; sprintf(rootFileName, "RootOutput/Run_%06d_Output.root",RunNum);
  #endif
  fOut = new TFile(rootFileName, "RECREATE");
  fOut->cd();
  cout<<"Writing Output File: "<<rootFileName<<endl;
  HistList->Write("HistDQM", TObject::kSingleKey);
  fOut->Close();
  printf("File written & closed OK \n");
  delete fOut;
  //HistList->Delete();
  
  //=====================================================================================
  //===        S A V E   S I N G L E   T R A C K   H I T   T T R E E S               ====
  //=====================================================================================
  
  #ifdef SAVE_TRACK_HITS
    printf("Writing Hit Info TTree files... \n");
    fHits->cd();
    EVENT_VECT_GEM->Write();
    EVENT_VECT_MMG1->Write();
    if (RunNum<3262 && RunNum>3147) EVENT_VECT_URW->Write();
    fHits->Close();
    printf("TTree files written & closed OK \n");
  #endif
  
  //=====================================================================================
  //===                 P L O T     H I S T O G R A M S                               ===
  //=====================================================================================
  #if ANALYZE_MERGED
    const char *OutputDir="RootOutput/fermiMerged";
  #else
    const char *OutputDir="RootOutput";
  #endif
  #ifdef SAVE_PDF
    char ctit[120];
    #if ANALYZE_MERGED
      sprintf(G_DIR,"%s/Run_%06d_%06dEntries",OutputDir,RunNum,nEntries);
    #else
      sprintf(G_DIR,"%s/Run_%06d",OutputDir,RunNum);
    #endif
    sprintf(ctit,"File=%s",G_DIR);
    bool COMPACT=false;
    TCanvas *cc;
    int nxd=3;
    int nyd=5;
    char pdfname[120];  sprintf(pdfname,"%s_evdisp.pdf",G_DIR);
    
    TBox fbox(x_cut1,y_cut1,x_cut2,y_cut2);  //-- draw fid. area cut --
    fbox.Draw("same");
    fbox.SetLineColor(kRed);
    fbox.SetFillStyle(0);
    fbox.SetLineWidth(1);
    
    //--------------------- new page --------------------
    htitle(" FADC250 - Cherenkovs & Calorimeter ");   //if (!COMPACT) cc=NextPlot(0,0);
    nxd=2; nyd=5;
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hcount->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hCher_u_adc->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hCher_dout_adc->Draw();
    cc=NextPlot(nxd,nyd);                     hCCor_ud->Draw("colz");
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hCal_sum->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hCal_sum_el->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hCal_sum_pi->Draw();
    
    //--------------------- new page --------------------
    htitle(" Calorimeter Cell Distributions ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);                      hCal_occ->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[6]->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[3]->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[4]->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[5]->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[0]->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[1]->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();   hCal_adc[2]->Draw();
    
    //--------------------- new page --------------------
    htitle(" Calorimeter Cell Calibrations ");   if (!COMPACT) cc=NextPlot(0,0);
    for (int i=0; i<NCAL; i++) {
      cc=NextPlot(nxd,nyd);   hCal_cal[i]->Draw("colz");
    }
    
    //--------------------- new page --------------------
    htitle(" Calorimeter Cell Energy Sums ");   if (!COMPACT) cc=NextPlot(0,0);
    for (int i=0; i<NCAL; i++) {
      cc=NextPlot(nxd,nyd);   gPad->SetLogy();  hCal_cell_sum[i]->Draw();
    }
    
    //--------------------- new page --------------------
    htitle(" SRS Correlations & Distributions ");   if (!COMPACT) cc=NextPlot(0,0);
    //cc=NextPlot(nxd,nyd);   hgemtrkr_peak_x->Draw();
    //cc=NextPlot(nxd,nyd);   hgemtrkr_peak_x_height->Draw();
    //cc=NextPlot(nxd,nyd);   hgemtrkr_peak_y->Draw();
    //cc=NextPlot(nxd,nyd);   hgemtrkr_peak_y_height->Draw();
    cc=NextPlot(nxd,nyd);   hmmg1_peak_y->Draw();
    cc=NextPlot(nxd,nyd);   hmmg1_peak_y_height->Draw();
    cc=NextPlot(nxd,nyd);   hmmg1_peak_y_height_el->Draw();
    cc=NextPlot(nxd,nyd);   hmmg1_peak_y_height_pi->Draw();
    cc=NextPlot(nxd,nyd);   hurw_peak_y->Draw();
    cc=NextPlot(nxd,nyd);   hurw_peak_y_height->Draw();
    cc=NextPlot(nxd,nyd);   hurw_peak_y_height_el->Draw();
    cc=NextPlot(nxd,nyd);   hurw_peak_y_height_pi->Draw();
    //for (int i=0; i<NCAL; i++) {
      //cc=NextPlot(nxd,nyd);   hCal_cor[i]->Draw("colz");
    //}
    
    //--------------------- new page --------------------
    /*htitle(" Calorimeter & GEMTRKR Correlation ");   if (!COMPACT) cc=NextPlot(0,0);
    for (int i=0; i<NCAL; i++) {
      cc=NextPlot(nxd,nyd);   hCal_trk[i]->Draw("colz");
    }
    */
    //--------------------- new page --------------------
    htitle(" Hit Mapping (2D) Correlations ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);  mmg1_xy->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  urw_xy->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_urw_xy->Draw("colz");
    cc=NextPlot(nxd,nyd);  srs_mmg1_xy->Draw("colz");
    cc=NextPlot(nxd,nyd);  hgemtrkr_peak_xy->Draw("colz");
    cc=NextPlot(nxd,nyd);  srs_etrd_beam->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_gemtrd_el->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_gemtrd_pion->Draw("colz"); fbox.Draw("same");
    
    //--------------------- new page --------------------
    htitle(" Radiator Shadow in Y ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);  hradshadow->Draw("colz");
    cc=NextPlot(nxd,nyd);  hradshadow_m->Draw("colz");
    cc=NextPlot(nxd,nyd);  hradshadow_u->Draw("colz");
    cc=NextPlot(nxd,nyd);  hradshadow_ux->Draw("colz");
    cc=NextPlot(nxd,nyd);  srs_gem_xy->Draw("colz");  fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  mmg1_srs_xy->Draw("colz");  fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  urw_srs_xy->Draw("colz");  fbox.Draw("same");
    
    //--------------------- new page --------------------
    htitle(" SRS Correlations ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);  srs_cal_corr->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  urw_cal_corr->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  gem_urw_corr->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  mmg1_cal_corr->Draw("colz"); fbox.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_mmg1_y->Draw("colz"); ftrk.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_urw_y->Draw("colz"); ftrk.Draw("same");
    cc=NextPlot(nxd,nyd);  mmg1_urw_y->Draw("colz"); ftrk.Draw("same");
    
    //--------------------- new page --------------------
    htitle(" Track Number Distributions ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);  singleTrackIndex->Draw("colz text");
    cc=NextPlot(nxd,nyd);  multiTrackIndex->Draw("colz text");
    cc=NextPlot(nxd,nyd);  hNTracks_e->Draw();
    cc=NextPlot(nxd,nyd);  hNTracks_pi->Draw();
    cc=NextPlot(nxd,nyd);  hNTracks->Draw();
    
    //--------------------- new page --------------------
    htitle(" GEMTRD Efficiency Plots ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);  hNHits_el->Draw();
    cc=NextPlot(nxd,nyd);  hNHits_pi->Draw();
    cc=NextPlot(nxd,nyd);  hNExpected_el->Draw();
    cc=NextPlot(nxd,nyd);  hNExpected_pi->Draw();
    if (hNHits_el->GetEntries()>0 && hNExpected_el->GetEntries()>0) {
      trdRatio_el->Divide(hNHits_el, hNExpected_el);
      cc=NextPlot(nxd,nyd);  trdRatio_el->Draw();
    }
    if (hNHits_pi->GetEntries()>0 && hNExpected_pi->GetEntries()>0) {
      trdRatio_pi->Divide(hNHits_pi, hNExpected_pi);
      cc=NextPlot(nxd,nyd);  trdRatio_pi->Draw();
    }
    
    //--------------------- new page --------------------
    htitle(" SRS-fADC Correlations ");   if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);  srs_gem_x->Draw("colz"); ftrk.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_mmg1_x->Draw("colz"); ftrk.Draw("same");
    cc=NextPlot(nxd,nyd);  srs_urw_x->Draw("colz"); ftrk.Draw("same");
    cc=NextPlot(nxd,nyd);  gem_mmg1_x->Draw("colz");  ftrk.Draw("same");
    if (RunNum<3262 && RunNum>3147) {
      cc=NextPlot(nxd,nyd); gem_urw_x->Draw("colz"); ftrk.Draw("same");
      cc=NextPlot(nxd,nyd); mmg1_urw_x->Draw("colz"); ftrk.Draw("same");
    }    
    
    //--------------------- new page --------------------
    htitle(" TRD Prototype (Fadc125) Amplitudes ");    if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();    f125_el->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();    f125_pi->Draw();
    cc=NextPlot(nxd,nyd);   f125_el_max->Draw();
    cc=NextPlot(nxd,nyd);   f125_pi_max->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();    mmg1_f125_el->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();    mmg1_f125_pi->Draw();
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();    if (RunNum<3262 && RunNum>3147) {urw_f125_el->Draw();}
    cc=NextPlot(nxd,nyd);   gPad->SetLogy();    if (RunNum<3262 && RunNum>3147) {urw_f125_pi->Draw();}
    
    //--------------------- new page --------------------
    htitle(" TRD Prototype (Fadc125) Amplitudes - 2D");    if (!COMPACT) cc=NextPlot(0,0);
    cc=NextPlot(nxd,nyd);   f125_el_amp2ds->Draw("colz");
    cc=NextPlot(nxd,nyd);   f125_pi_amp2ds->Draw("colz");
    cc=NextPlot(nxd,nyd);   mmg1_f125_el_amp2ds->Draw("colz");
    cc=NextPlot(nxd,nyd);   mmg1_f125_pi_amp2ds->Draw("colz");
    cc=NextPlot(nxd,nyd);   if (RunNum<3262 && RunNum>3147) {urw_f125_el_amp2ds->Draw("colz");}
    cc=NextPlot(nxd,nyd);   if (RunNum<3262 && RunNum>3147) {urw_f125_pi_amp2ds->Draw("colz");}
    
    //}
    //--- close PDF file ----
    cc=NextPlot(-1,-1);
  #endif
  cout<<"=========== END OF RUN "<<RunNum<<" ============"<<endl;
}
