//******************************************************************************
//	FileName: 	Define_ServerInfo.h
//------------------------------------------------------------------------------
//	Created	:	2019/6/25
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeIO Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	클라/서버 공용
//				... 서버 상태 정보
//				... 네트워크 기본 정보
//******************************************************************************



#pragma once


// 월드 상태 유형
struct EWRServerStatus
{
	enum en
	{
		eClose = 0,			//	서버 OFF
		eMonitor,			//	Slave 상태
		eReady,				//	월드 구성중
		eStandby,			//	오픈 대기 (특정 그룹만 진입 허용)
		eOpen,				//	오픈 (해당 값보다 작다면 <점검중> 이다)
		eBusy,				//	혼잡 (해당 상태로 변경되는 시점은 정해진 수치에 따라 다르다)
		eFull,				//	진입불가

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


// 월드 정보
struct stWorldInfo : IMSerialize
{
	WSN_t				worldSerialNumber;			// 월드 DB 키값 (DB에서 발급된 키값)
	NAME_t				worldName;					// 월드명
	IP_t				ip;							// 월드 대표 IP
	int8				worldState;					// 월드 상태
	HOST_t				lobbyDomain;				// 해당 월드 로비 도메인
	HOST_t				gameDomain;					// 해당 월드 게임 서버 도메인
	bool				recommendation;				// 추천 서버 여부
	bool				newServer;					// 신규 서버 여부
	bool				enableCharacterCreate;		// 캐릭터 생성 제한 여부
	int8				ServerNo;					// 서버 넘버

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




