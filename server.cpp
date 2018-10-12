/* server.cpp 
 * Nery Lara, Angel Castro
 * cs171 Voting system HW2
 *
 *
 *
*/

#include <iostream>
#include "dictionary.h"
#include "timetable.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <thread>
#include <system_error>
#include <mutex>
#include <sstream>
#include <vector>
#include <strings.h>
#include "log.h"
#include "vote.h"
#include <fcntl.h>
#include <signal.h>
#define SIZE 4096
#define DELAY 3

using namespace std;


mutex mtx;
int SERVER_ID, SEND_ID, RECV_ID;
int socketFD, sendFD;
struct sockaddr_in server, sendServer, recvServer;
socklen_t sendServerLen, recvServerLen;
bool alive = true;
bool quit = false;
dictionary dict;
timetable tt;
log list;

void setIDs();
void receiveHandler();
void sendHandler();
void voteHandler();
/* send state of server 
 * returns false if send failed
*/
bool sendState();
void parseBuffer(char *buf, timetable &tempTT, vector<vote> &temp);
void restoreState();
void castVote(char name);
void saveState();

int main(int argc, char ** argv){
  //argv < id , port, sendport, sendIP  >
  if(argc < 4){
    cout<<"argv < id , port, sendport, sendIP >"<<endl;
    return 0;
  }
  SERVER_ID = atoi(argv[1]);
  tt.setID(SERVER_ID);
  setIDs();
  int opt = 1;
  int port = atoi(argv[2]);
  int sendPort = atoi(argv[3]);
  string name = argv[4];
  char sendHostName[100];
  hostent * sendHost;
  cout<<"This is server "<<((char)('A' + SERVER_ID))<<endl;
  restoreState();
  strcpy(sendHostName, name.c_str());
  sendHost = gethostbyname(sendHostName);

  signal(SIGPIPE, SIG_IGN);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);

  bzero((char *) &sendServer, sizeof(sendServer));
  sendServer.sin_family = AF_INET;
  inet_pton(AF_INET, sendHost->h_addr, &sendServer.sin_addr);
  sendServer.sin_port = htons(sendPort);

  if((socketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Could not open socket");
    exit(EXIT_FAILURE);
  }

  setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  
  if((bind(socketFD, (struct sockaddr *) &server, sizeof(server))) < 0){
    perror("Could not bind socket");
    exit(EXIT_FAILURE);
  }
  if((listen(socketFD, 2)) < 0){
    cout<<"Listen failed"<<endl;
  }

  fcntl(socketFD, F_SETFL, O_NONBLOCK);
  
  thread receiveThread(receiveHandler);
  thread sendThread(sendHandler);
  thread voteThread(voteHandler);
  
  receiveThread.join();
  sendThread.join();
  voteThread.join();

  close(socketFD);
  cout << "Exiting gracefully " << endl;
  
  return 0;
}/* end main */


void receiveHandler(){
  int clientFD;
  char buf[SIZE];
  timetable tempTT;
  vector<vote> temp;
  while(!quit){
    while((clientFD = accept(socketFD,(struct sockaddr *)&recvServer, &recvServerLen)) < 0){
      sleep(1);
      if(quit) return;
    }
    cout<<"Accepted connection"<<endl;
    while(!quit){
      if((recv(clientFD, buf, SIZE, 0)) == 0){
	close(clientFD);
	break;
      }
      /* Parse buffer */
      /* A 2 5 B 2 6 A 2 7:0 0 1 3 2 4 3 4 5 */
      parseBuffer(buf, tempTT, temp);
      mtx.lock();
      list.update(temp, tt, dict); /* Updates log and dictionary */
      tt.merge(tempTT); /* Updates timetable */
      list.garbageCollect(tt);
      saveState();  /* Write to disk */
      mtx.unlock();
      temp.clear();
    }
  }
  close(clientFD);
}

