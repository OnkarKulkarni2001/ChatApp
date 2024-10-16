#include "cBuffer.h"

cBuffer::cBuffer(int N)
{
	m_BufferData.resize(N);		// Resizing the buffer to the given size
	m_WriteIndex = 0;			// Initializing writing index to start of buffer
	m_ReadIndex = 0;			// Initializing reading index to start of buffer
}

cBuffer::~cBuffer() {}

void cBuffer::GrowBuffer(int additionalBytes, cBuffer& buffer) {
	if (additionalBytes + buffer.m_ReadIndex > buffer.m_BufferData.size()) {
		// Double the buffer size or add additional bytes based on need
		int newSize = m_BufferData.size() * 2;
		if (newSize < additionalBytes + buffer.m_ReadIndex) {
			newSize = additionalBytes + buffer.m_ReadIndex;
			newSize = newSize * 2;
		}
		buffer.m_BufferData.resize(newSize);
	}
}

void cBuffer::WriteUInt32_LE(uint32_t value)					// Serializing/Writing the 32 bit integer by 
{													// breaking it into individual bytes and stored 
	//GrowBuffer(sizeof(value));
	m_BufferData[m_WriteIndex++] = value;							// in LE order in buffer
	m_BufferData[m_WriteIndex++] = value >> 8;
	m_BufferData[m_WriteIndex++] = value >> 16;
	m_BufferData[m_WriteIndex++] = value >> 24;
}

uint32_t cBuffer::ReadUInt32_LE()									// Deserializing/Reading the individual bytes by
{																	// reconstructing it into original 32 bit integer
	uint32_t value = 0;
	value |= m_BufferData[m_ReadIndex++];							// in LE order in buffer
	value |= m_BufferData[m_ReadIndex++] << 8;
	value |= m_BufferData[m_ReadIndex++] << 16;
	value |= m_BufferData[m_ReadIndex++] << 24;

	return value;
}

void cBuffer::WriteUShort16_LE(uint16_t value)					// Serializing/Writing the 16 bit short by 
{												// breaking it into individual bytes and stored 
	//GrowBuffer(sizeof(value));
	m_BufferData[m_WriteIndex++] = value;							// in LE order in buffer
	m_BufferData[m_WriteIndex++] = value >> 8;
}

uint16_t cBuffer::ReadUShort16_LE()									// Deserializing/Reading the individual bytes by
{																	// reconstructing it into original 16 bit short
	uint16_t value = 0;
	value |= m_BufferData[m_ReadIndex++];							// in LE order in buffer
	value |= m_BufferData[m_ReadIndex++] << 8;

	return value;
}

void cBuffer::WriteString(const std::string& stringToWrite)	// Writes a given string by retrieving length of 	
{													// given string after that copying value of string 
	//GrowBuffer(sizeof(stringToWrite));
	int stringLength = stringToWrite.length();						// letter by letter while incrementing write index
	for (int i = 0; i < stringLength; i++)							// buffer
	{
		m_BufferData[m_WriteIndex++] = stringToWrite[i];
	}
}

std::string cBuffer::ReadString(uint32_t stringLength)				// Reads a string if string length is given
{																	// by creating a string of same given length
	//GrowBuffer(sizeof(stringLength));
	std::string stringToRead;										// stringToRead is created by pushing back values 
	for (int i = 0; i < stringLength; i++)							// present at the reading index of buffer
	{																// and then the same string is returned which is created
		stringToRead.push_back(m_BufferData[m_ReadIndex++]);
	}
	return stringToRead;
}