#ifndef SPLIT_H
#define SPLIT_H

#include <iostream>
#include <vector>

using namespace std;

typedef long StoreType;

void splitFile(string fileName, int splits){
	string command = "split -l$((`wc -l < "+  fileName +" `/" + to_string(splits) +")) "+ fileName  + " " + fileName + " -da 4";
	cout<<command<<endl;
	system(command.c_str());
}

StoreType getSizeOfFile(string fileName){
	string tempFileName = ".tempSize";
	string command = "ls -l " + fileName +" | awk '{print $5}' > " + tempFileName;
	system(command.c_str());
	ifstream tempFile(tempFileName);
	string s_size = "";
	tempFile>>s_size;
	StoreType res_size = stoi(s_size);
	tempFile.close();
	string rmCommand = "rm " + tempFileName;
	system(rmCommand.c_str());
	return res_size;
}

void splitFilev2(string fileName, int splits){

	ifstream file(fileName);
	ofstream outFile;
	string outName = "";
	StoreType size = getSizeOfFile(fileName);
	StoreType limitSize = size / splits;
	StoreType actualSize = 0;
	string word = "";

	vector<string> words;
	for(int i = 0; i < splits; i++){
		outName = fileName;
		actualSize = 0;
		words.clear();
		words.shrink_to_fit();
		if(i < 10) outName += "000" + to_string(i);
		else if(i < 100) outName += "00" + to_string(i);
		else if(i < 1000) outName += "0" + to_string(i);
		else outName += to_string(i);
		outFile.open(outName);
		while(actualSize < limitSize and file>>word){
			words.push_back(word);
			actualSize += word.size() + 1;
		}
		for(auto iter = words.begin(); iter != words.end(); ++iter){
			outFile<<*iter<<" "<<endl;
		}
		outFile.close();
	}
	file.close();
}

void splitFilev3(string fileName, int splits, ifstream & file, int actualSplit){
	ofstream outFile;
	string outName = "";
	StoreType size = getSizeOfFile(fileName);
	StoreType limitSize = size / splits;
	StoreType actualSize = 0;
	string word = "";

	outName = fileName;
	actualSize = 0;
	if(actualSplit < 10) outName += "000" + to_string(actualSplit);
	else if(actualSplit < 100) outName += "00" + to_string(actualSplit);
	else if(actualSplit < 1000) outName += "0" + to_string(actualSplit);
	else outName += to_string(actualSplit);
	outFile.open(outName);
	while(actualSize < limitSize and file>>word){
		outFile<<word<<endl;
		actualSize += word.size() + 1;
	}
	outFile.close();
	
	if(splits == actualSplit - 1) file.close();	
}


#endif