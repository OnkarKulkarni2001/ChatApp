#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "structs.h"

class cClient
{
public:
	std::string clientName;

	void ReceiveMessage(SOCKET socket);
	int SendMessageToServer(SOCKET socket);
	int SendClientNameToServer(SOCKET socket, std::string& clientName);
};