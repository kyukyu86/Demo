//******************************************************************************
//	FileName: 	Define_Battle.h
//------------------------------------------------------------------------------
//	Created	:	2019/08/30
//------------------------------------------------------------------------------
//	Author	:	shyoon
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	
//******************************************************************************


#pragma once
#include "../Protocol/IMMessage.h"
#include "Define_Type.h"
#include "Define_BluePrint.h"


// 스킬 종류
struct EWRSkillUseType
{
	enum en
	{
		Normal,					// 일반적인 공격
		Avoid,					// 회피
		Casting,				// 캐스팅이 끝나면 발동스킬을 시전하는것.
		Counter,				// 반격
		Dash,					// 대쉬
		Fire,					// 차지나 캐스팅 후 나가는 스킬
		Friendly,				// 우리편 대상으로?
		Groggy,					// 그로기
		Hold,					// 잡기
		Move,					// 이동
		SeqCasting,				// 캐스팅에 레벨이 존재하여 시간이 지나면 다음 캐스팅 레벨로 진행하는것
		SkillMoving,			// 이동하면서 사용가능한 스킬 -> Move로 통합할것
		SuicideBomb,			// 자살 공격
		MultiShot,				// 멀티샷
		Chain,					// 체인라이트닝
		Party,					// 파티 스킬
		FellowSkill,			// 펠로우 스킬
		Summon,					// 스킬로 actor 소환
		ComboSO,				// 미술사 등의 스킬중 SO 연속소환 기능
		FireBall,				// 마법사 파이어볼

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Normal,				L"normal");
		InsertMap(Avoid,				L"Avoid");
		InsertMap(Casting,				L"Casting");
		InsertMap(Counter,				L"Counter");
		InsertMap(Dash,					L"Dash");
		InsertMap(Fire,					L"Fire");
		InsertMap(Friendly,				L"Friendly");
		InsertMap(Groggy,				L"groggy");
		InsertMap(Hold,					L"Hold");
		InsertMap(Move,					L"Move");
		InsertMap(SeqCasting,			L"SeqCasting");
		InsertMap(SkillMoving,			L"SkillMoving");
		InsertMap(SuicideBomb,			L"SuicideBomb");
		InsertMap(MultiShot,			L"multishot");
		InsertMap(Chain,				L"chain");
		InsertMap(Party,				L"party");
		InsertMap(FellowSkill,			L"fellowskill");
		InsertMap(Summon,				L"Summon");
		InsertMap(ComboSO,				L"ComboSO");
		InsertMap(FireBall,				L"FireBall");
	}
};

struct EWRSkillAnimType
{
	enum en
	{
		Attack = 0,
		Skill = 1,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Attack, L"attack");
		InsertMap(Skill, L"skill");
	}
};

struct EWRSkillRangeType
{
	enum en
	{
		Melee,		// 근접
		Range,		// 원거리
		Move,		// player skill에 포함되어있는 값이지만 현재 사용처 불분명 2017.09.12
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Melee, L"Melee");
		InsertMap(Range, L"Range");
		InsertMap(Move, L"Move");
	}
};

// 스킬 발동 대상 종류
struct EWRDoActorType
{
	enum en
	{
		Driver,			// 탑승자
		Vehicle,		// 펠로우
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Driver, L"Driver");
		InsertMap(Vehicle, L"Vehicle");
	}
};

// 스킬 슬롯 정의
struct EWRSkillSlotType
{
	enum en
	{
		None					= 0x00000000,
		Normal					= 0x00000001,	// 1,		// 기본 공격 스킬

		Skill_1					= 0x00000002,	// 2,		// 1번 스킬
		Skill_2					= 0x00000004,	// 4,		// 2번 스킬
		Skill_3					= 0x00000008,	// 8,		// 3번 스킬

		Party					= 0x00000010,	// 16,		// 파티 스킬
		Sequence				= 0x00000020,	// 32,
		Conditional				= 0x00000040,	// 64,

