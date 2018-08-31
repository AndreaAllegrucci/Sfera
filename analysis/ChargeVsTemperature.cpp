#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <iomanip>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include "TSpectrum.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

#define MAX 5000
#define MIN 50
#define NBIN 500
#define LEN 100 // lunghezza degli array
#define PAR 14 // numero di parametri del fit
#define Nsigma 5

using namespace std;
using namespace boost;

bool isNumber(const string& s);

int main(int argc, char* argv[]) {

	if(argc != 2) {

		cout << "USAGE: ./ChargeVsTemperature [rootfilename]" << endl;

		exit(1);

		}

	string fileName(argv[1]);
  
	TFile* file = TFile::Open(fileName.c_str());

	TTree *tree = (TTree *) file->Get("tree");

	cout << "-> Opened root data file: " << fileName << endl;

	int ev;

	float vcharge, temp, time, rate;

	tree->SetBranchAddress("ev", &ev);

	tree->SetBranchAddress("vcharge", &vcharge);

	tree->SetBranchAddress("temp", &temp);

	tree->SetBranchAddress("time", &time);

	tree->SetBranchAddress("rate", &rate);

	string data = fileName;

	bool bool_var = false;

	unsigned int car = 0;

	while ((bool_var == false && car<=data.length())) {

		if ((data[car] == 'y') && (data[car+1] == '_')) {

			data.erase(data.begin(), data.begin()+car+2);

			data.erase(data.end()-5, data.end());

			bool_var = true;

			}

		car++;

	}

	string dir = "ChargeVsTemperatureDir/" + data;

	string dir_plots = dir + "/Plots";

	string dir_ch_hist = dir_plots + "/ChargeHistograms";

	cout << dir_ch_hist << endl;

	string cmd = "mkdir -p " + dir_ch_hist;

	system (cmd.c_str());

	string pdf, root;

	int k, l;

	double par[PAR], err_par[PAR];

	double maxpos, minpos, leftmax, leftmaxpos, comptonpos;

	TH1F *hcharge = new TH1F("hcharge", "hcharge", NBIN, MIN, MAX);

	int i = 0, i_new = 0;

	float tempMean[LEN], gaussMu[LEN], gaussMuErr[LEN], gaussSigma[LEN], gaussSigmaErr[LEN], res[LEN], chi2_ndf[LEN], prob_fit[LEN];

	double resErr[LEN];

	float tempMean_new[LEN], gaussMu_new[LEN], gaussMuErr_new[LEN], gaussSigma_new[LEN], gaussSigmaErr_new[LEN], res_new[LEN];

	double resErr_new[LEN];

	tree->GetEntry(0);

	tempMean[i] = temp; // acquisizione temperatura associata al blocco di 300 s iniziale

	ofstream textfile, log1, log2, log3;

	textfile.open(dir + "/Data_" + data + ".txt");

	textfile << "Misura \t Temperatura (°C) \t Mu (pC) \t Errore Mu (pC)  Sigma (pC) \t Errore sigma (pC) \t Risoluzione \t Errore risoluzione \n" << endl;

	log1.open(dir + "/log1_" + data + ".txt");

	log1 << "Misura \t [0] \t\t [1] \t\t [2] \t\t [3] \t\t [4] \t\t [5] \t\t [6] \n" << endl;

	log2.open(dir + "/log2_" + data + ".txt");

	log2 << "Misura \t [7] \t\t [8] \t\t [9] \t\t [10] \t\t [11] \t\t [12] \n" << endl;

	log3.open(dir + "/log3_" + data + ".txt");

	log3 << "Misura \t Probabilità del fit \t Chi^2/ndf \n" << endl;

	double time_diff = 0;

	int n;

	int nentries = tree->GetEntries();

	for (n=0; n<nentries; n++) {

		tree->GetEntry(n);

		hcharge->Fill(fabs(vcharge)); // acquisizione cariche associate al blocco di 300 s attuale

		if (time - time_diff >= rate) {

			time_diff += rate; // incrementa di un rate temporale il tempo da sottrarre a time

			TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);

			c1->cd();
					
			c1->SetLeftMargin(0.15);

// inizio fit gaussiano

			TSpectrum *s = new TSpectrum(2);

			s->Search(hcharge, 2, "", 0.1);

			maxpos = s->GetPositionX()[1]; // ascissa del picco gaussiano
  
			hcharge->SetAxisRange(0.5 * maxpos, maxpos, "X");

			minpos = MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMinimumBin(); // ascissa del minimo dell'istogramma compreso tra (0.5 * maxpos, maxpos)

			TF1 *gauss = new TF1("gauss", "gaus", 1.1 * minpos, maxpos + 0.9 * (maxpos - minpos));
  
			hcharge->SetAxisRange(1.1 * minpos, maxpos + 0.9 * (maxpos - minpos), "X");

			hcharge->Fit(gauss, "RQ");

// fine fit gaussiano

// inizio fit totale 1

			hcharge->SetAxisRange(MIN, minpos, "X");

			hcharge->SetLineColor(1);

			leftmax = hcharge->GetMaximum(); // ordinata del massimo dell'istogramma compreso tra (MIN, minpos)

			leftmaxpos = MIN + (double) (MAX - MIN) / NBIN * hcharge->GetMaximumBin(); // ascissa del massimo dell'istogramma compreso tra (MIN, minpos)
	
			hcharge->SetAxisRange(MIN, 1.3 * maxpos, "X");

			comptonpos = gauss->GetParameter(1) * (1 - 1 / (1 + 2 * 662. / 511)); // posizione della fermi-dirac per l'effetto compton
		
			TF1 *all = new TF1("all", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3])) + [4] * exp(-(x - [5]) * (x - [5]) / (2 * [6] * [6])) + [7] / (exp((x - [8]) * [9]) + 1) + [10] / ([12] * exp((x - [13]) * [11]) + 1)", 1.1 * leftmaxpos, 1.3 * maxpos);

			all->SetParLimits(0, 0.5 * leftmax, 10 * leftmax);

			all->SetParLimits(1, 0.0001, 0.005);

			all->SetParLimits(2, 0.1 / comptonpos, 1. / comptonpos);

			all->SetParLimits(3, 1. / comptonpos, 10. / comptonpos);

			all->SetParLimits(4, 0.9 * gauss->GetParameter(0), 1.1 * gauss->GetParameter(0));

			all->SetParLimits(5, 0.9 * gauss->GetParameter(1), 1.1 * gauss->GetParameter(1));

			all->SetParLimits(6, 0.8 * gauss->GetParameter(2), 1.2 * gauss->GetParameter(2));

			all->SetParLimits(7, 0., 0.5 * leftmax);

			all->SetParLimits(8, 0.9 * comptonpos, 1.1 * comptonpos);

			all->SetParLimits(9, 0.005, 0.5);
	
			all->SetParLimits(10, 0., 0.25 * leftmax); 

			all->SetParLimits(11, 0.1, 1.);

			all->SetParLimits(12, 0.01, 20.);

			all->SetParLimits(13, 1.1 * comptonpos, gauss->GetParameter(1));

			all->SetParameter(0, leftmax);

			all->SetParameter(1, 0.001);

			all->SetParameter(2, 0.5 / comptonpos);

			all->SetParameter(3, 5.0 / comptonpos);
					
			all->SetParameter(4, gauss->GetParameter(0));

			all->SetParameter(5, gauss->GetParameter(1));

			all->SetParameter(6, gauss->GetParameter(2));

			all->SetParameter(7, leftmax / 3.);

			all->SetParameter(8, comptonpos);

			all->SetParameter(9, 0.05);

			all->SetParameter(10, 0.05 * leftmax);

			all->SetParameter(11, 0.6);

			all->SetParameter(12, 0.95);

			all->SetParameter(13, 0.9 * gauss->GetParameter(1));
		    
			hcharge->Fit("all", "RQ");

