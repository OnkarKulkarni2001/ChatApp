#pragma once
#include <cstdint>
#include <string>

struct sPacketHeader
{
	uint32_t packetSize;
	uint32_t messageType;
};

struct sMessage
{
	sPacketHeader packetHeader;
	uint32_t messageLength;
	std::string messageString;
};

struct addrinfo sHints;