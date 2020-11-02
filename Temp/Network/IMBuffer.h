// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.
#pragma once

#include "IMMemPool.h"

class IMBuffer
{
private:
	enum { MIN_BUFFER_SIZE = 64 };

public:
	virtual ~IMBuffer() {}
	virtual char* GetBuffer() = 0;
	virtual const char* GetBuffer() const = 0;

public:
	static IMBuffer* AllockBuffer(size_t szSize);
};

template<size_t SIZE, size_t BLOCK_SIZE = 64>
class TBuffer : public IMBuffer, public IMMemPool<TBuffer<SIZE>, BLOCK_SIZE>
{
private:
	char	m_Buffer[SIZE];

public:
	virtual ~TBuffer() {}
	virtual char* GetBuffer() { return m_Buffer; }
	virtual const char* GetBuffer() const { return m_Buffer; }
};

inline IMBuffer* IMBuffer::AllockBuffer(size_t szSize)
{
	if(szSize <= MIN_BUFFER_SIZE)				return new TBuffer<MIN_BUFFER_SIZE>;		//64
	else if(szSize <= MIN_BUFFER_SIZE << 1)		return new TBuffer<(MIN_BUFFER_SIZE << 1)>; //128
	else if(szSize <= MIN_BUFFER_SIZE << 2)		return new TBuffer<(MIN_BUFFER_SIZE << 2)>; //256
	else if(szSize <= MIN_BUFFER_SIZE << 3)		return new TBuffer<(MIN_BUFFER_SIZE << 3)>; //512
	else if(szSize <= MIN_BUFFER_SIZE << 4)		return new TBuffer<(MIN_BUFFER_SIZE << 4)>; //1024
	else if(szSize <= MIN_BUFFER_SIZE << 5)		return new TBuffer<(MIN_BUFFER_SIZE << 5)>; //2048
	else if(szSize <= MIN_BUFFER_SIZE << 6)		return new TBuffer<(MIN_BUFFER_SIZE << 6)>; //4096
	else if(szSize <= MIN_BUFFER_SIZE << 7)		return new TBuffer<(MIN_BUFFER_SIZE << 7)>; //8192
	else if(szSize <= MIN_BUFFER_SIZE << 8)		return new TBuffer<(MIN_BUFFER_SIZE << 8)>; //16384
	else if(szSize <= MIN_BUFFER_SIZE << 9)		return new TBuffer<(MIN_BUFFER_SIZE << 9)>; //32768
	else if(szSize <= MIN_BUFFER_SIZE << 10)	return new TBuffer<(MIN_BUFFER_SIZE << 10)>; //65536
	else if(szSize <= MIN_BUFFER_SIZE << 11)	return new TBuffer<(MIN_BUFFER_SIZE << 11)>; //131072
	else if(szSize <= MIN_BUFFER_SIZE << 12)	return new TBuffer<(MIN_BUFFER_SIZE << 12), 1>; //262144
	else if(szSize <= MIN_BUFFER_SIZE << 13)	return new TBuffer<(MIN_BUFFER_SIZE << 13), 1>; //524288
	else if(szSize <= MIN_BUFFER_SIZE << 14)	return new TBuffer<(MIN_BUFFER_SIZE << 14), 1>; //1048576
	return nullptr;
}