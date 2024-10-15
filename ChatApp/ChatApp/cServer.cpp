#pragma once

// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cBuffer.h"
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")			// linking Ws2_32.lib library

#define DEFAULT_PORT "8412"					// Setting default port

std::vector<SOCKET> vConnections;			// vector of active connections


int main(int arg, char* argv[])
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		std::cout << "WSAStartup failed with error " << result << std::endl;
		return 1;
	}

	ZeroMemory(&sHints, sizeof(sHints));	// Ensuring we dont have garbage data in sHints
	sHints.ai_family = AF_INET;				// IPv4
	sHints.ai_socktype = SOCK_STREAM;		// Specifying stream socket
	sHints.ai_protocol = IPPROTO_TCP;		// Setting protocol to TCP
	sHints.ai_flags = AI_PASSIVE;			// Binds socket to local machine's address

	result = getaddrinfo(NULL, DEFAULT_PORT, &sHints, &sInfo);

	if (result != 0)
	{
		std::cout << "getaddrinfo failed with error " << result << std::endl;
		WSACleanup();
		return 1;
	}
	std::cout << "getaddrinfo successfull!" << std::endl;

	// Create a listening socket!
	SOCKET listenSocket = socket(sInfo->ai_family, sInfo->ai_socktype, sInfo->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(sInfo);
		WSACleanup();
		return 1;
	}
	std::cout << "Socket created successfully!" << std::endl;

	u_long mode = 1;
	result = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (result != NO_ERROR)
	{
		std::cout << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// Bind the listening socket!
	result = bind(listenSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		std::cout << "Socket binding failed due to: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		freeaddrinfo(sInfo);
		WSACleanup();
		return 1;
	}
	std::cout << "Socket binding successfull!" << std::endl;

	// Listen for connections!
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		std::cout << "Listening to socket failed due to: " << WSAGetLastError() << std::endl;
		closesocket(listenSocket);
		freeaddrinfo(sInfo);
		WSACleanup();
		return 1;
	}
	std::cout << "Listening to socket successfull!" << std::endl;

	
	FD_SET activeSockets;						// Creating set of active sockets
	FD_SET socketsReadyForReading;				// Creating set of sockets ready for reading

	FD_ZERO(&activeSockets);
	FD_ZERO(&socketsReadyForReading);			// Initializing sets of both sockets

	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while (true)
	{
		FD_ZERO(&socketsReadyForReading);
		FD_SET(listenSocket, &socketsReadyForReading);				// Added listenSocket to socketsReadyToRead

		for (int i = 0; i < vConnections.size(); i++)
		{
			FD_SET(vConnections[i], &socketsReadyForReading);		// Adding every single socket to socketSReadyToRead
		}

		int count = select(0, &socketsReadyForReading, NULL, NULL, &tv);

		if (count == 0)
		{
			//std::cout << "No sockets are ready for reading." << std::endl;
			continue;
		}
		if (count == SOCKET_ERROR)
		{
			std::cout << "Select had an error: " << WSAGetLastError() << std::endl;
			continue;
		}

		for (int i = 0; i < vConnections.size(); i++)
		{
			SOCKET socket = vConnections[i];
			if (FD_ISSET(socket, &socketsReadyForReading))
			{
				const int bufSize = 512;
				cBuffer buffer(bufSize);

				int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
				if (result == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						continue;
					}
					else
					{
						std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
						closesocket(socket);
						FD_CLR(socket, &activeSockets);
						vConnections.erase(vConnections.begin() + i);
						i--;
						continue;
					}
				}
				if (result == 0)
				{
					std::cout << "Client disconnected!" << std::endl;
					closesocket(socket);
					FD_CLR(socket, &activeSockets);
					vConnections.erase(vConnections.begin() + i);
					i--;
					continue;
				}

				uint16_t packetSize = buffer.ReadUShort16_LE();
				uint16_t messageType = buffer.ReadUShort16_LE();

				if (messageType == 1)
				{
					uint32_t messageLength = buffer.ReadUInt32_LE();
					std::string messageString = buffer.ReadString(messageLength);

					std::cout /*<< "Packet Size: " << packetSize << "\n"
						<< "Message Type: " << messageType << "\n"
						<< "Message Length: " << messageLength << "\n"*/
						<< "Message: " << messageString.c_str() << "\n";

					sMessage receivedMessage;
					receivedMessage.messageString = "Server received message from " + (int)socket;
					receivedMessage.messageLength = receivedMessage.messageString.length();
					receivedMessage.packetHeader.messageType = 1;
					receivedMessage.packetHeader.packetSize =
						receivedMessage.messageString.length()
						+ sizeof(receivedMessage.messageLength)
						+ sizeof(receivedMessage.packetHeader.messageType)
						+ sizeof(receivedMessage.packetHeader.packetSize);

					cBuffer bufferSend(bufSize);

					bufferSend.WriteUShort16_LE(receivedMessage.packetHeader.packetSize);
					bufferSend.WriteUShort16_LE(receivedMessage.packetHeader.messageType);
					bufferSend.WriteUInt32_LE(receivedMessage.messageLength);
					bufferSend.WriteString(receivedMessage.messageString);

					for (int j = 0; j < vConnections.size(); j++)
					{
						SOCKET outSocket = vConnections[j];
						if (outSocket != listenSocket)
						{
							send(outSocket, (const char*)(&bufferSend.m_BufferData[0]), receivedMessage.packetHeader.packetSize, 0);
						}
					}
				}

				FD_CLR(socket, &socketsReadyForReading);
				count--;
			}
		}

		if (count > 0)
		{
			if (FD_ISSET(listenSocket, &socketsReadyForReading))
			{
				SOCKET newConnection = accept(listenSocket, NULL, NULL);
				if (newConnection == INVALID_SOCKET)
				{
					std::cout << "Accept failed with error: " << WSAGetLastError();
				}
				else
				{
					result = ioctlsocket(newConnection, FIONBIO, &mode);
					if (result != NO_ERROR)
					{
						std::cout << "Failed to set non-blocking mode for client: " << WSAGetLastError() << std::endl;
						closesocket(newConnection);
					}
					else
					{
						vConnections.push_back(newConnection);
						//FD_SET(newConnection, &vConnections);
						//FD_CLR(listenSocket, &socketsReadyForReading);
						std::cout << "Client connected with socket: " << (int)newConnection << std::endl;
					}
				}
			}
		}
	}

	freeaddrinfo(sInfo);
	closesocket(listenSocket);

	for (int i = 0; i < vConnections.size(); i++)
	{
		closesocket(vConnections[i]);
	}

	freeaddrinfo(sInfo);
	WSACleanup();

	return 0;
}