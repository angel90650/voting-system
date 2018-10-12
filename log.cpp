#include "log.h"

void log::update(vector<vote> v, timetable t, dictionary &d){

  int id = t.getID();
  int server;
  int clk;
  char voteType;
  
  for(vector<vote>::iterator it = v.begin(); it != v.end(); ++it){
    server = it->server;
    clk = it->clock;
    voteType = it->name;
    
    if(clk > t.get(id, server)){
      list.push_back((*it));
      d.update(voteType);
    }
  }
}

void log::fillSendStream(stringstream &ss, timetable t, int sendID){

  int id = t.getID();
  int server;
  int clk;
  char voteType;
  
  for(vector<vote>::iterator it = list.begin(); it != list.end(); ++it){
    server = it->server;
    clk = it->clock;
    voteType = it->name;
    if(clk > t.get(sendID,server)){
      ss << voteType;
      ss.put(' ');
      ss << server;
      ss.put(' ');
      ss << clk;
      ss.put(' ');
    }
  }
}

void log::insert(vote v){
  list.push_back(v);
}

int log::getSize(){
  return list.size();
}

vote log::getVote(int i){
  return list[i];
}

void log::garbageCollect(timetable t){

  int val;
  
  for(int i = 0; i < 3; i++){
    val = min(t.get(0, i), t.get(1, i));
    val = min(val, t.get(2, i));
    if(val > 0){
      erase(val, i);
    }
  }
}

void log::erase(int min, int server){

  for(int it = 0; it < list.size(); it++){
    if (list[it].server == server && list[it].clock <= min){
      list.erase(list.begin() + it);
    }
  }  
}


void log::print(){

  cout<<"(V)ote, (S)erver, (C)lock"<<endl;
  cout<<"| V | S |   C |"<<endl;
   
  for(vector<vote>::iterator it = list.begin(); it != list.end(); ++it){
    printEvent(*it);
  }
  cout<<endl;
}
void log::printEvent(vote v){
  cout<<"| "<<v.name<<" | "<<((char)(v.server + 'A'))<<" | ";
  cout<<setw(3)<<v.clock<<" |"<<endl;;
}
