//******************************************************************************
//	FileName: 	Define_User.h
//------------------------------------------------------------------------------
//	Created	:	2016/11/21
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2016 WemadeIO Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	User ���� ���� ����
//******************************************************************************


#pragma once

#include "../Protocol/ProtocolStringError.h"


//-----------------------------------------------------
//	������������ Ÿ��
//	!!! .... �̰��� �߰��� EIMProtocolErrorSC ������ �߰� �ʿ�
//-----------------------------------------------------
struct EWRForceKick
{
	enum en
	{
		None = 0,

		Push,							//	���������� ������������ ��������
		GM,								//	���񽺿�� ���� ��������
		Error,							//	���α׷� ������ ���� ��������
		DBError,						//	DB ������ ���� ��������
		Timeout,						//	��Ⱓ �������� ��������						
		Etc,							//	��Ÿ���������� ��������
		Channel,						//	ä�κ��濡���� ��������
		ShutdownTime,					//	�˴ٿ� ������ ���� ��������
		AutoCheck,						//	���� üũ�� ���� ���� ����
		MoveServer,						//	���� ���� �������� ����
		AbuseMovingCheck,				//	�������� �̵����� ���� ��������
		HackingPacket,					//	�������� ��Ŷ���� ���� ��������
		SequenceNumberCheck,			//	�޽��� ������ �ѹ� ���������� ���� ���� ����
		Disconnect,						//	���� ���迡 ���� ���� ����
		InvalidCurrencyDetected,		//	��ȿ���� ���� ��ȭ�� ������
		ToolCommand,					//	������� ű �����

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
//	�α׾ƿ� Ÿ��
//-----------------------------------------------------
struct EWRLogoutType
{
	enum en
	{
		Lobby	= 0,				//	�κ� �ܰ迡�� �α׾ƿ���
		Game,						//	�ΰ��� ���� �� �α׾ƿ���

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Lobby,			L"Lobby" );
		InsertMap( Game,			L"Game" );
	}
};

// ���� ���� ����
struct EWRUserSequence
{
	enum en
	{
		Close,								// ���� �Ǿ� ���� �ʰų� ���� �������� ����
		Accept,								// ���Ӹ� �Ǿ��ִ� ����
		CertifyRegistWaiting,				// ����/��� ��� ����
		InLoginServer,						// �α��� ���� ���� ���� (����/��� �Ϸ�)
		MovingToGameServer,					// ���Ӽ����� �̵��� ����
		InGameServer,						// ���Ӽ��� ���� ���� ����
		InLab,								// [�κ�] ������ ���� - �÷��̾� ��� �޾ƿ� ����
		InGame,								// [����] �÷��̾�� ���� ����

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

//	���� ��� ����
struct EWRUserGrade
{
	enum en
	{
		User = 0,					// �Ϲݰ���
		JuniorGM,					// GM ������
		MasterGM,					// GM �����
		Observer,					// ������
		Developer,					// ������
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
//// ���� ����
//struct stLoginUserInfo : IMSerialize
//{
//	SOCKET_INDEX_t			socketindex;			// SocketIndex
//	USN_t					usn;					// USN
//	PROTOCOL_VERSION_t		protocolVersion;		// �������� ��ȣ
//
//	PSN_t					selectPSN;				//�������� ������ �÷��̾� PSN
//	int8					playerSlotCount;		//���� ������ �� �ִ� �÷��̾� ��
//	int8					royalStamp;				//�÷��̾� ���� ���� �ø��� �ִ� ��ȭ
//	int8					userGrade;				//���� ���
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