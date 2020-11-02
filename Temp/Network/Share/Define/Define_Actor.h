//******************************************************************************
//	FileName: 	Define_Actor.h
//------------------------------------------------------------------------------
//	Created	:	2019/08/30
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	액터 관리용
//******************************************************************************


#pragma once


#include "../Protocol/IMMessage.h"
#include "Define_Time.h"
#include "Define_BluePrint.h"


//-----------------------------------------------------
//	define 모음
//-----------------------------------------------------
#define MAX_ACTOR_LEVEL					30			//	Actor Level

#define DEF_PLAYER_UNBEATABLE_TIME		3000		//	플레이어 생성후 무적 기본 시간 (3초)
#define DEF_FELLOW_UNBEATABLE_TIME		3000		//	플레이어 생성후 무적 기본 시간 (3초)

#define DEF_PLAYER_REBORN_TIME			1000		//	플레이어가 다시 살아나기까지의 기본 시간

#define DEF_CORPSE_COMA_TIME			100			//	플레이어가 아닌 액터들의 시체 유지 상태 기본 시간
#define DEF_CORPSE_PLAYER_COMA_TIME		3000		//	플레이어 시체 유지 상태 기본 시간

#define DEF_RESERVE_RESURRECTION_WAITING_TIME	3000	//	유료 부활 요청 후 DB 처리 예약 대기 기본 시간 ... 이 시간내 안되면 다음 단계 진행

#define ERROR_STATE_TIME				360000		//	스테이트 설정 시간 오류 체크용 ... 이 이상 설정이면 오류

#define DEF_SPAWN_ANI_TIME				500			// 스폰 애니메이션 기본 시간

#define GATHER_CASTING_OFFSET_TIME		500			// 서버와 클라간의 채집 동기화 시간이 어긋날수 있으므로 어긋날수 있는 시간을 세팅( 0.5초 )

#define DEF_CHANGE_VEHICLE_STATE_TIME	100			//	Vehicle State 기본 전환 시간

#define DEF_CHEERS_SUGGEST_LIMIT_TIME	3000		// 건배 제안 제한 시간


//-----------------------------------------------------
//	액터 유형 ... 클라와 동일한 유형 사용
//-----------------------------------------------------
struct EWRActorType
{
	enum en
	{
		Player				= 0,
		Fellow				= 1,			//	Player 종속 개념의 객체
		Npc					= 2,			//	Npc
		InteractionObject	= 3,			//	Interaction Object
		Portal				= 4,			//	IO - Portal
		Brick				= 5,			//	IO - Brick
		SkillObject			= 6,			//	Skill Object
		DropObject			= 7,			//	Drop Object
		ActiveObject		= 8,			//	Active Object
		ClientObject		= 9,			//	클라이언트에서 사용하는 공간으로 / 서버에 해당 정보가 오는 경우 오류 처리하게 됨
		ActionProp			= 10,			//	트리거 행위 연출 등
		Elevator			= 11,			//	(운송 수단) 엘리베이터
		Monorail			= 12,			//	(운송 수단) 모노레일
		Tardis				= 13,			//	(운송 수단) T ime a nd R elative D imension i n S pace
		FloatingBoard		= 14,			//	(운송 수단) 부유섬(판)
		Vehicle				= 15,			//	(자가용)
		SecretDialog		= 16,			//  비밀대화 포탈

		MAX,

