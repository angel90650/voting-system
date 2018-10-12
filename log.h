/* log.h 
 * 
 *
 *
 */


#ifndef LOG_H
#define LOG_H

#include <vector>
#include <fstream>
#include <sstream>
#include "timetable.h"
#include "dictionary.h"
#include "vote.h"
#include <iomanip>

using namespace std;


class log{

 public:
  void insert(vote v);
  void update(vector<vote> v, timetable t, dictionary &d);
  void garbageCollect(timetable t);
  void fillSendStream(stringstream &ss, timetable t, int sendID);
  vote getVote(int i);
  int getSize();
  void erase(int min, int server);
  void print();
  

 private:
  void printEvent(vote v);
  vector<vote> list;
};


#endif
