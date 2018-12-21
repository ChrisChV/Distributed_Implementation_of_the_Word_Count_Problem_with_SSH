#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include "MyTime.h"
#include "split.h"
#include "send.h"

using namespace std;

MyTime mytime;

const string countFileName = "bin/countWords";
const string resCountFileName = "bin/CountOutFile";
const string finalResFileName = "bin/FinalCount";


void countInNode(string host, string user, string pass, string fileForCount, int numOfThreads){
	string command = "sshpass -p '" + pass + "' ssh " + user +"@" + host + " \"/home/" + user + "/" + countFileName + " /home/" + user + "/" + fileForCount + " " + to_string(numOfThreads) + "\"";
	cout<<"Iniciando conteo en nodo "<<user<<"@"<<host<<endl;
	system(command.c_str());
	cout<<"Conteo terminado en nodo "<<user<<"@"<<host<<endl;
	cout<<"Copiando archivo "<<resCountFileName<<" desde el nodo "<<user<<"@"<<host<<endl;
	command = "sshpass -p '" + pass + "' scp " + user + "@" + host + ":/home/" + user + "/" + resCountFileName + " ./" + resCountFileName + user; 
	system(command.c_str());
	cout<<"Archivo recivido desde el nodo "<<user<<"@"<<host<<endl;
}

string getNowTime(){
	auto nowTimeClock = chrono::system_clock::now();
	time_t nowTime_t = chrono::system_clock::to_time_t(nowTimeClock);
	string nowTime(ctime(&nowTime_t));
	nowTime.pop_back();
	return nowTime;
}


void writeCount(map<string, long> & res){
	ofstream file(finalResFileName);
	for(auto iter = res.begin(); iter != res.end(); ++iter){
		file<<iter->first<<" "<<iter->second<<endl;
	}
	file.close();
}

int getNumOfNodes(string fileOfHosts){
	string tempFile = ".tempFile";
	string command = "wc -l " + fileOfHosts + " > " + tempFile;
	system(command.c_str());
	ifstream file(tempFile);
	string word  = "";
	file>>word;
	file.close();
	command = "rm " + tempFile;
	system(command.c_str());
	return stoi(word);
}

int main(int argc, char ** argv){
	cout<<"Hora de inicio: "<<getNowTime()<<endl;
	if(argc != 4){
		cout<<"Faltan argumentos <fileForCount> <fileOfHosts> <numOfThreads>"<<endl;
		return 0;
	}
	string fileForCount(argv[1]);
	string fileOfHosts(argv[2]);
	string s_numOfThreads(argv[3]);
	int numOfThreads = stoi(s_numOfThreads);
	int numOfNodes = getNumOfNodes(fileOfHosts) + 1;
	thread countNodesThreads[numOfNodes - 1];
	cout<<"Init: "<<getNowTime()<<endl;
	cout<<"Dividiendo archivo "<<fileForCount<<" en "<<numOfNodes<<" partes..."<<endl;

	
//	mytime.init();

//	splitFile(fileForCount, numOfNodes);
//	mytime.end();
//	mytime.print();
//	cout<<"End: "<<getNowTime()<<endl;

	string partOfFile = "";
	string host = "";
	string user = "";
	string pass = "";
	ifstream fileHosts(fileOfHosts);
	vector<string> parts(numOfNodes);
	for(int i = 1; i < numOfNodes; i++){
		partOfFile = fileForCount;
		if(i < 10) partOfFile += "000" + to_string(i);
		else if(i < 100) partOfFile += "00" + to_string(i);
		else if(i < 1000) partOfFile += "0" + to_string(i);
		else partOfFile += to_string(i);
		parts[i] = partOfFile;
		fileHosts>>host;
		fileHosts>>user;
		fileHosts>>pass;
		sendFile(partOfFile, host, user, pass);
	}
	fileHosts.close();
	sendToAll(fileOfHosts, countFileName);

	fileHosts.open(fileOfHosts);

	for(int i = 0; i < numOfNodes - 1;  i++){
		fileHosts>>host;
		fileHosts>>user;
		fileHosts>>pass;
		countNodesThreads[i] = thread(countInNode, host, user, pass, parts[i + 1], numOfThreads);
	}

	fileHosts.close();

	string command = "./bin/countWords " + fileForCount + "0000 " + to_string(numOfThreads);
	cout<<"Iniciando conteo en nodo master"<<endl;
	system(command.c_str());
	cout<<"Conteo terminado en nodo master"<<endl;

	for(int i = 0; i < numOfNodes - 1; i++){
		countNodesThreads[i].join();
	}

	cout<<"Iniciando conteo general"<<endl;
	map<string, long> res;
	ifstream countFile;
	string word = "";
	string s_count = "";
	long count = 0;
	map<string,long>::iterator findRes;
	fileHosts.open(fileOfHosts);
	for(int i = 0; i < numOfNodes; i++){
		if(i == 0){
			countFile.open(resCountFileName);
			while(countFile>>word){
				countFile>>s_count;
				count = stol(s_count);
				findRes = res.find(word);
				if(findRes == res.end()) res[word] = count;
				else findRes->second += count;
			}
			countFile.close();
		}
		else{
			fileHosts>>host;
			fileHosts>>user;
			fileHosts>>pass;
			countFile.open(resCountFileName + user);
			while(countFile>>word){
				countFile>>s_count;
				count = stol(s_count);
				findRes = res.find(word);
				if(findRes == res.end()) res[word] = count;
				else findRes->second += count;
			}
			countFile.close();
		}
	}
	fileHosts.close();
	writeCount(res);
	cout<<"Hora de fin: "<<getNowTime()<<endl;
}