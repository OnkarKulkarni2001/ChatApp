#pragma once
// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cClient.h"
#include "cBuffer.h"
#include <iostream>
#include <map>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

std::atomic<bool> isRunningReceive(true);

int cClient::SendMessageToServer(SOCKET socket, std::string& clientName)
{
	int result = 0;
	while (isRunningReceive)
	{
		int bufSize = 4096;
		cBuffer buffer(bufSize);
		sMessage chatMessage;
		std::getline(std::cin, chatMessage.messageString);
		chatMessage.messageString = clientName + ": " + chatMessage.messageString;
		if (chatMessage.messageString.find("/quit") != std::string::npos)
		{
			std::cout << "Exiting the chat..\n";
			chatMessage.messageString = clientName + " has disconnected!";
		}

		chatMessage.messageLength = chatMessage.messageString.length();
		chatMessage.packetHeader.messageType = 1;
		chatMessage.packetHeader.packetSize =
			+ chatMessage.messageString.length()
			+ sizeof(chatMessage.messageLength)
			+ sizeof(chatMessage.packetHeader.messageType)
			+ sizeof(chatMessage.packetHeader.packetSize);

		buffer.GrowBuffer(chatMessage.packetHeader.packetSize, buffer);
		buffer.WriteUShort16_LE(chatMessage.packetHeader.packetSize);
		buffer.WriteUShort16_LE(chatMessage.packetHeader.messageType);
		buffer.WriteUInt32_LE(chatMessage.messageLength);
		buffer.WriteString(chatMessage.messageString);
		

		result = send(socket, (const char*)(&buffer.m_BufferData[0]), chatMessage.packetHeader.packetSize, 0);
		if (result == SOCKET_ERROR)
		{
			std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
			return SOCKET_ERROR;
		}

		if (chatMessage.messageString.find(clientName + " has disconnected!") != std::string::npos)
		{
			std::cout << "Exiting the chat..\n";
			isRunningReceive.store(false, std::memory_order_relaxed);
			closesocket(socket);
			break;
		}
	}
	
	return result;
}

int cClient::SendClientNameToServer(SOCKET socket, std::string& clientName)
{
	int bufSize = 4096;
	cBuffer buffer(bufSize);
	sMessage passClientName;
	passClientName.messageString = clientName;
	passClientName.messageLength = passClientName.messageString.length();
	passClientName.packetHeader.messageType = 2;
	passClientName.packetHeader.packetSize =
		+ passClientName.messageString.length()
		+ sizeof(passClientName.messageLength)
		+ sizeof(passClientName.packetHeader.messageType)
		+ sizeof(passClientName.packetHeader.packetSize);

	buffer.GrowBuffer(passClientName.packetHeader.packetSize, buffer);
	buffer.WriteUShort16_LE(passClientName.packetHeader.packetSize);
	buffer.WriteUShort16_LE(passClientName.packetHeader.messageType);
	buffer.WriteUInt32_LE(passClientName.messageLength);
	buffer.WriteString(passClientName.messageString);

	int result = send(socket, (const char*)(&buffer.m_BufferData[0]), passClientName.packetHeader.packetSize, 0);
	if (result == SOCKET_ERROR)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		return SOCKET_ERROR;
	}
	return result;
}

void cClient::ReceiveMessage(SOCKET socket)
{
	/*while (true)
	{
		uint32_t bufSize = 512;
		cBuffer buffer(bufSize);

		int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
		if (result == 0)
		{
			std::cout << "Server closed the connection" << std::endl;
			break;
		}
		else if (result == SOCKET_ERROR)
		{
			std::cout << "recv failed with an error: " << WSAGetLastError() << std::endl;
			break;
		}
		uint16_t packetSize = buffer.ReadUShort16_LE();
		uint16_t messageType = buffer.ReadUShort16_LE();

		if (messageType == 1)
		{
			uint32_t messageLength = buffer.ReadUInt32_LE();
			std::string messageString = buffer.ReadString(messageLength);

			std::cout << "Message: " << messageString << std::endl;
		}
	}*/

	while (isRunningReceive.load(std::memory_order_relaxed))
	{
		int bufSize = 4096;
		cBuffer buffer(bufSize);
		int result = recv(socket, (char*)(&buffer.m_BufferData[0]), bufSize, 0);
		if (result > 0)
		{
			uint32_t packetSize = buffer.ReadUShort16_LE();
			uint32_t messageType = buffer.ReadUShort16_LE();
			buffer.GrowBuffer(packetSize, buffer);
			if (messageType == 1)
			{
				// handle the message
				uint32_t messageLength = buffer.ReadUInt32_LE();
				std::string msg = buffer.ReadString(messageLength);

				std::cout << msg << "\n";
			}
		}
		else if (result == 0)
		{
			std::cout << "Server closed the connection.\n";
			break;
		}
		else {
			printf("recv failed with error %d\n", WSAGetLastError());
			break;
		}
	}
}


//void cClient::ReceiveMessage(SOCKET socket)
//{
//
//	const int bufSize = 512;
//	cBuffer buffer(bufSize);
//	char recvbuf[bufSize];
//	int result;
//
//	while (true)
//	{
//		// Clear the buffer before receiving new data
//		ZeroMemory(recvbuf, bufSize);
//
//		// Receive data from the socket
//		result = recv(socket, recvbuf, bufSize, 0);
//
//		if (result > 0)
//		{
//			// Data was received, now store it in the buffer
//			buffer.m_BufferData.assign(recvbuf, recvbuf + result);
//
//			// Read the packet size and message type
//			uint16_t packetSize = buffer.ReadUShort16_LE();
//			uint16_t messageType = buffer.ReadUShort16_LE();
//
//			// Process the message based on message type
//			if (messageType == 1)
//			{
//				// Message contains text
//				uint32_t messageLength = buffer.ReadUInt32_LE();
//				std::string messageString = buffer.ReadString(messageLength);
//
//				std::cout << "Message: " << messageString << std::endl;
//			}
//			else if (messageType == 2)
//			{
//				// Message contains a client name
//				uint32_t clientNameLength = buffer.ReadUInt32_LE();
//				std::string clientNameString = buffer.ReadString(clientNameLength);
//
//				std::cout << "Client Name: " << clientNameString << std::endl;
//			}
//		}
//		else if (result == 0)
//		{
//			std::cout << "Server closed the connection." << std::endl;
//			break;
//		}
//		else
//		{
//			std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
//			break;
//		}
//
//		// Add a small delay to avoid high CPU usage in case of no incoming data
//		std::this_thread::sleep_for(std::chrono::milliseconds(10));
//	}
//}