		INVALID = MAX,
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Player,				L"Player" );
		InsertMap( Fellow,				L"Fellow" );
		InsertMap( Npc,					L"Npc" );
		InsertMap( InteractionObject,	L"IO" );
		InsertMap( Portal,				L"Portal" );
		InsertMap( Brick,				L"Brick" );
		InsertMap( SkillObject,			L"SO" );
		InsertMap( DropObject,			L"DO" );
		InsertMap( ActiveObject,		L"AO" );
		InsertMap( ClientObject,		L"CO" );
		InsertMap( ActionProp,			L"AP" );
		InsertMap( Elevator,			L"Elevator" );
		InsertMap( Monorail,			L"Monorail" );
		InsertMap( Tardis,				L"Tardis" );
		InsertMap( FloatingBoard,		L"FloatingBoard" );
		InsertMap( Vehicle,				L"Vehicle" );
		InsertMap( SecretDialog,		L"SecretDialog" );
	}

	//	공격 가능한 유형인가
	static inline const bool	IsAttackable( const EWRActorType::en enActorType )
	{
		switch ( enActorType ) 
		{
			case EWRActorType::Player:				return true;
			case EWRActorType::Fellow:				return true;		
			case EWRActorType::Npc:					return true;
			case EWRActorType::InteractionObject:	return false;
			case EWRActorType::Portal:				return false;
			case EWRActorType::Brick:				return false;
			case EWRActorType::SkillObject:			return true;
			case EWRActorType::DropObject:			return false;
			case EWRActorType::ActiveObject:		return false;
			case EWRActorType::ClientObject:		return false;	//	클라에서 별도 판단 필요
			case EWRActorType::ActionProp:			return false;
			case EWRActorType::Elevator:			return false;
			case EWRActorType::Monorail:			return false;
			case EWRActorType::Tardis:				return false;
			case EWRActorType::FloatingBoard:		return false;
			case EWRActorType::Vehicle:				return false;
			case EWRActorType::SecretDialog:		return false;
		}

		return false;
	}

	//	타격할 수 있는가?
	static inline const bool	IsBeatable( const EWRActorType::en enActorType )
	{
		switch ( enActorType )
		{
			case EWRActorType::Player:				return true;
			case EWRActorType::Fellow:				return false;
			case EWRActorType::Npc:					return true;
			case EWRActorType::InteractionObject:	return false;
			case EWRActorType::Portal:				return false;
			case EWRActorType::Brick:				return false;
			case EWRActorType::SkillObject:			return false;
			case EWRActorType::DropObject:			return false;
			case EWRActorType::ActiveObject:		return false;
			case EWRActorType::ClientObject:		return false;	//	클라에서 별도 판단 필요
			case EWRActorType::ActionProp:			return false;
			case EWRActorType::Elevator:			return false;
			case EWRActorType::Monorail:			return false;
			case EWRActorType::Tardis:				return false;
			case EWRActorType::FloatingBoard:		return false;
			case EWRActorType::Vehicle:				return false;
			case EWRActorType::SecretDialog:		return false;
		}

		return false;
	}
	
	//	생명체 유형인가
	static inline const bool IsLiving( const EWRActorType::en enActorType )
	{
		switch ( enActorType )
		{
			case EWRActorType::Player:				return true;
			case EWRActorType::Fellow:				return true;
			case EWRActorType::Npc:					return true;
			case EWRActorType::InteractionObject:	return false;
			case EWRActorType::Portal:				return false;
			case EWRActorType::Brick:				return false;
			case EWRActorType::SkillObject:			return false;
			case EWRActorType::DropObject:			return false;
			case EWRActorType::ActiveObject:		return false;
			case EWRActorType::ClientObject:		return false;	//	클라에서 별도 판단 필요
			case EWRActorType::ActionProp:			return false;
			case EWRActorType::Elevator:			return false;
			case EWRActorType::Monorail:			return false;
			case EWRActorType::Tardis:				return false;
			case EWRActorType::FloatingBoard:		return false;
			case EWRActorType::Vehicle:				return false;
			case EWRActorType::SecretDialog:		return false;
		}

		return false;
	}

	//	stat 을 가지는가
	static inline const bool IsHavingStat( const EWRActorType::en enActorType )
	{
		switch ( enActorType )
		{
			case EWRActorType::Player:				return true;
			case EWRActorType::Fellow:				return true;
			case EWRActorType::Npc:					return true;
			case EWRActorType::InteractionObject:	return false;
			case EWRActorType::Portal:				return false;
			case EWRActorType::Brick:				return false;
			case EWRActorType::SkillObject:			return true;
			case EWRActorType::DropObject:			return false;
			case EWRActorType::ActiveObject:		return false;
			case EWRActorType::ClientObject:		return false;	//	클라에서 별도 판단 필요
			case EWRActorType::ActionProp:			return false;
			case EWRActorType::Elevator:			return false;
			case EWRActorType::Monorail:			return false;
			case EWRActorType::Tardis:				return false;
			case EWRActorType::FloatingBoard:		return false;
			case EWRActorType::Vehicle:				return false;
			case EWRActorType::SecretDialog:		return false;
		}

		return false;
	}

	//	운송 수단인가
	static inline const bool IsVehicle( const EWRActorType::en enActorType )
	{
		switch ( enActorType )
		{
			case EWRActorType::Elevator:			return true;
			case EWRActorType::Monorail:			return true;
			case EWRActorType::Tardis:				return true;
			case EWRActorType::FloatingBoard:		return true;
		}

		return false;
	}

	//	운송 수단인가 ... 문을 가지고 있는가
	static inline const bool DoesItHasADoor( const EWRActorType::en enActorType )
	{
		switch ( enActorType )
		{
			case EWRActorType::Elevator:			return true;
			case EWRActorType::Monorail:			return true;
			case EWRActorType::Tardis:				return true;
			case EWRActorType::FloatingBoard:		return false;
		}

		return false;
	}
	//	플레이어가 아닌 객체인가
	static inline const bool IsNonPlayer( const EWRActorType::en enActorType )
	{
		return ( ( ( EWRActorType::Player < enActorType ) && ( EWRActorType::MAX > enActorType ) ) ? true : false );
	}
};


//-----------------------------------------------------
//	NPC Power 권력 유형
//-----------------------------------------------------
struct EWRNpcPowerType
{
	enum en
	{
		Normal = 0,			//	일반
		Boss,				//	대장
		
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Normal,		L"Normal" );
		InsertMap( Boss,		L"Boss" );
	}
};


