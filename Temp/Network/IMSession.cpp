// Copyright 2015-2017 WemadeIO, Inc. All Rights Reserved.
//#include "WRPCH.h"

#include "IMSession.h"
#include "IMBuffer.h"
#include "IMCircularQueue.h"
#include "IMNetwork.h"

#include "Share/Protocol/IMMessage.h"
#include "GenericPlatformMisc.h"
#include "MessageDialog.h"

void IMSession::Close(bool bInShowPopup /*= true*/)
{
	if(m_pSocket)
	{
		// ---------------------------------------------------------------------------------------------------------
		// 로그
		//FString _Message4DonQ = FString::Printf(TEXT("Close : ID[%s]"), m_ID.c_str());
		//UE_LOG(IMError, Error, TEXT("%s"), *_Message4DonQ);

		//ShowDebugMSG(FString::Printf(TEXT("Close : ID[%s]"), m_ID.c_str()));
		// ---------------------------------------------------------------------------------------------------------
		if (bInShowPopup == true)
		{
			uint32 Error = FGenericPlatformMisc::GetLastError();
			FText Title = FText::FromString(TEXT("Server DisConnect"));
			FText Message = FText::FromString(FString::Printf(TEXT("Server Disconnect error(%d)"), Error));
			FMessageDialog::Open(EAppMsgType::Ok, Message, &Title);
		}

		m_pSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(m_pSocket);
		m_pSocket = nullptr;
		m_ID.clear();
	}

	if(m_pRecvQueue)
	{
		delete m_pRecvQueue;
		m_pRecvQueue = nullptr;

		for(auto& data : m_SendQueue) delete data;
	}
}

bool IMSession::Connect(const std::wstring& id, const std::wstring& ip, int port)
{
	Close(false);

	m_pRecvQueue = new IMCircularQueue;
	m_pRecvQueue->Alloc(MAX_SEND_SIZE, 1024);

	FIPv4Address ip4;
	FIPv4Address::Parse(ip.c_str(), ip4);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bValid = false;
	addr->SetIp(*(ip4.ToString()), bValid);
	addr->SetPort(port);

	UE_LOG(WRLog, Log, TEXT("Connect id : %s, ip : %s, port : %d"), id.c_str(), ip.c_str(), port);

	FSocket* pSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, id.c_str(), false);
	if(!pSocket->Connect(*addr))
	{
		UE_LOG(WRLog, Log, TEXT("Connect Error !!!"));
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(pSocket);
		return false;
	}
	m_pSocket = pSocket;
	m_ID = *m_pSocket->GetDescription();

	m_LastRecvTime = m_LastSendTime = FPlatformTime::Seconds();

	// ---------------------------------------------------------------------------------------------------------
	// 로그
	//FString _Message4DonQ = FString::Printf(TEXT("Connect : ID[%s] / IP[%s]:[%d] "), id.c_str(), ip.c_str(), port);
	//UE_LOG(IMError, Error, TEXT("%s"), *_Message4DonQ);

	//ShowDebugMSG(FString::Printf(TEXT("Connect : ID[%s] / IP[%s]:[%d]"), id.c_str(), ip.c_str(), port));
	// ---------------------------------------------------------------------------------------------------------

	return true;
}

void IMSession::Recv()
{
	if(m_IsClosing) return;

	unsigned int pendingData;
	if(!m_pSocket->HasPendingData(pendingData)) return;

	// ---------------------------------------------------------------------------------------------------------
	// 테스트용 Packet
	char buff[MAX_SEND_SIZE];
	int len;
	if(!m_pSocket->Recv(reinterpret_cast<unsigned char*>(buff), sizeof(buff), len, ESocketReceiveFlags::None) || len <= 0)
	{
		m_IsClosing = true;
		return;
	}

	if (0 < len)
	{
		FString serialized(buff);

		// ---------------------------------------------------------------------------------------------------------
		// 로그
		//FString _Message4DonQ = FString::Printf(TEXT("Recv : [%s] / [%d] / [%d] "), *serialized, len, len);
		//UE_LOG(IMError, Error, TEXT("%s"), *_Message4DonQ);

		//ShowDebugMSG(FString::Printf(TEXT("Recv : [%s] / [%d] / [%d]"), *serialized, len, len));
		// ---------------------------------------------------------------------------------------------------------
	}
	// ---------------------------------------------------------------------------------------------------------
	//return;

	m_LastRecvTime = FPlatformTime::Seconds();
	m_pRecvQueue->WriteData(buff, len);

	while(true)
	{
		if(m_RecvHeader)
		{
			if(m_pRecvQueue->GetLoadSize() < sizeof(unsigned short)) break;
			m_pRecvQueue->ReadData(reinterpret_cast<char*>(&m_RecvLen), sizeof(unsigned short));
#ifdef WR_BIG_ENDIAN
			m_RecvLen = ntohs(m_RecvLen);
#endif //WR_BIG_ENDIAN
			m_RecvHeader = false;
		}
		else
		{
			if(m_pRecvQueue->GetLoadSize() < m_RecvLen) break;
			IMBuffer* buffer = IMBuffer::AllockBuffer(m_RecvLen);
			m_pRecvQueue->ReadData(buffer->GetBuffer(), m_RecvLen);
			m_rNetwork.PushRecvMessage(m_ID, buffer, m_RecvLen);
			m_RecvHeader = true;
		}
	}
}

