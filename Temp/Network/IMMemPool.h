// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.
#pragma once

#ifdef _DEBUG
#define NOT_USE_MEMPOOL
#endif//#ifdef _DEBUG

template<typename TYPE, size_t BLOCK_SIZE = 64>
class IMMemPool
{
#ifndef NOT_USE_MEMPOOL
private:
	class IMMemPoolBlock
	{
	private:
		typedef std::vector<char*> SBlockVector;

		char**				m_ppNextBlock;
		SBlockVector		m_BlockVector;
		size_t				m_nAlignSize;
		FCriticalSection	m_Lock;

	public:
		IMMemPoolBlock()
		{
			m_ppNextBlock = (char**)0;
			m_nAlignSize = (sizeof(TYPE) > sizeof(char*))? sizeof(TYPE) : sizeof(char*);
		}
		~IMMemPoolBlock()
		{
			for(size_t i = 0; i < m_BlockVector.size(); i++) delete[] m_BlockVector[i];
		}
		TYPE* AllocBlock(size_t nSize)
		{
			if(nSize != sizeof(TYPE)) return (TYPE*)0;

			FScopeLock AutoLock(&m_Lock);

			if(!m_ppNextBlock || !*m_ppNextBlock)
			{
				char* pNewBlock = new char[BLOCK_SIZE * m_nAlignSize];
				m_BlockVector.push_back(pNewBlock);
				m_ppNextBlock = (char**)pNewBlock;
				for(int i = 0; i < BLOCK_SIZE - 1; i++)
				{
					*((char**)(pNewBlock + i * m_nAlignSize)) = pNewBlock + (i + 1) * m_nAlignSize;
				}
				*((char**)(pNewBlock + (BLOCK_SIZE - 1) * m_nAlignSize)) = (char*)0;
			}

			char* pMemory = (char*)m_ppNextBlock;
			m_ppNextBlock = (char**)*m_ppNextBlock;

			return (TYPE*)pMemory;
		}
		void FreeBlock(TYPE* pMemory)
		{
			FScopeLock AutoLock(&m_Lock);

			if(pMemory)
			{
				*((char**)pMemory) = (char*)m_ppNextBlock;
				m_ppNextBlock = (char**)pMemory;
			}
		}
	};

protected:
	static IMMemPoolBlock s_Block;

public:
	void* operator new(size_t nSize) { return s_Block.AllocBlock(nSize); }
	void operator delete(void* pMemory) { s_Block.FreeBlock((TYPE*)pMemory); }
	void* operator new(size_t nSize, const char* strFileName, int nLine) { return s_Block.AllocBlock(nSize); }
	void operator delete(void* pMemory, const char* strFileName, int nLine) { s_Block.FreeBlock((TYPE*)pMemory); }
#endif//#ifndef NOT_USE_MEMPOOL
};

#ifndef NOT_USE_MEMPOOL
template<class TYPE, size_t BLOCK_SIZE>
typename IMMemPool<TYPE, BLOCK_SIZE>::IMMemPoolBlock IMMemPool<TYPE, BLOCK_SIZE>::s_Block;
#endif//#ifndef NOT_USE_MEMPOOL

// [주의] : 부모클래스와 자식클래스 모두 메모리풀을 사용시 부모클래스와 자식클래스에 CBMemPool 매크로를 선언할것
#ifndef NOT_USE_MEMPOOL
#define IMMemPool(TYPE)																												\
	public:																															\
	void* operator new(size_t nSize) { return IMMemPool<TYPE>::s_Block.AllocBlock(nSize); }											\
	void operator delete(void* pMemory) { IMMemPool<TYPE>::s_Block.FreeBlock((TYPE*)pMemory); }										\
	void* operator new(size_t nSize, const char* strFileName, int nLine) { return IMMemPool<TYPE>::s_Block.AllocBlock(nSize); }		\
	void operator delete(void* pMemory, const char* strFileName, int nLine) { IMMemPool<TYPE>::s_Block.FreeBlock((TYPE*)pMemory); }
#else//#ifndef NOT_USE_MEMPOOL
#define IMMemPool(TYPE)
#endif//#ifndef NOT_USE_MEMPOOL
