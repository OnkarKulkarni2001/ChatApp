#pragma once
// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cClient.h"
#include "cBuffer.h"
#include "structs.h"
#include <iostream>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8412"

void cClient::ReceiveMessage(SOCKET socket)
{
	uint32_t bufSize = 512;
	cBuffer buffer(bufSize);

	while (true)
	{
		int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
		if (result > 0)
		{
			uint16_t packetSize = buffer.ReadUShort16_LE();
			uint16_t messageType = buffer.ReadUShort16_LE();

			if (messageType == 1)
			{
				uint32_t messageLength = buffer.ReadUInt32_LE();
				std::string messageString = buffer.ReadString(messageLength);

				std::cout << "Message: " << messageString << std::endl;
			}
		}
		else if (result == 0)
		{
			std::cout << "Server closed the connection" << std::endl;
			break;
		}
		else
		{
			std::cout << "recv failed with an error: " << WSAGetLastError() << std::endl;
			break;
		}
	}
}

int cClient::SendMessageToServer(SOCKET socket)
{
	sMessage chatMessage;
	chatMessage.messageString = "Voila you are the best!";
	chatMessage.messageLength = chatMessage.messageString.length();
	chatMessage.packetHeader.messageType = 1;
	chatMessage.packetHeader.packetSize =
		chatMessage.messageString.length()
		+ sizeof(chatMessage.messageLength)
		+ sizeof(chatMessage.packetHeader.messageType)
		+ sizeof(chatMessage.packetHeader.packetSize);

	uint32_t bufSize = 512;
	cBuffer buffer(bufSize);

	buffer.WriteUShort16_LE(chatMessage.packetHeader.packetSize);
	buffer.WriteUShort16_LE(chatMessage.packetHeader.messageType);
	buffer.WriteUInt32_LE(chatMessage.messageLength);
	buffer.WriteString(chatMessage.messageString);

	int result = send(socket, (const char*)(&buffer.m_BufferData[0]), chatMessage.packetHeader.packetSize, 0);
	return result;
}

int main(int arg, char* argv[])
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (result != 0)
	{
		std::cout << "WSAStartup failed with an error: " << result << std::endl;
		return 1;
	}

	struct addrinfo* sInfo = nullptr;
	ZeroMemory(&sHints, sizeof(&sHints));
	sHints.ai_family = AF_INET;
	sHints.ai_socktype = SOCK_STREAM;
	sHints.ai_protocol = IPPROTO_TCP;
	sHints.ai_flags = AI_PASSIVE;

	result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &sHints, &sInfo);
	if (result != 0)
	{
		std::cout << "getaddrinfo failed with an error: " << result << std::endl;
		WSACleanup();
		return 1;
	}
	std::cout << "getaddrinfo successfull!" << std::endl;

	// Creating a serverSocket!
	SOCKET serverSocket = socket(sInfo->ai_family, sInfo->ai_socktype, sInfo->ai_protocol);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cout << "Socket creation failed with an error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(sInfo);
		WSACleanup();
		return 1;
	}
	
	// Connecting the serverSocket!
	result = connect(serverSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cout << "Connection failed with an error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		freeaddrinfo(sInfo);
		WSACleanup();
		return 1;
	}

	cClient client;

	std::thread receiveThread(&cClient::ReceiveMessage, &client, serverSocket);
	std::thread sendThread(&cClient::SendMessageToServer, &client, serverSocket);

	result = client.SendMessageToServer(serverSocket);

	if (result == SOCKET_ERROR)
	{
		std::cout << "send failed with an error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		freeaddrinfo(sInfo);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(sInfo);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}