//-----------------------------------------------------
//	NPC 형태별 분류
//-----------------------------------------------------
struct EWRNpcClassifyType
{
	enum en
	{
		None,
		Human,
		Monster,
		Beast,
		Spirit,
		Giant,
		Object,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Human,		L"Human" );
		InsertMap( Monster,		L"Monster" );
		InsertMap( Beast,		L"Beast" );
		InsertMap( Spirit,		L"Spirit" );
		InsertMap( Giant,		L"Giant" );
		InsertMap( Object,		L"Object" );
	}
};


//-----------------------------------------------------
//	NPC 등급
//-----------------------------------------------------
struct EWRNpcGradeType
{
	enum en
	{
		None = 0,

		Normal,
		Superior,
		Elite,
		Hero,
		Legend,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Normal,		L"Normal" );
		InsertMap( Superior,	L"Superior" );
		InsertMap( Elite,		L"Elite" );
		InsertMap( Hero,		L"Hero" );
		InsertMap( Legend,		L"Legend" );
	}
};


//-----------------------------------------------------
//	Portal 사용 형태
//-----------------------------------------------------
struct EWRPortalUseType
{
	enum en
	{
		None,
		Normal,
		Interaction,

		MAX,
	};

	DeclareMapTable();

	static inline void BuildMap()
	{
		InsertMap( Normal,		L"normal" );
		InsertMap( Interaction, L"interaction" );
	}
};

//---


struct stCustomizeInfo : IMSerialize
{
	std::string		customize;
	int32			customizeSize;

	stCustomizeInfo()
		: customize("")
		, customizeSize(0)
	{
	}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg	>> customize >> customizeSize;
	}

	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << customize << customizeSize;
	}

	void Copy(stCustomizeInfo* pCustomizeInfoSrc)
	{
		customize = pCustomizeInfoSrc->customize;
		customizeSize = pCustomizeInfoSrc->customizeSize;
	}

	void Clear()
	{
		customize = "";
		customizeSize = 0;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"customize : %s, customizeSize : %d", customize.c_str(), customizeSize);
	}
#endif	// USE_PROTOCOL_TOSTRING
};


struct stVideoChatMember : IMSerialize
{
	int					shortcutNum;	// 단축번호
	AID_t				aid;
	NAME_t				name;			// 캐릭터 이름
	NAME_t				nickName;		// 별칭
	TID_t 				m_PlayerCostumeTID;
	std::list<TID_t>	listCostumeTID; // 의상 정보
	uint8				m_PlayerGender;
	stCustomizeInfo		m_PlayerCustomizeInfo;
	bool				bOnline;

	stVideoChatMember() : shortcutNum(-1), aid(INVALID_PSN), bOnline(false)
	{
	}

	//#ifndef USE_CLIENT_ONLY
	//	stVideoChatMember(const stVideoChatMember& rhs)
	//	{
	//		shortcutNum = rhs.shortcutNum;
	//		aid			= rhs.aid;
	//		name		= rhs.name;
	//		nickName	= rhs.nickName;
	//		bOnline		= rhs.bOnline;
	//	}
	//
	//	stVideoChatMember(const stVideoChatMember&& rhs)
	//	{
	//		shortcutNum = std::move(rhs.shortcutNum);
	//		aid			= std::move(rhs.aid);
	//		name		= std::move(rhs.name);
	//		nickName	= std::move(rhs.nickName);
	//		bOnline		= std::move(rhs.bOnline);
	//	}
	//
	//#endif //USE_CLIENT_ONLY

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> shortcutNum >> aid >> name >> nickName >> m_PlayerCostumeTID >> listCostumeTID >> m_PlayerGender >> m_PlayerCustomizeInfo >> bOnline;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << shortcutNum << aid << name << nickName << m_PlayerCostumeTID << listCostumeTID << m_PlayerGender << m_PlayerCustomizeInfo << bOnline;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"shortcutNum:%d, aid:%d, nickName:%s, anotherName:%s online:%s", shortcutNum, aid, name.c_str(), nickName.c_str(), bOnline ? L"true" : L"false");
	}
#endif	// USE_PROTOCOL_TOSTRING
};


//-----------------------------------------------------
//	플레이어 정보 (클라/서버 정보 공유용)
//-----------------------------------------------------
struct stPlayerInfo : IMSerialize
{
	PSN_t					psn;						// 플레이어 키값 (DB에서 발급된 키값)
	NAME_t					playerName;					// 플레이어명
	AID_t					playerAID;					// 플레이어 Actor ID (서버에서 생성한 키값)

	LEVEL_t					playerLevel;				// 플레이어 레벨
	EXP_t					playerExp;					// 플레이어 경험치
	
	TID_t					playerCostumeTID;			// 플레이어 프리셋 번호

	uint8					playerGender;				// 플레이어 성별
	stCustomizeInfo			playerCustomizeInfo;		// 플레이어 커스터마이즈 정보

