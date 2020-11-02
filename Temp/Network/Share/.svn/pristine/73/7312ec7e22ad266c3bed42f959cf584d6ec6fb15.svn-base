//******************************************************************************
//	FileName: 	Define_User.h
//------------------------------------------------------------------------------
//	Created	:	2016/11/21
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2016 WemadeIO Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	User 관련 정의 모음
//******************************************************************************


#pragma once

#include "../Protocol/ProtocolStringError.h"


//-----------------------------------------------------
//	강제종료명령의 타입
//	!!! .... 이곳에 추가시 EIMProtocolErrorSC 에러형 추가 필요
//-----------------------------------------------------
struct EWRForceKick
{
	enum en
	{
		None = 0,

		Push,							//	같은계정의 접속으로인한 강제종료
		GM,								//	서비스운영에 의한 강제종료
		Error,							//	프로그램 오류에 의한 강제종료
		DBError,						//	DB 오류에 의한 강제종료
		Timeout,						//	장기간 대기로인한 강제종료						
		Etc,							//	기타사유에의한 강제종료
		Channel,						//	채널변경에의한 강제종료
		ShutdownTime,					//	셧다운 제도에 의한 강제종료
		AutoCheck,						//	오토 체크에 의한 강제 종료
		MoveServer,						//	물리 서버 변경으로 종료
		AbuseMovingCheck,				//	비정상적 이동으로 인한 강제종료
		HackingPacket,					//	비정상적 패킷으로 인한 강제종료
		SequenceNumberCheck,			//	메시지 시퀀스 넘버 비정상으로 인한 강제 종료
		Disconnect,						//	연결 끊김에 의한 강제 종료
		InvalidCurrencyDetected,		//	유효하지 않은 재화값 감지됨
		ToolCommand,					//	운영툴에서 킥 명령함

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Push,					L"Push" );
		InsertMap( GM,						L"GM" );
		InsertMap( Error,					L"Error" );
		InsertMap( DBError,					L"DBError" );
		InsertMap( Timeout,					L"Timeout" );
		InsertMap( Etc,						L"Etc" );
		InsertMap( Channel,					L"Channel" );
		InsertMap( ShutdownTime,			L"ShutdownTime" );
		InsertMap( AutoCheck,				L"AutoCheck" );
		InsertMap( MoveServer,				L"MoveServer" );
		InsertMap( AbuseMovingCheck,		L"AbuseMovingCheck" );
		InsertMap( HackingPacket,			L"HackingPacket" );
		InsertMap( SequenceNumberCheck,		L"SequenceNumberCheck" );
		InsertMap( Disconnect,				L"Disconnect" );
		InsertMap( InvalidCurrencyDetected, L"InvalidCurrencyDetected");
		InsertMap( ToolCommand,				L"ToolCommand" );
	}

	static const EWRError::en		GetErrorStr( const EWRForceKick::en enForceKick )
	{
		switch( enForceKick )
		{
			case Push:							return EWRError::Failure_ForceKick_Push;
			case GM:							return EWRError::Failure_ForceKick_GM;
			case Error:							return EWRError::Failure_ForceKick_Error;
			case DBError:						return EWRError::Failure_ForceKick_DBError;
			case Timeout:						return EWRError::Failure_ForceKick_Timeout;
			case Etc:							return EWRError::Failure_ForceKick_Etc;
			case Channel:						return EWRError::Failure_ForceKick_Channel;
			case ShutdownTime:					return EWRError::Failure_ForceKick_ShutdownTime;
			case AutoCheck:						return EWRError::Failure_ForceKick_AutoCheck;
			case MoveServer:					return EWRError::Failure_ForceKick_MoveServer;
			case AbuseMovingCheck:				return EWRError::Failure_ForceKick_AbuseMovingCheck;
			case HackingPacket:					return EWRError::Failure_ForceKick_HackingPacket;
			case SequenceNumberCheck:			return EWRError::Failure_ForceKick_SequenceNumberCheck;
			case Disconnect:					return EWRError::Failure_ForceKick_Disconnect;
			case InvalidCurrencyDetected:		return EWRError::Failure_ForceKick_InvalidCurrencyDetected;
			case ToolCommand:					return EWRError::Failure_ForceKick_ToolCommand;
		}

		return EWRError::Failure_ForceKick_Invalid;
	}
};


