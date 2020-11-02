// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#include "IMNetwork.h"

#include "IMMessageSender.h"
#include "Manager/WRConsoleManager.h"

IMNetwork::IMNetwork()
{
}

IMNetwork::~IMNetwork()
{
}

void IMNetwork::OnInitialize()
{
	m_LoginIPAdress = *(WRConsoleManager::Get()->GetString(EWRConsole::Network_LoginServerIP));
}

bool IMNetwork::OnTick(float InDeltaTime)
{
	Update();

	return true;
}

void IMNetwork::OnClosed(const std::wstring& id)
{ 

}

void IMNetwork::OnShutdown()
{
	IMMessageSender::RequestLogout(); 
	Clear(true);
}

void IMNetwork::OnReceived(const std::wstring& id, const char* buff, unsigned short buffLen)
{
	m_MessageHandler.OnReceived(id, buff, buffLen);
}

void IMNetwork::SendToLoginServer(IMSerialize* SerializedMessage)
{
	if (!IsConnected(L"Login"))
		return;

	Send(L"Login", SerializedMessage);
}

void IMNetwork::SendToGameServer(IMSerialize* SerializedMessage)
{
	if (!IsConnected(L"Game"))
		return;

	Send(L"Game", SerializedMessage);
}

void IMNetwork::ConnectToLoginServer()
{
	ConnectToServer(L"Login", m_LoginIPAdress, 4201);
}

void IMNetwork::ConnectToServer(const std::wstring& IN ServerToConnect, const std::wstring& IN InIP, int32 IN InPort)
{
	if (Connect(ServerToConnect, InIP, InPort))
	{
		m_LastTestIP = InIP;
		m_LastTestPort = InPort;
	}
}

void IMNetwork::DisconnectServer(const std::wstring& ServerToDisconnect)
{
	Close(ServerToDisconnect);
}