	MAP_INDEX_t				mapIndex;					// Map Unique ID
	TID_t					mapTID;						// 생성되는 Map TID (MapData_맵종류.xml의 tid참조)
	float					startPosX;					// 플레이어 위치 (X축)
	float					startPosY;					// 플레이어 위치 (Y축)
	float					startPosZ;					// 플레이어 위치 (Z축)
	float					startAngle;					// 생성시 방향 결정용

	HP_t					hp;							// 플레이어 현재 HP
	GAME_MONEY_t			gold;						// 골드
	
	stPlayerInfo()	
		: psn( INVALID_PSN )
		, playerName( INVALID_NAME )
		, playerAID( INVALID_AID )
		, playerLevel( INVALID_LEVEL )
		, playerExp( 0 )
		, playerCostumeTID( INVALID_TID )
		, playerGender( (uint8)(EWRGender::Male) )
		, mapIndex( INVALID_MAP_INDEX )
		, mapTID( INVALID_TID )
		, startPosX( 0.0f )
		, startPosY( 0.0f )
		, startPosZ( 0.0f )
		, startAngle( 0.0f )
		, hp( 0 )
		, gold( 0 )
	{
	}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg
			>> psn
			>> playerName
			>> playerAID
			>> playerLevel
			>> playerExp
			>> playerCostumeTID
			>> playerGender
			>> playerCustomizeInfo
			>> mapIndex
			>> mapTID
			>> startPosX
			>> startPosY
			>> startPosZ
			>> startAngle
			>> hp
			>> gold
			;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg
			<< psn
			<< playerName
			<< playerAID
			<< playerLevel
			<< playerExp
			<< playerCostumeTID
			<< playerGender
			<< playerCustomizeInfo
			<< mapIndex
			<< mapTID
			<< startPosX
			<< startPosY
			<< startPosZ
			<< startAngle
			<< hp
			<< gold
			;
	}

	void	Copy( stPlayerInfo* pPlayerInfoSrc )
	{
		psn					= pPlayerInfoSrc->psn;
		playerName			= pPlayerInfoSrc->playerName;
		playerAID			= pPlayerInfoSrc->playerAID;
		playerLevel			= pPlayerInfoSrc->playerLevel;
		playerExp			= pPlayerInfoSrc->playerExp;
		playerCostumeTID	= pPlayerInfoSrc->playerCostumeTID;
		playerGender		= pPlayerInfoSrc->playerGender;
		playerCustomizeInfo	= pPlayerInfoSrc->playerCustomizeInfo;
		mapIndex			= pPlayerInfoSrc->mapIndex;
		mapTID				= pPlayerInfoSrc->mapTID;
		startPosX			= pPlayerInfoSrc->startPosX;
		startPosY			= pPlayerInfoSrc->startPosY;
		startPosZ			= pPlayerInfoSrc->startPosZ;
		startAngle			= pPlayerInfoSrc->startAngle;
		hp					= pPlayerInfoSrc->hp;
		gold				= pPlayerInfoSrc->gold;
	}

	void	Clear()
	{
		psn					= INVALID_PSN;
		playerName			= INVALID_NAME;
		playerAID			= INVALID_AID;
		playerLevel			= INVALID_LEVEL;
		playerExp			= 0;
		playerCostumeTID	= INVALID_TID;
		playerGender		= (uint8)(EWRGender::Male);
		mapIndex			= INVALID_MAP_INDEX;
		mapTID				= INVALID_TID;
		startPosX			= 0.0f;
		startPosY			= 0.0f;
		startPosZ			= 0.0f;
		startAngle			= 0.0f;
		hp					= 0;
		gold				= 0;

		playerCustomizeInfo.Clear();
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u, playerName : %s", psn, playerName.c_str() );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerAID : %llu", playerAID );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerLevel : %u, playerExp : %lld, playerCostumeTID : %d", playerLevel, playerExp , playerCostumeTID);
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapIndex : %d, mapTID : %d, startPosX : %f, startPosY : %f, startPosZ : %f, startAngle : %f"
								  , mapIndex, mapTID, startPosX, startPosY, startPosZ, startAngle );
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"hp : %d, gold : %d", hp, gold );								  
	}
#endif	// USE_PROTOCOL_TOSTRING
};


//-----------------------------------------------------
// 로비에서 사용되는 간단한 형태의 Player Info 보내줄때 사용하는 구조체
//-----------------------------------------------------
struct stSimplePlayerInfo : IMSerialize
{
	PSN_t					psn;					// 플레이어 키값 (DB에서 발급된 키값)
	NAME_t					playerName;				// 플레이어명
	LEVEL_t					playerLevel;			// 플레이어 레벨
	EXP_t					playerExp;				// 플레이어 경험치

	TID_t					mapTID;					//	현재 맵 정보 ... 최초 생성시 INVALID (MapData_맵유형.xml의 tid 참조)

	stSmallDateTime			deleteDT;				// 삭제한 날짜
	bool					bDelete;				// 삭제 대기 상태 여부

