#pragma once
#include <string>
#include "structs.h"

class cServer
{
public:
	struct addrinfo* sInfo = nullptr;
	struct addrinfo sHints;
	
	std::vector<SOCKET> vConnections;			// vector of active connections
	std::map<SOCKET, std::string> ConnectionWithName;

	//void broadcastMessage(const std::string& message);
	void handleNewConnections(SOCKET listenSocket);
	void handleClientMessages(SOCKET socket, SOCKET& listenSocket, std::vector<SOCKET> vConnections);
		
	int initializeServer(SOCKET& listenSocket);
};
