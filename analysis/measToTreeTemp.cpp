#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;


bool isNumber(const std::string& s);


int main( int argc, char* argv[] ) {

	if (argc != 3) {

		cout << "USAGE: ./measToTreeTemp [filename] [temperature filename]" << endl;
    
		exit(1);

		}

	string fileName(argv[1]);

	string fileNameTemp(argv[2]);

	if (starts_with(argv[1], "../data/")) {

		fileName.erase(0, 8);

		}

	ifstream fs(Form("../data/%s", fileName.c_str()));
  
	ifstream fsT(Form("../data/%s", fileNameTemp.c_str()));

	if (!fs.good()) {

		cout << "-> No file called '" << fileName << "' found in '../data/'. Exiting." << std::endl;

		exit(1);

		}

	if (!fsT.good()) {

		cout << "-> No temperature file called '" << fileNameTemp << "' found in '../data/'. Exiting." << std::endl;
    
		exit(1);
	
		}

	cout << "-> Opened measurements-only data file: " << fileName << std::endl;

	cout << "-> Opened temperature data file: " << fileNameTemp << std::endl;

	size_t pos = 0;

	string outfileName;

	if ((pos = fileName.find(".")) != string::npos) {

		string prefix = fileName.substr(0, pos);

		outfileName = prefix + ".root";

		}

	TFile* outfile = TFile::Open(outfileName.c_str(), "recreate");

	TTree* tree = new TTree("tree", "");

	int ev;

	float vcharge, temp, time, rate;

	tree->Branch("ev", &ev, "ev/I");

	tree->Branch("vcharge", &vcharge, "vcharge/F");

	tree->Branch("temp", &temp, "temp/F");

	tree->Branch("time", &time, "time/F");

	tree->Branch("rate", &rate, "rate/F");

	string line;

	bool wasReadingEvent = false;

	bool checkTemp = false;

	double time_in, time_diff = 0, temp_check;

	if (fs.good() && fsT.good()) {

		cout << "-> Starting parsing file." << endl;

		fsT >> time_in >> rate; // acquisisce il tempo iniziale e il rate temporale al quale vengono stampate le temperature dal file delle temperature

		while (getline(fs,line)) {

			line.erase(remove(line.begin(), line.end(), '\n'), line.end());

			string delimiter = " ";

			size_t pos = 0;

			vector <string> words;

			string word;

      			while ((pos = line.find(delimiter)) != string::npos) {

				word = line.substr(0, pos);

				line.erase(0, pos + delimiter.length());

				words.push_back(word);

				}

			vector <string> words_cleaned;

			for (unsigned i=0; i<words.size(); ++i) {

				if (isNumber(words[i])) words_cleaned.push_back(words[i]);

				}

			if (words[0]=="===" && words[1]=="Event" && wasReadingEvent) {

				if(ev % 10000 == 0) cout << " ... analyzing event: " << ev << endl;

				tree->Fill();
 
				wasReadingEvent = false;

				} else if (words[0]!="===" && words_cleaned.size()==7) {

					wasReadingEvent = true;

					vcharge = atof(words_cleaned[4].c_str());

					}

			if (words[0]=="===" && words[1]=="Event" && wasReadingEvent==false) {

				ev = atoi(words[2].c_str());

				time = stod(words[5].c_str()) - time_in; // tempo relativo al tempo iniziale

				if ((int) (time / rate) >= 1 && checkTemp == false) { // condizione verificata se time >= rate

					for (int k=1; k<=(int) (time / rate); k++) { // esegue il ciclo un numero di volte pari a (int) time / rate

						fsT >> temp_check; // scarta le temperature precedenti alla presa dati di carica

						time_diff += rate; // incrementa di un rate temporale il tempo da sottrarre a time

						}

					fsT >> temp; // acquisisce la temperatura iniziale dal file delle temperature, dopo aver scartato le temperature precedenti alla presa dati di carica

					checkTemp = true;

					}

				if (time - time_diff >= rate) { // condizione verificata se si trova sulla riga del file meas che inizia con === Event e se la differenza time - time_diff >= rate

					time_diff += rate; // incrementa di un rate temporale il tempo da sottrarre a time

					fsT >> temp; // acquisisce le temperature successive dal file delle temperature

					}

				}

			}

		}

	if (wasReadingEvent) {

		cout << " ... analyzing event: " << ev << endl;

		tree->Fill();

		}

	fs.close();
 
	fsT.close();

	tree->Write();

	outfile->Close();

	cout << "-> Tree saved in: " << outfile->GetName() << endl;

	return 0;

	}

bool isNumber(const string& s) {

	string::const_iterator it = s.begin();

	while (it != s.end() && (isdigit(*it) || (*it)==string(".") || (*it)==string("-"))) ++it;

	return !s.empty() && it == s.end();

}
