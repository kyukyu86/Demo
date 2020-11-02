//******************************************************************************
//	FileName: 	Define_Map.h
//------------------------------------------------------------------------------
//	Created	:	2019/08/29
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	맵 관련 정보
//******************************************************************************


#pragma once


//-----------------------------------------------------
//	로드 밸런싱용 - 서버/클라 공용
//-----------------------------------------------------
#define MAX_BASE_STATIC_CHANNEL_COUNT		3							//	기본 생성되어야 하는 정적 맵 개수

#define MAX_ENTRANCE_SEAT_RATE				0.7f						//	입장 가능 정족수

#define DEFALUT_ENTRANCE_INSTANCE_MAP_USER	10							//	인던 디폴트 유저 입장수(기획 데이터 max_user_num 필요)
#define CHANNEL_CONGESTION_TABLE			5

#define SHAPE_RADIUS						0.25f
#define UNIT_FACTOR							(1.0f)
#define TO_GAME_COORD(X)					((X)/UNIT_FACTOR+0.0002f)	//	정수와 실수간의 오차를 보정하기 위하여 0.0002f 를 더함
#define TO_PATH_COORD(X)					((long)((X)*UNIT_FACTOR))
#define DUPLEX_FLOOR_HEIGHT_LIMIT			(5.0f)						//	복층 판단 높이

#define	START_MAP_TID						30000001					//	최초 시작맵(엘로라신전) ... 오류시 사용

#define MOVE_POS_RANDOM_RANGE				100.0f						//	이동 위치 랜덤 반경 => 데이빗 요청으로 200->100 으로 변경 2018-12-17

#define MAX_MAP_RANDOM_START_POS			50							//	미리 로드 해놓는 랜덤 좌표 개수
#define MAX_SPAWN_RANDOM_START_POS			20							//  미리 로드 해놓는 스폰위치 기준 랜덤 좌표 개수
#define SPAWN_RANDOM_START_POS_RADIUS		100							//  미리 로드 해놓는 스폰위치 기준 랜덤 좌표 반경


//-----------------------------------------------------
//	맵 유형에 따른 사망 유형 분류
//-----------------------------------------------------
struct EWRMapDeathRuleType
{
	enum en
	{
		None = 0,

		DeathRule_1 = 1,
		DeathRule_2,
		DeathRule_3,
		DeathRule_4,

		MAX,
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( DeathRule_1, L"DeathRule_1" );
		InsertMap( DeathRule_2, L"DeathRule_2" );
		InsertMap( DeathRule_3, L"DeathRule_3" );
		InsertMap( DeathRule_4, L"DeathRule_4" );
	}
};


//-----------------------------------------------------
// 맵 종류
//-----------------------------------------------------
struct EWRMapType
{
	enum en
	{
		None = 0,

		Lab,								// 연구소
		Town,								// 기본 마을
		Field,								// 필드
		Scenario,							// 시나리오 미션 필드
		Stage,								// 협동 미션 필드
		BattleField,						// PVP 가능 맵

		MAX,
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Lab,						L"Lab" );
		InsertMap( Town,					L"Town" );
		InsertMap( Field,					L"Field" );
		InsertMap( Scenario,				L"Scenario" );
		InsertMap( Stage,					L"Stage" );
		InsertMap( BattleField,				L"BattleField" );
	}
	
	//	타인을 죽일 수 있는 맵인가
	static const bool IsEnablePKMap( const EWRMapType::en enMapType )
	{
		switch (enMapType)
		{
			case EWRMapType::Scenario:		return true;
			case EWRMapType::Stage:			return true;
			case EWRMapType::BattleField:	return true;
		}

		return false;
	}

	//	인스턴스 던전 체크
	static const bool IsInstanceMap( const EWRMapType::en enMapType )
	{
		switch ( enMapType )
		{
			case EWRMapType::Lab:			return false;
			case EWRMapType::Town:			return false;
			case EWRMapType::Field:			return false;
			case EWRMapType::Scenario:		return true;
			case EWRMapType::Stage:			return true;
			case EWRMapType::BattleField:	return true;
		}

		return false;
	}

	//	부활 가능 체크
	static const bool IsEnableReviveMap( const EWRMapType::en enMapType )
	{
		switch ( enMapType )
		{
			case EWRMapType::Lab:			return true;
			case EWRMapType::Town:			return true;
			case EWRMapType::Field:			return true;
			case EWRMapType::Scenario:		return true;
			case EWRMapType::Stage:			return true;
			case EWRMapType::BattleField:	return true;
		}

		return false;
	}

};


//-----------------------------------------------------
// 맵 위치 유형
//-----------------------------------------------------
struct EWRMapPosType
{
	enum en
	{
		Start		= 0,			// 시작점
		Revive		= 1,			// 부활
		Exit		= 2,			// 나가기

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Start,		L"Start" );
		InsertMap( Revive,		L"Revive" );
		InsertMap( Exit,		L"Exit" );
	}
};


