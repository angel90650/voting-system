// Example program
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
int main(){
  string fileName = "example.txt";
  ifstream file;
  string category;
  stringstream ss;
  file.open(fileName);
  file>>category;
  while(category != "end"){
    cout<<"\""<<category<<"\""<<endl;
    file>>category;
  }
  file.close();

  ss << "this";
  ss << " ";
  ss << "is";
  ss.put(' ');
  ss << "a";
  ss.put(' ');
  ss << "test";
  cout<<ss.str()<<endl;

}