		Avoid					= 0x00000080,	// 128,		// 회피 스킬
		Awaken					= 0x00000100,	// 256,		// 각성 스킬
		FellowSkill				= 0x00000200,	// 512,		// 동행 펠로우 스킬

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,					L"None");
		InsertMap(Normal,				L"Normal");

		InsertMap(Skill_1,				L"Skill_1");
		InsertMap(Skill_2,				L"Skill_2");
		InsertMap(Skill_3,				L"Skill_3");

		InsertMap(Party,				L"Party");
		InsertMap(Sequence,				L"Sequence");
		InsertMap(Conditional,			L"Conditional");

		InsertMap(Avoid,				L"Avoid");
		InsertMap(FellowSkill,			L"FellowSkill");
	}
};

// 스킬 ID( 0-3 : 공격 / 4:회피 / 5:대쉬 )
struct EWRSkillIndex
{
	enum en
	{
		ATTACK_0,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
		AVOID,
		DASH,

		MAX,
	};
};

// 스킬 ActionMove 종류
struct EWRActionMoveType
{
	enum en
	{
		Move_by_dist,           // 데이터에 따른 목표점으로 이동
		Away_from_target,		// 공격대상으로부터 데이터만큼 떨어진 곳으로 이동
		Move_by_jump,		    // 점프처럼 이동( 최종 목표 지점은 바닥!! )
		Floating_In_Air,		// Move_by_jump 처럼 이동하되, 2번째 구간에서는 공중에 머물러 있다가 다시 떨어짐
		Move_Position,			// 해당 위치로 즉시 이동

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Move_by_dist, L"Move_by_dist");
		InsertMap(Away_from_target, L"Away_from_target");
		InsertMap(Move_by_jump, L"Move_by_jump");
		InsertMap(Floating_In_Air, L"Floating_In_Air");
		InsertMap(Move_Position, L"Move_Position");
	}
};

// 리액션이냐 액션이냐 구분

struct EWRReactionType
{
//#ifdef USE_CLIENT_ONLY
	enum en : uint8
//#else
//	enum en
//#endif // USE_CLIENT_ONLY
	{
		None,
		ActionMove,
		ReactionMove,
		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(ActionMove, L"ActionMove");
		InsertMap(ReactionMove, L"ReactionMove");
	}
};


// 스킬 ActionMove 이동방향 종류
struct EWRActionMoveDirectionType
{
	enum en
	{
		ToDefencer,		// 피격자에 대한 방향으로 이동
		Forward,		// 공격자의 정면 방향으로 이동
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(ToDefencer, L"ToDefencer");
		InsertMap(Forward, L"Forward");
	}
};

// 스킬 대상 종류
struct EWRSkillTargetType
{
	enum en
	{
		None,           // 대상없음
		Caster,			// 시전자
		Target,		    // 대상자
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Caster, L"Caster");
		InsertMap(Target, L"Target");
	}
};

struct EWRSKillObjectCreateTime
{
	enum en
	{
		None,
		AllHit,
		FirstHit,
		LastHit,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(AllHit, L"AllHit");
		InsertMap(FirstHit, L"FirstHit");
		InsertMap(LastHit, L"LastHit");
	}
};

// table에 있는 value_a, value_b 를 쉽게 계산하기 위한 함수
template< typename T >
T calc_a_b_value(T value_a, T value_b, LEVEL_t lv)
{
	return value_a + ( value_b *lv );
}

// 스킬 Param 구조체
struct STBuffParamInfo
{
	STBuffParamInfo()
	{
		Type = EWRParamType::None;
		ParamValue = 0;
	}

#ifdef USE_SERVER_ONLY
	EWRParamType::en	Type;        // Param 종류
#else//USE_SERVER_ONLY
	EWRParamType		Type;        // Param 종류
#endif//USE_SERVER_ONLY
	int32				ParamValue;      // 세팅할 값	

	void Reset()
	{
		Type = EWRParamType::None;
		ParamValue = 0;
	}
};

struct EWRChangeGroup
{
	enum en
	{
		NONE,			//같이 묶인 그룹아이디 없음
		MAX,
	};
};

