// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#include "IMNetworkBase.h"

#include "IMBuffer.h"
#include "IMSession.h"

#include "Share/Protocol/IMMessage.h"

IMNetworkBase::~IMNetworkBase()
{
	FScopeLock autoLock(&m_Lock);

	for(auto& pair : m_SessionMap) delete pair.second;
	m_SessionMap.clear();
	for(auto& data : m_RecvMessageQueue) delete[] data.m_pBuffer;
	m_RecvMessageQueue.clear();
}

void IMNetworkBase::Close(const std::wstring& id)
{
	FScopeLock autoLock(&m_Lock);

	auto it = m_SessionMap.find(id);
	if(it != m_SessionMap.end())
	{
		if (id == TEXT("Login"))
			it->second->SetShowClosePopup(false);

		delete it->second;
		m_SessionMap.erase(it);
		OnClosed(id);
	}
}

bool IMNetworkBase::IsConnected(const std::wstring& id)
{
	FScopeLock autoLock(&m_Lock);

	return m_SessionMap.find(id) != m_SessionMap.end();
}

bool IMNetworkBase::Connect(const std::wstring& id, const std::wstring& ip, int port)
{
	FScopeLock autoLock(&m_Lock);

	if(IsConnected(id)) return false;

	IMSession* session = new IMSession(*this);
	if (!session->Connect(id, ip, port))
	{
		delete session;
		return false;
	}
	m_SessionMap.insert(SessionMap::value_type(id, session));

	return true;
}

void IMNetworkBase::Send(const std::wstring& id, IMSerialize* message)
{
	FScopeLock autoLock(&m_Lock);

	auto it = m_SessionMap.find(id);
	if (it != m_SessionMap.end()) it->second->Send(message);
}

void IMNetworkBase::Update()
{
	FScopeLock autoLock(&m_Lock);

	std::list<std::wstring> deleteList;
	for(auto& pair : m_SessionMap) pair.second->Update(deleteList);
	for(auto& data : deleteList) Close(data);

	while(!m_RecvMessageQueue.empty())
	{
		IMMessage message = m_RecvMessageQueue.front();
		m_RecvMessageQueue.pop_front();
		OnReceived(message.m_ID, message.m_pBuffer->GetBuffer(), message.m_BufferLen);
		delete message.m_pBuffer;
	}
}

void IMNetworkBase::Clear(bool bShutdown /*= false*/)
{
	FScopeLock autoLock(&m_Lock);
	for (auto& pair : m_SessionMap)
	{
		pair.second->SetShowClosePopup(!bShutdown);
		delete pair.second;
	}
	m_SessionMap.clear();
	for (auto& data : m_RecvMessageQueue) delete[] data.m_pBuffer;
	m_RecvMessageQueue.clear();
}

void IMNetworkBase::PushRecvMessage(const std::wstring& id, const IMBuffer* buff, unsigned short buffLen)
{
	FScopeLock autoLock(&m_Lock);

	IMMessage message;
	message.m_ID = id;
	message.m_pBuffer = buff;
	message.m_BufferLen = buffLen;
	m_RecvMessageQueue.push_back(message);
}

bool IMNetworkBase::Read(IMSerialize& message, const char* buff, unsigned short buffLen)
{
	IMMessageReader Reader(buff, buffLen, message);
	return !Reader.IsError();
}