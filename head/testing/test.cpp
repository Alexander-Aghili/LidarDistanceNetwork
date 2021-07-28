
#include <iostream>
#include <thread>

#include <unistd.h>

#include "../server/server.h"

int main() {
  double targetDistance = 2.0;
  int numClients = 0;
  Server server = Server(&targetDistance, &numClients);
  std::thread serverThread(server);

  while (true) {
    sleep(2);
    std::cout << targetDistance << std::endl;
    std::cout << numClients << std::endl;
  }

}