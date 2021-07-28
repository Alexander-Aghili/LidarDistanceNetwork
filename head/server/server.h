#ifndef SERVER_H
#define SERVER_H

#include <iostream>

#define PORT          3001
#define MAX_CLIENTS   5

class Server{
private:
    double* m_targetDistance;
    int* m_numClients;
    int* clientSocket[5];
public:
    Server(double* targetDisance, int* numClients, int* clientSocketPtr);

    int startServer();
    void keyValueAssesment(std::string key, std::string value);
    void sendMessageToHost(std::string message);

    void operator()();
};

#endif