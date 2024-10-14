#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "structs.h"

class cClient
{
public:
	void ReceiveMessage(SOCKET socket);
	int SendMessageToServer(SOCKET socket);
};