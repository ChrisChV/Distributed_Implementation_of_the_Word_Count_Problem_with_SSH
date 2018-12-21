#ifndef SEND_H
#define SEND_H

#include <iostream>
#include <fstream>

using namespace std;

void sendFile(string fileName, string host, string user, string pass){
	cout<<"Enviando el archivo "<<fileName<<" a "<<user<<"@"<<host<<"..."<<endl;
	string command = "sshpass -p '" +  pass + "' scp ./" + fileName + " " + user + "@" + host + ":/home/" + user + "/";
	cout<<command<<endl;
	system(command.c_str());
}

void sendToAll(string fileOfHosts, string fileToSend){
	ifstream file(fileOfHosts);
	string host = "";
	string user = "";
	string pass = "";
	while(file>>host){
		file>>user;
		file>>pass;
		sendFile(fileToSend, host, user, pass);
	}
	file.close();
}



#endif