#pragma once


#ifdef USE_PACKET_SAMPLE

#include <winsock2.h>
#include <limits.h>
//#include <vector>

#pragma comment(lib, "ws2_32.lib")

#include "Crypto.h"

#else

	#ifdef USE_CLIENT_ONLY

	  
		#if PLATFORM_WINDOWS
		#include "AllowWindowsPlatformTypes.h"
		#include <winsock2.h>
		#include "HideWindowsPlatformTypes.h"
		#include <limits.h>
		#endif // PLATFORM_WINDOWS

		//#include "Network/Crypto.h"

	#else

		#include <limits.h>
		#include <winsock2.h>
		#include <vector>
		#include <deque>

	#endif // USE_CLIENT_ONLY

#endif // USE_PACKET_SAMPLE


#define BYTESWAP16(n) (((n&0xFF00)>>8)|((n&0x00FF)<<8))
#define BYTESWAP32(n) ((BYTESWAP16((n&0xFFFF0000)>>16))|((BYTESWAP16(n&0x0000FFFF))<<16))
#define BYTESWAP64(n) ((BYTESWAP32((n&0xFFFFFFFF00000000)>>32))|((BYTESWAP32(n&0x00000000FFFFFFFF))<<32))


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef USE_PACKET_SAMPLE

#else

	#if PLATFORM_WINDOWS || PLATFORM_ANDROID
	inline unsigned long long htonll(unsigned long long value)
	{
		int x = 1;
		return (*(char*)&x == 1) ? (((unsigned long long)htonl(static_cast<unsigned long>(value)) << 32) + htonl(value >> 32)) : value;
	}
	inline unsigned long long ntohll(unsigned long long value)
	{
		int x = 1;
#ifdef WR_BIG_ENDIAN
		return (*(char*)&x == 1) ? (((unsigned long long)ntohl(static_cast<unsigned long>(value)) << 32) + ntohl(value >> 32)) : value;
#else
		return (*(char*)&x == 1) ? (((unsigned long long)(static_cast<unsigned long>(value)) << 32) + (value >> 32)) : value;
#endif // #ifdef WR_BIG_ENDIAN
	}
	#endif

#endif

#define MAX_PACKET_SIZE			16384
#define INVALID_PACKET_INDEX	0
#define	RESET_PACKET_INDEX		60000

