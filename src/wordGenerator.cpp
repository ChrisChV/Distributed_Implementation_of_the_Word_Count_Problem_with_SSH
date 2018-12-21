#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <csignal>
#include <cstdlib>
#include <cstdio>

#define MB_TO_B 1000000
#define COUT_RANGE 100

using namespace std;

typedef long StoreType;

ofstream resFile;
const string OutFileName = "OutFile";

vector<string> getWords(string wordFileName){
	ifstream wordFile(wordFileName);
	string word;
	vector<string> res;
	while(wordFile>>word){
		res.push_back(word);
	}
	wordFile.close();
	return res;
}

void kill_event(int s){
	resFile.close();
}

int main(int argv, char** argc){
	if(argv != 3){
		cout<<"Faltan Argumentos <wordFile> <OutFileSize(MB)>"<<endl;
		return 0;
	}
	string wordFile(argc[1]);
	string finalSize_s(argc[2]);
	StoreType finalSize = stoi(finalSize_s);
	
	cout<<"Generating dictionary..."<<endl;
	vector<string> words = getWords(wordFile);

	random_device rd; //Generador random no determinista.
	mt19937 mt(rd()); // Mersenne Twister 19937 generator.
	uniform_int_distribution<int> dist(0, words.size() - 1); //Distribucion de numeros random.

	StoreType byteFinalSize = finalSize * MB_TO_B;
	cout<<"Generating "<<byteFinalSize<<" bytes ("<<finalSize<<" MB)..."<<endl;

	StoreType actualSize = 0;
	int actualCout = 1;
	StoreType actualSizeCout = COUT_RANGE * MB_TO_B;
	resFile.open(OutFileName);
	//sigaction(SIGINT, &kill_event, NULL);
	int randNum = 0;
	string actualWord = "";
	while(actualSize < byteFinalSize){
		randNum = dist(mt);
		actualWord = words[randNum];
		resFile<<actualWord<<endl;
		actualSize += actualWord.size() + 1;
		//if(actualSize >= actualSizeCout){
			//cout<<actualSize<<"/"<<byteFinalSize<<endl;
		//	actualCout++;
		//	actualSizeCout = COUT_RANGE * MB_TO_B *  actualCout;
		//}
	}
	resFile.close();
}