	stSimplePlayerInfo()
		: psn( INVALID_PSN )
		, playerName( INVALID_NAME )
		, playerLevel( INVALID_LEVEL )
		, playerExp( 0 )
		, mapTID( INVALID_TID )
	{
	}

	stSimplePlayerInfo( IN const stSimplePlayerInfo& rSimplePlayerInfo )
		: psn( rSimplePlayerInfo.psn )
		, playerName( rSimplePlayerInfo.playerName.c_str() )
		, playerLevel( rSimplePlayerInfo.playerLevel )
		, playerExp( rSimplePlayerInfo.playerExp )
		, mapTID( rSimplePlayerInfo.mapTID )
		, deleteDT( rSimplePlayerInfo.deleteDT )
		, bDelete( rSimplePlayerInfo.bDelete )
	{
	}

	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg
			>> psn
			>> playerName
			>> playerLevel
			>> playerExp
			>> mapTID
			>> deleteDT
			>> bDelete
			;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg
			<< psn
			<< playerName
			<< playerLevel
			<< playerExp
			<< mapTID
			<< deleteDT
			<< bDelete
			;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString( std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg ) override
	{
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"psn : %u, playerName : %s", psn, playerName.c_str() );								  
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"playerLevel : %u, playerExp : %lld", playerLevel, playerExp );								  
		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"mapTID : %d, bDelete : %s", mapTID, bDelete );
	}
#endif	// USE_PROTOCOL_TOSTRING

public:
	bool operator==( const stSimplePlayerInfo& rSimplePlayerInfo ) const
	{
		return psn == rSimplePlayerInfo.psn;
	}
};

//-----------------------------------------------------
//	자가용 외형 정보
//-----------------------------------------------------
struct stVehicleExteriorInfo : IMSerialize
{
	enum TurnIndicator : uint8
	{
		OFF			= 0,
		LEFT		= 1,
		RIGHT		= 2,
		EMERGENCY	= 3,
	};

	stVehicleExteriorInfo() : turnIndicator(OFF), light(false), foldingTop(false)
	{}

	uint8		turnIndicator;
	bool		light;
	bool		foldingTop;

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> turnIndicator >> light >> foldingTop;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << turnIndicator << light << foldingTop;
	}

	void	Copy(stVehicleExteriorInfo* info)
	{
		*this = *info;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		// 		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"permanentStatType : %s, permanentStatvalue : %d"
		// 								  , EWRStatOptionType::GetStr( ( EWRStatOptionType::en )permanentStatType ), permanentStatvalue );
	}
#endif	// USE_PROTOCOL_TOSTRING
};

struct stVector3 : IMSerialize
{
public:
	stVector3():x(0.0f), y(0.0f), z(0.0f)
	{}

	float x;
	float y;
	float z;

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> x >> y >> z;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << x << y << z;
	}

#ifdef USE_SERVER_ONLY
	void Get(POS3_t& pos)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}
	void Set(POS3_t& pos)
	{
		x = pos.x;
		y = pos.y;
		z = pos.z;
	}
#else//USE_SERVER_ONLY
#endif//USE_SERVER_ONLY

	void	Copy(stVector3* src)
	{
		*this = *src;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		// 		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"permanentStatType : %s, permanentStatvalue : %d"
		// 								  , EWRStatOptionType::GetStr( ( EWRStatOptionType::en )permanentStatType ), permanentStatvalue );
	}
#endif	// USE_PROTOCOL_TOSTRING

#ifdef USE_CLIENT_ONLY
	void operator=(const FVector& rFVector)
	{
		x = rFVector.X;
		y = rFVector.Y;
		z = rFVector.Z;
	}
	void operator=(const FRotator& rFRotator)
	{
		x = rFRotator.Roll;
		y = rFRotator.Pitch;
		z = rFRotator.Yaw;
	}
#endif //USE_CLIENT_ONLY
};
//-----------------------------------------------------
//	영구 스탯 한 항목
//-----------------------------------------------------
struct stPermanentStat : IMSerialize
{
	PERMANENT_STAT_TYPE_t	permanentStatType;		//	영구 스탯 유형 (EWRStatOptionType::en)
	STAT_t					permanentStatvalue;		//	영구 스탯 값 (예 - 영구스탯이 HP라면, HP값)

	stPermanentStat()
		: permanentStatType( INVALID_PERMANENT_STAT_TYPE )
		, permanentStatvalue( 0 )
	{}

	IMMessage& ReadMsg( IMMessage& rMsg ) override
	{
		return rMsg >> permanentStatType >> permanentStatvalue;
	}
	IMMessage& WriteMsg( IMMessage& rMsg ) override
	{
		return rMsg << permanentStatType << permanentStatvalue;
	}

