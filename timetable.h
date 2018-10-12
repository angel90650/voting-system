/* timetable.h 
 * 
 *
 *
 */


#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>

#define SERVERS 3

using namespace std;

class timetable{

 public:
  timetable();
  void load(int id);
  void update(int row, int column, int value);
  void update();
  void merge(timetable tt);
  void fillSendStream(stringstream &ss);
  int getClk();
  int get(int row, int column);
  void setID(int val);
  int getID();
  void print();
 private:
  int timeTable[SERVERS][SERVERS];
  int SERVER_ID;
  
  
};

#endif
