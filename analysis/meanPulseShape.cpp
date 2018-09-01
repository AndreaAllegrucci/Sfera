#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"



int main( int argc, char* argv[] ) {

  if( argc!= 1 ) {
    std::cout << "USAGE: ./meanPulseShape" << std::endl;
    exit(1);
  }

  std::string fileName="Cosmici_Ch_0_Data_5_21_2018_Ascii.root";
  int channel=0;


  TFile* file = TFile::Open(fileName.c_str());
  TTree* tree = (TTree*)file->Get("tree");

  std::cout << "-> Opened file " << fileName.c_str() << std::endl;

  TCanvas* c1 = new TCanvas("c1", "c1", 600, 600);
  c1->cd();
  double massimo;
  int nev=0;
  int ev;
  int nch;
  //float norm;
  float vamp[16];
  float pshape[16][1024];
  float meanpshape[1024]={0.};


  tree->SetBranchAddress( "ev" , &ev );
  tree->SetBranchAddress( "nch" , &nch );
  tree->SetBranchAddress( "vamp", &vamp );
  tree->SetBranchAddress( "pshape" , &pshape );

  TH1D* h1 = new TH1D("h1", "", 1024, 0., 1024. );

  int nentries = tree->GetEntries();
  std::cout << "nentries =  " << nentries << std::endl;



  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);
    //norm=fabs(0.500/vamp[channel]);
    //for(int i=0; i<1024;i++) meanpshape[i] += pshape[channel][i]*norm;
    massimo=pshape[channel][0];
    for(int i=0;i<1024;i++) if(pshape[channel][i]>massimo) massimo=pshape[channel][i];
    if(massimo>0.210&&massimo<0.230){
      for(int i=0; i<1024;i++){
	meanpshape[i] += pshape[channel][i];
      }
      nev++;
    }
  }
  for(int i=0; i<1024;i++) meanpshape[i] = meanpshape[i]/nev;

  for( unsigned i=0; i<1024; ++i ) h1->SetBinContent( i+1, meanpshape[i] );
  h1->SetTitle("Pulseshape fotoni");
  h1->GetXaxis()->SetTitle("ADC counts");
  h1->GetYaxis()->SetTitle("Volt");
  h1->Draw();
  
  size_t pos = 0;
  std::string prefix;
  if((pos = fileName.find(".")) != std::string::npos) {
    prefix = fileName.substr(0, pos);
  }
  std::cout<<"numero di eventi: "<<nev<<std::endl;
  std::string plotsDir(Form("plots/forme"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  c1->SaveAs(Form("%s/pulseshapePhotons.root",plotsDir.c_str()));


  
  std::string fileName2="distrang_120_Data_6_11_2018_Ascii.root";
  TFile* file2 = TFile::Open(fileName2.c_str());
  TTree* tree2 = (TTree*)file2->Get("tree");

  std::cout << "-> Opened file " << fileName2.c_str() << std::endl;
  TCanvas* c2 = new TCanvas("c2", "c2", 600, 600);
  c2->cd();
  double massimo2;
  int nev2=0;
  int ev2;
  int nch2;
  //float norm;
  float vamp2[16];
  float pshape2[16][1024];
  float meanpshape2[1024]={0.};


  tree2->SetBranchAddress( "ev" , &ev2 );
  tree2->SetBranchAddress( "nch" , &nch2 );
  tree2->SetBranchAddress( "vamp", &vamp2 );
  tree2->SetBranchAddress( "pshape" , &pshape2 );

  TH1D* h2 = new TH1D("h2", "", 1024, 0., 1024. );

  int nentries2 = tree2->GetEntries();
  std::cout << "nentries =  " << nentries2 << std::endl;



  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree2->GetEntry(iEntry);
    massimo2=pshape2[channel][0];
    for(int i=0;i<1024;i++) if(pshape2[channel][i]>massimo2) massimo2=pshape2[channel][i];
    if(massimo2>0.170&&massimo2<0.190){
      for(int i=0; i<1024;i++){
	meanpshape2[i] += pshape2[channel][i];
      }
      nev2++;
    }
  }
  for(int i=0; i<1024;i++) meanpshape2[i] = meanpshape2[i]/nev2;

  for( unsigned i=0; i<1024; ++i ) h2->SetBinContent( i+1, meanpshape2[i] );
  h2->SetTitle("Pulseshape muoni");
  h2->GetXaxis()->SetTitle("ADC counts");
  h2->GetYaxis()->SetTitle("Volt");

  h2->Draw();
  
  size_t pos2 = 0;
  std::string prefix2;
  if((pos2 = fileName2.find(".")) != std::string::npos) {
    prefix2 = fileName2.substr(0, pos2);
  }
  std::cout<<"numero di eventi: "<<nev2<<std::endl;
  c2->SaveAs(Form("%s/pulseshapeMuons.root",plotsDir.c_str()));


  float diffmeanpshape[1024];
  for(int i=0;i<1024;i++){
    diffmeanpshape[i]=meanpshape[i]-meanpshape2[i];
  }
  TCanvas* c3 = new TCanvas("c3", "c3", 600, 600);
  c3->cd();
  TH1D* h3 = new TH1D("h3", "", 1024, 0., 1024. );
  for( unsigned i=0; i<1024; ++i ) h3->SetBinContent( i+1, diffmeanpshape[i] );
  h3->SetTitle("Differenza delle pulseshape");
  h3->GetXaxis()->SetTitle("ADC counts");
  h3->GetYaxis()->SetTitle("Volt");
  h3->Draw();
  c3->SaveAs(Form("%s/pulseshapeDiff.root",plotsDir.c_str()));

  return 0;

}