struct EWRConditionalType
{
	enum en
	{
		NONE,
		HP,						// hp 채크 하는 스킬 이라면 : 일정 HP 이하면... 
		BUFF,					// 2번째 값이 buff tid => 해당 버프로 발동되는 스킬이 성공하면 해당 버프를 삭제
		RACT,					// 2번째 값이 ract tid => 
		USECHECK,				// 2번째 스킬tid, 3번째 횟수, 4번째 스택 초기화 시간, 

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(NONE, L"None");
		InsertMap(HP, L"HP");
		InsertMap(BUFF, L"BUFF");
		InsertMap(RACT, L"RACT");
		InsertMap(USECHECK, L"USECHECK");
	}
};

struct EWRConditionInfoIndex
{
	enum en
	{
		Useable,
		SlotType,
		MAX,
	};
};

struct STCondionalSkill
{
	EWRConditionalType::en	ConditionType;
	int32					ConditionValue1;
	int32					ConditionValue2;
	int32					ConditionValue3;

	STCondionalSkill() : ConditionType(EWRConditionalType::NONE), ConditionValue1(0), ConditionValue2(0), ConditionValue3(0)
	{}
};

struct EWRSkillBonusType
{
	enum en
	{
		NONE,
		HP,
		SP,
		CRIT,
		SPD,

		EGRGY,
		ESP,
		ESPD,
		ECRIT,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(NONE, L"None");
		InsertMap(HP, L"HP");
		InsertMap(SP, L"SP");
		InsertMap(CRIT, L"CRIT");
		InsertMap(SPD, L"SPD");

		InsertMap(EGRGY, L"EGRGY");
		InsertMap(ESP, L"ESP");
		InsertMap(ESPD, L"ESPD");
		InsertMap(ECRIT, L"ECRIT");
	}
};

struct STSkillBonusParam
{
	EWRSkillBonusType::en	BonusParamType;
	int32					BonusValue;
};

struct EWRSkillCombatType
{
	enum en
	{
		GROUND,
		FLY,
		ALL,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(GROUND, L"Ground");
		InsertMap(FLY, L"Fly");
		InsertMap(ALL, L"All");
	}
};

namespace EWRAIMode
{
	enum Type : uint8
	{
		Manual,
		Semi_Auto,
		Full_Auto,
		Quest_Auto,

		MAX,
	};
};

struct EWRAISkillType
{
	enum en
	{
		NONE,
		EnemyOverHP,	// 적군의 HP 가 value 보다 클 경우
		EnemyLessHP,	// 적군의 HP 가 value 보다 작을 경우
		OwnerOverHP,	// 자신의 HP 가 value 보다 클 경우
		OwnerLessHP,	// 자신의 HP 가 value 보다 작을 경우

		EnemyLessCount,	// 스킬범위(skill range) 안의 적군의 수가 value 보다 작으면 사용
		EnemyOverCount,	// 스킬범위(skill range) 안의 적군의 수가 value 보다 크면 사용
		EnemyLessRange,	// 타겟이 value 보다 가까이 있으면 사용
		EnemyOverRange, // 타겟이 value 보다 멀면 사용

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(NONE, L"None");

		InsertMap(EnemyOverHP, L"EnemyOverHP");
		InsertMap(EnemyLessHP, L"EnemyLessHP");
		InsertMap(OwnerOverHP, L"OwnerOverHP");
		InsertMap(OwnerLessHP, L"OwnerLessHP");

		InsertMap(EnemyLessCount, L"EnemyLessCount");
		InsertMap(EnemyOverCount, L"EnemyOverCount");
		InsertMap(EnemyLessRange, L"EnemyLessRange");
		InsertMap(EnemyOverRange, L"EnemyOverRange");
	}
};

struct STAISkillParam
{
	STAISkillParam()
	{
		ParamType = EWRAISkillType::NONE;
		CheckValue = 0;
	}

	EWRAISkillType::en		ParamType;
	int32					CheckValue;
};

