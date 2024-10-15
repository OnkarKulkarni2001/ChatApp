#pragma once
// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cClient.h"
#include "cBuffer.h"
#include <iostream>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8412"
//
//int main(int arg, char* argv[])
//{
//	WSADATA wsaData;
//	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
//
//	if (result != 0)
//	{
//		std::cout << "WSAStartup failed with an error: " << result << std::endl;
//		return 1;
//	}
//
//	struct addrinfo* sInfo = nullptr;
//	struct addrinfo sHints;
//
//	ZeroMemory(&sHints, sizeof(sHints));
//	sHints.ai_family = AF_INET;
//	sHints.ai_socktype = SOCK_STREAM;
//	sHints.ai_protocol = IPPROTO_TCP;
//	sHints.ai_flags = AI_PASSIVE;
//	result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &sHints, &sInfo);
//	if (result != 0)
//	{
//		std::cout << "getaddrinfo failed with an error: " << result << std::endl;
//		WSACleanup();
//		return 1;
//	}
//	std::cout << "getaddrinfo successfull!" << std::endl;
//
//	// Creating a serverSocket!
//	SOCKET serverSocket = socket(sInfo->ai_family, sInfo->ai_socktype, sInfo->ai_protocol);
//	if (serverSocket == INVALID_SOCKET)
//	{
//		std::cout << "Socket creation failed with an error: " << WSAGetLastError() << std::endl;
//		freeaddrinfo(sInfo);
//		WSACleanup();
//		return 1;
//	}
//
//	// Connecting the serverSocket!
//	result = connect(serverSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
//	if (serverSocket == SOCKET_ERROR)
//	{
//		std::cout << "Connection failed with an error: " << WSAGetLastError() << std::endl;
//		closesocket(serverSocket);
//		freeaddrinfo(sInfo);
//		WSACleanup();
//		return 1;
//	}
//
//	cClient client;
//	std::cout << "Enter your name: ";
//	std::getline(std::cin, client.clientName);
//
//
//	result = client.SendClientNameToServer(serverSocket, client.clientName);
//	if (result == SOCKET_ERROR)
//	{
//		std::cout << "send failed with an error: " << WSAGetLastError() << std::endl;
//		closesocket(serverSocket);
//		freeaddrinfo(sInfo);
//		WSACleanup();
//		return 1;
//	}
//
//	std::thread receiveThread(&cClient::ReceiveMessage, &client, serverSocket);
//	
//	while (true)
//	{
//		result = client.SendMessageToServer(serverSocket);
//		if (result == SOCKET_ERROR)
//		{
//			std::cout << "send failed with an error: " << WSAGetLastError() << std::endl;
//			closesocket(serverSocket);
//			freeaddrinfo(sInfo);
//			WSACleanup();
//			return 1;
//		}
//	}
//
//	//std::thread sendNameThread(&cClient::SendClientNameToServer, &client, serverSocket);
//	//std::thread sendThread(&cClient::SendMessageToServer, &client, serverSocket);
//
//	receiveThread.join();  // Make sure threads finish before cleanup
//	//sendNameThread.join();
//	//sendThread.join();
//
//	system("Pause"); // Force the user to press enter to continue;
//
//	closesocket(serverSocket);
//	freeaddrinfo(sInfo);
//	WSACleanup();
//
//	return 0;
//}

//#pragma once
//// WinSock2 Windows Sockets
//#define WIN32_LEAN_AND_MEAN
//
//#include "cClient.h"
//#include "cBuffer.h"
//#include <iostream>
//#include <thread>
//#pragma comment(lib, "Ws2_32.lib")
//
//#define DEFAULT_PORT "8412"
void ReceiveMessage(SOCKET socket)
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
            /*if (messageType == 2)
            {
                uint32_t clientNameLength = buffer.ReadUInt32_LE();
                std::string clientNameString = buffer.ReadString(clientNameLength);
                ConnectionWithName[socket] = clientNameString;
                std::cout << ConnectionWithName[socket] << " has joined room." << std::endl;
            }
            if (messageType == 1)
            {
                uint32_t messageLength = buffer.ReadUInt32_LE();
                std::string messageString = buffer.ReadString(messageLength);
                std::cout << ConnectionWithName[socket] << ": " << messageString.c_str() << "\n";
            }*/
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

    // Connecting the serverSocket!
    result = connect(serverSocket, sInfo->ai_addr, (int)sInfo->ai_addrlen);
    if (serverSocket == SOCKET_ERROR)
    {
        std::cout << "Connection failed with an error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        freeaddrinfo(sInfo);
        WSACleanup();
        return 1;
    }

    cClient client;
    std::cout << "Enter your name: ";
    std::getline(std::cin, client.clientName);

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
    std::thread receiveThread(ReceiveMessage, serverSocket);

    // Sending messages to the server in the main thread
    while (true)
    {
        result = client.SendMessageToServer(serverSocket);
        if (result == SOCKET_ERROR)
        {
            std::cout << "send failed with an error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            freeaddrinfo(sInfo);
            WSACleanup();
            return 1;
        }
    }

    receiveThread.join(); // Ensure that the receive thread finishes before cleanup

    closesocket(serverSocket);
    freeaddrinfo(sInfo);
    WSACleanup();

    return 0;
}
