#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPaveText.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"


#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <tgmath.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#define NCH 16
#define MIN 0
//#define MAX 80
#define MAX 450

int main( int argc, char* argv[]) {
  
  if( argc != 6 ) { 
    std::cout << "USAGE: ./spettro [rootfilename] [canale] [E min] [E max] [nbin]" << std::endl;
    exit(1);     
  }
  
  std::string fileName(argv[1]);
  int ch(atoi(argv[2]));
  double Emin(atoi(argv[3]));
  double Emax(atoi(argv[4]));
  int nbin(atoi(argv[5]));

  size_t pos = 0;
  std::string outfileName;
  if((pos = fileName.find(".")) != std::string::npos) {
    std::string prefix = fileName.substr(0, pos);
    outfileName = "ch_" + std::to_string(ch) + "_" + prefix + ".pdf";
  }
  float vcharge[NCH];
  float pshape[NCH][1024];
  int satura;
  double energy;

  double a[NCH]={0.};
  double b[NCH]={0.};

  //for(int i=0;i<NCH;i++){
  //  a[i]=300.;
  //  b[i]=0.;
  //}
  
  for(int i=0;i<NCH;i++){
    a[i]=150.;
    b[i]=0.;
  }
  
  TCanvas* c = new TCanvas("c", "c", 1000, 600);
  TH1D* charge = new TH1D("charge", "charge", nbin, MIN, MAX);
  TFile *f = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)f->Get(Form("tree"));
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "pshape", &pshape );
  int nentries = tree->GetEntries();

  std::string plotsDir(Form("plots/energia"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );
  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
    tree->GetEntry(iEntry);
    energy=(vcharge[ch]-b[ch])/a[ch];
    satura=0;
    //std::cout<<"**************"<<std::endl;

    for(int i=0; i<1024; i++){
      if(pshape[ch][i]>2) satura=1;
      //std::cout<<pshape[ch][i]<<std::endl;
    }
    if(satura==1) std::cout<<"saturato"<<std::endl;
    if(satura==0) charge->Fill(energy);
  }
  
  charge->SetTitle(Form("SPETTRO CANALE %d", ch));
  charge->GetXaxis()->SetTitle("energia (MeV)");
  charge->GetYaxis()->SetTitle("nEv");

  charge->GetXaxis()->SetRangeUser(Emin,Emax);

  charge->Draw();
  
  c->SaveAs(Form("plots/energia/%s",outfileName.c_str()));
  c->SaveAs(Form("plots/energia/%s",fileName.c_str()));

  return 0;
}