// 스킬 대상 타입
struct EWRTargetType
{
	enum en
	{
		NONE,			
		PARTY,
		GUILD,
		ENEMY,
		SELF,
		GROUP,
		ALLY,
		GROUPMASTER,
		GROUPSLAVE,
		FACTION_A,
		FACTION_B,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(NONE, L"None");
		InsertMap(PARTY, L"Party");
		InsertMap(GUILD, L"Guild");
		InsertMap(ENEMY, L"Enemy");
		InsertMap(SELF, L"Self");
		InsertMap(GROUP, L"GROUP");
		InsertMap(ALLY, L"ALLY");
		InsertMap(GROUPMASTER, L"GROUPMASTER");
		InsertMap(GROUPSLAVE, L"GROUPSLAVE");
		InsertMap(FACTION_A, L"FactionA");
		InsertMap(FACTION_B, L"FactionB");
	}
};

struct EWRAvoidDirection
{
	enum en
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(FORWARD, L"Forward");
		InsertMap(BACKWARD, L"Backward");
		InsertMap(LEFT, L"Left");
		InsertMap(RIGHT, L"Right");
	}
};

// 스킬 정보
struct stSkillInfo : IMSerialize
{
	TID_t							skillTid;		// 스킬 TID (SkillPlayer_Atk.xml의 tid참조)
	SKILL_LEVEL_t					skillLevel;		// 스킬 레벨

	stSkillInfo()
		: skillTid(0)
		, skillLevel(0)
	{}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> skillTid >> skillLevel;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << skillTid << skillLevel;
	}

	void	Copy(stSkillInfo* pPlayerInfoSrc)
	{
		skillTid = pPlayerInfoSrc->skillTid;
		skillLevel = pPlayerInfoSrc->skillLevel;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"skillTid : %d, skillLevel : %u", skillTid, skillLevel);
	}
#endif	// USE_PROTOCOL_TOSTRING
};

// 장착 스킬 정보
struct stEquipSkillInfo : IMSerialize
{
	std::list<TID_t>	skillTidList;		// EWRSkillSlotType::Skill_1 부터시작하는 Tid 리스트 - 장착한 스킬 TID 리스트 (SkillPlayer_Atk.xml의 tid참조)

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> skillTidList;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << skillTidList;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_LIST_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, skillTidList);
	}
#endif	// USE_PROTOCOL_TOSTRING
};

struct stSkillSetPageInfo : IMSerialize
{
	std::list<stSkillInfo>		skillList;			// 스킬 정보 리스트
	std::list<TID_t>			groundEquipSkill;	// 지상 장착 스킬 정보 리스트
	std::list<TID_t>			flyEquipSkill;		// 공중 장착 스킬 정보 리스트

	stSkillSetPageInfo()
	{
		Clear();
	}

	void Clear()
	{
		skillList.clear();
		groundEquipSkill.clear();
		flyEquipSkill.clear();
	}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> skillList >> groundEquipSkill >> flyEquipSkill;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << skillList << groundEquipSkill << flyEquipSkill;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_PROTOCOL_NAME_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"pt_Game_User::InformEntryInfoSkillList_FromGame");
		ADD_STRUCT_NAME_AND_ELEMENTS_LIST_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, skillList, L"stSkillInfo");
		ADD_ELEMENTS_LIST_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, groundEquipSkill);
		ADD_ELEMENTS_LIST_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, flyEquipSkill);
	}
#endif	// USE_PROTOCOL_TOSTRING
};

// 스킬 레벨업 정보
struct stSkillLevelUpInfo : IMSerialize
{
	TID_t							skillTid;			// 스킬 TID (SkillPlayer_Atk.xml의 tid참조)		<ex> 4001 </ex>
	int8							increaseLevel;		// 증가 레벨									<ex> 3 </ex>
		
	stSkillLevelUpInfo() 
		: skillTid(INVALID_TID)
		, increaseLevel(0)
	{}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> skillTid >> increaseLevel;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << skillTid << increaseLevel;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"skillTid : %d, increaseLevel : %d", skillTid, increaseLevel);
	}