//-----------------------------------------------------
//	로그아웃 타입
//-----------------------------------------------------
struct EWRLogoutType
{
	enum en
	{
		Lobby	= 0,				//	로비 단계에서 로그아웃됨
		Game,						//	인게임 진입 후 로그아웃됨

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Lobby,			L"Lobby" );
		InsertMap( Game,			L"Game" );
	}
};

// 유저 진행 상태
struct EWRUserSequence
{
	enum en
	{
		Close,								// 접속 되어 있지 않거나 접속 종료중인 상태
		Accept,								// 접속만 되어있는 상태
		CertifyRegistWaiting,				// 인증/등록 대기 상태
		InLoginServer,						// 로그인 서버 진입 상태 (인증/등록 완료)
		MovingToGameServer,					// 게임서버로 이동중 상태
		InGameServer,						// 게임서버 서버 진입 상태
		InLab,								// [로비] 연구소 진입 - 플레이어 목록 받아온 상태
		InGame,								// [게임] 플레이어로 게임 진행

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Close,					L"Close" );
		InsertMap( Accept,					L"Accept" );
		InsertMap( CertifyRegistWaiting,	L"CertifyRegistWaiting" );
		InsertMap( InLoginServer,			L"InLoginServer" );
		InsertMap( MovingToGameServer,		L"MovingToGameServer" );
		InsertMap( InGameServer,			L"InGameServer" );
		InsertMap( InLab,					L"InLab" );
		InsertMap( InGame,					L"InGame" );
	};

	static inline const bool IsInGameServer( const EWRUserSequence::en enUserSequence )
	{
		if ( ( InGameServer <= enUserSequence ) && ( MAX > enUserSequence ) )
			return true;

		return false;
	}
};

//	유저 등급 정보
struct EWRUserGrade
{
	enum en
	{
		User = 0,					// 일반계정
		JuniorGM,					// GM 팀원급
		MasterGM,					// GM 팀장급
		Observer,					// 옵져버
		Developer,					// 개발자
		Bot,
		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( User,			L"User" );
		InsertMap( JuniorGM,		L"JuniorGM" );
		InsertMap( MasterGM,		L"MasterGM" );
		InsertMap( Observer,		L"Observer" );
		InsertMap( Developer,		L"Developer" );
		InsertMap( Bot,				L"Bot" );
	}
};


//
//
//// 유저 정보
//struct stLoginUserInfo : IMSerialize
//{
//	SOCKET_INDEX_t			socketindex;			// SocketIndex
//	USN_t					usn;					// USN
//	PROTOCOL_VERSION_t		protocolVersion;		// 프로토콜 번호
//
//	PSN_t					selectPSN;				//마지막에 선택한 플레이어 PSN
//	int8					playerSlotCount;		//현재 생성할 수 있는 플레이어 수
//	int8					royalStamp;				//플레이어 생성 수를 늘릴수 있는 재화
//	int8					userGrade;				//유저 등급
//
//	stLoginUserInfo()
//		: socketindex(INVALID_SOCKET_INDEX)
//		, usn(INVALID_USN)
//		, protocolVersion(INVALID_PROTOCOL_VERSION)
//		, selectPSN(INVALID_PSN)
//		, playerSlotCount(0)
//		, royalStamp(0)
//		, userGrade(EIMAccountGrade::MAX)
//	{
//	}
//
//	IMMessage& ReadMsg(IMMessage& rMsg) override
//	{
//		return rMsg >> socketindex >> usn >> protocolVersion
//			>> selectPSN >> playerSlotCount >> royalStamp >> userGrade;
//	}
//	IMMessage& WriteMsg(IMMessage& rMsg) override
//	{
//		return rMsg << socketindex << usn << protocolVersion
//			<< selectPSN << playerSlotCount << royalStamp << userGrade;
//	}
//
//#ifdef USE_PROTOCOL_TOSTRING
//	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
//	{
//		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"userSocketIndex : %d, usn : %d"
//			, socketindex
//			, usn);
//		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"protocolVersion : %d, selectPSN : %d, playerSlotCount : %d, royalStamp : %d, userGrade : %d"
//			, protocolVersion, selectPSN, playerSlotCount, royalStamp, userGrade);
//	}
//#endif	// USE_PROTOCOL_TOSTRING
//};