void sendHandler(){
  int opt = 1;
  while(!quit){
    
    if((sendFD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      perror("Could not open socket");
      exit(EXIT_FAILURE);
    }
    setsockopt(sendFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    while(connect(sendFD, (struct sockaddr *) &sendServer, sizeof(sendServer)) < 0){
      if(quit) return;
      sleep(DELAY);
    }
    cout<<"Connected to Server "<<((char)('A'+ SEND_ID))<<endl;
    while(!quit){
      if(alive){
	if(!sendState()){
	  break;
	}
      }
      sleep(DELAY);
    }
  }
  close(sendFD);
}

void voteHandler(){
  while(1){
    char option;
    cout << "Vote for (A)lice, Vote for (B)ob, (C)lose, (R)econnect, (Q)uit" << endl;
    cout << "Print (L)og, print (T)ime table, print (D)ictionary" << endl;
    cin >> option;

    switch(option){
    case 'A':
      castVote(option);
      cout<<"Vote for Alice processed"<<endl;
      break;
    case 'B':
      castVote(option);
      cout<<"Vote for Bob processed"<<endl;
      break;
    case 'C':
      alive = false;
      cout<<"Network link closed "<<((char)('A' + SERVER_ID))<<"--\\-->"<<((char)('A' + SEND_ID))<<endl;
      break;
    case 'R':
      alive = true;
      cout<<"Network link Re-Established "<<((char)('A' + SERVER_ID))<<"---->"<<((char)('A' + SEND_ID))<<endl;
      break;
    case 'L':
      list.print();
      break;
    case 'T':
      tt.print();
      break;
    case 'D':
      dict.print();
      break;
    case 'Q':
      quit = true;
      return;
    } 
  }
}

void parseBuffer(char * buf, timetable &tempTT, vector<vote> &temp){
  stringstream ss;
  int val;

  ss << buf;
  while(!ss.eof()){
    ss>>ws;
    if (ss.peek() != ':'){
      vote Vote;
      ss >> Vote.name;
      ss >> Vote.server;
      ss >> Vote.clock;
      temp.push_back(Vote);
    }else{
      ss.get();
      for (int i = 0; i < SERVERS; i++){
	for (int j = 0; j < SERVERS; j++){
	  ss >> val;
	  tempTT.update(i, j, val);
	}
      }
      ss >> val;
      tempTT.setID(val);
      break;
    }
  }
}
 
void restoreState(){
  string fileName = "server" + to_string(SERVER_ID) + ".txt";
  ifstream file;
  string category;
  int val;
  
  file.open(fileName);
  file>>category;
  if(category == "timetable"){
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 3; j++){
	file>>val;
	tt.update(i, j, val);
      }
    } 
  }else{
    /* File empty */
  }
  file>>category;
  if(category == "log"){
    file>>val;
    for(int k = 0; k < val; k++){
      vote v;
      file>>v.name;
      file>>v.server;
      file>>v.clock;
      list.insert(v);
    }
  }
  file>>category;
  if(category == "dictionary"){
    file>>val;
    dict.setAlice(val);
    file>>val;
    dict.setBob(val);
  }
  file.close();
}

void saveState(){
  string fileName = "server" + to_string(SERVER_ID) + ".txt";
  ofstream file;
  vote v;
  
  file.open(fileName);
  file<<"timetable"<<endl;
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      file<<tt.get(i, j);
      file<< " ";
    }
    file<<endl;
  }
  file<<"log"<<endl;
  file<<list.getSize()<<endl;
  for(int i = 0; i < list.getSize(); i++){
    v = list.getVote(i);
    file<<v.name<< " ";
    file<<v.server<< " ";
    file<<v.clock<<endl;
  }
  file<<"dictionary"<<endl;
  file<<dict.getAlice()<<endl;
  file<<dict.getBob()<<endl;
  file<<"end"<<endl;
  file.close();
}


bool sendState(){
  char buf[SIZE];
  stringstream ss;
  memset(buf,0,SIZE);
  mtx.lock();
  list.fillSendStream(ss, tt, SEND_ID);
  ss.put(':');
  ss.put(' ');
  tt.fillSendStream(ss);
  mtx.unlock();
  ss.getline(buf, SIZE);
  if(send(sendFD, buf, SIZE, MSG_NOSIGNAL) < 0){
    return false;
  }else{
    return true;
  }
}

void castVote(char name){
  vote v;
  
  mtx.lock();
  tt.update();
  dict.update(name);
  v.name = name;
  v.server = SERVER_ID;
  v.clock = tt.getClk();
  list.insert(v);
  mtx.unlock();
}

void setIDs(){

  SEND_ID = (SERVER_ID + 1) %3;
  RECV_ID = (SERVER_ID - 1) %3;
  if(RECV_ID == -1) RECV_ID = 2;
  
}