#endif	// USE_PROTOCOL_TOSTRING
};

// 스킬 레벨업 시 DB로 넘기는 정보
struct stSkillLevelUpInfoDB
{
	TID_t	skillTID;
	int8	increaseLevel;
	int16	use_skill_point;
	stSkillLevelUpInfoDB() : skillTID(INVALID_TID), increaseLevel(INVALID_LEVEL), use_skill_point(0)
	{}
};

// 스킬 세트 페이지 정의
struct EWRSkillSetPageType
{
	enum en
	{
		First	= 0,	// 1페이지
		Second	= 1,	// 2페이지
		Third	= 2,	// 3페이지

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(First, L"First");
		InsertMap(Second, L"Second");
		InsertMap(Third, L"Third");
	}
};

//스킬 hit 타입
struct EWRSkillHitType
{
	enum en
	{
		None,
		Normal,	// 일반 히트처리
		Miss,	// 미스
		Inv,	// 무적
		Guard,	// 가드
		Reflect,// 반사
		ReactionImune,	// 리액션 면역
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Normal, L"Normal");
		InsertMap(Miss, L"Miss");
		InsertMap(Inv, L"Inv");
		InsertMap(Guard, L"Guard");
		InsertMap(Reflect, L"Reflect");
		InsertMap(ReactionImune, L"ReactionImune");
	}
};

// 어떤 스킬을 특별한 타겟을 대상으로 사용하고 싶을때 
struct EWRTargetParam 
{
	enum en
	{
		None,
		FarthestRange,
		NearestRange,
		HighestHP,
		LowestHP,
		Random,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"*");
		InsertMap(FarthestRange, L"FarRange");
		InsertMap(NearestRange, L"NearRange");
		InsertMap(HighestHP, L"HighHP");
		InsertMap(LowestHP, L"LowHP");
		InsertMap(Random, L"Random");
	}
};

struct EWRStatRefType
{
	enum en
	{
		Owner,
		Self,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Owner, L"Owner");
		InsertMap(Self, L"Self");
	}
};

struct EWRBuffApplyType
{
	enum en
	{
		None,
		First,
		AllHit,
		LastHit,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"*");
		InsertMap(First, L"First");
		InsertMap(AllHit, L"AllHit");
		InsertMap(LastHit, L"LastHit");
	}

};


// 그로기 봉인 타입
struct EWRBubbleType
{
	enum en : uint8
	{
		None,
		Red,
		Blue,
		Green,
		Yellow,
		Purple,
		Rainbow,

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"*");
		InsertMap(Red, L"Red");
		InsertMap(Blue, L"Blue");
		InsertMap(Green, L"Green");
		InsertMap(Yellow, L"Yellow");
		InsertMap(Purple, L"Purple");
		InsertMap(Rainbow, L"Rainbow");
	}

};


// npc 사망 시 버프 부여
struct EWRDieBuffTargetType
{
	enum en : uint8 
	{
		None = 0,
		LastAtk,
		AggroList,
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"*");
		InsertMap(LastAtk, L"LastAtk");
		InsertMap(AggroList, L"AggroList");
	}
};

// 데미지 타입.
struct EWRDamageType
{
	enum en : uint8
	{
		Normal,
		Critical_WithoutEement,		// 치명타-속성공격 없음
		Penetrate_WithoutEement,	// 방어무시-속성공격 없음
		Critical_WithEement,		// 치명타-속성공격 포함
		Penetrate_WithEement,		// 방어무시-속성공격 포함
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Normal, L"Normal");
		InsertMap(Critical_WithoutEement, L"Critical_WithoutEement");
		InsertMap(Penetrate_WithoutEement, L"Penetrate_WithoutEement");
		InsertMap(Critical_WithEement, L"Critical_WithEement");
		InsertMap(Penetrate_WithEement, L"Penetrate_WithEement");
	}

	static EWRDamageType::en GetDamageType(bool bCritical, bool bPenetrate, bool bElement)
	{
		if (bElement)
		{
			if (bCritical)
				return EWRDamageType::Critical_WithEement;
			if (bPenetrate)
				return EWRDamageType::Penetrate_WithEement;
		}
		else
		{
			if (bCritical)
				return EWRDamageType::Critical_WithoutEement;
			if (bPenetrate)
				return EWRDamageType::Penetrate_WithoutEement;
		}

		return EWRDamageType::Normal;
	}
};

