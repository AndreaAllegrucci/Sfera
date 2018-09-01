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
#define NBIN 150
#define MIN 0
#define MAX 1500
#define EN 1000
#define NRUNCOMP 1 // 3 cesio
#define NRUNSING 0
#define NPART 1

int main( int argc, char* argv[]) {
  double x1[5]={1450, 1500, 1550, 1600, 1650};
  double c1[5]={6.3, 6.63, 6.94, 7.25, 7.53};
  double x2[5]={1650, 1700, 1750, 1800, 1850};
  double c2[5]={7.62, 7.88, 8.14, 8.41, 8.67};
  TCanvas*   c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();;
  TMultiGraph* mg  = new TMultiGraph();
  TGraph* gr1 =new TGraph(5,x1,c1);
  TGraph* gr2 =new TGraph(5,x2,c2);
  TF1 *f1 = new TF1("f1","[0]*x+[1]",0,3000);
  f1->SetLineColor(2);
  f1->SetLineStyle(2);
  f1->SetLineWidth(1);
  gr1->Fit(f1,"R");
  TF1 *f2 = new TF1("f2","[0]*x+[1]",0,3000);
  f2->SetLineColor(4);
  f2->SetLineStyle(2);
  f2->SetLineWidth(1);
  gr2->Fit(f2,"R");
  gr1->SetMarkerStyle(8);
  gr2->SetMarkerStyle(8);
  gr1->SetMarkerColor(2);
  gr2->SetMarkerColor(4);
  gr1->SetMarkerSize(0.8);
  gr2->SetMarkerSize(0.8);
  mg->Add(gr1);
  mg->Add(gr2);
  gr1->GetXaxis()->SetRangeUser(1400,1900);
  gr1->GetYaxis()->SetRangeUser(6.2,9);
  mg->SetTitle(Form("canale 3"));
  mg->GetXaxis()->SetTitle("Tensione di alimentazione (V)");
  mg->GetYaxis()->SetTitle("log(carica picco)");

  mg->Draw("AP");
  TLegend* leg = new TLegend(0.15,0.65,0.55,0.85);
  leg->AddEntry(gr1, "dati del 4 aprile"  ,"p");
  leg->AddEntry(gr2, "dati del 28 marzo","p");
  leg->AddEntry(f1, "retta sui dati del 4 aprile"  ,"l");
  leg->AddEntry(f2, "retta sui dati del 28 marzo","l");
  
  leg->Draw();
  c->SaveAs(Form("plots/2rette.pdf"));
  c->SaveAs(Form("plots/2rette.root"));


  return 0;
}
