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
#include <map>

#pragma comment(lib, "Ws2_32.lib")			// linking Ws2_32.lib library

#define DEFAULT_PORT "8412"					// Setting default port

std::vector<SOCKET> vConnections;			// vector of active connections
std::map<SOCKET, std::string> ConnectionWithName;

//int main(int arg, char* argv[])
//{
//	WSADATA wsaData;
//	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//	if (result != 0)
//	{
//		std::cout << "WSAStartup failed with error " << result << std::endl;
//		return 1;
//	}
//
//	ZeroMemory(&sHints, sizeof(sHints));	// Ensuring we dont have garbage data in sHints
//	sHints.ai_family = AF_INET;				// IPv4
//	sHints.ai_socktype = SOCK_STREAM;		// Specifying stream socket
//	sHints.ai_protocol = IPPROTO_TCP;		// Setting protocol to TCP
//	sHints.ai_flags = AI_PASSIVE;			// Binds socket to local machine's address
//
//	result = getaddrinfo(NULL, DEFAULT_PORT, &sHints, &sInfo);
//
//	if (result != 0)
//	{
//		std::cout << "getaddrinfo failed with error " << result << std::endl;
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "getaddrinfo successfull!" << std::endl;
//
//	// Create a listening socket!
//	SOCKET listenSocket = socket(sInfo->ai_family, sInfo->ai_socktype, sInfo->ai_protocol);
//
//	if (listenSocket == INVALID_SOCKET)
//	{
//		std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
//		freeaddrinfo(sInfo);
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "Socket created successfully!" << std::endl;
//
//	u_long mode = 1;
//	result = ioctlsocket(listenSocket, FIONBIO, &mode);
//	if (result != NO_ERROR)
//	{
//		std::cout << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
//		closesocket(listenSocket);
//		WSACleanup();
//		return 1;
//	}
//
//	// Bind the listening socket!
//	result = bind(listenSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
//	if (result == SOCKET_ERROR)
//	{
//		std::cout << "Socket binding failed due to: " << WSAGetLastError() << std::endl;
//		closesocket(listenSocket);
//		freeaddrinfo(sInfo);
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "Socket binding successfull!" << std::endl;
//
//	// Listen for connections!
//	result = listen(listenSocket, SOMAXCONN);
//	if (result == SOCKET_ERROR)
//	{
//		std::cout << "Listening to socket failed due to: " << WSAGetLastError() << std::endl;
//		closesocket(listenSocket);
//		freeaddrinfo(sInfo);
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "Listening to socket successfull!" << std::endl;
//
//	
//	FD_SET activeSockets;						// Creating set of active sockets
//	FD_SET socketsReadyForReading;				// Creating set of sockets ready for reading
//
//	FD_ZERO(&activeSockets);
//	FD_ZERO(&socketsReadyForReading);			// Initializing sets of both sockets
//
//	timeval tv;
//	tv.tv_sec = 1;
//	tv.tv_usec = 0;
//
//
//	while (true)
//	{
//		FD_ZERO(&socketsReadyForReading);
//		FD_SET(listenSocket, &socketsReadyForReading);				// Added listenSocket to socketsReadyToRead
//
//		for (int i = 0; i < vConnections.size(); i++)
//		{
//			FD_SET(vConnections[i], &socketsReadyForReading);		// Adding every single socket to socketSReadyToRead
//		}
//
//		int count = select(0, &socketsReadyForReading, NULL, NULL, &tv);
//
//		if (count == 0)
//		{
//			//std::cout << "No sockets are ready for reading." << std::endl;
//			continue;
//		}
//		if (count == SOCKET_ERROR)
//		{
//			std::cout << "Select had an error: " << WSAGetLastError() << std::endl;
//			continue;
//		}
//
//		for (int i = 0; i < vConnections.size(); i++)
//		{
//			SOCKET socket = vConnections[i];
//				
//			if (FD_ISSET(socket, &socketsReadyForReading))
//			{
//				const int bufSize = 512;
//				cBuffer buffer(bufSize);
//
//				int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
//				if (result == SOCKET_ERROR)
//				{
//					if (WSAGetLastError() == WSAEWOULDBLOCK)
//					{
//						continue;
//					}
//					else
//					{
//						std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
//						closesocket(socket);
//						FD_CLR(socket, &activeSockets);
//						vConnections.erase(vConnections.begin() + i);
//						i--;
//						continue;
//					}
//				}
//				if (result == 0)
//				{
//					std::cout << "Client disconnected!" << std::endl;
//					closesocket(socket);
//					FD_CLR(socket, &activeSockets);
//					vConnections.erase(vConnections.begin() + i);
//					i--;
//					continue;
//				}
//
//				uint16_t packetSize = buffer.ReadUShort16_LE();
//				uint16_t messageType = buffer.ReadUShort16_LE();
//
//				if (messageType == 2)
//				{
//					uint32_t clientNameLength = buffer.ReadUInt32_LE();
//					std::string clientNameString = buffer.ReadString(clientNameLength);
//					ConnectionWithName[socket] = clientNameString;
//					std::cout << ConnectionWithName[socket] << " has joined room." << std::endl;
//				}
//				if (messageType == 1)
//				{
//					uint32_t messageLength = buffer.ReadUInt32_LE();
//					std::string messageString = buffer.ReadString(messageLength);
//					std::cout << ConnectionWithName[socket] << ": " << messageString.c_str() << "\n";
//				}
//
//
//				FD_CLR(socket, &socketsReadyForReading);
//				count--;
//			}
//		}
//
//		if (count > 0)
//		{
//			if (FD_ISSET(listenSocket, &socketsReadyForReading))
//			{
//				SOCKET newConnection = accept(listenSocket, NULL, NULL);
//				sClient client;
//				ConnectionWithName.insert(std::make_pair(newConnection, client.clientName));
//
//				if (newConnection == INVALID_SOCKET)
//				{
//					std::cout << "Accept failed with error: " << WSAGetLastError();
//				}
//				else
//				{
//					result = ioctlsocket(newConnection, FIONBIO, &mode);
//					if (result != NO_ERROR)
//					{
//						std::cout << "Failed to set non-blocking mode for client: " << WSAGetLastError() << std::endl;
//						closesocket(newConnection);
//					}
//					else
//					{
//						vConnections.push_back(newConnection);
//						//std::cout << "Client connected with socket: " << (int)newConnection << std::endl;
//					}
//				}
//			}
//		}
//	}
//
//	freeaddrinfo(sInfo);
//	closesocket(listenSocket);
//
//	for (int i = 0; i < vConnections.size(); i++)
//	{
//		closesocket(vConnections[i]);
//	}
//
//	freeaddrinfo(sInfo);
//	WSACleanup();
//
//	return 0;
//}

