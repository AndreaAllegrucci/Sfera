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
#define NRUNCOMP 3 // 3 cesio
#define NRUNSING 0
#define NPART 3

int main( int argc, char* argv[]) {
  
  if( argc != 1 ) { 
    std::cout << "USAGE: ./StarterCesio137" << std::endl;
    exit(1);     
  }
  int ch;
  int Run;
  int Part;

  double peak;
  double charge[NRUNCOMP+NRUNSING][NPART][NCH];
  double ercharge[NRUNCOMP+NRUNSING][NPART][NCH];
  double chargeNC[NRUNCOMP+NRUNSING][NPART][NCH];
  double erchargeNC[NRUNCOMP+NRUNSING][NPART][NCH];
  double x[NCH]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  double cal[NCH];
  double media[NRUNCOMP+NRUNSING][NPART];
  double mediaNC[NRUNCOMP+NRUNSING][NPART];
  double stdev[NRUNCOMP+NRUNSING][NPART];
  double stdevNC[NRUNCOMP+NRUNSING][NPART];
  for(int i=0;i<NRUNCOMP+NRUNSING;i++){
    for(int j=0;j<NPART;j++){
      media[i][j]=0.;
      mediaNC[i][j]=0.;
      stdev[i][j]=0.;
      stdevNC[i][j]=0.;
    }
  }
  std::vector< std::string > filename;
  filename.push_back("Cesio_AllChannels_Measurements_Only_5_2_2018.root");
  filename.push_back("Cesio_AllChannels_Measurements_Only_5_4_2018.root");
  filename.push_back("Cesio_AllChannels_Measurements_Only_5_7_2018.root");
 

  TH1F *histo;
  TFile *f;
  TCanvas *c;
  TTree* tree;
  TDirectory *where;
  TF1* fitg;
  TPaveText *pv;
  std::string plotsDir(Form("plots/CalibrazioneCesio137_long"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

  for(Run=0;Run<NRUNCOMP;Run++){
    for(Part=0;Part<NPART;Part++){
      for(ch=0; ch<NCH; ch++){
	where = gDirectory;
	histo = new TH1F("charge", "charge", NBIN, MIN, MAX);
	f = TFile::Open(filename[Run].c_str());
	tree = (TTree*)f->Get(Form("tree%d",Part+1));
	where->cd();
	tree->Project("charge", Form("vcharge[%d]", ch));
	histo->GetXaxis()->SetRange(80,NBIN);
	peak = histo->GetMaximumBin();
	peak=peak*10;
	histo->GetXaxis()->SetRange(15,NBIN);
	c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
	fitg = new TF1("fitg", "gaus", (peak+(8./100.)*peak), (peak-(6./100.)*peak));
	histo->Fit(fitg, "R");
	histo->SetTitle(Form("SPETTRO CANALE %d cesio137", ch));
	histo->GetXaxis()->SetTitle("carica (pC)");
	histo->GetYaxis()->SetTitle("nEv");
	histo->GetXaxis()->CenterTitle();
	histo->GetYaxis()->CenterTitle();
	histo->Draw();
	pv = new TPaveText(.1,.9,.3,.75,"brNDC");
	pv->AddText("FIT GAUSSIANO");
	pv->AddLine(.0,.67,1.,.67);
	pv->AddText(Form("mean: %f +/- %f", fitg->GetParameter(1),fitg->GetParError(1)));
	pv->AddText(Form("stdev: %f +/- %f", fitg->GetParameter(2),fitg->GetParError(2)));
	pv->Draw();
	c->SaveAs(Form("plots/CalibrazioneCesio137_long/Spettro_Ch_%d_Run_%d_Part_%d.pdf", ch, Run, Part));
	chargeNC[Run][Part][ch]=fitg->GetParameter(1);
	erchargeNC[Run][Part][ch]=fitg->GetParError(1);
	delete histo;
	delete c;
	delete fitg;
	delete tree;
	f->Close();
      }
    }
    for(Part=0;Part<NPART;Part++){
      if(Run==0&&Part==0){
	for(ch=0;ch<NCH;ch++){
	  charge[Run][Part][ch]=chargeNC[Run][Part][ch];
	  ercharge[Run][Part][ch]=erchargeNC[Run][Part][ch];
	  cal[ch]=EN/charge[Run][Part][ch];
	}
      }
    }
    for(Part=0;Part<NPART;Part++){
      for(ch=0;ch<NCH;ch++){
	charge[Run][Part][ch]=chargeNC[Run][Part][ch]*cal[ch];
	ercharge[Run][Part][ch]=erchargeNC[Run][Part][ch]*cal[ch];
	std::cout<<cal[ch]<<std::endl;
      }
    }
    for(Part=0;Part<NPART;Part++){
      for(ch=0;ch<NCH;ch++){
	media[Run][Part]+=charge[Run][Part][ch];
	
      }
    }
    for(Part=0;Part<NPART;Part++){
      media[Run][Part]=media[Run][Part]/(NCH);
      std::cout<<media[Run][Part]<<std::endl;
      for(ch=0;ch<NCH;ch++){
	stdev[Run][Part]+=pow((charge[Run][Part][ch]-media[Run][Part]),2);
      }
      stdev[Run][Part]=sqrt(stdev[Run][Part]/(NCH-1));
    }
    for(Part=0;Part<NPART;Part++){
      for(ch=0;ch<NCH;ch++){
	mediaNC[Run][Part]+=chargeNC[Run][Part][ch];
      }
      mediaNC[Run][Part]=mediaNC[Run][Part]/(NCH);
    
      for(ch=0;ch<NCH;ch++){
	stdevNC[Run][Part]+=pow((chargeNC[Run][Part][ch]-mediaNC[Run][Part]),2);
      }
      stdevNC[Run][Part]=sqrt(stdevNC[Run][Part]/(NCH-1));
    }
  }

  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  
  TMultiGraph* mg  = new TMultiGraph();
  TGraph* gr[NRUNCOMP][NPART];
  for(Run=0;Run<NRUNCOMP;Run++){
    for(Part=0;Part<NPART;Part++){
      gr[Run][Part] = new TGraph(NCH,x,charge[Run][Part]);
      gr[Run][Part]->SetMarkerStyle(8);
      gr[Run][Part]->SetMarkerSize(0.5);
      gr[Run][Part]->SetMarkerColor(Run+1);
      mg->Add(gr[Run][Part]);
    }
  }
  
  mg->SetTitle(Form("INTERCALIBRAZIONE CANALI"));
  mg->GetXaxis()->SetTitle("canali");
  mg->GetYaxis()->SetTitle("carica picco [pC]");

  mg->Draw("AP");

  std::vector< std::string > legend;
  legend.push_back(Form("Cesio 2 maggio parte 1: media = %lf; stdev = %lf",media[0][0],stdev[0][0]));
  legend.push_back(Form("Cesio 2 maggio parte 2: media = %lf; stdev = %lf",media[0][1],stdev[0][1]));
  legend.push_back(Form("Cesio 2 maggio parte 3: media = %lf; stdev = %lf",media[0][2],stdev[0][2]));
  legend.push_back(Form("Cesio 4 maggio parte 1: media = %lf; stdev = %lf",media[1][0],stdev[1][0]));
  legend.push_back(Form("Cesio 4 maggio parte 2: media = %lf; stdev = %lf",media[1][1],stdev[1][1]));
  legend.push_back(Form("Cesio 4 maggio parte 3: media = %lf; stdev = %lf",media[1][2],stdev[1][2]));
  legend.push_back(Form("Cesio 7 maggio parte 1: media = %lf; stdev = %lf",media[2][0],stdev[2][0]));
  legend.push_back(Form("Cesio 7 maggio parte 2: media = %lf; stdev = %lf",media[2][1],stdev[2][1]));
  legend.push_back(Form("Cesio 7 maggio parte 3: media = %lf; stdev = %lf",media[2][2],stdev[2][2]));
  
  TLegend* leg = new TLegend(0.3,0.65,0.65,0.85);
  leg->AddEntry(gr[0][0], legend[0].c_str() ,"p");
  leg->AddEntry(gr[0][1], legend[1].c_str() ,"p");
  leg->AddEntry(gr[0][2], legend[2].c_str() ,"p");
  leg->AddEntry(gr[1][0], legend[3].c_str() ,"p");
  leg->AddEntry(gr[1][1], legend[4].c_str() ,"p");
  leg->AddEntry(gr[1][2], legend[5].c_str() ,"p");
  leg->AddEntry(gr[2][0], legend[6].c_str() ,"p");
  leg->AddEntry(gr[2][1], legend[7].c_str() ,"p");
  leg->AddEntry(gr[2][2], legend[8].c_str() ,"p");
  leg->Draw();
  
  c->SaveAs(Form("plots/CalibrazioneCesio137_long/intercalibrazioneCorretta.pdf"));
  c->SaveAs(Form("plots/CalibrazioneCesio137_long/intercalibrazioneCorretta.root"));

  delete c;
  delete mg;
  delete leg;


  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  mg  = new TMultiGraph();
  TGraph* grNC[NRUNCOMP][NPART];
  for(Run=0;Run<NRUNCOMP;Run++){
    for(Part=0;Part<NPART;Part++){
      grNC[Run][Part] = new TGraph(NCH,x,chargeNC[Run][Part]);
      grNC[Run][Part]->SetMarkerStyle(8);
      grNC[Run][Part]->SetMarkerSize(0.5);
      grNC[Run][Part]->SetMarkerColor(Run+1);
      mg->Add(grNC[Run][Part]);
    }
  }

  mg->SetTitle(Form("INTERCALIBRAZIONE CANALI"));
  mg->GetXaxis()->SetTitle("canali");
  mg->GetYaxis()->SetTitle("carica picco [pC]");

  mg->Draw("AP");

  std::vector< std::string > legendNC;
  legendNC.push_back(Form("Cesio 2 maggio parte 1: media = %lf; stdev = %lf",mediaNC[0][0],stdevNC[0][0]));
  legendNC.push_back(Form("Cesio 2 maggio parte 2: media = %lf; stdev = %lf",mediaNC[0][1],stdevNC[0][1]));
  legendNC.push_back(Form("Cesio 2 maggio parte 3: media = %lf; stdev = %lf",mediaNC[0][2],stdevNC[0][2]));
  legendNC.push_back(Form("Cesio 4 maggio parte 1: media = %lf; stdev = %lf",mediaNC[1][0],stdevNC[1][0]));
  legendNC.push_back(Form("Cesio 4 maggio parte 2: media = %lf; stdev = %lf",mediaNC[1][1],stdevNC[1][1]));
  legendNC.push_back(Form("Cesio 4 maggio parte 3: media = %lf; stdev = %lf",mediaNC[1][2],stdevNC[1][2]));
  legendNC.push_back(Form("Cesio 7 maggio parte 1: media = %lf; stdev = %lf",mediaNC[2][0],stdevNC[2][0]));
  legendNC.push_back(Form("Cesio 7 maggio parte 2: media = %lf; stdev = %lf",mediaNC[2][1],stdevNC[2][1]));
  legendNC.push_back(Form("Cesio 7 maggio parte 3: media = %lf; stdev = %lf",mediaNC[2][2],stdevNC[2][2]));

  leg = new TLegend(0.3,0.65,0.65,0.85);
  leg->AddEntry(grNC[0][0], legendNC[0].c_str() ,"p");
  leg->AddEntry(grNC[0][1], legendNC[1].c_str() ,"p");
  leg->AddEntry(grNC[0][2], legendNC[2].c_str() ,"p");
  leg->AddEntry(grNC[1][0], legendNC[3].c_str() ,"p");
  leg->AddEntry(grNC[1][1], legendNC[4].c_str() ,"p");
  leg->AddEntry(grNC[1][2], legendNC[5].c_str() ,"p");
  leg->AddEntry(grNC[2][0], legendNC[6].c_str() ,"p");
  leg->AddEntry(grNC[2][1], legendNC[7].c_str() ,"p");
  leg->AddEntry(grNC[2][2], legendNC[8].c_str() ,"p");
  leg->Draw();
  
  c->SaveAs(Form("plots/CalibrazioneCesio137_long/intercalibrazione.pdf"));
  c->SaveAs(Form("plots/CalibrazioneCesio137_long/intercalibrazione.root"));

  delete c;

  
  double time[9]={7.1,21.35,35.75,58.6,79.8,102.6,122.4,137,152.1};
  double ertime[9]={7.1,7.15,7.25,10.6,10.6,12.2,7.3,7.3,7.8};

  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  
  TMultiGraph* mt  = new TMultiGraph();
  TGraphErrors* gt[NCH];
  double charget[NCH][NRUNCOMP*NPART];
  double ercharget[NCH][NRUNCOMP*NPART];
  for(ch=0;ch<NCH;ch++){
    for(Run=0;Run<NRUNCOMP;Run++){
      for(Part=0;Part<NPART;Part++){
	charget[ch][NRUNCOMP*Run+Part]=chargeNC[Run][Part][ch];
	ercharget[ch][NRUNCOMP*Run+Part]=erchargeNC[Run][Part][ch];
      }
    }
  }
  double charge0[NCH] = {0.};
  for(ch=0;ch<NCH;ch++){   
    charge0[ch] = charget[ch][0];
  }
  for(ch=0;ch<NCH;ch++){
    for(Run=0;Run<NRUNCOMP;Run++){
      for(Part=0;Part<NPART;Part++){
	charget[ch][NRUNCOMP*Run+Part]=charget[ch][NRUNCOMP*Run+Part]-charge0[ch];
      }
    }
  }
  std::cout<<charge[2][1][11]<<"         "<<charget[11][7]<<std::endl;
  for(ch=0;ch<NCH;ch++){
    gt[ch] = new TGraphErrors(9,time,charget[ch],ertime,ercharget[ch]);
    gt[ch]->SetLineStyle(1);
    gt[ch]->SetLineWidth(2);
    gt[ch]->SetLineColor(ch+1);
    mt->Add(gt[ch]);
  }

  mt->SetTitle(Form("INTERCALIBRAZIONE CANALI"));
  mt->GetXaxis()->SetTitle("canali");
  mt->GetYaxis()->SetTitle("carica picco [pC]");

  mt->Draw("AP");

  c->SaveAs(Form("plots/CalibrazioneCesio137_long/ChVsTime.pdf"));
  c->SaveAs(Form("plots/CalibrazioneCesio137_long/ChVsTime.root"));
  delete c;
  double mediaCH[NCH] = {0.};
  double stdevCH[NCH] = {0.};
  for(ch=0;ch<NCH;ch++){
    for(int i=0; i<NRUNCOMP*NPART; i++){
      mediaCH[ch]+=charget[ch][i]/(NRUNCOMP*NPART);      
    }
  }
  for(ch=0;ch<NCH;ch++){
    for(int i=0; i<NRUNCOMP*NPART; i++){
      stdevCH[ch]+=pow((charget[ch][i]-mediaCH[ch]),2)/(NRUNCOMP*NPART-1);     
    }
  }
  for(ch=0;ch<NCH;ch++){
    stdevCH[ch]=sqrt(stdevCH[ch]);
  }
  
  for(ch=0;ch<NCH;ch++){
    c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
    c->SetGrid();
    gt[ch]->SetLineColor(1);
    gt[ch]->Draw("AP");
    gt[ch]->SetTitle(Form("CANALE %d",ch));
    gt[ch]->GetXaxis()->SetTitle("tempo (ore)");
    gt[ch]->GetYaxis()->SetTitle("carica picco corretta [pC]");
    gt[ch]->GetYaxis()->SetRangeUser(-50,+50);
    pv = new TPaveText(.75,.95,.95,.75,"brNDC");
    pv->AddText(Form("mean: %f", mediaCH[ch]));
    pv->AddText(Form("stdev: %f", stdevCH[ch]));
    pv->Draw();
    c->SaveAs(Form("plots/CalibrazioneCesio137_long/Ch%dVsTime.pdf",ch));
    //c->SaveAs(Form("plots/CalibrazioneCesio137_long/Ch%dVsTime.root",ch));
    delete c;
  }
  return 0;
}