	void	Copy( stPermanentStat* pPermanentStatSrc )
	{
		permanentStatType	= pPermanentStatSrc->permanentStatType;
		permanentStatvalue	= pPermanentStatSrc->permanentStatvalue;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
// 		ADD_ELEMENTS_TO_LOG_LIST( rStrPacketMsgList, rStrMsg, L"permanentStatType : %s, permanentStatvalue : %d"
// 								  , EWRStatOptionType::GetStr( ( EWRStatOptionType::en )permanentStatType ), permanentStatvalue );
	}
#endif	// USE_PROTOCOL_TOSTRING
};


//-----------------------------------------------------
//	리액션 타입
//-----------------------------------------------------
struct EWRReactableType
{
	enum en
	{
		Reaction,		// 리액션
		OnlyPushed,		// 밀리기만
		NotReaction,	// 리액션 하지 않음

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Reaction,		L"Reaction" );
		InsertMap( OnlyPushed,		L"OnlyPushed" );
		InsertMap( NotReaction,		L"NotReaction" );
	}
};


//-----------------------------------------------------
//	NPC/ENPC 타입
//-----------------------------------------------------
struct EWRNpcBaseType
{
	enum en
	{
		NPC,		// npc
		ENPC,		// enpc
		ANIMAL,		// animal
		DanceNPC,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( NPC,			L"NPC" );
		InsertMap( ENPC,		L"ENPC" );
		InsertMap( ANIMAL,		L"ANIMAL" );
		InsertMap( DanceNPC,	L"DanceNPC" );
	}
};

// 전투 시 주변 이동시 바라보는 방향
struct EWRNpcBattleType
{
	enum en
	{
		Normal = 0,
		Front = 1,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Normal, L"Normal");
		InsertMap(Front, L"Front");
	}
};

//-----------------------------------------------------
//	NPC PATROL 타입
//-----------------------------------------------------
struct EWRNpcPatrolType
{
	enum en
	{
		None		= 0,
		ROAMING		= 1,	// 방황
		ROTATION	= 2,	// 정방향 패트롤 1,2,3,1,2,3
		REVERSE		= 3,	// 1,2,3,2,1,2,3
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(ROAMING, L"ROAMING");
		InsertMap(ROTATION, L"ROTATION");
		InsertMap(REVERSE, L"REVERSE");
	}
};
//-----------------------------------------------------
//	ObjectMainType
//-----------------------------------------------------
struct EWRObjectMainType
{
	enum en
	{
		None,
		Interaction,			//	인터렉션 가능
		ActionProp,				//	트리거 등을 통해 동작 시키는 액션이 있는 프랍
		Brick,					//	길막기용 브릭

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,			L"none" );
		InsertMap( Interaction,		L"Interaction" );
		InsertMap( ActionProp,		L"ActionProp" );
		InsertMap( Brick,			L"Brick" );
	}
};

//-----------------------------------------------------
//	상호 작용 종류
//-----------------------------------------------------
struct EWRObjectInteractionType
{
	enum en
	{
		None,
		DirectInteraction,
		HookInteraction,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"none");
		InsertMap(DirectInteraction, L"DirectInteraction");
		InsertMap(HookInteraction, L"HookInteraction");
	}
};

//-----------------------------------------------------
//	직접 상호 작용 전용
//-----------------------------------------------------
struct EWRDirectType
{
	enum en
	{
		None,
		KeyInput,				//	키를 눌러 작동

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,		L"none" );
		InsertMap( KeyInput,	L"KeyInput" );
	}
};


//-----------------------------------------------------
//	직접 상호작용이 완료 되었을 때 결과
//-----------------------------------------------------
struct EWRDirectResultType
{
	enum en
	{
		None,
		Drop,					//	아이템 드랍
		Trigger,				//	트리거 동작
		SingleTrigger,			//	개인 트리거 동작

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,			L"none" );
		InsertMap( Drop,			L"Drop" );
		InsertMap( Trigger,			L"Trigger" );
		InsertMap( SingleTrigger,	L"SingleTrigger" );
	}
};


//-----------------------------------------------------
//	직접 상호작용 결과 후 해당 오브젝트를 어떻게 처리 할지 판단
//-----------------------------------------------------
struct EWRDirectResultAfter
{
	enum en
	{
		None,
		Retain,					//	유지하며 기능 리셋
		Delete,					//	오브젝트를 삭제하고 SpawnData 호출
		RetainHide,				//	오브젝트 매시를 숨김

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,		L"none" );
		InsertMap( Retain,		L"Retain" );
		InsertMap( Delete,		L"Delete" );
		InsertMap( RetainHide,	L"RetainHide" );
	}
};

//-----------------------------------------------------
//	갈고리 동작이 완료 되었을 때 결과 ( Activate, Pull, Dash 에 무관 )
//-----------------------------------------------------
struct EWRHookResultType
{
	enum en
	{
		None,
		Drop,					//	아이템 드랍
		Trigger,				//	트리거 동작
		SingleTrigger,			//	개인 트리거 동작

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,			L"none" );
		InsertMap( Drop,			L"Drop" );
		InsertMap( Trigger,			L"Trigger" );
		InsertMap( SingleTrigger,	L"SingleTrigger" );
	}
};


