// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once


class IMBuffer;
struct IMSerialize;
class IMSession;

class IMNetworkBase
{
private:
	struct IMMessage
	{
		std::wstring m_ID = L"";
		const IMBuffer* m_pBuffer = nullptr;
		unsigned short m_BufferLen = 0;
	};

	typedef std::list<IMMessage> MessageQueue;
	typedef std::map<std::wstring, IMSession*> SessionMap;
	

private:
	FCriticalSection m_Lock;
	SessionMap m_SessionMap;
	MessageQueue m_RecvMessageQueue;

public:
	virtual ~IMNetworkBase();

public:
	void Close(const std::wstring& id);
	bool IsConnected(const std::wstring& id);
	bool Connect(const std::wstring& id, const std::wstring& ip, int port);
	void Send(const std::wstring& id, IMSerialize* message);
	void Update();
	void Clear(bool bShutdown = false);

public:
	void PushRecvMessage(const std::wstring& id, const IMBuffer* buff, unsigned short buffLen);
	bool Read(IMSerialize& message, const char* buff, unsigned short buffLen);

public:
	virtual void OnClosed(const std::wstring& id) = 0;
	virtual void OnReceived(const std::wstring& id, const char* buff, unsigned short buffLen) = 0;
};