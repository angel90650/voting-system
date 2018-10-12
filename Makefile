#Makefile voting system

server: server.o dictionary.o timetable.o log.o
	g++ -std=c++11 -pthread -g -o server server.o dictionary.o timetable.o log.o

dictionary.o: dictionary.cpp
	g++ -std=c++11 -g -c dictionary.cpp

server.o: server.cpp
	g++ -std=c++11 -g -c server.cpp

timetable.o: timetable.cpp
	g++ -std=c++11 -g -c timetable.cpp

log.o: log.cpp
	g++ -std=c++11 -g -c log.cpp

clean:
	rm -f server server.o dictionary.o timetable.o log.o

restart:
	rm -f server0.txt server1.txt server2.txt
