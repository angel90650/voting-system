/* dictionary.cpp
 *
 */

#include "dictionary.h"

dictionary::dictionary(){
  aliceVotes = 0;
  bobVotes = 0;
}
void dictionary::update(char vote){
  
  if(vote == 'A'){
    aliceVotes++;
  }else if (vote == 'B'){
    bobVotes++;
  }else{
    cout << "Unexpected vote type*" << vote << "*" <<  endl;
    return;
  } 
}

void dictionary::setAlice(int votes){
  aliceVotes = votes;
}

void dictionary::setBob(int votes){
  bobVotes = votes;
}

int dictionary::getAlice(){
  return aliceVotes;
}

int dictionary::getBob(){
  return bobVotes;
}

void dictionary::print(){
  cout<<"Alice: "<<aliceVotes<<endl;
  cout<<"  Bob: "<<bobVotes<<endl;
}
