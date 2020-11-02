//******************************************************************************
//	FileName: 	Define_ServerInfo.h
//------------------------------------------------------------------------------
//	Created	:	2019/6/25
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeIO Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	Ŭ��/���� ����
//				... ���� ���� ����
//				... ��Ʈ��ũ �⺻ ����
//******************************************************************************



#pragma once


// ���� ���� ����
struct EWRServerStatus
{
	enum en
	{
		eClose = 0,			//	���� OFF
		eMonitor,			//	Slave ����
		eReady,				//	���� ������
		eStandby,			//	���� ��� (Ư�� �׷츸 ���� ���)
		eOpen,				//	���� (�ش� ������ �۴ٸ� <������> �̴�)
		eBusy,				//	ȥ�� (�ش� ���·� ����Ǵ� ������ ������ ��ġ�� ���� �ٸ���)
		eFull,				//	���ԺҰ�

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( eClose,		L"Close" );
		InsertMap( eMonitor,	L"Monitor" );
		InsertMap( eReady,		L"Ready" );
		InsertMap( eStandby,	L"Standby" );
		InsertMap( eOpen,		L"Open" );
		InsertMap( eBusy,		L"Busy" );
		InsertMap( eFull,		L"Full" );
	}
};


// ���� ����
struct stWorldInfo : IMSerialize
{
	WSN_t				worldSerialNumber;			// ���� DB Ű�� (DB���� �߱޵� Ű��)
	NAME_t				worldName;					// �����
	IP_t				ip;							// ���� ��ǥ IP
	int8				worldState;					// ���� ����
	HOST_t				lobbyDomain;				// �ش� ���� �κ� ������
	HOST_t				gameDomain;					// �ش� ���� ���� ���� ������
	bool				recommendation;				// ��õ ���� ����
	bool				newServer;					// �ű� ���� ����
	bool				enableCharacterCreate;		// ĳ���� ���� ���� ����
	int8				ServerNo;					// ���� �ѹ�

	stWorldInfo() : worldSerialNumber( INVALID_WSN ), worldName( INVALID_NAME ), ip( INVALID_IP ), worldState( 0 ), lobbyDomain( INVALID_HOST ), gameDomain( INVALID_HOST ), recommendation( false )
		, newServer( false ), enableCharacterCreate( false ), ServerNo( 0 ) {}

	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg >> worldSerialNumber >> worldName >> ip >> worldState >> lobbyDomain >> gameDomain >> recommendation >> newServer >> enableCharacterCreate >> ServerNo;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg << worldSerialNumber << worldName << ip << worldState << lobbyDomain << gameDomain << recommendation << newServer << enableCharacterCreate << ServerNo;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"worldSerialNumber : %u, worldName : %s", worldSerialNumber, worldName.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"worldSerialNumber : %u, worldName : %s, ip : %s, worldState : %d, lobbyDomain : %s, gameDomain : %s, recommendation : %s, newServer : %s, enableCharacterCreate : %s, ServerNo : %d"
								  , worldSerialNumber, worldName.c_str(), ip.c_str(), worldState, lobbyDomain.c_str(), gameDomain.c_str(), recommendation ? L"true" : L"false", newServer ? L"true" : L"false", enableCharacterCreate ? L"true" : L"false", ServerNo );
	}
#endif	//  USE_PROTOCOL_TOSTRING
};




