// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.
#pragma once

#include "IMBuffer.h"

class IMCircularQueue;
struct IMSerialize;

class FSocket;
class IMNetworkBase;

class IMSession
{
	friend IMNetworkBase;

private:
	struct IMSendQueueData : public IMMemPool<IMSendQueueData>
	{
		unsigned short m_Len = 0;
		IMBuffer* m_pBuffer = nullptr;

		~IMSendQueueData()
		{
			if(m_pBuffer) delete m_pBuffer;
		}
	};

private:
	enum { MAX_SEND_SIZE = 4096 };

private:
	IMNetworkBase& m_rNetwork;
	FSocket* m_pSocket = nullptr;

	std::wstring m_ID;
	bool m_IsClosing = false;

	bool m_RecvHeader = true;
	unsigned short m_RecvLen = 0;
	IMCircularQueue* m_pRecvQueue = nullptr;

	double m_LastRecvTime = 0;
	double m_LastSendTime = 0;

	int m_SendPendingLen = 0;
	char m_SendPending[MAX_SEND_SIZE];
	std::list<IMSendQueueData*> m_SendQueue;

	bool bShowPopup = true;

private:
	IMSession(IMNetworkBase& network) : m_rNetwork(network) {}
	~IMSession() { Close(bShowPopup); }

private:
	const std::wstring& GetID() const { return m_ID; }
	bool IsClosing() const { return m_IsClosing; }

private:
	void Close(bool bInShowPopup = true);
	bool Connect(const std::wstring& id, const std::wstring& ip, int port);
	void Recv();
	void Send(IMSerialize* message);
	void SetSendPending(const char* buff, unsigned short buffLen);
	void Update(std::list<std::wstring>& deleteSessionList);

public:
	void SetShowClosePopup(bool bInShowPopup) { bShowPopup = bInShowPopup; }
	//DeclearDebugMSG(-1, true, 2.0f, FColor::Green);
};