void IMSession::Send(IMSerialize* message)
{
	if(m_IsClosing) return;

	// ---------------------------------------------------------------------------------------------------------
	// 테스트용 Packet
	//if (message)
	//{
	//	FString serialized = TEXT("myText");

	//	TCHAR* serializedChar = serialized.GetCharArray().GetData();
	//	int32 size = FCString::Strlen(serializedChar);
	//	int32 sent = 0;
	//	m_pSocket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);

	//	// ---------------------------------------------------------------------------------------------------------
	//	// 로그
	//	//FString _Message4DonQ = FString::Printf(TEXT("Send : [%s] / [%d] / [%d] "), serializedChar, size, sent);
	//	//UE_LOG(IMError, Error, TEXT("%s"), *_Message4DonQ);

	//	//ShowDebugMSG(FString::Printf(TEXT("Send : [%s] / [%d] / [%d]"), serializedChar, size, sent));
	//}
	// ---------------------------------------------------------------------------------------------------------
	//return;

	if(message)
	{
		IMMessageWriter writer(*message);
		if(writer.IsError())
		{
			m_IsClosing = true;
			return;
		}

		IMSendQueueData* data = new IMSendQueueData;
		data->m_Len = sizeof(unsigned short) + writer.GetBufferLength();
		data->m_pBuffer = IMBuffer::AllockBuffer(sizeof(unsigned short) + writer.GetBufferLength());
#ifdef WR_BIG_ENDIAN
		unsigned short sendLen = htons(writer.GetBufferLength());
#else //WR_BIG_ENDIAN
		unsigned short sendLen = writer.GetBufferLength();
#endif //WR_BIG_ENDIAN
		memcpy(data->m_pBuffer->GetBuffer(), &sendLen, sizeof(unsigned short));
		memcpy(data->m_pBuffer->GetBuffer() + sizeof(unsigned short), writer.GetBuffer(), writer.GetBufferLength());

		m_SendQueue.push_back(data);
	}

	if(m_SendPendingLen > 0)
	{
		int len;
		if(!m_pSocket->Send(reinterpret_cast<unsigned char*>(m_SendPending), m_SendPendingLen, len))
		{
			m_IsClosing = true;
			return;
		}

		if(m_SendPendingLen > len)
		{
			SetSendPending(m_SendPending + len, m_SendPendingLen - len);
			return;
		}
		m_SendPendingLen = 0;
	}

	while(!m_SendQueue.empty())
	{
		IMSendQueueData* data = m_SendQueue.front();
		m_SendQueue.pop_front();

		int len;
		if(!m_pSocket->Send(reinterpret_cast<unsigned char*>(data->m_pBuffer->GetBuffer()), data->m_Len, len))
		{
			delete data;
			m_IsClosing = true;
			return;
		}
		if(data->m_Len > len)
		{
			SetSendPending(data->m_pBuffer->GetBuffer() + len, data->m_Len - len);
			delete data;
			break;
		}

		delete data;
		m_LastSendTime = FPlatformTime::Seconds();
	}

}

void IMSession::SetSendPending(const char* buff, unsigned short buffLen)
{
	m_SendPendingLen = buffLen;
	if(buff) memcpy(m_SendPending, buff, buffLen);
}

void IMSession::Update(std::list<std::wstring>& deleteSessionList)
{
	Recv();
	Send(nullptr);

	if(m_IsClosing) deleteSessionList.push_back(m_ID);
}