// 힐 타입.
// [Kyoungha] : 2018/04/14 ] : 힐타입은 따로 정의되지 않아 일단 클라이언트에서만 정의해서 사용
//								서버와 공용으로 필요해질 경우 디파인 풀어서 사용!
#ifdef USE_CLIENT_ONLY
struct EWRHealType
{
	enum en : uint8
	{
		Dot,			// 도트힐
		Vampiric,		// 흡혈
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Dot, L"Dot");
		InsertMap(Vampiric, L"Vampiric");
	}
};
#endif // USE_CLIENT_ONLY

struct EWRFellowSkillAutoType
{
	enum en
	{
		None,
		Manual,			// 자동 사용 하지 않음.
		Auto,			// 무조건 자동 사용
		PurifyAuto,		// 특정 조건 하에 자동 사용.
		Heal,			// 힐전용 고유 룰에 따라 사용.
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Manual, L"Manual");
		InsertMap(Auto, L"Auto");
		InsertMap(PurifyAuto, L"PurifyAuto");
		InsertMap(Heal, L"Heal");
	}
};

struct EWRStatLinkType
{
	enum en
	{
		None,
		SkillObject,		// 스텟 링크 스킬
		Buff,				// 스텟 링크 버프
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(SkillObject, L"SkillObject");
		InsertMap(Buff, L"Buff");
	}
};

struct EWRPriestSkillMotesType
{
	enum en
	{
		None,
		ChargeMotes1,	//징표 1개 생성
		ChargeMotes2,	//징표 2개 생성
		ChargeMotes3,	//징표 3개 생성
		MaxChargeMotes = ChargeMotes3,	//최대 징표 개수
		ConsumeMotes,	//징표를 소모하는 스킬

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(ChargeMotes1, L"ChargeMotes1");
		InsertMap(ChargeMotes2, L"ChargeMotes2");
		InsertMap(ChargeMotes3, L"ChargeMotes3");
		InsertMap(ConsumeMotes, L"ConsumeMotes");
	}
};


#define GROGGY_BUBBLE_COUNT_NORMAL					3				// 보스의 그로기 봉인 구슬 개수 설정 디폴트
#define GROGGY_BUBBLE_COUNT_PARTY					5				// 보스의 그로기 봉인 구슬 개수 설정 파티맵

#define AVOID_GAUGE_MAX								100.0f			// 방어 게이지 최대값

#define AVOID_GAUGE_INC_PER_CHECK					14				// 한번 채크할 때 마다 채워주는 값 14
#define AVOID_GAUGE_DEC								20.0f			// 회피 한번당 사용하는 방어게이지 값
#define AVOID_GAUGE_CHECK_SEC						2				// 2초에 한번채크

#define AVOID_GAUGE_RECOVER_CHECK_TIME_MS			2000			// 2초에 대한 ms 시간 -> update 때마다 확인하므로 디파인처리
#define AVOID_GAUGE_COOL_TIME_INIT_TICK				0				// 방어 게이지 쿨타임의 초기값

#define FIRST_BUBBLE_TYPE							1				// 첫번째 버블 타입 인덱스 값
#define LAST_BUBBLE_TYPE							5				// 마지막 버블 타입 인덱스 값

#define SKILL_OBJECT_LIFE_TIME_UNLIMIT				-1				// 스킬오브젝트가 사라지지 않고 계속 존재

#define LEVEL_UP_SKILL_TID							9001

#define BUFF_UNLIMITED_REMAIN_TIME					-1				// 버프가 무한 지속
#define FLOOR_HIGHT_CM								1000			// 층 구분 z 거리 10m

