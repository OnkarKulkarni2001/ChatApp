#include "cBuffer.h"

cBuffer::cBuffer(int N = 512)
{
	m_BufferData.resize(N);		// Resizing the buffer to the given size
	m_WriteIndex = 0;			// Initializing writing index to start of buffer
	m_ReadIndex = 0;			// Initializing reading index to start of buffer
}

cBuffer::~cBuffer() {}

void cBuffer::GrowBuffer(int additionalBytes)
{
	if (m_WriteIndex + additionalBytes > m_BufferData.size())
	{
		m_BufferData.resize(m_BufferData.size() * 2);
	}
}

uint32_t cBuffer::WriteUInt32_LE(uint32_t value)					// Serializing/Writing the 32 bit integer by 
{	
	GrowBuffer(4);													// breaking it into individual bytes and stored 
	m_BufferData[m_WriteIndex++] = value;							// in LE order in buffer
	m_BufferData[m_WriteIndex++] = value >> 8;
	m_BufferData[m_WriteIndex++] = value >> 16;
	m_BufferData[m_WriteIndex++] = value >> 24;
}

uint32_t cBuffer::ReadUInt32_LE(uint32_t value)						// Deserializing/Reading the individual bytes by
{																	// reconstructing it into original 32 bit integer
	m_BufferData[m_ReadIndex++] = value;							// in LE order in buffer
	m_BufferData[m_ReadIndex++] = value << 8;
	m_BufferData[m_ReadIndex++] = value << 16;
	m_BufferData[m_ReadIndex++] = value << 24;
}

uint16_t cBuffer::WriteUShort16_LE(uint16_t value)					// Serializing/Writing the 16 bit short by 
{				
	GrowBuffer(4);													// breaking it into individual bytes and stored 
	m_BufferData[m_WriteIndex++] = value;							// in LE order in buffer
	m_BufferData[m_WriteIndex++] = value >> 8;
	m_BufferData[m_WriteIndex++] = value >> 16;
	m_BufferData[m_WriteIndex++] = value >> 24;
}

uint16_t cBuffer::ReadUShort16_LE(uint16_t value)					// Deserializing/Reading the individual bytes by
{																	// reconstructing it into original 16 bit short
	m_BufferData[m_ReadIndex++] = value;							// in LE order in buffer
	m_BufferData[m_ReadIndex++] = value << 8;
	m_BufferData[m_ReadIndex++] = value << 16;
	m_BufferData[m_ReadIndex++] = value << 24;
}

std::string cBuffer::WriteString(const std::string& stringToWrite)	// Writes a given string by retrieving length of 	
{	
	GrowBuffer(4);													// given string after that copying value of string 
	int stringLength = stringToWrite.length();						// letter by letter while incrementing write index
	for (int i = 0; i < stringLength; i++)							// buffer
	{
		m_BufferData[m_WriteIndex++] = stringToWrite[i];
	}
}

std::string cBuffer::ReadString(uint32_t stringLength)				// Reads a string if string length is given
{																	// by creating a string of same given length
	std::string stringToRead;										// stringToRead is created by pushing back values 
	for (int i = 0; i < stringLength; i++)							// present at the reading index of buffer
	{																// and then the same string is returned which is created
		stringToRead.push_back(m_BufferData[m_ReadIndex++]);
	}
	return stringToRead;
}