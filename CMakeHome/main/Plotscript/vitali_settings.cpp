#include <vector>
#include <string>
#include <TH1D.h>
#include "Plotvariable.h"
#include <TTree.h>
#include <TChain.h>
#include <iostream>

#include <fstream>
using std::cout;
using std::endl;

void vitali_current(std::vector<Plotvariable*> *vecp, bool &normalized_plots, int &nbins, std::string &saveto) {
    
    TChain* MCtree = new TChain("LambdaBOfflineTree/DecayTree");
   // TChain* MCtree = new TChain("DecayTree");
    
   // MCtree -> Add("/afs/cern.ch/work/k/kheijhof/public/Lb2LcKKpi/2011MagDown/LambdaB2LcHHH.root");
   // MCtree -> Add("/afs/cern.ch/work/k/kheijhof/public/Lb2LcKKpi/2011MagUp/LambdaB2LcHHH.root");
   MCtree -> Add("/afs/cern.ch/work/k/kheijhof/public/Lb2LcKKpi/2011MagDown/LambdaB2LcHHH_preselect.root");
  
  // MCtree -> Add("../LambdaB2LcHHH_preselect_sWeighted.root");
   
   // TChain* MCtreeM = MCtree.copyTree("","" ,1000,0);
    
    //MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_down.root");
    //MCtree -> Add("/afs/cern.ch/work/c/clinn/public/f2mumu/Bsf2mumu_MC_up.root");
    
    //std::string cuts = "B0_BKGCAT == 20";
    
    normalized_plots = true;                //<-------- Normalized plots?
    nbins = 200;                            //<-------- Default number of bins
    saveto = "../plots/";                   //<-------- Path to save it

  new Plotvariable("lab0_M", MCtree, "Lb mass after preselection", "", nbins, 5150, 6200, "m_{Lb}", "MeV", "", vecp);
   //  new Plotvariable("lab0_M", MCtree, "Without cuts",  "", vecp); 

    //There are basically two types of Constructos:
    //
    //Type1: First (or only) histogram on one canvas
    //Plotvariable Type1(variable to plot, pointer to Tree, Title of Canvas, Label in legend, #bins, lower bound, upper bound, x-axis label, unit, cuts (optional), container (do not edit))
    //
    //Type2: Additional histogram(s) on same canvas
    //Plotvariable Type2(variable to plot, pointer to Tree, Label in legend, cuts(optional), container (do not edit))

     /* Log Plots Probability
    
    new Plotvariable("lab2_ProbNNk", MCtree, "Lc daughter p: Prob K", "before truth", nbins, 0, 1, "ProbNNk", "", vecp, "logy");
    new Plotvariable("lab3_ProbNNk", MCtree, "Lc daughter K: Prob K", "before truth", nbins, 0, 1, "ProbNNk", "", vecp, "logy");
    new Plotvariable("lab4_ProbNNk", MCtree, "Lc daughter pi: Prob K", "before truth", nbins, 0, 1, "ProbNNk", "", vecp, "logy");
    new Plotvariable("lab2_ProbNNpi", MCtree, "Lc daughter p: Prob pi", "before truth", nbins, 0, 1, "ProbNNpi", "", vecp, "logy");  
    new Plotvariable("lab3_ProbNNpi", MCtree, "Lc daughter K: Prob pi", "before truth", nbins, 0, 1, "ProbNNpi", "", vecp, "logy");  
    new Plotvariable("lab4_ProbNNpi", MCtree, "Lc daughter pi: Prob pi", "before truth", nbins, 0, 1, "ProbNNkpi", "", vecp, "logy"); 
    new Plotvariable("lab2_ProbNNp", MCtree, "Lc daughter p: Prob p", "before truth", nbins, 0, 1, "PIDp", "Prob.", vecp, "logy"); 
    new Plotvariable("lab3_ProbNNp", MCtree, "Lc daughter K: Prob p", "before truth", nbins, 0, 1, "PIDp", "Prob.", vecp, "logy"); 
    new Plotvariable("lab4_ProbNNp", MCtree, "Lc daughter pi: Prob p", "before truth", nbins, 0, 1, "PIDp", "Prob.", vecp, "logy");
    */
     
    /* Log Plots Impact Parameter
    new Plotvariable("lab6_IP_ORIVX", MCtree, "IP of K (6) w.r.t. Lc vertex", "before truth", nbins, 0, 0.5, "IP_{K}", "mm", vecp, "logy");  
    new Plotvariable("lab7_IP_ORIVX", MCtree, "IP of K (7) w.r.t. Lc vertex", "before truth", nbins, 0, 0.5, "IP_{K}", "mm", vecp, "logy");
    new Plotvariable("lab8_IP_ORIVX", MCtree, "IP of pi (8) w.r.t. Lc vertex", "before truth", nbins, 0, 0.5, "IP_{Pi}", "mm", vecp, "logy");
    */



    //********************** CUTS ******************************
    
    // Just the cut on Lc mass with no other cuts
   //  new Plotvariable("lab0_M", MCtree, "Lb mass", "With cuts", nbins, 5200, 6200, "m_{Lc}", "MeV", "abs(lab1_M-2286.5)<10", vecp);
   //  new Plotvariable("lab0_M", MCtree, "Without cuts",  "", vecp); 
    

     /* ******************** cuts on Lc ***************
  
    new Plotvariable("lab1_M", MCtree, "Lc mass", "PIDp(2)>10", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab2_PIDp>10", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 

    new Plotvariable("lab1_M", MCtree, "Lc mass", "PIDK(3)>10", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab3_PIDK>10", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);

  
    new Plotvariable("lab1_M", MCtree, "Lc mass", "PIDp(4)<-3, PIDK(4)<-3", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab4_PIDK<-3  && lab4_PIDp<-3", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);

    new Plotvariable("lab1_M", MCtree, "Lc mass", "IPChi2ORIVX(1)<1", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab1_IPCHI2_ORIVX<1", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    
    new Plotvariable("lab1_M", MCtree, "Lc mass", "lab2_PIDp-lab2_PIDK > 0 ", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab2_PIDp-lab2_PIDK > 0 ", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    

    new Plotvariable("lab1_M", MCtree, "Lc mass", "IPChi2OWNPV(0)<7", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab0_IPCHI2_OWNPV<7", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    
    new Plotvariable("lab1_M", MCtree, "Lc mass", "pT(1)>4 GeV", nbins, 2170, 2400,"M_{Lc}", "MeV", "lab1_PT>4000", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    
  
    std::string allcutsLc = "lab6_IP_OWNPV>0.1 && lab7_IP_OWNPV>0.1 && lab8_IP_OWNPV>0.1 && "
    "lab2_PIDp>10 && lab3_PIDK>10 && lab4_PIDK<-3  && lab4_PIDp<-3 && "
    "lab1_IPCHI2_ORIVX<1 && lab2_PIDp-lab2_PIDK > 0 ";
    
    new Plotvariable("lab1_M", MCtree, "Lc mass", "All cuts", nbins, 2170, 2400, "m_{Lc}", "MeV", allcutsLc, vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);
    
    // ************************************************************
    */
    
/*
    //cut00 on FD of Lc
   new Plotvariable("lab1_M", MCtree, "Lc mass", "abs(FD_m-FD_t)<0.1", nbins, 2170, 2400,"M_{Lc}", "MeV", "abs(lab1_FD_ORIVX-lab1_PE/2286*0.06)<0.1", vecp);
   new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
   
   //cut01 on FD of Lb
   new Plotvariable("lab0_M", MCtree, "Lb mass", "abs(FD_m-FD_t)<0.1", nbins, 5200, 6800,"M_{Lb}", "MeV", "abs(lab0_FD_OWNPV-lab0_PE/5619.4*0.427)<0.1", vecp);
   new Plotvariable("lab0_M", MCtree, "Without cuts",  "", vecp); 
    
  //cut02 on pT (tracks)
    new Plotvariable("lab1_M", MCtree, "Lb mass", "sum(all pT)>40 GeV", nbins, 2170, 2400,"M_{Lc}", "MeV", "lab0_PT+lab1_PT+lab2_PT+lab3_PT+lab4_PT+lab5_PT+lab6_PT+lab7_PT+lab8_PT>40*10**3", vecp);
   new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
  
   // cut03 on IP (tracks)
   new Plotvariable("lab1_M", MCtree, "Lc mass", "IP_ORIPV(6,7,8)<0.05", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab6_IP_ORIVX<0.05 && lab7_IP_ORIVX<0.05 && lab8_IP_ORIVX<0.05", vecp);
   new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
  
  
     //cut05 TRACKCHI2 NDF
   // new Plotvariable("lab1_M", MCtree, "Lc mass", "TrackChi2NDOF(6,7,8)<2", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab8_TRACK_CHI2NDOF<2 && lab7_TRACK_CHI2NDOF<2 && lab6_TRACK_CHI2NDOF<2", vecp);
   // new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);
    
       //cut06 IPCHI2ORIVX 6-8
    //new Plotvariable("lab1_M", MCtree, "Lc mass", "IPChi2ORIVX(6,7,8)<0.5", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab8_IPCHI2_ORIVX<0.5 && lab7_IPCHI2_ORIVX<0.5 && lab6_IPCHI2_ORIVX<0.5", vecp);
    //new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);
  
     //cut08
   // new Plotvariable("lab1_M", MCtree, "Lc mass", "Cut on lab4_PIDK, lab4_PIDp", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab4_PIDK<-15  && lab4_PIDp<-15", vecp);
   // new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);
   
   
    new Plotvariable("lab1_M", MCtree, "Lc mass", "PIDK(6)>10", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab6_PIDK>10", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 

    new Plotvariable("lab1_M", MCtree, "Lc mass", "PIDK(7)>10", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab7_PIDK>10", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);
  
    new Plotvariable("lab1_M", MCtree, "Lc mass", "PIDp(8)<-3, PIDK(8)<-3", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab8_PIDK<-3  && lab8_PIDp<-3", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp);
    
        new Plotvariable("lab1_M", MCtree, "Lc mass", "IP_OWNPV(6,7,8)>0.1", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab6_IP_OWNPV>0.1 && lab7_IP_OWNPV>0.1 && lab8_IP_OWNPV>0.1", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts", "", vecp);
    
    */
    
    //doesnt produce better SN ratio
    /*
    new Plotvariable("lab1_M", MCtree, "Lc mass", "lab0_OWNPV_CHI2<40", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab0_OWNPV_CHI2<30", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 

    new Plotvariable("lab1_M", MCtree, "Lc mass", "lab1_ORIVX_CHI2<10", nbins, 2170, 2400, "m_{Lc}", "MeV", "lab1_ORIVX_CHI2<10", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    
       
    new Plotvariable("lab1_M", MCtree, "Lb mass", "pT(6,7,8)>2 GeV", nbins, 2170, 2400,"M_{Lc}", "MeV", "lab6_PT>2000 && lab7_PT>2000 && lab8_PT>2000", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    
    new Plotvariable("lab1_M", MCtree, "Lb mass", "pT(0)>8 GeV", nbins, 2170, 2400,"M_{Lc}", "MeV", "lab0_PT>8000", vecp);
    new Plotvariable("lab1_M", MCtree, "Without cuts",  "", vecp); 
    
    */
     

 
   // new Plotvariable("lab7_PIDK", MCtree, "lab6 PIDK", "", nbins, -100, 100, "PID", "", "", vecp);
 
  
   //new Plotvariable("lab1_M", MCtree, "Lc mass", "all cuts", nbins, 2170, 2400, "m_{Lc}", "MeV", allcuts, vecp);

   

   /* ************* trying to cut on direction of flight
   
   //std::string dirplot = "(lab0_PX*(lab1_ORIVX_X-lab1_OWNPV_X)+lab0_PY*(lab1_ORIVX_Y-lab1_OWNPV_Y)+lab0_PZ*(lab1_ORIVX_Z-lab1_OWNPV_Z))/(lab0_P*sqrt((lab1_ORIVX_X-lab1_OWNPV_X)**2+(lab1_ORIVX_Y-lab1_OWNPV_Y)**2+(lab1_ORIVX_Z-lab1_OWNPV_Z)**2))";
   
   new Plotvariable("(lab0_PX*(lab1_ORIVX_X-lab1_OWNPV_X)+lab0_PY*(lab1_ORIVX_Y-lab1_OWNPV_Y)+lab0_PZ*(lab1_ORIVX_Z-lab1_OWNPV_Z))/(lab0_P*sqrt((lab1_ORIVX_X-lab1_OWNPV_X)**2+(lab1_ORIVX_Y-lab1_OWNPV_Y)**2+(lab1_ORIVX_Z-lab1_OWNPV_Z)**2))", MCtree, "direction of flight", "", nbins, 0.99999, 1,"cos(angle)", "", "", vecp);
   new Plotvariable("lab1_DIRA_ORIVX", MCtree, "DIRA_ORIVX",  "", vecp);
   
   
   new Plotvariable("(lab0_PX*(lab1_ORIVX_X-lab1_OWNPV_X)+lab0_PY*(lab1_ORIVX_Y-lab1_OWNPV_Y)+lab0_PZ*(lab1_ORIVX_Z-lab1_OWNPV_Z))/(lab0_P*sqrt((lab1_ORIVX_X-lab1_OWNPV_X)**2+(lab1_ORIVX_Y-lab1_OWNPV_Y)**2+(lab1_ORIVX_Z-lab1_OWNPV_Z)**2))", MCtree, "direction of flight", "", nbins, 0.99999, 1,"cos(angle)", "", "", vecp);
   new Plotvariable("lab0_DIRA_OWNPV", MCtree, "DIRA_ORIVX",  "", vecp);
  
    new Plotvariable("lab0_DIRA_OWNPV", MCtree, "DIRA_ORIVX", "", nbins, 0.99999, 1,"cos(angle)", "", vecp);
   
    */
    
   
    
    /*
   
    
    std::string allcutsLb00 = "abs(2286-lab1_M)<20 && "
    "lab6_IP_OWNPV>0.1 && lab7_IP_OWNPV>0.1 && lab8_IP_OWNPV>0.1 && "
    "lab2_PIDp>10 && lab3_PIDK>10 && lab4_PIDK<-3  && lab4_PIDp<-3 && "
    "lab1_IPCHI2_ORIVX<1 && lab2_PIDp-lab2_PIDK > 0  ";
    
    
    new Plotvariable("lab0_M", MCtree, "Lb mass", "All Lc cuts + Lc mass cut", nbins, 5200, 6200, "m_{Lb}", "MeV", allcutsLb00, vecp);
    new Plotvariable("lab0_M", MCtree, "Without cuts",  "", vecp); 
    
   
    new Plotvariable("lab5_M", MCtree, "a1 mass", "All Lc cuts", nbins, 1000, 3000, "m_{a1}", "MeV", allcutsLb00, vecp);
    new Plotvariable("lab5_M", MCtree, "Without cuts",  "", vecp);
    
   */
   
   
    
    /* for some reason here it doesnt work to split lines...
    std::string cuts[] = {"abs(1968 -lab5_M)>10", "
    " "abs(2286-lab1_M)<20", "
    " "lab6_PIDK>5", "
    " "lab7_PIDK>5", "
    " "lab6_IP_OWNPV>0.1", "
    " "lab7_IP_OWNPV>0.1", "
    " "lab8_IP_OWNPV>0.1", "
    " "lab2_PIDp>10", "
    " "lab3_PIDK>10", "
    " "lab4_PIDK<-3", "
    " "lab4_PIDp<-3", "
    " "lab1_IPCHI2_ORIVX<1", "
    " "lab2_PIDp-lab2_PIDK > 0" "
    " };
    */
    
    
    
        
   // new Plotvariable("lab0_M", MCtree, "Lb mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 5200, 6200, "m_{Lb}", "MeV", "", vecp);
   // new Plotvariable("lab0_M", MCtree, "Without cuts",  "", vecp);
    
    //------------------------------------------------------------------
    
    
    
    std::string cuts[] = {"abs(1968 -lab5_M)<20",  "abs(2286-lab1_M)<20", "lab6_PIDK>5",  "lab7_PIDK>5", "lab6_IP_OWNPV>0.1",  "lab7_IP_OWNPV>0.1",  "lab8_IP_OWNPV>0.1",  "lab2_PIDp>10", "lab3_PIDK>10",  "lab4_PIDK<-3",  "lab4_PIDp<-3", "lab1_IPCHI2_ORIVX<1",  "lab2_PIDp-lab2_PIDK > 0" };
    int nocuts=sizeof(cuts)/sizeof(cuts[0]);
    
    std::string allcuts = "";
    for(int i=0;i<nocuts-1;i++){
      allcuts+=cuts[i]+" && ";
    }
    allcuts+=cuts[nocuts-1];
    
   //   new Plotvariable("lab0_M", MCtree, "Lb mass", "With cuts", nbins, 5200, 6200, "m_{Lc}", "MeV", allcuts, vecp);
   //  new Plotvariable("lab5_M", MCtree, "a1 mass", "With cuts", nbins, 1000, 3000, "m_{Lc}", "MeV", allcuts, vecp);
  
    
    
    // ********************************** ALL BUT ONE PLOTS ******************************************
    /* 
    std::string allb1cuts = "";
    for(int i1=0;i1<nocuts;i1++){
      for(int i2=0;i2<nocuts-2;i2++){
	if(i1!=i2) {
	  allb1cuts+=cuts[i2]+" && ";
	}
      }
      if(i1!=nocuts-1){
	allb1cuts+=cuts[nocuts-2]+" && ";
	allb1cuts+=cuts[nocuts-1];
      }
      else{
	allb1cuts+=cuts[nocuts-2];
      }
      cout << endl << allb1cuts << endl;
      new Plotvariable("lab0_M", MCtree, "Lb mass", "All but "+cuts[i1] + " cut", nbins, 5200, 6200, "m_{Lb}", "MeV", allb1cuts, vecp); // All but " + cuts[i1] + " cut"
      new Plotvariable("lab0_M", MCtree, "All cuts", allcuts, vecp); 
      allb1cuts="";
    } */
    
  
    std::ofstream cutsfile;
    cutsfile.open ("../plots/cuts.txt");
    for(int i=0;i<nocuts-1;i++){
      cutsfile << "SELECT "+cuts[i]+"\n";
    }
    cutsfile.close();
    /*
    
    // ************************* 2body subsystem masses**********************************
    
    // To check: Lc+a1 mass plot should give the same as lab0_M (Lb mass) plot
    //new Plotvariable("sqrt((lab1_PE+lab5_PE)**2-(lab1_PX+lab5_PX)**2-(lab1_PY+lab5_PY)**2-(lab1_PZ+lab5_PZ)**2)", MCtree, "Lc + a1 mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 5150, 6250, "m_{1+5}", "MeV", allcuts, vecp);
    //new Plotvariable("sqrt((lab1_PE+lab5_PE)**2-(lab1_PX+lab5_PX)**2-(lab1_PY+lab5_PY)**2-(lab1_PZ+lab5_PZ)**2)", MCtree, "Without cuts",  "", vecp); 
    
    new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Lc + pi- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 2400, 5700, "m_{1+8}", "MeV", allcuts, vecp);
    new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);
    
    new Plotvariable("sqrt((lab1_PE+lab7_PE)**2-(lab1_PX+lab7_PX)**2-(lab1_PY+lab7_PY)**2-(lab1_PZ+lab7_PZ)**2)", MCtree, "Lc + K+ mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 2500, 6000, "m_{1+7}", "MeV", allcuts, vecp);
    new Plotvariable("sqrt((lab1_PE+lab7_PE)**2-(lab1_PX+lab7_PX)**2-(lab1_PY+lab7_PY)**2-(lab1_PZ+lab7_PZ)**2)", MCtree, "Without cuts",  "", vecp);
    
    new Plotvariable("sqrt((lab1_PE+lab6_PE)**2-(lab1_PX+lab6_PX)**2-(lab1_PY+lab6_PY)**2-(lab1_PZ+lab6_PZ)**2)", MCtree, "Lc + K- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 2500, 6000, "m_{1+6}", "MeV", allcuts, vecp);
    new Plotvariable("sqrt((lab1_PE+lab6_PE)**2-(lab1_PX+lab6_PX)**2-(lab1_PY+lab6_PY)**2-(lab1_PZ+lab6_PZ)**2)", MCtree, "Without cuts",  "", vecp);
   
    new Plotvariable("sqrt((lab7_PE+lab6_PE)**2-(lab7_PX+lab6_PX)**2-(lab7_PY+lab6_PY)**2-(lab7_PZ+lab6_PZ)**2)", MCtree, "K+ + K- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 950, 2900, "m_{7+6}", "MeV", allcuts, vecp);
    new Plotvariable("sqrt((lab7_PE+lab6_PE)**2-(lab7_PX+lab6_PX)**2-(lab7_PY+lab6_PY)**2-(lab7_PZ+lab6_PZ)**2)", MCtree, "Without cuts",  "", vecp);

    new Plotvariable("sqrt((lab7_PE+lab8_PE)**2-(lab7_PX+lab8_PX)**2-(lab7_PY+lab8_PY)**2-(lab7_PZ+lab8_PZ)**2)", MCtree, "K+ + pi- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 600, 2500, "m_{7+8}", "MeV", allcuts, vecp);
    new Plotvariable("sqrt((lab7_PE+lab8_PE)**2-(lab7_PX+lab8_PX)**2-(lab7_PY+lab8_PY)**2-(lab7_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);

    new Plotvariable("sqrt((lab6_PE+lab8_PE)**2-(lab6_PX+lab8_PX)**2-(lab6_PY+lab8_PY)**2-(lab6_PZ+lab8_PZ)**2)", MCtree, "K- + pi- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 600, 2500, "m_{6+8}", "MeV", allcuts, vecp);
    new Plotvariable("sqrt((lab6_PE+lab8_PE)**2-(lab6_PX+lab8_PX)**2-(lab6_PY+lab8_PY)**2-(lab6_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);
    
    */
    //with Lb mass cut
    
    std::string allcuts2bLb = "abs(5619.4-lab0_M)<40 && ";
    for(int i=0;i<nocuts-1;i++){
      allcuts2bLb+=cuts[i]+" && ";
    }
    allcuts2bLb+=cuts[nocuts-1];
    /* With a mass cut on Lb
    new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Lc + pi- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 2400, 5700, "m_{1+8}", "MeV", allcuts2bLb, vecp);
    new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);
    
    new Plotvariable("sqrt((lab1_PE+lab7_PE)**2-(lab1_PX+lab7_PX)**2-(lab1_PY+lab7_PY)**2-(lab1_PZ+lab7_PZ)**2)", MCtree, "Lc + K+ mass", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 2500, 6000, "m_{1+7}", "MeV", allcuts2bLb, vecp);
    new Plotvariable("sqrt((lab1_PE+lab7_PE)**2-(lab1_PX+lab7_PX)**2-(lab1_PY+lab7_PY)**2-(lab1_PZ+lab7_PZ)**2)", MCtree, "Without cuts",  "", vecp);
    
    new Plotvariable("sqrt((lab1_PE+lab6_PE)**2-(lab1_PX+lab6_PX)**2-(lab1_PY+lab6_PY)**2-(lab1_PZ+lab6_PZ)**2)", MCtree, "Lc + K- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 2500, 6000, "m_{1+6}", "MeV", allcuts2bLb, vecp);
    new Plotvariable("sqrt((lab1_PE+lab6_PE)**2-(lab1_PX+lab6_PX)**2-(lab1_PY+lab6_PY)**2-(lab1_PZ+lab6_PZ)**2)", MCtree, "Without cuts",  "", vecp);
   
    new Plotvariable("sqrt((lab7_PE+lab6_PE)**2-(lab7_PX+lab6_PX)**2-(lab7_PY+lab6_PY)**2-(lab7_PZ+lab6_PZ)**2)", MCtree, "K+ + K- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 950, 2900, "m_{7+6}", "MeV", allcuts2bLb, vecp);
    new Plotvariable("sqrt((lab7_PE+lab6_PE)**2-(lab7_PX+lab6_PX)**2-(lab7_PY+lab6_PY)**2-(lab7_PZ+lab6_PZ)**2)", MCtree, "Without cuts",  "", vecp);

    new Plotvariable("sqrt((lab7_PE+lab8_PE)**2-(lab7_PX+lab8_PX)**2-(lab7_PY+lab8_PY)**2-(lab7_PZ+lab8_PZ)**2)", MCtree, "K+ + pi- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 600, 2500, "m_{7+8}", "MeV", allcuts2bLb, vecp);
    new Plotvariable("sqrt((lab7_PE+lab8_PE)**2-(lab7_PX+lab8_PX)**2-(lab7_PY+lab8_PY)**2-(lab7_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);

    new Plotvariable("sqrt((lab6_PE+lab8_PE)**2-(lab6_PX+lab8_PX)**2-(lab6_PY+lab8_PY)**2-(lab6_PZ+lab8_PZ)**2)", MCtree, "K- + pi- mass", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 600, 2500, "m_{6+8}", "MeV", allcuts2bLb, vecp);
    new Plotvariable("sqrt((lab6_PE+lab8_PE)**2-(lab6_PX+lab8_PX)**2-(lab6_PY+lab8_PY)**2-(lab6_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);

   */ 
    
    //************************ Zoom into Sigma_c peak
  //  new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Lc + pi- mass: All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", "All Lc, a1, Lc mass, PIDK(6,7)>5 cuts", nbins, 2350, 2550, "m_{1+8}", "MeV", allcuts, vecp);
   // new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);
   
  //  new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Lc + pi- mass: All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", "All Lc, a1, Lc mass, PIDK(6,7)>5, Lb mass cuts", nbins, 2350, 2550, "m_{1+8}", "MeV", allcuts2bLb, vecp);
  //  new Plotvariable("sqrt((lab1_PE+lab8_PE)**2-(lab1_PX+lab8_PX)**2-(lab1_PY+lab8_PY)**2-(lab1_PZ+lab8_PZ)**2)", MCtree, "Without cuts",  "", vecp);
   
    
    
    //D_s mass sigma 6.3 MeV
    // ******************************* Looking for D_s peak *****************************
    
    /*
    std::string allcutsDs = "abs(1968 -lab5_M)<10 && ";
    for(int i=1;i<nocuts-1;i++){
      allcutsDs+=cuts[i]+" && ";
    }
    allcutsDs+=cuts[nocuts-1];
    cout << endl << allcutsDs << endl;
    new Plotvariable("lab0_M", MCtree, "Lb mass: All Lc cuts + abs(1968 -lab5_M)<10 + Lc mass cut + PIDK(6,7)>5", "All Lc cuts + inverted a1 cut + Lc mass cut + PIDK(6,7)>5", nbins, 5200, 6200, "m_{Lb}", "MeV", allcutsDs, vecp);
    
    new Plotvariable("lab0_M", MCtree, "Lb mass:  abs(1968 -lab5_M)<10 cut", "All Lc cuts + inverted a1 cut + Lc mass cut + PIDK(6,7)>5", nbins, 5200, 6200, "m_{Lb}", "MeV", "abs(1968 -lab5_M)<10", vecp);
    
    */
    // Looking for CP violation
    /*
    
    std::string allcutsIDs0 = "lab0_ID <0 && ";
    for(int i=0;i<nocuts-1;i++){
      allcutsIDs0+=cuts[i]+" && ";
    }
    allcutsIDs0+=cuts[nocuts-1];
    
    std::string allcutsIDg0 = "lab0_ID > 0 && ";
    for(int i=0;i<nocuts-1;i++){
      allcutsIDg0+=cuts[i]+" && ";
    }
    allcutsIDg0+=cuts[nocuts-1];
    
    
    new Plotvariable("lab0_M", MCtree, "Lb mass", "All cuts, lab0_ID < 0", nbins, 5200, 6200, "m_{Lb}", "MeV", allcutsIDg0, vecp); // All but " + cuts[i1] + " cut"
    new Plotvariable("lab0_M", MCtree, "All cuts, lab0_ID > 0", allcutsIDs0, vecp); 
    new Plotvariable("lab0_M", MCtree, "All cuts", allcuts, vecp); 
    */
    
    
    
    //******************************************sweighted plots
    
 //  new Plotvariable("lab0_M", MCtree, "Lb mass", "sig-sw", nbins, 5200, 6200, "m_{Lb}", "MeV", "sig0_yield_sw", vecp); 
 //  new Plotvariable("lab0_M", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
     /*
   new Plotvariable("lab0_DIRA_OWNPV", MCtree, "Flight direction of Lb w.r.t OWNPV", "sig-sw", nbins, 0.99999, 1, "cos(theta)", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab0_DIRA_OWNPV", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   
   new Plotvariable("lab0_IP_OWNPV", MCtree, "Impact par. (IP) of Lb w.r.t OWNPV", "sig-sw", nbins, 0, 0.12, "IP", "mm", "sig0_yield_sw", vecp); 
   new Plotvariable("lab0_IP_OWNPV", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);

   
   new Plotvariable("lab1_IPCHI2_OWNPV", MCtree, "IPCHI2 of Lc (lab1) w.r.t. OWNPV ", "sig-sw", nbins, -50, 2000, "IPCHI2", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab1_IPCHI2_OWNPV", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   
   new Plotvariable("lab0_IPCHI2_OWNPV", MCtree, "IPCHI2 of Lb (lab0) w.r.t OWNPV ", "sig-sw", nbins, -1, 30, "IPCHI2", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab0_IPCHI2_OWNPV", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   
   new Plotvariable("lab7_PT", MCtree, "Transverse momentum of K+ (lab7)", "sig-sw", nbins, 0, 8000, "pT_{K^{+}}", "MeV", "sig0_yield_sw", vecp); 
   new Plotvariable("lab7_PT", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   */ 
 //  new Plotvariable("lab0_PT", MCtree, "Lb pT", "sig-sw", nbins, 2000, 30000, "pT_{Lb}", "MeV", "sig0_yield_sw", vecp); 
 //  new Plotvariable("lab0_PT", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
  /* 
   new Plotvariable("lab2_PIDK", MCtree, "lab2_PIDK", "sig-sw", nbins, -20, 80, "PIDK", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab2_PIDK", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   
   new Plotvariable("lab2_PIDp", MCtree, "lab2_PIDp", "sig-sw", nbins, 0, 100, "PIDp", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab2_PIDp", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   

   
   new Plotvariable("lab0_FD_OWNPV", MCtree, "lab0_FD_OWNPV", "sig-sw", nbins, 0, 70, "FD", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab0_FD_OWNPV", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
   

   
    new Plotvariable("lab1_PT", MCtree, "lab1_PT", "sig-sw", nbins, 0, 16000, "pT", "", "sig0_yield_sw", vecp); 
   new Plotvariable("lab1_PT", MCtree, "bkg-sw", "bkg0_yield_sw", vecp);
    */
}


