#pragma once
// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include "cClient.h"
#include "cBuffer.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

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
	std::cout << "Type a message: ";
	std::getline(std::cin, chatMessage.messageString);
	chatMessage.messageLength = chatMessage.messageString.length();
	chatMessage.packetHeader.messageType = 1;
	chatMessage.packetHeader.packetSize =
		+ chatMessage.messageString.length()
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
	if (result == SOCKET_ERROR)
	{
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		return SOCKET_ERROR;
	}
	return result;
}

int cClient::SendClientNameToServer(SOCKET socket, std::string& clientName)
{
	sMessage passClientName;
	passClientName.messageString = clientName;
	passClientName.messageLength = passClientName.messageString.length();
	passClientName.packetHeader.messageType = 2;
	passClientName.packetHeader.packetSize =
		+passClientName.messageString.length()
		+ sizeof(passClientName.messageLength)
		+ sizeof(passClientName.packetHeader.messageType)
		+ sizeof(passClientName.packetHeader.packetSize);

	uint32_t bufSize = 512;
	cBuffer buffer(bufSize);

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
