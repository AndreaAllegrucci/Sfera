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
#define EN 993
#define NRUNCOMP 4
#define NRUNSING 2

int main( int argc, char* argv[]) {
  
  if( argc != 1 ) { 
    std::cout << "USAGE: ./StarterCesio137" << std::endl;
    exit(1);     
  }
  int ch;
  int Run;

  double peak;
  double charge[NRUNCOMP+NRUNSING][NCH];
  double chargetemp[NCH];
  double ercharge[NRUNCOMP+NRUNSING][NCH];
  double chargeNC[NRUNCOMP+NRUNSING][NCH];
  double erchargeNC[NRUNCOMP+NRUNSING][NCH];
  double x[NCH]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  double day[33]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
    
  double cal[NCH];
  double media[NRUNCOMP+NRUNSING] = {0.};
  double mediaNC[NRUNCOMP+NRUNSING] = {0.};
  double stdev[NRUNCOMP+NRUNSING] = {0.};
  double stdevNC[NRUNCOMP+NRUNSING] = {0.};
  double stdevday[33];

  double stattemp;
  std::vector< std::string > filename;
  filename.push_back("Canali_Calibrati_Measurements_Only_4_4_2018.root");
  filename.push_back("Cesio_AllChannels_Measurements_Only_4_18_2018.root");
  filename.push_back("Cesio_AllChannels_Measurements_Only_4_19_2018.root");
  filename.push_back("Cesio_AllChannels_Measurements_Only_5_2_2018.root");
  filename.push_back("Ch_$_Measurements_Only_5_2_2018.root");
  filename.push_back("Cesio_Ch_$_Measurements_Only_5_4_2018.root");

  TH1F *histo;
  TFile *f;
  TCanvas *c;
  TTree* tree;
  TDirectory *where;
  TF1* fitg;
  TPaveText *pv;
  size_t pos=0;
  std::string plotsDir(Form("plots/CalibrazioneCesio137_finale"));
  system( Form("mkdir -p %s", plotsDir.c_str()) );

		     for(Run=0;Run<NRUNCOMP;Run++){
    for(ch=0; ch<NCH; ch++){
      where = gDirectory;
      histo = new TH1F("charge", "charge", NBIN, MIN, MAX);
      f = TFile::Open(filename[Run].c_str());
      tree = (TTree*)f->Get("tree");
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
      c->SaveAs(Form("plots/CalibrazioneCesio137_finale/Spettro_Ch_%d_Run_%d.pdf", ch, Run));
      chargeNC[Run][ch]=fitg->GetParameter(1);
      erchargeNC[Run][ch]=fitg->GetParError(1);
      delete histo;
      delete c;
      delete fitg;
      delete tree;
      f->Close();
    }
    /*
    if(Run==3){
      for(ch=0;ch<NCH;ch++){
	charge[Run][ch]=chargeNC[Run][ch];
	ercharge[Run][ch]=erchargeNC[Run][ch];
	cal[ch]=EN/charge[Run][ch];
      }
    }
    for(ch=0;ch<NCH;ch++){
      charge[Run][ch]=chargeNC[Run][ch]*cal[ch];
      ercharge[Run][ch]=erchargeNC[Run][ch]*cal[ch];
    }
    for(ch=0;ch<NCH;ch++){
	media[Run]+=charge[Run][ch];
    }
    media[Run]=media[Run]-charge[Run][2]-charge[Run][13];
    media[Run]=media[Run]/(NCH-2);
    for(ch=0;ch<NCH;ch++){
      stdev[Run]+=pow((charge[Run][ch]-media[Run]),2);
    }
    
    stdev[Run]=stdev[Run]-pow((charge[Run][2]-media[Run]),2)-pow((charge[Run][13]-media[Run]),2);
    stdev[Run]=sqrt(stdev[Run]/(NCH-3));

 
    for(ch=0;ch<NCH;ch++){
      mediaNC[Run]+=chargeNC[Run][ch];
    }
    mediaNC[Run]=mediaNC[Run]-chargeNC[Run][2]-chargeNC[Run][13];
    mediaNC[Run]=mediaNC[Run]/(NCH-2);

    for(ch=0;ch<NCH;ch++){
      stdevNC[Run]+=pow((chargeNC[Run][ch]-mediaNC[Run]),2);
    }

    stdevNC[Run]=stdevNC[Run]-pow((chargeNC[Run][2]-mediaNC[Run]),2)-pow((chargeNC[Run][13]-mediaNC[Run]),2);
    stdevNC[Run]=sqrt(stdevNC[Run]/(NCH-3));
    */
  }
  

  for(Run=NRUNCOMP; Run<NRUNCOMP+NRUNSING; Run++){
    for(ch=0; ch<NCH; ch++){
      if(ch==0) pos = filename[Run].find("$");
      if(ch<=10) filename[Run].replace(pos, 1, Form("%d",ch));
      if(ch>10) filename[Run].replace(pos, 2, Form("%d",ch));
      where = gDirectory;
      histo = new TH1F("charge", "charge", NBIN, MIN, MAX);
      f = TFile::Open(filename[Run].c_str());
      tree = (TTree*)f->Get("tree");
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
      c->SaveAs(Form("plots/CalibrazioneCesio137_finale/Spettro_Ch_%d_Run_%d.pdf", ch, Run));
      chargeNC[Run][ch]=fitg->GetParameter(1);
      erchargeNC[Run][ch]=fitg->GetParError(1);
      delete histo;
      delete c;
      delete fitg;
      delete tree;
      f->Close();
    }

    
    /*   
    for(ch=0;ch<NCH;ch++){
      charge[Run][ch]=chargeNC[Run][ch]*cal[ch];
      ercharge[Run][ch]=erchargeNC[Run][ch]*cal[ch];
    }
    
    for(ch=0;ch<NCH;ch++){
      media[Run]+=charge[Run][ch];
    }
    media[Run]=media[Run]-charge[Run][2]-charge[Run][13];
    media[Run]=media[Run]/(NCH-2);

    for(ch=0;ch<NCH;ch++){
      stdev[Run]+=pow((charge[Run][ch]-media[Run]),2);
    }
    
    stdev[Run]=stdev[Run]-pow((charge[Run][2]-media[Run]),2)-pow((charge[Run][13]-media[Run]),2);
    stdev[Run]=sqrt(stdev[Run]/(NCH-3));
    
    
    for(ch=0;ch<NCH;ch++){
      mediaNC[Run]+=chargeNC[Run][ch];
    }
    mediaNC[Run]=mediaNC[Run]-chargeNC[Run][2]-chargeNC[Run][13];
    mediaNC[Run]=mediaNC[Run]/(NCH-2);

    for(ch=0;ch<NCH;ch++){
      stdevNC[Run]+=pow((chargeNC[Run][ch]-mediaNC[Run]),2);
    }
    stdevNC[Run]=stdevNC[Run]-pow((chargeNC[Run][2]-mediaNC[Run]),2)-pow((chargeNC[Run][13]-mediaNC[Run]),2);
    stdevNC[Run]=sqrt(stdevNC[Run]/(NCH-3));*/
  }
  std::cout<<"OKKKKKKKKKKKKKKKK"<<std::endl;
	     
  for(ch=0;ch<NCH;ch++){
    charge[3][ch]=chargeNC[3][ch];
    ercharge[3][ch]=erchargeNC[3][ch];
    cal[ch]=EN/charge[3][ch];
  }

  for(Run=0;Run<NRUNCOMP+NRUNSING;Run++){
    for(ch=0;ch<NCH;ch++){
      charge[Run][ch]=chargeNC[Run][ch]*cal[ch];
      ercharge[Run][ch]=erchargeNC[Run][ch]*cal[ch];
    }
    for(ch=0;ch<NCH;ch++){
	media[Run]+=charge[Run][ch];
    }
    media[Run]=media[Run]/NCH;
    for(ch=0;ch<NCH;ch++){
      stdev[Run]+=pow((charge[Run][ch]-media[Run]),2);
    }
    
    stdev[Run]=sqrt(stdev[Run]/(NCH-1));

 
    for(ch=0;ch<NCH;ch++){
      mediaNC[Run]+=chargeNC[Run][ch];
    }
    mediaNC[Run]=mediaNC[Run]-chargeNC[Run][2]-chargeNC[Run][13];
    mediaNC[Run]=mediaNC[Run]/(NCH-2);

    for(ch=0;ch<NCH;ch++){
      stdevNC[Run]+=pow((chargeNC[Run][ch]-mediaNC[Run]),2);
    }

    stdevNC[Run]=stdevNC[Run]-pow((chargeNC[Run][2]-mediaNC[Run]),2)-pow((chargeNC[Run][13]-mediaNC[Run]),2);
    stdevNC[Run]=sqrt(stdevNC[Run]/(NCH-3));
  }

  
  for(ch=0; ch<NCH; ch++){
    chargetemp[ch]=charge[3][ch];
    charge[3][ch]=charge[4][ch];
    charge[4][ch]=chargetemp[ch];
  }
  for(ch=0; ch<NCH; ch++){
    chargetemp[ch]=ercharge[3][ch];
    ercharge[3][ch]=ercharge[4][ch];
    ercharge[4][ch]=chargetemp[ch];
  }
  for(ch=0; ch<NCH; ch++){
    chargetemp[ch]=chargeNC[3][ch];
    chargeNC[3][ch]=chargeNC[4][ch];
    chargeNC[4][ch]=chargetemp[ch];
  }
  for(ch=0; ch<NCH; ch++){
    chargetemp[ch]=erchargeNC[3][ch];
    erchargeNC[3][ch]=erchargeNC[4][ch];
    erchargeNC[4][ch]=chargetemp[ch];
  }
  stattemp=media[3];
  media[3]=media[4];
  media[4]=stattemp;
  stattemp=mediaNC[3];
  mediaNC[3]=mediaNC[4];
  mediaNC[4]=stattemp;
  stattemp=stdev[3];
  stdev[3]=stdev[4];
  stdev[4]=stattemp;
  stattemp=stdevNC[3];
  stdevNC[3]=stdevNC[4];
  stdevNC[4]=stattemp;
  std::cout<<"OKKKKKKKKKKKKKKKK"<<std::endl;

  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  
  TMultiGraph* mgNC  = new TMultiGraph();
  
  TGraph* grNC[NRUNCOMP+NRUNSING];
  for(Run=0;Run<NRUNCOMP+NRUNSING;Run++){
    grNC[Run] = new TGraph(NCH,x,chargeNC[Run]);
    grNC[Run]->SetMarkerStyle(8);
    grNC[Run]->SetMarkerSize(0.5);
    mgNC->Add(grNC[Run]);
  }
  grNC[0]->SetMarkerColor(2);
  grNC[1]->SetMarkerColor(3);
  grNC[2]->SetMarkerColor(4);
  grNC[3]->SetMarkerColor(6);
  grNC[4]->SetMarkerColor(7);
  grNC[5]->SetMarkerColor(28);

  mgNC->SetTitle(Form("INTERCALIBRAZIONE CANALI"));
  mgNC->GetXaxis()->SetTitle("canale");
  mgNC->GetYaxis()->SetTitle("carica picco fotoelettrico [pC]");
  mgNC->GetYaxis()->SetRangeUser(0,1500);

  mgNC->Draw("AP");
  std::vector< std::string > legendNC;
  legendNC.push_back(Form("Run 1: Cesio 4 aprile: media = %lf; stdev = %lf",mediaNC[0],stdevNC[0]));
  legendNC.push_back(Form("Run 2: Cesio 19 aprile: media = %lf; stdev = %lf",mediaNC[1],stdevNC[1]));
  legendNC.push_back(Form("Run 3: Cesio 20 aprile: media = %lf; stdev = %lf",mediaNC[2],stdevNC[2]));
  legendNC.push_back(Form("Run 4: Cesio 2 maggio: media = %lf; stdev = %lf",mediaNC[3],stdevNC[3]));
  legendNC.push_back(Form("Run 5: Cesio 3 maggio: media = %lf; stdev = %lf",mediaNC[4],stdevNC[4]));
  legendNC.push_back(Form("Run 6: Cesio 4 maggio: media = %lf; stdev = %lf",mediaNC[5],stdevNC[5]));
  
  TLegend* legNC = new TLegend(0.2,0.2,0.8,0.45);
  for(Run=0;Run<NRUNCOMP+NRUNSING;Run++) legNC->AddEntry(grNC[Run], legendNC[Run].c_str() ,"p");
  legNC->Draw();
  
  c->SaveAs(Form("plots/CalibrazioneCesio137_finale/intercalibrazione.root"));
  c->SaveAs(Form("plots/CalibrazioneCesio137_finale/intercalibrazione.pdf"));
  
  delete c;

  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  /*for(int i=0; i<33; i++){
    stdevday[i]=NULL;
    }*/
  stdevday[0] = stdevNC[0];
  stdevday[1] = stdevNC[1];
  stdevday[2] = stdevNC[2];
  stdevday[3] = stdevNC[3];
  stdevday[4] = stdevNC[4];
  stdevday[5] = stdevNC[5];
  TGraph* stdvst;
  double x2[6]={1,2,3,4,5,6};
  stdvst = new TGraph(NRUNCOMP+NRUNSING,x2,stdevday);
  stdvst->SetMarkerStyle(8);
  stdvst->SetMarkerSize(0.5);
  stdvst->SetMarkerColor(2);
  stdvst->SetTitle(Form("dispersione nel tempo"));
  stdvst->GetXaxis()->SetTitle("run");
  stdvst->GetYaxis()->SetTitle("deviazione standard [pC]");
  stdvst->GetYaxis()->SetRangeUser(0,75);
  stdvst->Draw("AP");
  c->SaveAs(Form("plots/CalibrazioneCesio137_finale/stdevVsT.root"));
  c->SaveAs(Form("plots/CalibrazioneCesio137_finale/stdevVsT.pdf"));
  
  
  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  c->SetGrid();
  
  TMultiGraph* mg  = new TMultiGraph();
  
  TGraph* gr[2];
  for(Run=0;Run<2;Run++){
    gr[Run] = new TGraph(NCH,x,charge[Run+4]);
    gr[Run]->SetMarkerStyle(8);
    gr[Run]->SetMarkerSize(0.5);
    mg->Add(gr[Run]);
  }
  gr[0]->SetMarkerColor(2);
  gr[1]->SetMarkerColor(3);


  mg->SetTitle(Form("INTERCALIBRAZIONE CANALI"));
  mg->GetXaxis()->SetTitle("canale");
  mg->GetYaxis()->SetTitle("carica picco fotoelettrico [pC]");
  mg->GetYaxis()->SetRangeUser(900,1100);

  mg->Draw("AP");

  std::vector< std::string > legend;
  
  legend.push_back(Form("Run 4: Cesio 3 maggio: media = %lf; stdev = %lf",media[4],stdev[4]));
  legend.push_back(Form("Run 5: Cesio 4 maggio: media = %lf; stdev = %lf",media[5],stdev[5]));

  TLegend* leg = new TLegend(0.2,0.65,0.8,0.85);
  for(Run=0;Run<2;Run++) leg->AddEntry(gr[Run], legend[Run].c_str() ,"p");

  leg->Draw();

  c->SaveAs(Form("plots/CalibrazioneCesio137_finale/intercalibrazioneCorretta.root"));
  c->SaveAs(Form("plots/CalibrazioneCesio137_finale/intercalibrazioneCorretta.pdf"));

  delete c;

  /*
  
  c = new TCanvas ("c","Graph Draw Options",200,10,600,400);
  TMultiGraph  *mgNC  = new TMultiGraph();
  //TGraphErrors* gr1 = new TGraphErrors(NCH,x,charge,ex,ercharge);
  //TGraphErrors* gr2 = new TGraphErrors(NCH,x,charge2,ex,ercharge2);
  //TGraphErrors* gr3 = new TGraphErrors(NCH,x,charge3,ex,ercharge3);
  //TGraphErrors* gr4 = new TGraphErrors(NCH,x,charge4,ex,ercharge4);
  //TGraphErrors* gr5 = new TGraphErrors(NCH,x,charge5,ex,ercharge5);
  TGraph* gr1NC = new TGraph(NCH,x,chargeNC[0]);
  TGraph* gr2NC = new TGraph(NCH,x,chargeNC[1]);
  TGraph* gr3NC = new TGraph(NCH,x,chargeNC[2]);
  TGraph* gr4NC = new TGraph(NCH,x,chargeNC[3]);
  TGraph* gr5NC = new TGraph(NCH,x,chargeNC[4]);
  TGraph* gr6NC = new TGraph(NCH,x,chargeNC[5]);
  TGraph* gr7NC = new TGraph(NCH,x,chargeNC[6]);
  c->SetGrid();
  gr1NC->SetMarkerStyle(8);
  gr1NC->SetMarkerSize(0.5);
  gr1NC->SetMarkerColor(2);
  gr2NC->SetMarkerStyle(8);
  gr2NC->SetMarkerSize(0.5);
  gr2NC->SetMarkerColor(3);
  gr3NC->SetMarkerStyle(8);
  gr3NC->SetMarkerSize(0.5);
  gr3NC->SetMarkerColor(7);
  gr4NC->SetMarkerStyle(8);
  gr4NC->SetMarkerSize(0.5);
  gr4NC->SetMarkerColor(4);
  gr5NC->SetMarkerStyle(8);
  gr5NC->SetMarkerSize(0.5);
  gr5NC->SetMarkerColor(5);
  gr6NC->SetMarkerStyle(8);
  gr6NC->SetMarkerSize(0.5);
  gr6NC->SetMarkerColor(6);
  gr7NC->SetMarkerStyle(8);
  gr7NC->SetMarkerSize(0.5);
  gr7NC->SetMarkerColor(8);
  
  mgNC->SetTitle(Form("INTERCALIBRAZIONE CANALI"));
  mgNC->GetXaxis()->SetTitle("canali");
  mgNC->GetYaxis()->SetTitle("carica picco [pC]");
  mgNC->Add(gr1NC);
  mgNC->Add(gr2NC);
  mgNC->Add(gr3NC);
  mgNC->Add(gr4NC);
  mgNC->Add(gr5NC);
  mgNC->Add(gr6NC);
  mgNC->Add(gr7NC);
  

  mgNC->Draw("AP");
  

  TLegend* legNC = new TLegend(0.3,0.65,0.65,0.85);
  //leg->AddEntry(f1,"fit lineare","l");
  legNC->AddEntry(gr1NC,Form("18-19 aprile iniziali: media = %lf; stdev = %lf",mediaNC[0],stdevNC[0]),"p");
  legNC->AddEntry(gr2NC,Form("19-20 aprile: media = %lf; stdev = %lf",mediaNC[1],stdevNC[1]),"p");
  legNC->AddEntry(gr4NC,Form("23 aprile (1): media = %lf; stdev = %lf",mediaNC[3],stdevNC[3]),"p");
  legNC->AddEntry(gr5NC,Form("23 aprile (2): media = %lf; stdev = %lf",mediaNC[4],stdevNC[4]),"p");
  legNC->AddEntry(gr6NC,Form("2 maggio: media = %lf; stdev = %lf",mediaNC[5],stdevNC[5]),"p");
  legNC->AddEntry(gr3NC,Form("2-3-4  maggio: media = %lf; stdev = %lf",mediaNC[2],stdevNC[2]),"p");
  legNC->AddEntry(gr7NC,Form("4 maggio: media = %lf; stdev = %lf",mediaNC[6],stdevNC[6]),"p");
  //leg->AddEntry(chargerr,"errori di calibrazione","e");
  legNC->Draw();


  
  c->SaveAs(Form("plots/CalibrazioneCesio137/intercalibrazione.root"));
  c->SaveAs(Form("plots/CalibrazioneCesio137/intercalibrazione.pdf"));
  */
  return 0;
}
