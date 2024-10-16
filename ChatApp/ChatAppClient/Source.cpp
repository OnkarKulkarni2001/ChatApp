#pragma once
// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cClient.h"
#include "cBuffer.h"
#include <iostream>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8412"

std::atomic<bool> isRunning(true);

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
    struct addrinfo sHints;

    ZeroMemory(&sHints, sizeof(sHints));
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
    std::cout << "getaddrinfo successful!" << std::endl;

    // Creating a serverSocket!
    SOCKET serverSocket = socket(sInfo->ai_family, sInfo->ai_socktype, sInfo->ai_protocol);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed with an error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }

    cClient client;
    std::cout << "Enter your name: ";
    std::getline(std::cin, client.clientName);
    
    // Connecting the serverSocket!
    result = connect(serverSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
    if (result == SOCKET_ERROR)
    {
        std::cout << "Connection failed with an error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }

    std::cout << "Conected to room as " << client.clientName << "...\n";
    std::cout << "Type '/exit' to leave the chat.\n";

    // Send client name to server
    result = client.SendClientNameToServer(serverSocket, client.clientName);
    if (result == SOCKET_ERROR)
    {
        std::cout << "send failed with an error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }

    // Start threads for sending and receiving messages
    std::thread receiveThread(&cClient::ReceiveMessage, &client, serverSocket);

    // Sending messages to the server in the main thread
    result = client.SendMessageToServer(serverSocket, client.clientName);
    if (result == SOCKET_ERROR)
    {
        std::cout << "send failed with an error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }
    
    receiveThread.join(); // Ensure that the receive thread finishes before cleanup

    shutdown(serverSocket, SD_BOTH);
    closesocket(serverSocket);
    freeaddrinfo(sInfo);
    WSACleanup();

    return 0;
}