//-----------------------------------------------------
//	갈고리 결과 후 해당 오브젝트를 어떻게 처리 할지 판단
//-----------------------------------------------------
struct EWRHookResultAfter
{
	enum en
	{
		None,
		Retain,					//	유지하며 기능 리셋
		Delete,					//	오브젝트를 삭제하고 SpawnData 호출
		RetainHide,				//	오브젝트 매시를 숨김

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,		L"none" );
		InsertMap( Retain,		L"Retain" );
		InsertMap( Delete,		L"Delete" );
		InsertMap( RetainHide,	L"RetainHide" );
	}
};


//-----------------------------------------------------
//	IO 유형
//-----------------------------------------------------
struct EWRIOType
{
	enum en
	{
		None				= 0,
		Etc					= 1,
		Gather				= 2,
		Recovery_hp_add		= 3,
		Recovery_hp_per		= 4,

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,				L"none" );
		InsertMap( Etc,					L"etc" );
		InsertMap( Gather,				L"gather" );
		InsertMap( Recovery_hp_add,		L"recovery_hp_add" );
		InsertMap( Recovery_hp_per,		L"recovery_hp_per" );
	}
};


//-----------------------------------------------------
// Interaction 상태
//-----------------------------------------------------
struct EWRInteractionState
{
	enum en
	{
		None				= 0,		// 초기값
		Usable				= 1,		// IO를 사용할 수 있는 상태
		Using				= 2,
		Unusable			= 3,		// IO를 사용할 수 없는 상태

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,			L"None" );
		InsertMap( Usable,			L"Usable" );
		InsertMap( Using,			L"Using");
		InsertMap( Unusable,		L"Unusable" );
	}
};


struct EWRActionType
{
	enum en
	{
		NONE = 0,					// 초기값

		ENTER,						// 처음 맵 진입시 or 클라이언트 연출시 반투명의 무적상태
		STAND,						// 서 있는 상태 기본 Action
		FELLOW_RIDING_STAND,		// 펠로우를 타고 서 있는 상태
		DEAD,						// 죽어 있는 상태
		FELLOW_RIDING_DEAD,			// 펠로우를 타고 죽은 상태
		REVIVAL,					// 부활
		FELLOW_RIDING_REVIVAL,		// 펠로우 타고 부활

		RUN,						// 뛰는중
		WALK,						// 걷는중
		BACK_STEP,					// Player가 타켓시 뒷걸음질 하는 액션
		FELLOWFLING,				// Fellow를 타고 비행중( 이동중 )
		AWAY_STEP,					// enpc 원거리 몬스터가 거리를 두기 위해 이동
		SIDE_STEP,					// 옆으로 경계하면서 걷기 속도로 적군을 처다보며 이동

		ATTACK,						// 일반 평타 공격
		ATTACK_F,					// 공중평타 ( 펠로우를 타고 석궁등으로 공격 )
		SKILL,						// 스킬 시전중

		STUN,						// 스턴 상태
		PUSHED,						// 밀려나는거
		KNOCKBACK,					// 넉백
		KNOCKDOWN,					// 넉다운
		FREEZING,					// 
		GROGGY,						// 그로기( BOSS ENPC )
		DAMAGE,						// 피격당하는 액션

		GATHER,						// 채집중

		TAMING,						// 길들이는 액션( PLALYER )
		TAMIED,						// 길들여 지는 액션( ENPC )

		SPRINT,						// 펠로우 가속 OR 활강
		ACTION_SEQ,					// 액션 시퀀스 - 특별한 위치 동기화 필요
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( NONE,					L"NONE" );
		InsertMap( ENTER,					L"ENTER" );
		InsertMap( STAND,					L"STAND" );
		InsertMap( FELLOW_RIDING_STAND,		L"FELLOW_RIDING_STAND" );
		InsertMap( DEAD,					L"DEAD" );
		InsertMap( FELLOW_RIDING_DEAD,		L"FELLOW_RIDING_DEAD" );
		InsertMap( REVIVAL,					L"REVIVAL" );
		InsertMap( FELLOW_RIDING_REVIVAL,	L"FELLOW_RIDING_REVIVAL" );
		InsertMap( RUN,						L"RUN" );
		InsertMap( WALK,					L"WALK" );
		InsertMap( BACK_STEP,				L"BACK_STEP" );
		InsertMap( FELLOWFLING,				L"FELLOWFLING" );
		InsertMap( AWAY_STEP,				L"AWAY_STEP" );
		InsertMap( SIDE_STEP,				L"SIDE_STEP" );
		InsertMap( ATTACK,					L"ATTACK" );
		InsertMap( ATTACK_F,				L"ATTACK_F" );
		InsertMap( SKILL,					L"SKILL" );
		InsertMap( STUN,					L"STUN" );
		InsertMap( PUSHED,					L"PUSHED" );
		InsertMap( KNOCKBACK,				L"KNOCKBACK" );
		InsertMap( KNOCKDOWN,				L"KNOCKDOWN" );
		InsertMap( FREEZING,				L"FREEZING" );
		InsertMap( GROGGY,					L"GROGGY" );
		InsertMap( DAMAGE,					L"DAMAGE" );
		InsertMap( GATHER,					L"GATHER" );
		InsertMap( TAMING,					L"TAMING" );
		InsertMap( TAMIED,					L"TAMIED" );
		InsertMap( SPRINT,					L"SPRINT" );
		InsertMap( ACTION_SEQ,				L"ACTION_SEQ" );
	}

