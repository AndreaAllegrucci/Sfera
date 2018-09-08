#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TPaveText.h"


#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <tgmath.h>



#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>


int main( int argc, char* argv[]) {
  if( argc != 3 ) { 
    std::cout << "USAGE: ./rootFit [canale] [numero punti]" << std::endl;
    exit(1);     
  }
  int ch = std::stoi(argv[1]);
  int n = std::stoi(argv[2]);
  std::string outfileNameS = "calibrazione_Ch_"+std::to_string(ch)+".pdf";
  const char *outfileName;
  outfileName = (char*)malloc(outfileNameS.size()*sizeof(char));
  outfileName = outfileNameS.c_str();
  std::string titoloS = "CALIBRAZIONE CANALE "+std::to_string(ch);
  const char *titolo;
  titolo = (char*)malloc(titoloS.size()*sizeof(char));
  titolo = titoloS.c_str();

  double x[n];
  double y[n];
  for(int i=0; i<n; i++){
    std::cout<<"x"<<i+1<<": ";
    std::cin>>x[i];
    std::cout<<"y"<<i+1<<": ";
    std::cin>>y[i];
    y[i]=log(y[i]);
  }
  TGraph *gr  = new TGraph(n, x, y);
  TCanvas *c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  gr->SetMarkerStyle(8);
  gr->SetMarkerSize(0.8);
  gr->SetMarkerColor(2);
  gr->SetTitle(titolo);
  gr->GetXaxis()->SetTitle("Voltaggio [Volt]");
  gr->GetYaxis()->SetTitle("Log(carica picco)");
  gr->GetXaxis()->CenterTitle();
  gr->GetYaxis()->CenterTitle();
  TF1 *f1 = new TF1("f1","[0]*x+[1]",0,2000);
  f1->SetLineColor(4);
  gr->Fit(f1,"R");
  TPaveText pv(.1,.9,.25,.75,"brNDC");
  pv.AddText("y = a#upointx + b");
  ((TText*)pv.GetListOfLines()->Last())->SetTextColor(kBlue);
  pv.AddLine(.0,.67,1.,.67);
  pv.AddText(Form("a: %f +/- %f", f1->GetParameter(0),f1->GetParError(0)));
  pv.AddText(Form("b: %f +/- %f", f1->GetParameter(1),f1->GetParError(1)));
  gr->Draw("AP");
  pv.Draw();
  c->SaveAs(outfileName);
  return 0;
}
