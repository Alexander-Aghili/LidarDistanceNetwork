g++ -c ../server/server.cpp
g++ -c ../log/log.cpp
g++ -c test.cpp
g++ -pthread log.o server.o test.o -o test
sudo ./test