	static bool IsMoveAction( EWRActorType::en charType, EWRActionType::en actionType )
	{
		switch ( charType )
		{
			case EWRActorType::Player:
			case EWRActorType::Npc:
			case EWRActorType::Fellow:
			{
				switch ( actionType )
				{
					case EWRActionType::RUN:
					case EWRActionType::WALK:
					case EWRActionType::SPRINT:
					case EWRActionType::BACK_STEP:
					case EWRActionType::AWAY_STEP:
					case EWRActionType::SIDE_STEP:
						return true;
				}
			}
			default:
				break;
		}

		return false;
	}
};

// 성별
struct EWRGenderType
{
	enum en
	{
		NONE = 0,					// 초기값

		MALE_A,						// 남성A
		MALE_B,						// 남성B
		MALE_C,						// 남성C
		FEMALE_A,					// 여성A
		FEMALE_B,					// 여성B
		FEMALE_C,					// 여성C

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(NONE,			L"NONE");
		InsertMap(MALE_A,		L"MALE_A");
		InsertMap(MALE_B,		L"MALE_B");
		InsertMap(MALE_C,		L"MALE_C");
		InsertMap(FEMALE_A,		L"FEMALE_A");
		InsertMap(FEMALE_B,		L"FEMALE_B");
		InsertMap(FEMALE_C,		L"FEMALE_C");
	}
};


//-----------------------------------------------------
//	Vehicle 상태
//-----------------------------------------------------
struct EWRVehicleState
{
	enum en
	{
		Creating	= 0,		//	생성중

		Stop		= 1,		//	정지 상태 - 목적지 도착
		Open		= 2,		//	문 열기		(문이 있는 경우 사용)
		Wait		= 3,		//	탑승 대기
		Close		= 4,		//	문 닫기		(문이 있는 경우 사용)
		Start		= 5,		//	출발		(변속 유형에서 사용)
		Move		= 6,		//	이동
		Break		= 7,		//	제동		(변속 유형에서 사용)

		MAX,

		INVALID = MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Creating,	L"Creating" );
		InsertMap( Stop,		L"Stop" );
		InsertMap( Open,		L"Open" );
		InsertMap( Wait,		L"Wait" );
		InsertMap( Close,		L"Close" );
		InsertMap( Start,		L"Start" );
		InsertMap( Move,		L"Move" );
		InsertMap( Break,		L"Break" );
		InsertMap( INVALID,		L"INVALID" );
	}

	static const bool IsMoving( EWRVehicleState::en vehicleState )
	{
		switch ( vehicleState )
		{
			case EWRVehicleState::Start:
			case EWRVehicleState::Move:
			case EWRVehicleState::Break:
				return true;
		}

		return false;
	}
};



//-----------------------------------------------------
//	액터 패킷 캐스팅 유형
//-----------------------------------------------------
struct EWRActorCastType
{
	enum en
	{
		Sector		= 0,		//	섹터 영역에서 보여짐
		Global		= 1,		//	전체 영역

		MAX,

		INVALID = MAX
	};

	DeclareMapTable();

	static inline void BuildMap()
	{
		InsertMap( Sector,		L"Sector" );
		InsertMap( Global,		L"Global" );
	}
};


//-----------------------------------------------------
//	채팅 타입
//-----------------------------------------------------
struct EWRChatType
{
	enum en
	{
		Normal = 0,
		Secret = 1,		//	섹터 영역에서 보여짐
		Group  = 2,		//	전체 영역
		VideoChat = 3,	//	화상 채팅

		MAX,

		INVALID = MAX
	};

	DeclareMapTable();

	static inline void BuildMap()
	{
		InsertMap(Normal,	L"Normal");
		InsertMap(Secret,	L"Secret");
		InsertMap(Group,	L"Group");
	}

	static bool IsValidType(EWRChatType::en type)
	{
		switch (type)
		{
		case Secret:
		case Group:
			break;
		default:
			return false;
		}

		return true;
	}
};


//-----------------------------------------------------
//	ActionProp 상태
//-----------------------------------------------------
struct EWRAPState
{
	enum en
	{
		Close = 0,				// 최초 비활성화 상태
		Open,					// 활성화된 상태

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Close,		L"Close" );
		InsertMap( Open,		L"Open" );
	}
};


