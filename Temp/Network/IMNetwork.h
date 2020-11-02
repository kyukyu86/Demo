// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
#pragma once

#include "IMNetworkBase.h"
#include "IMMessageReceiver.h"
#include "IMMessageSender.h"
#include "../Manager/SingletonClass/WRSingleton.h"

struct IMSerialize;

class IMNetwork : public WRSingleton<IMNetwork>, public IMNetworkBase
{
public:
	IMNetwork();
	virtual ~IMNetwork();

private:
	IMMessageReceiver	m_MessageHandler;
	std::wstring		m_LastTestIP = L"";
	int					m_LastTestPort = 0;
	std::wstring		m_LoginIPAdress = L"172.20.50.40";

private:
	virtual void OnClosed(const std::wstring& id) override;
	virtual void OnReceived(const std::wstring& id, const char* buff, unsigned short buffLen) override;

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown();
	virtual void OnLoadLevelComplete() override {};// { IMMessageSender::RequestReadyToPlay(); };
	virtual void OnLoadLevelStart() override {};
	virtual void OnRenderStart() override {};
	virtual bool OnTick(float InDeltaTime);

	void ConnectToServer(const std::wstring& IN ServerToConnect, const std::wstring& IN InIP, int32 IN InPort);
	void ConnectToLoginServer();
	void SendToLoginServer(IMSerialize* IN InSerializedMessage);
	void SendToGameServer(IMSerialize* IN InSerializedMessage);
	void SetLoginIPAdress(const std::wstring& IN InIP) { m_LoginIPAdress = InIP; }
	void DisconnectServer(const std::wstring& ServerToDisconnect);
	bool IsSameAsConnectedIP(const std::wstring& IN InIpToCompare) { return m_LastTestIP == InIpToCompare ? true : false; }
};