//-----------------------------------------------------
// 맵에 배치되는 Object - Actor는 제외
//-----------------------------------------------------
struct EWRObjectType
{
	enum en
	{
		None				= 0,			// 초기값
		TriggerObject		= 1,			// 트리거 관련 Object
		Brick				= 2,			// 방벽

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,			L"None" );
		InsertMap( TriggerObject,	L"TriggerObject" );
		InsertMap( Brick,			L"Brick" );
	}
};


//-----------------------------------------------------
//	채널 혼잡도 상태
//-----------------------------------------------------
struct EWRCongestionState
{
	enum en
	{
		None				= 0,			//	비어있음
		Light				= 1,			//	원활
		Normal				= 2,			//	혼잡
		Busy				= 3,			//	초과

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,			L"None" );
		InsertMap( Light,			L"Light" );
		InsertMap( Normal,			L"Normal" );
		InsertMap( Busy,			L"Busy" );
	}

	//	혼잡도 체크
	static const EWRCongestionState::en GetDecideCongestionState(	const uint32 maxPlayerCount,
																	const uint32 entranceMaxPlayerCount,
																	const uint32 curPlayerCount )
	{
		if ( 0 == curPlayerCount )
		{
			return EWRCongestionState::None;
		}
		else if ( curPlayerCount >= maxPlayerCount )
		{
			//	가득찬 상태
			return EWRCongestionState::Busy;
		}
		else if ( curPlayerCount < entranceMaxPlayerCount )
		{
			//	유저 선택 입장 가능
			return EWRCongestionState::Light;
		}

		//	파티 합류 등을 통해 입장 가능
		return EWRCongestionState::Normal;
	}
};


//-----------------------------------------------------
//	맵에서 나가는 유형
//-----------------------------------------------------
struct EWRExitMapType
{
	enum en
	{
		Destroy = 0,			//	객체 소멸
		LogOut,					//	유저 로그 아웃
		Duplicate,				//	이미 있는 경우 정리

		MoveToLab,				//	연구소로 이동
		
		//	장비 이동 가능 유형
		Portal,					//	포탈 이동
		Die,					//	객체 죽음
		Channel,				//	채널간 이동
		Cheat,					//	치트
		MoveToMap,				//	맵 이동
		Revive,					//	객체 부활

		MAX
	};
	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( Destroy,		L"Destroy" );
		InsertMap( LogOut,		L"LogOut" );
		InsertMap( Duplicate,	L"Duplicate" );
		InsertMap( MoveToLab,	L"MoveToLab" );
		InsertMap( Portal,		L"Portal" );
		InsertMap( Die,			L"Die" );
		InsertMap( Channel,		L"Channel" );
		InsertMap( Cheat,		L"Cheat" );		
		InsertMap( MoveToMap,	L"MoveToMap" );		
		InsertMap( Revive,		L"Revive" );
		
	}

	//	장비 이동 가능 유형인가
	static const bool IsMoveToMap( const EWRExitMapType::en enExitMapType )
	{
		if ( ( EWRExitMapType::Portal <= enExitMapType ) && ( EWRExitMapType::MAX > enExitMapType ) )
		{
			return true;
		}

		return false;
	}
};


//-----------------------------------------------------
//	던전 난이도 유형
//-----------------------------------------------------
struct EWRDungeonDifficultyType
{
	enum en 
	{
		EASY		= 0,
		NORMAL		= 1,
		HARD		= 2,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( EASY,		L"EASY" );
		InsertMap( NORMAL,		L"NORMAL" );
		InsertMap( HARD,		L"HARD" );
	}
};


//-----------------------------------------------------
//	던전 클리어 등급
//-----------------------------------------------------
struct EWRDungeonClearGrade
{
	enum en
	{
		None	= 0,
		B		= 1,
		A		= 2,
		S		= 3,
		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,	L"None" );
		InsertMap( B,		L"B" );
		InsertMap( A,		L"A" );
		InsertMap( S,		L"S" );
	}
};


//-----------------------------------------------------
// 맵 이동후 펠로우 소환 / 해제 관련된 값
//-----------------------------------------------------
struct EWRArrival_Fellow_SummonType
{
	enum en
	{
		None					= 0,			//초기값

		RidingFellowUnSummon	= 1,			// 탑승 펠로우 소환 해제
		RidingFellowSummon		= 2,			// 탑승 펠로우 소환

		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,					L"None" );
		InsertMap( RidingFellowUnSummon,	L"RidingFellowUnSummon" );
		InsertMap( RidingFellowSummon,		L"RidingFellowSummon" );
	}
};