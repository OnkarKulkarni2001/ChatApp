#pragma once
#include <cstdint>
#include <string>

struct sPacketHeader
{
	uint32_t packetSize;
	uint32_t messageType;
};

struct sClient
{
	SOCKET clientSocket;
	std::string clientName;
};

struct sMessage
{
	sClient client;
	sPacketHeader packetHeader;
	uint32_t messageLength;
	std::string messageString;
};