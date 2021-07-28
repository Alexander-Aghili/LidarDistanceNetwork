g++ -c head_init.cpp
g++ -c log/log.cpp
g++ -c lidar/lidar_stream.cpp
g++ -c lidar/lidar_data.cpp
g++ -c server/server.cpp

g++ -pthread log.o lidar_data.o lidar_stream.o server.o head_init.o -o head_init

sudo ./head_init