//struct stLinkageUserInfo : IMSerialize
//{
//	struct stLinkageCharacter : IMSerialize
//	{
//		CLASS_TYPE_t classType;
//		NAME_t playerName;
//		AWAKEN_LEVEL_t awakenLevel;
//		LEVEL_t level;
//		double loginDate;
//		stLinkageCharacter()
//			: classType( INVALID_CLASSTYPE )
//			, playerName( INVALID_NAME )
//			, awakenLevel( INVALID_AWAKEN_LEVEL )
//			, level( INVALID_LEVEL )
//			, loginDate( 0.0f )
//		{}
//		IMMessage& ReadMsg( IMMessage& rMsg ) override
//		{
//			return rMsg >> classType >> playerName >> awakenLevel >> level >> loginDate;
//		}
//		IMMessage& WriteMsg( IMMessage& rMsg ) override
//		{
//			return rMsg << classType << playerName << awakenLevel << level << loginDate;
//		}
//#ifdef USE_PROTOCOL_TOSTRING
//		void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
//		{
//			ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"classType : %u, characterName : %s, awakenLevel : %u, level: %u, date: %f"
//										, classType, playerName.c_str(), awakenLevel, level, loginDate);
//		}
//#endif	// USE_PROTOCOL_TOSTRING
//	};
//	std::list<stLinkageCharacter> listLinkageCharacter;
//	stLinkageUserInfo()
//	{
//
//	}
//	IMMessage& ReadMsg( IMMessage& rMsg ) override
//	{
//		return rMsg >> listLinkageCharacter;
//	}
//	IMMessage& WriteMsg( IMMessage& rMsg ) override
//	{
//		return rMsg << listLinkageCharacter;
//	}
//#ifdef USE_PROTOCOL_TOSTRING
//	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
//	{
//		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, listLinkageCharacter, L"stLinkageCharacter");
//	}
//#endif	// USE_PROTOCOL_TOSTRING
//};



struct stWaitingInfo : IMSerialize
{
	USN_t				usn;
	int32				waitingCount;

	stWaitingInfo()
		: usn( INVALID_USN ), waitingCount( 0 )
	{
	}

#ifndef USE_CLIENT_ONLY
	stWaitingInfo( const stWaitingInfo& rhs )
	{
		usn = rhs.usn;
		waitingCount = rhs.waitingCount;
	}

	stWaitingInfo( const stWaitingInfo&& rhs )
	{
		usn = std::move( rhs.usn );
		waitingCount = std::move( rhs.waitingCount );
	}

	inline size_t GetSize()
	{
		const size_t size = sizeof( usn ) + sizeof( waitingCount );
		return size;
	}
#endif //USE_CLIENT_ONLY

	bool operator < ( stWaitingInfo& rhs )
	{
		return true;
	}

	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg >> usn >> waitingCount;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg << usn << waitingCount;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg
								  , L"usn : %d, waitingCount : %d"
								  , usn
								  , waitingCount );
	}
#endif	// USE_PROTOCOL_TOSTRING
};

struct stResultEntryInfo : IMSerialize
{
	RESULT_t				resultItemList;
	RESULT_t				resultSekaiItemList;
	RESULT_t				resultSekaiCurrencyList;

	stResultEntryInfo()
		: resultItemList(INVALID_RESULT)
		, resultSekaiItemList(INVALID_RESULT)
		, resultSekaiCurrencyList(INVALID_RESULT)
	{}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg
			>> resultItemList
			>> resultSekaiItemList
			>> resultSekaiCurrencyList
			;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg
			<< resultItemList
			<< resultSekaiItemList
			<< resultSekaiCurrencyList
			;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"resultItemList : %s", GetErrorString((EWRError::en)resultItemList));
	}
#endif	//  USE_PROTOCOL_TOSTRING
};