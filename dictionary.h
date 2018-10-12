/* dictionary.h 
 * 
 *
 *
 */


#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <iostream>

using namespace std;

class dictionary{

 public:
  dictionary();
  void update(char vote);
  void print();
  void setAlice(int votes);
  void setBob(int votes);
  int getAlice();
  int getBob();

 private:
  int aliceVotes;
  int bobVotes;
};

#endif
