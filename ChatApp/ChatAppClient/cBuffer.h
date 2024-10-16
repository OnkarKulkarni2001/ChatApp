#pragma once
#include <vector>
#include <string>

class cBuffer
{
public:
	std::vector<uint8_t> m_BufferData;
	int m_WriteIndex;
	int m_ReadIndex;

	cBuffer(int N);
	~cBuffer();

	void GrowBuffer(int additionalBytes, cBuffer& buffer);

	void WriteUInt32_LE(uint32_t value);
	uint32_t ReadUInt32_LE();

	void WriteUShort16_LE(uint16_t value);
	uint16_t ReadUShort16_LE();

	void WriteString(const std::string& stringToWrite);
	std::string ReadString(uint32_t stringLength);
};