#define MOB_GROGGY_BUFF_TID							2007000
#define NOT_REACTION_BUFF_TID						2009500
#define NEXT_DEFAULT_SKILL_INDEX					0
#define NEXT_AWAKEN_SKILL_INDEX						0
#define NEXT_NONE_AWAKEN_SKILL_INDEX				1
#define DISTANCE_2_BACKWARD_MOVE_WHEN_TARGET_ENPC	80.0f
#define DISTANCE_FROM_ACTOR_NO_TARGET_SKILL_OBJECT	500				// 타겟이 없는 스킬 오브젝트가 생성될 거리 (방향은 플레이어가 바라보는 위치)
#define DEF_HIGHEST_SKILL_COOLTIME					999				// 최초 공격상태로 들어갈때 쿨타임에 넣지 않고 나중에 넣게 되는 스킬에 대한 예외처리

//
//struct stSkillPoint : IMSerialize
//{
//	std::array<SKILL_POINT_t, EWRSkillSetPageType::MAX>		skillPointList;			// 스킬 포인트 리스트
//
//	stSkillPoint()
//	{
//		Clear();
//	}
//
//	void Clear()
//	{
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr)
//			*itr = 0;
//	}
//
//	IMMessage& ReadMsg(IMMessage& rMsg) override
//	{
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr)
//			rMsg >> *itr;
//		return rMsg;
//	}
//	IMMessage& WriteMsg(IMMessage& rMsg) override
//	{
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr)
//			rMsg << *itr;
//		return rMsg;
//	}
//
//	SKILL_POINT_t& operator [] (EWRSkillSetPageType::en page)
//	{
//		return skillPointList[page];
//	}
//
//	const stSkillPoint operator + (SKILL_POINT_t skillPoint)
//	{
//		stSkillPoint resultSkillPoint = *this;
//
//		for (auto itr = resultSkillPoint.skillPointList.begin(); itr != resultSkillPoint.skillPointList.end(); ++itr)
//			*itr += skillPoint;
//
//		return resultSkillPoint;
//	}
//
//	const stSkillPoint& operator += (SKILL_POINT_t skillPoint)
//	{
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr)
//			*itr += skillPoint;
//
//		return *this;
//	}
//
//	const stSkillPoint& operator = (const stSkillPoint& lhs)
//	{
//		auto itrLhs = lhs.skillPointList.begin();
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr, ++itrLhs)
//			*itr = *itrLhs;
//
//		return *this;
//	}
//
//	const stSkillPoint operator + (const stSkillPoint& lhs)
//	{
//		stSkillPoint resultSkillPoint = *this;
//
//		auto itrLhs = lhs.skillPointList.begin();
//		for (auto itr = resultSkillPoint.skillPointList.begin(); itr != resultSkillPoint.skillPointList.end(); ++itr, ++itrLhs)
//			*itr += *itrLhs;
//
//		return resultSkillPoint;
//	}
//
//	const stSkillPoint& operator += (const stSkillPoint& lhs)
//	{
//		auto itrLhs = lhs.skillPointList.begin();
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr, ++itrLhs)
//			*itr += *itrLhs;
//
//		return *this;
//	}
//
//	SKILL_POINT_t GetMax()
//	{
//		SKILL_POINT_t max = 0;
//		for (auto itr = skillPointList.begin(); itr != skillPointList.end(); ++itr)
//		{
//			SKILL_POINT_t point = *itr;
//			if (point > max)
//				max = point;
//		}
//
//		return max;
//	}
//
//#ifdef USE_PROTOCOL_TOSTRING
//	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
//	{
//		ADD_PROTOCOL_NAME_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"pt_Game_User::InformEntryInfoSkillList_FromGame");
//		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"skillPoint1 : %d, skillPoint2 : %d, skillPoint3 : %d", 
//			skillPointList[EWRSkillSetPageType::First], skillPointList[EWRSkillSetPageType::Second], skillPointList[EWRSkillSetPageType::Third]);
//	}
//#endif	// USE_PROTOCOL_TOSTRING
//};

