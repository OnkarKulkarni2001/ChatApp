#pragma once

// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cBuffer.h"
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "cServer.h"

#pragma comment(lib, "Ws2_32.lib")			// linking Ws2_32.lib library

#define DEFAULT_PORT "8412"					// Setting default port

// Function to initialize the server
int cServer::initializeServer(SOCKET& listenSocket) {
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
void cServer::handleNewConnections(SOCKET listenSocket) {
    SOCKET newConnection = accept(listenSocket, NULL, NULL);
    vConnections.push_back(newConnection);
}


// Function to handle client messages
void cServer::handleClientMessages(SOCKET socket, SOCKET& listenSocket, std::vector<SOCKET> vConnections) {
    int bufSize = 4096;
    cBuffer buffer(bufSize);

    int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
    if (result == 0) {
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
            std::cout << "------------ " << ConnectionWithName[socket] << " has joined the room. ------------" << std::endl;
            for (int j = 0; j < vConnections.size(); j++)
            {
                SOCKET outSocket = vConnections[j];

                if (outSocket != listenSocket && outSocket != socket)
                {
                    send(outSocket, (const char*)(&buffer.m_BufferData[0]), packetSize, 0);
                }
            }
        }
        if (messageType == 1) {
            buffer.GrowBuffer(packetSize, buffer);
            uint32_t messageLength = buffer.ReadUInt32_LE();
            std::string messageString = buffer.ReadString(messageLength);

            if (messageString.find(ConnectionWithName[socket] + " has disconnected!") != std::string::npos)
            {
                messageString = "------------ " + ConnectionWithName[socket] + " disconnected! ------------";
                closesocket(socket);
                vConnections.erase(std::remove(vConnections.begin(), vConnections.end(), socket), vConnections.end());
            }

            printf("PacketSize:%d\nMessageType:%d\nMessageLength:%d\nMessage:%s\n", packetSize, messageType, messageLength, messageString.c_str());
            
            // Prepare message for broadcasting
            for (int j = 0; j < vConnections.size(); j++)
            {
                SOCKET outSocket = vConnections[j];

                if (outSocket != listenSocket && outSocket != socket)
                {
                    send(outSocket, (const char*)(&buffer.m_BufferData[0]), packetSize, 0);
                }
            }
        }
    }
}