#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraph.h"


int main( int argc, char* argv[] ) {

  if( argc!= 3 ) {
    std::cout << "USAGE: ./meanPulseShape [rootFileName] [channel]" << std::endl;
    exit(1);
  }

  std::string fileName(argv[1]);
  int channel(atoi(argv[2]));


  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;

  double massimo;
  
  int ev;
  int nch;
  float vamp[16];
  float vcharge[16];
  float pshape[16][1024];

  tree->SetBranchAddress( "ev" , &ev );
  tree->SetBranchAddress( "nch" , &nch );
  tree->SetBranchAddress( "vamp", &vamp );
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "pshape" , &pshape );

  int nentries = tree->GetEntries();
  std::cout << "nentries =  " << nentries << std::endl;

  double Q[nentries];
  double V[nentries];


  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
    tree->GetEntry(iEntry);
    massimo=pshape[channel][0];
    for(int i=0;i<1024;i++) if(pshape[channel][i]>massimo) massimo=pshape[channel][i];
    Q[iEntry]=vcharge[channel];
    V[iEntry]=massimo;
    if(iEntry%500==0) std::cout<<"evento "<<iEntry<<std::endl;
  }
  
  TCanvas* c = new TCanvas("c", "c", 600, 600);
  c->SetGrid();
  TGraph* gr = new TGraph(nentries,Q,V);
  gr->SetMarkerStyle(8);
  gr->SetMarkerSize(0.5);
  gr->SetMarkerColor(2);
  gr->GetXaxis()->SetTitle("carica (pC)");
  gr->GetYaxis()->SetTitle("ampiezza segnale (V)");
  gr->SetTitle("relazione carica-ampiezza");


  gr->Draw("AP");
  
  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }
  std::string plotsDir(Form("plots/forme"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  c->SaveAs(Form("%s/TensVsCarica.pdf",plotsDir.c_str()));
  c->SaveAs(Form("%s/TensVsCarica.root",plotsDir.c_str()));

  return 0;

}
