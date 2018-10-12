#include "timetable.h"

timetable::timetable(){
  
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      timeTable[i][j] = 0;
    }
  }
}

void timetable::update(int row, int column, int value){

  timeTable[row][column] = value;
  
}

void timetable::update(){
  
  timeTable[SERVER_ID][SERVER_ID]++;
}

void timetable::merge(timetable tt){

  int id = tt.SERVER_ID;
  for(int k = 0; k < 3; k++){
    timeTable[SERVER_ID][k] = max(timeTable[SERVER_ID][k], tt.timeTable[id][k]);
  }    

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      timeTable[i][j] = max(timeTable[i][j], tt.timeTable[i][j]);
    }
  }
}



void timetable::fillSendStream(stringstream &ss){
   for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      ss << timeTable[i][j];
      ss.put(' ');
    }
   }
   ss << SERVER_ID;
}

int timetable::getClk(){
  return timeTable[SERVER_ID][SERVER_ID];
}

void timetable::setID(int val){
  SERVER_ID = val;
}

int timetable::getID(){
  return SERVER_ID;
}

int timetable::get(int row, int column){
  return timeTable[row][column];
}
void timetable::print(){
 
  cout<<"  ____A_____B_____C__"<<endl;
   for(int i = 0; i < 3; i++){
     cout<<((char)('A'+i))<<" | ";
    for(int j = 0; j < 3; j++){
      cout<<setw(3)<<timeTable[i][j]<<" | ";
    }
     cout<<endl;
   }
}
