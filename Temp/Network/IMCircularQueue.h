// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.
#pragma once

const size_t MAX_QUEUE_SIZE = static_cast<size_t>(-1);

class IMCircularQueue
{
private:
	size_t	m_szMaxBufferSize;
	size_t	m_szBufferSize;
	char*	m_pBuffer;
	size_t	m_szRead;
	size_t	m_szWrite;

public:
	IMCircularQueue() : m_szMaxBufferSize(MAX_QUEUE_SIZE), m_szBufferSize(0), m_pBuffer(nullptr), m_szRead(0), m_szWrite(0) {}
	~IMCircularQueue() { Release(); }

public:
	void Release()
	{
		m_szMaxBufferSize = MAX_QUEUE_SIZE;
		m_szBufferSize = 0;
		if(m_pBuffer)
		{
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
		}
		m_szRead = 0;
		m_szWrite = 0;
	}
	bool Alloc(size_t szBufferSize, size_t szChunkSize)
	{
		if(szBufferSize < szChunkSize) return false;

		Release();

		m_szBufferSize = szBufferSize;
		m_pBuffer = new char[m_szBufferSize];

		return true;
	}
	bool Realloc(size_t szBufferSize)
	{
		if(!m_pBuffer) return false;
		if(szBufferSize < GetLoadSize()) return false;
		if(szBufferSize > m_szMaxBufferSize) return false;

		char* pBuffer = new char[szBufferSize];
		size_t szRead = ReadData(pBuffer, MAX_QUEUE_SIZE);

		m_szBufferSize = szBufferSize;
		delete[] m_pBuffer;
		m_pBuffer = pBuffer;
		m_szRead = 0;
		m_szWrite = szRead;

		return true;
	}

public:
	void SetMaxBufferSize(size_t szMaxBufferSize = MAX_QUEUE_SIZE) { m_szMaxBufferSize = szMaxBufferSize; }
	size_t GetCapacity() const { return m_szBufferSize; }
	size_t GetLoadSize() const { return (m_szRead <= m_szWrite)? m_szWrite - m_szRead : m_szWrite + m_szBufferSize - m_szRead; }
	size_t GetLeftSize() const { return GetCapacity() - GetLoadSize(); }
	size_t ReadData(char* pData, size_t szLen)
	{
		if(!pData) return 0;
		if(szLen <= 0) return 0;

		size_t szOverflow = 0;
		size_t szLoad = GetLoadSize();
		size_t szRead = (szLen < szLoad) ? szLen : szLoad;
		if(m_szRead + szRead > m_szBufferSize)
		{
			szOverflow = m_szRead + szRead - m_szBufferSize;
			szRead = m_szBufferSize - m_szRead;
		}

		if(szRead > 0) memcpy(pData, m_pBuffer + m_szRead, szRead);
		if(szOverflow > 0) memcpy(pData + szRead, m_pBuffer, szOverflow);

		m_szRead = (m_szRead + szRead + szOverflow) % m_szBufferSize;

		return szRead + szOverflow;
	}
	bool WriteData(const char* pData, size_t szLen)
	{
		if(!pData) return false;
		if(szLen <= 0) return false;

		if(GetLeftSize() <= szLen)
		{
			if(!Realloc(m_szBufferSize * 2)) return false;
			return WriteData(pData, szLen);
		}

		size_t szOverflow = 0;
		size_t szWrite = szLen;
		if(m_szWrite + szWrite > m_szBufferSize)
		{
			szOverflow = m_szWrite + szWrite - m_szBufferSize;
			szWrite = m_szBufferSize - m_szWrite;
		}

		if(szWrite > 0) memcpy(m_pBuffer + m_szWrite, pData, szWrite);
		if(szOverflow > 0) memcpy(m_pBuffer, pData + szWrite, szOverflow);

		m_szWrite = (m_szWrite + szWrite + szOverflow) % m_szBufferSize;

		return true;
	}
};