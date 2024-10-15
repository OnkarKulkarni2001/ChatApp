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

// Main server loop
int main(int arg, char* argv[]) {
    cServer server;
    SOCKET listenSocket;
    if (server.initializeServer(listenSocket) != 0) {
        return 1;
    }

    FD_SET activeSockets, socketsReadyForReading;
    FD_ZERO(&activeSockets);
    FD_ZERO(&socketsReadyForReading);

    timeval tv = { 1, 0 };  // Timeout value for select

    while (true) {
        FD_ZERO(&socketsReadyForReading);
        FD_SET(listenSocket, &socketsReadyForReading);  // Add listenSocket

        for (SOCKET socket : server.vConnections) {
            FD_SET(socket, &socketsReadyForReading);    // Add all client sockets
        }

        int count = select(0, &socketsReadyForReading, NULL, NULL, &tv);
        if (count == SOCKET_ERROR) {
            std::cout << "Select failed with error: " << WSAGetLastError() << std::endl;
            break;
        }

        if (FD_ISSET(listenSocket, &socketsReadyForReading)) {
            server.handleNewConnections(listenSocket);
        }

        for (SOCKET socket : server.vConnections) {
            if (FD_ISSET(socket, &socketsReadyForReading)) {
                server.handleClientMessages(socket);
            }
        }
    }

    // Cleanup
    freeaddrinfo(server.sInfo);
    closesocket(listenSocket);
    for (SOCKET socket : server.vConnections) {
        closesocket(socket);
    }
    WSACleanup();

    return 0;
}
