#include <iostream>
#include <thread>
#include <map>
#include <fstream>
#include <vector>
#include <ctime>
#include <future>


using namespace std;

#define STANDAR_SIZE 100 //En MB
#define MB_TO_B 1000000

typedef long StoreType;

const string outFile = "CountOutFile";

void splitFile(string fileName, int splits){
	string command = "split -l$((`wc -l < "+  fileName +" `/" + to_string(splits) +")) "+ fileName  + " " + fileName + " -da 4";
	cout<<command<<endl;
	system(command.c_str());
}

StoreType getSizeOfFile(string fileName){
	string tempFileName = ".tempSize";
	string command = "ls -l OutFile | awk '{print $5}' > " + tempFileName;
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


void splitFilesIntoStandarSize(string fileName){
	StoreType size = getSizeOfFile(fileName);
	int numOfSplits = size / (STANDAR_SIZE * MB_TO_B);
	cout<<(STANDAR_SIZE * MB_TO_B)<<endl;
	if(size % (STANDAR_SIZE * MB_TO_B) != 0) numOfSplits++;
	cout<<numOfSplits<<endl;
	splitFile(fileName, numOfSplits);
}

void count(vector<string> * words, map<string, long> * res){
	//cout<<"Start count "<<words->size()<<endl;
	map<string,long>::iterator actualFind;
	for(auto iter = words->begin(); iter != words->end(); ++iter){
		actualFind = res->find(*iter);
		if(actualFind == res->end()){
			(*res)[*iter] = 1;
		}
		else{
			actualFind->second += 1;
		}
	}
	//cout<<"Finish "<<res->size()<<endl;
}

void addCounts(map<string, long> * res, map<string, long> * add){
	map<string,long>::iterator resFind;
	for(auto iter = add->begin(); iter != add->end(); ++iter){
		resFind = res->find(iter->first);
		if(resFind == res->end()){
			(*res)[iter->first] = iter->second;
		}
		else{
			resFind->second += iter->second;
		}
	}
}


void manager(string fileName, map<string, long> * res, int numThreads){
	numThreads--;
	if(numThreads == 0){
		cout<<"Debe haber por lo menos 2 threads"<<endl;
		return;
	}
	thread threads[numThreads];
	vector<bool> flags(numThreads, 0);
	vector<vector<string> *> threadWords(numThreads);
	vector<map<string,long> *> threadMaps(numThreads);
	int actualThread = 0;
	ifstream file(fileName);
	string word = "";
	StoreType actualSize = 0;
	vector<string> * newWords = new vector<string>();
	map<string,long> * newMap = nullptr;
	while(file>>word){
		newWords->push_back(word);
		actualSize += word.size();
		if(actualSize >= STANDAR_SIZE * MB_TO_B){
			if(flags[actualThread] == false){
				threadWords[actualThread] = newWords;
				threadMaps[actualThread] = new map<string,long>();
				threads[actualThread] = thread(count, threadWords[actualThread], threadMaps[actualThread]);
				flags[actualThread] = true;
				newWords = new vector<string>();
				actualThread++;
				if(actualThread == numThreads) actualThread = 0;
				actualSize = 0;
			}
			else{
				threads[actualThread].join();
				threadWords[actualThread]->clear();
				threadWords[actualThread]->shrink_to_fit();
				delete threadWords[actualThread];
				threadWords[actualThread] = newWords;
				newMap = threadMaps[actualThread];
				threadMaps[actualThread] = new map<string,long>();
				threads[actualThread] = thread(count, threadWords[actualThread], threadMaps[actualThread]);
				addCounts(res, newMap);
				newMap->clear();
				delete newMap;
				newWords = new vector<string>();
				actualThread++;
				if(actualThread == numThreads) actualThread = 0;
				actualSize = 0;
			}
		}
	}
	/*
	threads[actualThread].join();
	threadWords[actualThread]->clear();
	threadWords[actualThread]->shrink_to_fit();
	delete threadWords[actualThread];
	threadWords[actualThread] = newWords;
	newMap = threadMaps[actualThread];
	threadMaps[actualThread] = new map<string,long>();
	threads[actualThread] = thread(count, threadWords[actualThread], threadMaps[actualThread]);
	addCounts(res, newMap);
	newMap->clear();
	delete newMap;
	*/

	newMap = new map<string, long>();
	count(newWords, newMap);
	addCounts(res, newMap);
	newMap->clear();
	delete newMap;
	newWords->clear();
	newWords->shrink_to_fit();
	delete newWords;

	for(int i = 0; i < numThreads; i++){
		if(threads[i].joinable()){
			threads[i].join();
			threadWords[i]->clear();
			threadWords[i]->shrink_to_fit();
			delete threadWords[i];
			addCounts(res, threadMaps[i]);
			threadMaps[i]->clear();
			delete threadMaps[i];
		}
	}
	file.close();
}

void count(string fileName, map<string, long> & res){
	ifstream file(fileName);
	string word = "";
	map<string,long>::iterator actualFind;
	while(file>>word){
		actualFind = res.find(word);
		if(actualFind == res.end()){
			res[word] = 1;
		}
		else{
			actualFind->second += 1;
		}
	}
	file.close();
}

void writeCount(map<string, long> & res){
	ofstream file(outFile);
	for(auto iter = res.begin(); iter != res.end(); ++iter){
		file<<iter->first<<" "<<iter->second<<endl;
	}
	file.close();
}

/*
int main(int argv, char** argc){
	if(argv != 2){
		cout<<"Faltan argumentos <WordFile>"<<endl;
		return 0;
	}
	string fileName(argc[1]);
	map<string, long> res;
	clock_t begin = clock();
	count(fileName, res);
	writeCount(res);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout<<elapsed_secs<<"s"<<endl;
	cout<<getSizeOfFile(fileName)<<endl;
	begin = clock();
	splitFilesIntoStandarSize(fileName);
	end = clock();
	elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout<<elapsed_secs<<"s"<<endl;
}
*/


int main(int argv, char ** argc){
	if(argv != 3){
		cout<<"Faltan argumentos <WordFile> <NumThreads>"<<endl;
		return 0;
	}
	string fileName(argc[1]);
	string s_numThreads(argc[2]);
	int numThreads = stoi(s_numThreads);
	map<string, long> res;
	thread threadManager(manager, fileName, &res, numThreads);
	threadManager.join();
	cout<<res.size()<<endl;
	writeCount(res);
}
