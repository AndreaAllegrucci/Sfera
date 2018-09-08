#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPaveText.h"
#include "TGraphErrors.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#define NCH 16



int main( int argc, char* argv[]) {
    
  if( argc != 4 ) {        
    std::cout << "USAGE: ./measCleanToTree [rootfilename] [soglia bassa] [soglia alta]" << std::endl;
    exit(1);        
  }
    
  std::string fileName(argv[1]);
  double sogliab(std::stod(argv[2]));
  double sogliaa(std::stod(argv[3]));

  TFile *file = TFile::Open(fileName.c_str());
  TTree *tree = (TTree *) file->Get("tree");
  float vcharge[NCH];
  float letime[NCH];

  int scint[NCH]={0};
  tree->SetBranchAddress( "vcharge", &vcharge );
  tree->SetBranchAddress( "letime", &letime );

  int nentries = tree->GetEntries();

  TCanvas* c = new TCanvas ("c2","Graph Draw Options",200,10,600,400);
  TH1D* histo = new TH1D("charge", "charge", NCH , 0, NCH);
  TH1D* histoang = new TH1D("charge", "charge", NCH/2 , -90, 90);
  TH1D* spectrum[NCH];
  for(int ch=0;ch<NCH;ch++)  spectrum[ch] = new TH1D("charge", "charge", 100 , 0, 2000);
  std::string plotsDir(Form("plots/cosmici"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {
    tree->GetEntry(iEntry);
    for(int i=0;i<NCH;i++) scint[i]=0;
    for( int ch=0; ch<NCH; ch++){
      spectrum[ch]->Fill(vcharge[ch]);
      if( (vcharge[ch]<sogliaa) && (vcharge[ch]>sogliab) ){
	//histo->Fill(ch);
	scint[ch]=1;
      }
    }
    for(int i=0;i<NCH;i++){
      if(scint[i]==1){
	histo->Fill(i);
	//histo->Fill(i+1);
      }
    }
    for(int i=0;i<NCH;i+=2){
      if(scint[i]==1&&scint[i+1]==1){
	histoang->Fill(i/2*(22.5)-90);
      }
    }
    std::cout<<std::endl;
  }

  histo->GetXaxis()->SetTitle("canali");
  histo->GetYaxis()->SetTitle("numero scintillazioni");
  histo->Draw();
  c->SaveAs(Form("plots/cosmici/distribuzione_angolare_all_channels.root"));
  c->SaveAs(Form("plots/cosmici/distribuzione_angolare_all_channels.pdf"));
  std::cout << "-> histo saved in: plots/cosmici/distribuzione_angolare_all_channels.pdf and plots/cosmici/distribuzione_angolare_all_channels.root" << std::endl;

  TCanvas* c1 = new TCanvas ("c2","Graph Draw Options",200,10,600,400);
  c1->SetGrid();
  histoang->GetXaxis()->SetTitle("angoli (gradi)");
  histoang->GetYaxis()->SetTitle("numero scintillazioni");
  histoang->SetTitle("Distribuzione angolare dei raggi cosmici");
  histoang->GetXaxis()->SetNdivisions(508,"kFALSE");
  histoang->SetNdivisions(508,"kFALSE");
  histoang->Draw("E");
  c1->SaveAs(Form("plots/cosmici/distribuzione_angolare.root"));
  c1->SaveAs(Form("plots/cosmici/distribuzione_angolare.pdf"));
  std::cout << "-> histo saved in: plots/cosmici/distribuzione_angolare.pdf and plots/cosmici/distribuzione_angolare.pdf" << std::endl;

  TCanvas* c2;
  for(int ch=0; ch<NCH; ch++){
    c2 = new TCanvas ("c2","Graph Draw Options",200,10,600,400);
    spectrum[ch]->GetXaxis()->SetTitle("charge");
    spectrum[ch]->GetYaxis()->SetTitle("numero di eventi");
    spectrum[ch]->Draw();
    c2->SaveAs(Form("plots/cosmici/spettro_ch_%d.root",ch));
    c2->SaveAs(Form("plots/cosmici/spettro_ch_%d.pdf",ch));
    delete c2;
  }
 
  return 0;
  
}