// 버프 정보
struct stBuffInfo : IMSerialize
{
	stBuffInfo()
	{
		buffTid = INVALID_TID;
		skillSeq = INVALID_SKILL_SEQ;
		level = 0;
		stack = 0;
		remain_tm = 0;
#ifdef DEF_PRIEST_MOTES_BY_AUTULN_190102
		buff_tm_run = true;
#endif //DEF_PRIEST_MOTES_BY_AUTULN_190102
		caster_aid = INVALID_AID;
		isActiveBuff = false;
	}
	TID_t		buffTid;		// 버프 TID (SkillEffectParam_Buff.xml의 tid 참조)
	SKILL_SEQ_t skillSeq;		// 버프를 생성한 스킬 uid
	uint8		level;
	uint8		stack;			// 누적된 버프 개수 (일부 버프는 여러번 누적가능)
	int32		remain_tm;		// 남아있는 버프 시간
#ifdef DEF_PRIEST_MOTES_BY_AUTULN_190102
	bool		buff_tm_run;	// 버프 시간 상태(true : 흐름, false : 정지)
#endif //DEF_PRIEST_MOTES_BY_AUTULN_190102
	AID_t		caster_aid;		// 시전자 Actor ID (서버에서 생성한 키값)
	bool		isActiveBuff;	// 버프가 active(활성) 상태인가

#ifdef DEF_PRIEST_MOTES_BY_AUTULN_190102
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> buffTid >> skillSeq >> level >> stack >> remain_tm >> buff_tm_run >> caster_aid >> isActiveBuff;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << buffTid << skillSeq << level << stack << remain_tm << buff_tm_run << caster_aid << isActiveBuff;
	}
#else //DEF_PRIEST_MOTES_BY_AUTULN_190102
	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> buffTid >> skillSeq >> level >> stack >> remain_tm >> caster_aid >> isActiveBuff;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << buffTid << skillSeq << level << stack << remain_tm << caster_aid << isActiveBuff;
	}
#endif //DEF_PRIEST_MOTES_BY_AUTULN_190102
#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
#ifdef DEF_PRIEST_MOTES_BY_AUTULN_190102
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"buffTid : %d, level :%d, stack : %u, remain_tm : %d, buff_tm_run : %d, caster_aid : %u, isActiveBuff : %d"
			, buffTid, level, stack, remain_tm, buff_tm_run, caster_aid, isActiveBuff);
#else //DEF_PRIEST_MOTES_BY_AUTULN_190102
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"buffTid : %d, level :%d, stack : %u, remain_tm : %d, caster_aid : %u, isActiveBuff : %d"
			, buffTid, level, stack, remain_tm, caster_aid, isActiveBuff);
#endif //DEF_PRIEST_MOTES_BY_AUTULN_190102
	}
#endif	// USE_PROTOCOL_TOSTRING
};



struct stHookInfo : IMSerialize
{
	AID_t		casterAID;			// 갈고리 사용 액터 아이디
	int32		collisionType;		// 어디를 걸었는 지
	SKILL_SEQ_t skillSeq;			// 스킬 시퀀스
	TIME_t		endTick;			// 종료 시점
	TID_t		buffTID;			// 연결 끊어졌을 경우 해당 버프 삭제

	stHookInfo() : casterAID(INVALID_AID), collisionType(0), skillSeq(INVALID_SKILL_SEQ), endTick(0), buffTID(INVALID_TID)
	{
	}

	IMMessage& ReadMsg(IMMessage& rMsg) override
	{
		return rMsg >> casterAID >> collisionType;
	}
	IMMessage& WriteMsg(IMMessage& rMsg) override
	{
		return rMsg << casterAID << collisionType;
	}

#ifdef USE_PROTOCOL_TOSTRING
	void ToString(std::deque<std::wstring>& rStrPacketMsgList, std::wstring& rStrMsg) override
	{
		ADD_ELEMENTS_TO_LOG_LIST(rStrPacketMsgList, rStrMsg, L"collisionType :%d, casterAID : %u", collisionType, casterAID);
	}
#endif	// USE_PROTOCOL_TOSTRING
};