// fine fit totale 1

// inizio fit totale 2

			for (k=0; k<PAR; k++) { // acquisisce i parametri e gli errori sui parametri del primo fit all

				par[k] = all->GetParameter(k);

				err_par[k] = all->GetParError(k);

				}

			TF1 *all_2 = new TF1("all_2", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3])) + [4] * exp(-(x - [5]) * (x - [5]) / (2 * [6] * [6])) + [7] / (exp((x - [8]) * [9]) + 1) + [10] / ([12] * exp((x - [13]) * [11]) + 1)", 1.1 * leftmaxpos, 1.3 * maxpos);

			if (par[5] >= 800) { // scarta i primi valori di carica minori di 800 pC

				l = 0;

				chi2_ndf[0] = 0;

				do { // ciclo che minimizza il chi^2/ndf per i fit successivi, finché non diventa costante

					for (k=0; k<PAR; k++) { // imposta il range dei parametri del nuovo fit all_2 a +- Nsigma * err_par[k] da par[k] e il parametro iniziale del nuovo fit all_2 a par[k], prendendo par[k] di volta in volta dal fit precedente

						all_2->SetParLimits(k, par[k] - Nsigma * err_par[k], par[k] + Nsigma * err_par[k]);

						all_2->SetParameter(k, par[k]);

						}

					hcharge->Fit("all_2", "RQ");

					for (k=0; k<PAR; k++) { // acquisisce i parametri e gli errori sui parametri del nuovo fit all_2

						par[k] = all_2->GetParameter(k);

						err_par[k] = all_2->GetParError(k);

						}

					l++;

					prob_fit[l] = all_2->GetProb();

					chi2_ndf[l] = all_2->GetChisquare() / all_2->GetNDF();

					log3 << i + 1 << " \t " << prob_fit[l] << " \t\t " << chi2_ndf[l] << endl;

					} while (chi2_ndf[l] != chi2_ndf[l-1]); // condizione da verificare per uscire dal ciclo data dalla costanza del chi^2

				}

			log3 << endl;

			TF1 *de = new TF1("de", "[0] * ([1] * exp(-x * [2]) + (1 - [1]) * exp(-x * [3]))", 1.1 * leftmaxpos, 1.3 * gauss->GetParameter(1));

			TF1 *gauss_new = new TF1("gauss_new", "gaus", 1.1 * leftmaxpos, 1.3 * maxpos);

			TF1 *FD = new TF1("FD", "[0] / (exp((x - [1]) * [2]) + 1)", 1.1 * leftmaxpos, 1.3 * maxpos);

			TF1 *FDM = new TF1("FDM", "[0] / ([1] * exp((x - [2]) * [3]) + 1)", 1.1 * leftmaxpos, 1.3 * maxpos);

			de->SetParameters(all_2->GetParameter(0), all_2->GetParameter(1), all_2->GetParameter(2), all_2->GetParameter(3));
	
			de->SetLineStyle(2);

			de->SetLineColor(40);

			gauss_new->SetParameters(all_2->GetParameter(4), all_2->GetParameter(5), all_2->GetParameter(6));

			gauss_new->SetLineStyle(2);

			gauss_new->SetLineColor(41);
					
			FD->SetParameters(all_2->GetParameter(7), all_2->GetParameter(8), all_2->GetParameter(9));

			FD->SetLineStyle(2);

			FD->SetLineColor(30); // verde

			FDM->SetParameters(all_2->GetParameter(10), all_2->GetParameter(12), all_2->GetParameter(13), all_2->GetParameter(11));
	
			FDM->SetLineStyle(2);

			FDM->SetLineColor(46); // rosso

			gaussMu[i] = all_2->GetParameter(5); // medie dei picchi gaussiani

			stringstream MuStream;
			
			MuStream << fixed << setprecision(2) << gaussMu[i];

			string MuString = MuStream.str();

			stringstream TempStream;
			
			TempStream << fixed << setprecision(2) << tempMean[i];

			string TempString = TempStream.str();
	
			hcharge->SetTitle("");

			hcharge->SetStats(kFALSE);

			de->Draw("same");

			gauss_new->Draw("same");

			FD->Draw("same");

			FDM->Draw("same");

			pdf = dir_ch_hist + "/ChargeHisto_" + data + "_peak_ch_" + MuString + "_pC_temp_" + TempString + "_°C.pdf";

			hcharge->GetXaxis()->SetTitle("Carica (pC)");

			hcharge->GetYaxis()->SetTitle("Numero di conteggi");

			hcharge->GetXaxis()->CenterTitle();

			hcharge->GetYaxis()->CenterTitle();
  	
			c1->SaveAs(pdf.c_str()); // salva l'istogramma in un file pdf

			root = dir_ch_hist + "/ChargeHisto_" + data + "_peak_ch_" + MuString + "_pC_temp_" + TempString + "_°C.root";

			c1->SaveAs(root.c_str()); // salva l'istogramma in un file root

			c1->Clear();

			gaussMuErr[i] = all_2->GetParError(5); // errori sulle medie dei picchi gaussiani

			gaussSigma[i] = all_2->GetParameter(6); // sigma dei picchi gaussiani

			gaussSigmaErr[i] = all_2->GetParError(6); // errori sulle sigma dei picchi gaussiani

			res[i] = gaussSigma[i] / gaussMu[i]; // risoluzioni dei picchi gaussiani

			resErr[i] = sqrt(gaussMu[i] * gaussMu[i] * gaussSigmaErr[i] * gaussSigmaErr[i] + gaussSigma[i] * gaussSigma[i] * gaussMuErr[i] * gaussMuErr[i]) / (gaussMu[i] * gaussMu[i]); // errori sulle risoluzioni dei picchi gaussiani

			if ((gaussMu[i] >= 800)) { // crea dei nuovi array per eliminare i primi valori di carica minori di 800 pC

				tempMean_new[i_new] = tempMean[i];

				gaussMu_new[i_new] = gaussMu[i];

				gaussMuErr_new[i_new] = gaussMuErr[i];

				gaussSigma_new[i_new] = gaussSigma[i];

				gaussSigmaErr_new[i_new] = gaussSigmaErr[i];

				res_new[i_new] = res[i];

				resErr_new[i_new] = resErr[i];

				textfile << i + 1 << " \t " << tempMean_new[i_new] << " \t\t " << gaussMu_new[i_new] << " \t " << gaussMuErr_new[i_new] << " \t " << gaussSigma_new[i_new] <<  " \t " << gaussSigmaErr_new[i_new] << " \t\t " << res_new[i_new] << " \t " << resErr_new[i_new] << endl;
	
				log1 << i + 1 << " \t " << all_2->GetParameter(0) << " \t " << all_2->GetParameter(1) << " \t " << all_2->GetParameter(2) << " \t " << all_2->GetParameter(3) << " \t " << all_2->GetParameter(4) << " \t " << all_2->GetParameter(5) << " \t " << all_2->GetParameter(6) << endl;

				log2 << i + 1 << " \t " << all_2->GetParameter(7) << " \t " << all_2->GetParameter(8) << " \t " << all_2->GetParameter(9) << " \t " << all_2->GetParameter(10) << " \t " << all_2->GetParameter(11) << " \t " << all_2->GetParameter(12) << " \t " << endl;

				i_new++;

				}

			i++;

			tempMean[i] = temp; // acquisizione temperatura associata al blocco di 300 s successivo

			c1->Close();

			s->Delete();

			gauss->Delete();

			all->Delete();

			all_2->Delete();

			de->Delete();

			FD->Delete();

			FDM->Delete();

			gauss_new->Delete();

			hcharge->Reset();
		
			}

		}

	hcharge->Delete();

	file->Close();

	textfile.close();

	log1.close();
		
	log2.close();

	log3.close();

	TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);

	c2->cd();
			
	c2->SetLeftMargin(0.15);

	TGraph *gr1 = new TGraph(i_new, tempMean_new, gaussMu_new);

	gr1->SetTitle("");

	gr1->GetXaxis()->SetTitle("Media temperatura (#circC)");

	gr1->GetYaxis()->SetTitle("Carica fotopicco (pC)");

	gr1->GetXaxis()->CenterTitle();

	gr1->GetYaxis()->CenterTitle();

	gr1->Draw("AL*");

	pdf = dir_plots + "/ChVsTemp_" + data + ".pdf";

	c2->SaveAs(pdf.c_str());

	c2->Clear();

	gr1->Delete();

	TGraph *gr2 = new TGraph(i_new, tempMean_new, res_new);

	gr2->SetTitle("");

	gr2->GetXaxis()->SetTitle("Media temperatura (#circC)");

	gr2->GetYaxis()->SetTitle("Risoluzione gaussiana");

	gr2->GetXaxis()->CenterTitle();

	gr2->GetYaxis()->CenterTitle();

	gr2->Draw("AL*");

	pdf = dir_plots + "/ResVsTemp_" + data + ".pdf";

	c2->SaveAs(pdf.c_str());

	c2->Close();

	gr2->Delete();

	return 0;

}

bool isNumber(const string& s) {

	string::const_iterator it = s.begin();

	while (it != s.end() && (isdigit(*it) || (*it)==string(".") || (*it)==string("-")) ) ++it;

	return !s.empty() && it == s.end();

}