inline unsigned short GetNextPacketIndex(unsigned short idx)
{
	if (INVALID_PACKET_INDEX == idx) idx = 1;
	else if (RESET_PACKET_INDEX <= idx) idx = 1;
	else ++idx;

	return idx;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class IMMessage;

struct IMHeader
{
	unsigned short		m_Protocol;

	IMHeader() : m_Protocol(USHRT_MAX) {}
	~IMHeader() {}

	static size_t GetHeaderSize()
	{
		return sizeof(unsigned short);
	}
};

struct IMSerialize : public IMHeader
{
	virtual ~IMSerialize() {}
	virtual IMMessage& ReadMsg(IMMessage& rMsg) = 0;
	virtual IMMessage& WriteMsg(IMMessage& rMsg) = 0;

	virtual unsigned short GetProtocol() const { return m_Protocol; }

#ifdef USE_PROTOCOL_TOSTRING
	virtual void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) = 0;
#endif //#ifdef USE_PROTOCOL_STRING
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class IMMessage
{
protected:
	size_t		m_szRead;
	size_t		m_szWrite;
	size_t		m_szCapacity;
	char*		m_pBuffer;
	bool		m_bError;

public:
	IMMessage() : m_szRead(0), m_szWrite(0), m_szCapacity(0), m_pBuffer(nullptr), m_bError(false) {}

#ifndef USE_CLIENT_ONLY
	IMMessage(const IMMessage && rhs)
	{
		m_szRead = std::move(rhs.m_szRead);
		m_szWrite = std::move(rhs.m_szWrite);
		m_szCapacity = std::move(rhs.m_szCapacity);
		m_pBuffer = std::move(rhs.m_pBuffer);
		m_bError = std::move(rhs.m_bError);
	}

	IMMessage&& operator = (const IMMessage&& rhs)
	{
		m_szRead = std::move(rhs.m_szRead);
		m_szWrite = std::move(rhs.m_szWrite);
		m_szCapacity = std::move(rhs.m_szCapacity);
		m_pBuffer = std::move(rhs.m_pBuffer);
		m_bError = std::move(rhs.m_bError);

		return std::move(*this);
	}

	IMMessage& operator = (const IMMessage& rhs)
	{
		m_szRead = rhs.m_szRead;
		m_szWrite = rhs.m_szWrite;
		m_szCapacity = rhs.m_szCapacity;
		m_pBuffer = rhs.m_pBuffer;
		m_bError = rhs.m_bError;

		return *this;
	}
#endif //USE_CLIENT_ONLY

	virtual ~IMMessage() {}

public:
	bool IsError() const { return m_bError; }

public:
	IMMessage& Read(void* pDst, size_t szSize)
	{
		if(m_szRead + szSize > m_szWrite)
		{
			m_bError = true;
			return *this;
		}

		memcpy(pDst, m_pBuffer + m_szRead, szSize);
		m_szRead += szSize;
		return *this;
	}
	IMMessage& operator >> (bool& rDst)
	{
		Read(&rDst, sizeof(rDst));
		return *this;
	}
	IMMessage& operator >> (signed char& rDst)
	{
		Read(&rDst, sizeof(rDst));
		return *this;
	}
	IMMessage& operator >> (unsigned char& rDst)
	{
		Read(&rDst, sizeof(rDst));
		return *this;
	}
	IMMessage& operator >> (short& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = static_cast<short>(ntohs(static_cast<unsigned short>(rDst)));
#else
		rDst = static_cast<short>((static_cast<unsigned short>(rDst)));
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (unsigned short& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = ntohs(rDst);
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (int& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = static_cast<int>(ntohl(static_cast<unsigned long>(rDst)));
#else
		rDst = static_cast<int>((static_cast<unsigned long>(rDst)));
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (unsigned int& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = static_cast<unsigned int>(ntohl(static_cast<unsigned long>(rDst)));
#else
		rDst = static_cast<unsigned int>((static_cast<unsigned long>(rDst)));
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (long& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = static_cast<long>(ntohl(static_cast<unsigned long>(rDst)));
#else
		rDst = static_cast<long>((static_cast<unsigned long>(rDst)));
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (unsigned long& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = ntohl(rDst);
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (long long& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = static_cast<long long>(ntohll(static_cast<unsigned long long>(rDst)));
#else
		rDst = static_cast<long long>((static_cast<unsigned long long>(rDst)));
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (unsigned long long& rDst)
	{
		Read(&rDst, sizeof(rDst));

#ifdef WR_BIG_ENDIAN
		rDst = ntohll(rDst);
#endif // #ifdef WR_BIG_ENDIAN

		return *this;
	}
	IMMessage& operator >> (float& rDst)
	{
		int Temp;
		*this >> Temp;
		rDst = *reinterpret_cast<float*>(&Temp);
		return *this;
	}
	IMMessage& operator >> (double& rDst)
	{
		long long Temp;
		*this >> Temp;
		rDst = *reinterpret_cast<double*>(&Temp);
		return *this;
	}
	IMMessage& operator >> (std::string& rDst)
	{
		unsigned short usLength;
		*this >> usLength;

		if(usLength < 0 || m_szRead + usLength > m_szWrite)
		{
			m_bError = true;
			return *this;
		}

		if(usLength > 0)
		{
			rDst.reserve(usLength);
			rDst.assign(m_pBuffer + m_szRead, usLength);
			m_szRead += usLength;
		}

		return *this;
	}
	IMMessage& operator >> (std::wstring& rDst)
	{
		unsigned short usLength;
		*this >> usLength;

		if (usLength < 0 || m_szRead + usLength > m_szWrite)
		{
			m_bError = true;
			return *this;
		}

		if (usLength > 0)
		{
#ifdef USE_CLIENT_ONLY
			TStringConversion< TStringConvert<UCS2CHAR, TCHAR> > strConv(reinterpret_cast<const UCS2CHAR*>(m_pBuffer + m_szRead), usLength / sizeof(UCS2CHAR));

			rDst.reserve(usLength / sizeof(UCS2CHAR));
			rDst.assign(strConv.Get(), usLength / sizeof(UCS2CHAR));
#else
			rDst.reserve(usLength/sizeof(wchar_t));
			rDst.assign((const wchar_t*)(m_pBuffer + m_szRead), ( usLength / sizeof(wchar_t) ));
#endif // USE_CLIENT_ONLY

			m_szRead += usLength;
		}

		return *this;
	}
	IMMessage& operator >> (std::vector<unsigned char>& rDst)
	{
		unsigned short usLength;
		*this >> usLength;

		if(usLength < 0 || m_szRead + usLength > m_szWrite)
		{
			m_bError = true;
			return *this;
		}

		if(usLength > 0)
		{
			rDst.reserve(usLength);
			rDst.insert(rDst.begin(), m_pBuffer + m_szRead, m_pBuffer + m_szRead + usLength);
			m_szRead += usLength;
		}

		return *this;
	}
	template<typename T>
	IMMessage& operator >> (std::vector<T>& rDst)
	{
		unsigned short usLength;
		*this >> usLength;

		for (unsigned short i = 0; i < usLength; i++)
		{
			T temp;
			*this >> temp;
			rDst.push_back(temp);
		}

		return *this;
	}

	IMMessage& operator >> (IMSerialize& rDst)
	{
		rDst.ReadMsg(*this);
		return *this;
	}
	IMMessage& operator >> (IMHeader& rDst)
	{
		*this >> rDst.m_Protocol;
		return *this;
	}
	template<typename T>
	IMMessage& operator >> (std::list<T>& rDst)
	{ 
		unsigned short usLength;
		*this >> usLength;

		for(unsigned short i = 0; i < usLength; i++)
		{
			T temp;
			*this >> temp;
			rDst.push_back(temp);
		}

		return *this;
	}

	template<typename K, typename T>
	IMMessage& operator >> (std::map<K, T>& rDst)
	{
		unsigned short usLength;
		*this >> usLength;

		for (unsigned short i = 0; i < usLength; i++)
		{
			K key;
			T value;
			*this >> key;
			*this >> value;
			rDst.emplace(std::move(key), std::move(value));
		}

		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
	IMMessage& operator >> (T& rDst)
	{
		*this >> *((std::underlying_type_t<T>*)&rDst);
		return *this;
	}

public:
	IMMessage& Write(const void* pSrc, size_t szSize)
	{
		if(m_szWrite + szSize > m_szCapacity)
		{
			m_bError = true;
			return *this;
		}

		memcpy(m_pBuffer + m_szWrite, pSrc, szSize);
		m_szWrite += szSize;
		return *this;
	}
	IMMessage& operator << (bool Src)
	{
		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (signed char Src)
	{
		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (unsigned char Src)
	{
		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (short Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = static_cast<short>(htons(static_cast<unsigned short>(Src)));
#else
		Src = static_cast<short>((static_cast<unsigned short>(Src)));
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (unsigned short Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = htons(Src);
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (int Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = static_cast<int>(htonl(static_cast<unsigned long>(Src)));
#else
		Src = static_cast<int>((static_cast<unsigned long>(Src)));
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (unsigned int Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = static_cast<unsigned int>(htonl(static_cast<unsigned long>(Src)));
#else
		Src = static_cast<unsigned int>((static_cast<unsigned long>(Src)));
#endif // #ifdef WR_BIG_ENDIAN
		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (long Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = static_cast<long>(htonl(static_cast<unsigned long>(Src)));
#else
		Src = static_cast<long>((static_cast<unsigned long>(Src)));
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (unsigned long Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = htonl(Src);
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (long long Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = static_cast<long long>(htonll(static_cast<unsigned long long>(Src)));
#else
		Src = static_cast<long long>((static_cast<unsigned long long>(Src)));
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (unsigned long long Src)
	{
#ifdef WR_BIG_ENDIAN
		Src = htonll(Src);
#endif // #ifdef WR_BIG_ENDIAN

		Write(&Src, sizeof(Src));
		return *this;
	}
	IMMessage& operator << (float Src)
	{
		int Temp = *reinterpret_cast<int*>(&Src);
		*this << Temp;
		return *this;
	}
	IMMessage& operator << (double Src)
	{
		long long Temp = *reinterpret_cast<long long*>(&Src);
		*this << Temp;
		return *this;
	}
	IMMessage& operator << (std::string& rSrc)
	{
		unsigned short usLength = static_cast<unsigned short>(rSrc.size());
		*this << usLength;

		if(usLength > 0) Write(rSrc.c_str(), usLength);

		return *this;
	}
	IMMessage& operator << (std::wstring& rSrc)
	{
#ifdef USE_CLIENT_ONLY
		TStringConversion< TStringConvert<TCHAR, UCS2CHAR> > strConv(rSrc.c_str(), rSrc.length());
		unsigned short usLength = static_cast<unsigned short>(strConv.Length()) * sizeof(UCS2CHAR);
		*this << usLength;

		if (usLength > 0) Write(strConv.Get(), usLength);
#else
		unsigned short usLength = static_cast<unsigned short>(rSrc.length()) * sizeof(wchar_t);
		*this << usLength;

		if (usLength > 0) Write(rSrc.c_str(), usLength);
#endif // USE_CLIENT_ONLY

		return *this;
	}
	template<typename T>
	IMMessage& operator << (std::vector<T>& rSrc)
	{
		unsigned short usLength = static_cast<unsigned short>(rSrc.size());
		*this << usLength;

		for (int i = 0; i < usLength; ++i)
		{
			*this << rSrc[i];
		}

		return *this;
	}
	IMMessage& operator << (IMSerialize& rSrc)
	{
		rSrc.WriteMsg(*this);
		return *this;
	}
	IMMessage& operator << (IMHeader& rSrc)
	{
		*this << rSrc.m_Protocol;
		return *this;
	}
	template<typename T>
	IMMessage& operator << (std::list<T>& rSrc)
	{
		unsigned short usLength = static_cast<unsigned short>(rSrc.size());
		*this << usLength;

		auto itEnd = rSrc.end();
		for(auto it = rSrc.begin(); it != itEnd; ++it)
		{
			*this << *it;
		}

		return *this;
	}

	template<typename K, typename T>
	IMMessage& operator << (std::map<K, T>& rSrc)
	{
		unsigned short usLength = static_cast<unsigned short>(rSrc.size());
		*this << usLength;

		auto itEnd = rSrc.end();
		for(auto it = rSrc.begin(); it != itEnd; ++it)
		{
			*this << it->first;
			*this << it->second;
		}

		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
	IMMessage& operator << (T& rDst)
	{
		*this << *((std::underlying_type_t<T>*)&rDst);
		return *this;
	}

public:
	bool Check()
	{
		// check size
		size_t usSize = sizeof(unsigned short);
		m_szWrite = m_szWrite - usSize;

		unsigned short usOriSize = 0;
		memcpy(&usOriSize, m_pBuffer + m_szWrite, usSize);

#ifdef WR_BIG_ENDIAN
		usOriSize = ntohs(usOriSize);
#endif // #ifdef WR_BIG_ENDIAN

		if (m_szWrite != usOriSize)
		{// 던진 크기와 받은 크기가 이상하다?
			return false;
		}
		return true;
	}

	bool Final()
	{
		// 암호화 완료된 크기

#ifdef WR_BIG_ENDIAN
		unsigned short usOriSize = htons(static_cast<unsigned short>(m_szWrite));
#else
		unsigned short usOriSize = (static_cast<unsigned short>(m_szWrite));
#endif // #ifdef WR_BIG_ENDIAN

		size_t usSize = sizeof(unsigned short);
		memcpy(m_pBuffer + m_szWrite, &usOriSize, usSize);
		m_szWrite = m_szWrite + usSize;

		if (m_szWrite > m_szCapacity)
		{
			return false;
		}
		return true;
	}

	bool Check(const std::string& token)
	{
		bool bOK = false;
		// check size
		size_t usSize = sizeof(unsigned short);
		m_szWrite -= usSize;

		unsigned short usTokenSize = 0;
		memcpy(&usTokenSize, m_pBuffer + m_szWrite, usSize);

#ifdef WR_BIG_ENDIAN
		usTokenSize = ntohs(usTokenSize);
#endif // #ifdef WR_BIG_ENDIAN

		if (32 > usTokenSize)
		{// 토큰이 이상하다?
			return bOK;
		}

		// check token
		m_szWrite -= usTokenSize;
		char* pToken = new char [usTokenSize+1];
		if (nullptr == pToken)
		{
			return bOK;
		}
		memset(pToken, 0, usTokenSize+1);
		memcpy(pToken, m_pBuffer + m_szWrite, usTokenSize);
		if (0 == token.compare(pToken))
		{
			bOK = true;
		}

		if (pToken)
		{
			delete[] pToken;
			pToken = nullptr;
		}

		return bOK;
	}

	bool Final(const std::string& token)
	{
		// 토큰 내용
		memcpy(m_pBuffer + m_szWrite, token.c_str(), token.size());
		m_szWrite += token.size();
		if (m_szWrite > m_szCapacity)
		{
			return false;
		}

		// 토큰 크기

#ifdef WR_BIG_ENDIAN
		unsigned short usTokenSize = htons(static_cast<unsigned short>(token.size()));
#else
		unsigned short usTokenSize = (static_cast<unsigned short>(token.size()));
#endif // #ifdef WR_BIG_ENDIAN

		size_t usSize = sizeof(unsigned short);
		memcpy(m_pBuffer + m_szWrite, &usTokenSize, usSize);
		m_szWrite += usSize;

		if (m_szWrite > m_szCapacity)
		{
			return false;
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class IMMessageReader : public IMMessage
{
public:
	IMMessageReader(const char* pBuffer, size_t szLength, IMSerialize& rDst)
	{
		m_pBuffer = const_cast<char*>(pBuffer);
		m_szWrite = szLength;

		IMHeader header;
		*this >> header;
		if (header.m_Protocol != rDst.GetProtocol())
		{
			m_bError = true;
			return;
		}

		*this >> rDst;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<int SIZE>
class TIMMessageWriter : public IMMessage
{
private:
	char	m_Buffer[SIZE];

public:
	TIMMessageWriter(IMSerialize& rSrc)
	{
		m_pBuffer = m_Buffer;
		m_szCapacity = SIZE;
		*this << static_cast<IMHeader&>(rSrc);

		*this << rSrc;
	}

	TIMMessageWriter(const TIMMessageWriter& ref)
	{
		*this = ref;

		m_pBuffer = m_Buffer;
	}

#ifndef USE_CLIENT_ONLY
	TIMMessageWriter(IMSerialize* pSrc = nullptr)
	{
		m_pBuffer = m_Buffer;
		m_szCapacity = SIZE;

		if (pSrc)
			Serialize(*pSrc);
	}

	TIMMessageWriter(const TIMMessageWriter&& rhs)
		: IMMessage(std::move(rhs))
	{
		std::move(rhs.m_Buffer, rhs.m_Buffer + SIZE, m_Buffer);
		m_pBuffer = m_Buffer;
	}

	TIMMessageWriter&& operator = (const TIMMessageWriter&& rhs)
	{
		__super::operator = (std::move(rhs));

		std::move(rhs.m_Buffer, rhs.m_Buffer + SIZE, m_Buffer);
		m_pBuffer = m_Buffer;

		return std::move(*this);
	}

	TIMMessageWriter& operator = (const TIMMessageWriter& rhs)
	{
		__super::operator = (rhs);

		memcpy(m_Buffer, rhs.m_pBuffer, SIZE);
		m_pBuffer = m_Buffer;

		return *this;
	}

	bool Serialize(IMSerialize& rSrc)
	{
		if (0 < GetBufferLength())
			return false;

		*this << static_cast<IMHeader&>(rSrc);

		*this << rSrc;

		return true;
	}
#endif //USE_CLIENT_ONLY

public:
	const char* GetBuffer() const { return m_pBuffer; }
	unsigned short GetBufferLength() const { return static_cast<unsigned short>(m_szWrite); }
};

typedef TIMMessageWriter<MAX_PACKET_SIZE> IMMessageWriter;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