#pragma once

//// WinSock2 Windows Sockets
//#define WIN32_LEAN_AND_MEAN
//
//#include "cBuffer.h"
//#include <Windows.h>
//#include <WinSock2.h>
//#include <Ws2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include "structs.h"
//#include <iostream>
//#include <map>
//#include <vector>
//
//#pragma comment(lib, "Ws2_32.lib")  // linking Ws2_32.lib library
//
//#define DEFAULT_PORT "8412"          // Setting default port
//
//std::vector<SOCKET> vConnections;    // vector of active connections
//std::map<SOCKET, std::string> ConnectionWithName;

//addrinfo sHints, * sInfo = nullptr;   // Declare sHints and sInfo

void broadcastMessage(const std::string& message) {
    for (SOCKET clientSocket : vConnections) {
        send(clientSocket, message.c_str(), message.size(), 0);
    }
}

// Function to initialize the server
int initializeServer(SOCKET& listenSocket) {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup failed with error " << result << std::endl;
        return 1;
    }

    ZeroMemory(&sHints, sizeof(sHints));    // Ensuring we don't have garbage data in sHints
    sHints.ai_family = AF_INET;             // IPv4
    sHints.ai_socktype = SOCK_STREAM;       // Stream socket
    sHints.ai_protocol = IPPROTO_TCP;       // TCP protocol
    sHints.ai_flags = AI_PASSIVE;           // Binds socket to local machine's address

    // Resolve server address and port
    result = getaddrinfo(NULL, DEFAULT_PORT, &sHints, &sInfo);
    if (result != 0) {
        std::cout << "getaddrinfo failed with error " << result << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "getaddrinfo successful!" << std::endl;

    // Create a listening socket
    listenSocket = socket(sInfo->ai_family, sInfo->ai_socktype, sInfo->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        std::cout << "Socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }
    std::cout << "Socket created successfully!" << std::endl;

    // Set the socket to non-blocking mode
    u_long mode = 1;
    result = ioctlsocket(listenSocket, FIONBIO, &mode);
    if (result != NO_ERROR) {
        std::cout << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Bind the listening socket
    result = bind(listenSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
    if (result == SOCKET_ERROR) {
        std::cout << "Socket binding failed due to: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }
    std::cout << "Socket binding successful!" << std::endl;

    // Listen for connections
    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cout << "Listening to socket failed due to: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }
    std::cout << "Listening to socket successful!" << std::endl;

    return 0;   // Server initialized successfully
}

// Function to handle new connections
void handleNewConnections(SOCKET listenSocket) {
    SOCKET newConnection = accept(listenSocket, NULL, NULL);
    if (newConnection == INVALID_SOCKET) {
        std::cout << "Accept failed with error: " << WSAGetLastError() << std::endl;
    }
    else {
        u_long mode = 1;
        int result = ioctlsocket(newConnection, FIONBIO, &mode);
        if (result != NO_ERROR) {
            std::cout << "Failed to set non-blocking mode for client: " << WSAGetLastError() << std::endl;
            closesocket(newConnection);
        }
        else {
            vConnections.push_back(newConnection);
            sClient client;  // Create a client instance
            ConnectionWithName[newConnection] = client.clientName;
            std::cout << "New client connected!" << std::endl;

            // Broadcast to all clients
            std::string welcomeMessage = "A new client has joined the room.\n";
            broadcastMessage(welcomeMessage);
        }
    }
}


// Function to handle client messages
void handleClientMessages(SOCKET socket) {
    const int bufSize = 512;
    cBuffer buffer(bufSize);

    int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
    if (result == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
            closesocket(socket);
            vConnections.erase(std::remove(vConnections.begin(), vConnections.end(), socket), vConnections.end());
        }
    }
    else if (result == 0) {
        std::cout << "Client disconnected!" << std::endl;
        closesocket(socket);
        vConnections.erase(std::remove(vConnections.begin(), vConnections.end(), socket), vConnections.end());
    }
    else {
        // Process received data
        uint16_t packetSize = buffer.ReadUShort16_LE();
        uint16_t messageType = buffer.ReadUShort16_LE();

        if (messageType == 2) {
            uint32_t clientNameLength = buffer.ReadUInt32_LE();
            std::string clientNameString = buffer.ReadString(clientNameLength);
            ConnectionWithName[socket] = clientNameString;
            std::cout << ConnectionWithName[socket] << " has joined the room." << std::endl;
            broadcastMessage(ConnectionWithName[socket] + " has joined the room.\n");
        }
        else if (messageType == 1) {
            uint32_t messageLength = buffer.ReadUInt32_LE();
            std::string messageString = buffer.ReadString(messageLength);
            std::cout << ConnectionWithName[socket] << ": " << messageString << "\n";
            broadcastMessage(ConnectionWithName[socket] + ": " + messageString + "\n");
        }
    }
}


// Main server loop
int main(int arg, char* argv[]) {
    SOCKET listenSocket;
    if (initializeServer(listenSocket) != 0) {
        return 1;
    }

    FD_SET activeSockets, socketsReadyForReading;
    FD_ZERO(&activeSockets);
    FD_ZERO(&socketsReadyForReading);

    timeval tv = { 1, 0 };  // Timeout value for select

    while (true) {
        FD_ZERO(&socketsReadyForReading);
        FD_SET(listenSocket, &socketsReadyForReading);  // Add listenSocket

        for (SOCKET socket : vConnections) {
            FD_SET(socket, &socketsReadyForReading);    // Add all client sockets
        }

        int count = select(0, &socketsReadyForReading, NULL, NULL, &tv);
        if (count == SOCKET_ERROR) {
            std::cout << "Select failed with error: " << WSAGetLastError() << std::endl;
            break;
        }

        if (FD_ISSET(listenSocket, &socketsReadyForReading)) {
            handleNewConnections(listenSocket);
        }

        for (SOCKET socket : vConnections) {
            if (FD_ISSET(socket, &socketsReadyForReading)) {
                handleClientMessages(socket);
            }
        }
    }

    // Cleanup
    freeaddrinfo(sInfo);
    closesocket(listenSocket);
    for (SOCKET socket : vConnections) {
        closesocket(socket);
    }
    WSACleanup();

    return 0;
}
