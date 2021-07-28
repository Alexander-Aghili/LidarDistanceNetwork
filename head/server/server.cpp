
//C includes
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

//C++ includes
#include <iostream>

//Custom includes
#include "../log/log.h"
#include "server.h"

/* Sole constructor with 3 pointers to maintain values outside of Server thread
 * @param targetDistance: double pointer to address of targetDistance needed in head init. Initialized in hostServer function in head_init.cpp
 * @param numClients: int pointer to address of numClients. Initialized in hostServer() function in head_init.cpp
 * @param clientSocketPtr: int pointer to address of first int in clientSocket array. Initialized in hostServer() function in head_init.cpp
 * 
 * The reason to use clientSocketPtr is so that SendMessage functions will work when being called outside this thread, though they have to be
 * the same Server object.
 * 
 * Member variables(which are all pointers) are assigned their pointer counterparts from the parameters. A for loop is used to initialze clientSocket pointer
 * and set the value to zero.
 */
Server::Server(double *targetDistance, int *numClients, int* clientSocketPtr) {
	m_targetDistance = targetDistance;
	m_numClients = numClients;

	for (int i = 0; i < MAX_CLIENTS; i++) {
		clientSocket[i] = clientSocketPtr+i;
		std::cout << clientSocket[i] << std::endl;
		*clientSocket[i] = 0;
	}
}

/*
 * Called by the Operator()() used for C++11 threading
 * Returns int but is ignored by operator, just used to keep track of errors.
 * 
 * Sets up all required socket and binding seen in C for hosting server
 */
int Server::startServer() {
    
    writeToLog("Server initialization sequence start");
    
    std::string port = std::to_string(PORT);
    int new_socket, activity, valread, sd, max_sd, opt = 1;
	char buffer[1025];

	fd_set readfds;

    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listeningSocket == -1) {
		writeToLog("FATAL ERROR 5: COULD NOT CREATE LISTENING SOCKET.", false);
		return 5;
	} else {
		writeToLog("Listening Socket Created", false);
	}

	if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		writeToLog("FATAL ERORR 6: COULD NOT ALLOW MULTIPLE CLIENTS TO LISTENING SOCKET WITH SETSOCKOPT", false);
		return 6;
	} else {
		writeToLog("Allowing multiple client connection to listening socket", false);
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, "0.0.0.0", &address.sin_addr);

	if (bind(listeningSocket, (sockaddr*)&address, sizeof(address)) == -1) {
		writeToLog("FATAL ERROR 7: COULD NOT BIND TO PORT " + port, false);
		return 7;
	} else {
		writeToLog("Successful bind to port " + port, false);
	}

	if (listen(listeningSocket, SOMAXCONN) == -1) {
		writeToLog("FATAL ERROR 8: COULD NOT LISTEN ON PORT " + port, false);
		return 8;
	} else {
		writeToLog("Listening to port " + port, false);
	}

	int addrlen = sizeof(address);
	while (true) {
		FD_ZERO(&readfds);
		
		FD_SET(listeningSocket, &readfds);
		max_sd = listeningSocket;

		int tempClientCheck = 0;
		for (int i = 0; i < MAX_CLIENTS; i++) {
			std::cout << clientSocket[i] << std::endl; //for debug
			sd = *clientSocket[i];

			if (sd > 0) {
				FD_SET(sd, &readfds);
				tempClientCheck++;
			}
				
			if (sd > max_sd) {
				max_sd = sd;
			}
		}
		if (tempClientCheck != *m_numClients) {
			*m_numClients = tempClientCheck;
		}

		//Waits indefinitly here for a request of some sort
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		
		if ((activity < 0) && (errno != EINTR)) {
			writeToLog("ERROR: SELECT ERROR", true);
		}

		if (FD_ISSET(listeningSocket, &readfds)) {
			if ((new_socket = accept(listeningSocket, (sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
				writeToLog("FATAL ERROR 9: CANNOT ACCEPT INCOMING CLIENT OR ERROR WITH CLIENT CONNECTION", false);
				return 9;
			}
			std::string log = "Client connected on socket " + std::to_string(new_socket) + ". IP: " + inet_ntoa(address.sin_addr) + " Port: " + std::to_string(ntohs(address.sin_port));
			writeToLog(log, false);
			
			char initMessage[1025] = "Request init. Client connection to server recieved. Ack message.";
			if (send(new_socket, initMessage, strlen(initMessage), 0) != (ssize_t)strlen(initMessage)) {
				writeToLog("ERROR: UNABLE TO SEND ACK MSG TO SOCKET " + std::to_string(new_socket), true);
			}
			
			for (int i = 0; i < MAX_CLIENTS; i++) {
				std::cout << &clientSocket[i] << std::endl; //debug
				if (*clientSocket[i] == 0){
					*clientSocket[i] = new_socket;
					writeToLog("Adding new user to list of sockets as (" + std::to_string(new_socket) + ", " + inet_ntoa(address.sin_addr) + ", " + std::to_string(ntohs(address.sin_port)) + ")", true);
					break;
				}
			}
		}

		//IO on socket
		for (int i = 0; i < MAX_CLIENTS; i++) {
			sd = *clientSocket[i];
			
			if (FD_ISSET(sd, &readfds)) {
				if ((valread = read( sd, buffer, 1024)) == 0) {
					getpeername(sd, (sockaddr*)&address, (socklen_t*)&addrlen);
					writeToLog(std::string("Lost connection to client on IP: ") + inet_ntoa(address.sin_addr) + " and Port: " + std::to_string(ntohs(address.sin_port)), false);
					close(sd);
					*clientSocket[i] = 0;
				} else {
					/* Read data transform documentation listed here and more detailed on the docs found in documentation tab
					 * From user controller: 
					 *  targetDistance:2.0
					 * 
					 */

					buffer[valread] = '\0';
					std::string key = "";
					std::string value = "";

					bool isKey = true;
					for (int i = 0; i < valread; i++) {
						if (buffer[i]==':') {
							isKey = false;
						}
						if (isKey) key += buffer[i];
						else value += buffer[i];
					}
					if (value != "")
						keyValueAssesment(key, value.substr(1)); //Exclude : from value

					

					//Sending data to socket
					//buffer[valread] = '\0';
					//send(sd, buffer, strlen(buffer), 0);
				}
			} 
		}
	}

}

/*
 * Parser that also sets appropriate values for keys based on key input
 * @param key: Is the value to be changed
 * @param value: value for the key to be altered
 */
void Server::keyValueAssesment(std::string key, std::string value) {
	std::cout << m_targetDistance << std::endl;
	if (key == "targetDistance") {
		*m_targetDistance = stod(value);
	} 
}

//To start thread c++11
void Server::operator()() {
	int result = startServer();
}

//Takes in a message and sends it to the host at clientSocket[0]
void Server::sendMessageToHost( std::string message ) {
	send(*clientSocket[0], message.c_str(), strlen(message.c_str()), 0);
}