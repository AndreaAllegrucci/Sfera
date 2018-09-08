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
#define MAX 400

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
    outfileName = prefix + ".pdf";
  }
  float vcharge[NCH];
  double energy;

  double a[NCH]={0.};
  double b[NCH]={0.};
  a[12]=300.;
  b[12]=0.;
  a[7]=68.2;
  b[7]=0.;
  a[1]=300.;
  b[1]=0.;
  a[3]=300.;
  b[3]=0.;
  a[14]=300.;
  b[14]=0.;
  a[6]=1472.34;
  b[6]=0.0186713;
  a[11]=0.416095;
  b[11]=4.436439;
  a[9]=1.45058;
  b[9]=17.6056;
  for(int i=0;i<NCH;i++){
    a[i]=150.;
    b[i]=0.;
  }
  TCanvas* c = new TCanvas("c", "c", 1000, 600);
  TH1D* charge = new TH1D("charge", "charge", nbin, MIN, MAX);
  TFile *f = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)f->Get(Form("tree"));
  tree->SetBranchAddress( "vcharge", &vcharge );
  int nentries = tree->GetEntries();

  std::string plotsDir(Form("plots/energia"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );
  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
    tree->GetEntry(iEntry);
    energy=(vcharge[ch]-b[ch])/a[ch];
    charge->Fill(energy);
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
