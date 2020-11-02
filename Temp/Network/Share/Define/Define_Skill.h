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


// ��ų ����
struct EWRSkillUseType
{
	enum en
	{
		Normal,					// �Ϲ����� ����
		Avoid,					// ȸ��
		Casting,				// ĳ������ ������ �ߵ���ų�� �����ϴ°�.
		Counter,				// �ݰ�
		Dash,					// �뽬
		Fire,					// ������ ĳ���� �� ������ ��ų
		Friendly,				// �츮�� �������?
		Groggy,					// �׷α�
		Hold,					// ���
		Move,					// �̵�
		SeqCasting,				// ĳ���ÿ� ������ �����Ͽ� �ð��� ������ ���� ĳ���� ������ �����ϴ°�
		SkillMoving,			// �̵��ϸ鼭 ��밡���� ��ų -> Move�� �����Ұ�
		SuicideBomb,			// �ڻ� ����
		MultiShot,				// ��Ƽ��
		Chain,					// ü�ζ���Ʈ��
		Party,					// ��Ƽ ��ų
		FellowSkill,			// ��ο� ��ų
		Summon,					// ��ų�� actor ��ȯ
		ComboSO,				// �̼��� ���� ��ų�� SO ���Ӽ�ȯ ���
		FireBall,				// ������ ���̾

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
		Melee,		// ����
		Range,		// ���Ÿ�
		Move,		// player skill�� ���ԵǾ��ִ� �������� ���� ���ó �Һи� 2017.09.12
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

// ��ų �ߵ� ��� ����
struct EWRDoActorType
{
	enum en
	{
		Driver,			// ž����
		Vehicle,		// ��ο�
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Driver, L"Driver");
		InsertMap(Vehicle, L"Vehicle");
	}
};

// ��ų ���� ����
struct EWRSkillSlotType
{
	enum en
	{
		None					= 0x00000000,
		Normal					= 0x00000001,	// 1,		// �⺻ ���� ��ų

		Skill_1					= 0x00000002,	// 2,		// 1�� ��ų
		Skill_2					= 0x00000004,	// 4,		// 2�� ��ų
		Skill_3					= 0x00000008,	// 8,		// 3�� ��ų

		Party					= 0x00000010,	// 16,		// ��Ƽ ��ų
		Sequence				= 0x00000020,	// 32,
		Conditional				= 0x00000040,	// 64,

		Avoid					= 0x00000080,	// 128,		// ȸ�� ��ų
		Awaken					= 0x00000100,	// 256,		// ���� ��ų
		FellowSkill				= 0x00000200,	// 512,		// ���� ��ο� ��ų

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

// ��ų ID( 0-3 : ���� / 4:ȸ�� / 5:�뽬 )
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

// ��ų ActionMove ����
struct EWRActionMoveType
{
	enum en
	{
		Move_by_dist,           // �����Ϳ� ���� ��ǥ������ �̵�
		Away_from_target,		// ���ݴ�����κ��� �����͸�ŭ ������ ������ �̵�
		Move_by_jump,		    // ����ó�� �̵�( ���� ��ǥ ������ �ٴ�!! )
		Floating_In_Air,		// Move_by_jump ó�� �̵��ϵ�, 2��° ���������� ���߿� �ӹ��� �ִٰ� �ٽ� ������
		Move_Position,			// �ش� ��ġ�� ��� �̵�

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

// ���׼��̳� �׼��̳� ����

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


// ��ų ActionMove �̵����� ����
struct EWRActionMoveDirectionType
{
	enum en
	{
		ToDefencer,		// �ǰ��ڿ� ���� �������� �̵�
		Forward,		// �������� ���� �������� �̵�
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(ToDefencer, L"ToDefencer");
		InsertMap(Forward, L"Forward");
	}
};

// ��ų ��� ����
struct EWRSkillTargetType
{
	enum en
	{
		None,           // ������
		Caster,			// ������
		Target,		    // �����
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

// table�� �ִ� value_a, value_b �� ���� ����ϱ� ���� �Լ�
template< typename T >
T calc_a_b_value(T value_a, T value_b, LEVEL_t lv)
{
	return value_a + ( value_b *lv );
}

// ��ų Param ����ü
struct STBuffParamInfo
{
	STBuffParamInfo()
	{
		Type = EWRParamType::None;
		ParamValue = 0;
	}

#ifdef USE_SERVER_ONLY
	EWRParamType::en	Type;        // Param ����
#else//USE_SERVER_ONLY
	EWRParamType		Type;        // Param ����
#endif//USE_SERVER_ONLY
	int32				ParamValue;      // ������ ��	

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
		NONE,			//���� ���� �׷���̵� ����
		MAX,
	};
};

struct EWRConditionalType
{
	enum en
	{
		NONE,
		HP,						// hp äũ �ϴ� ��ų �̶�� : ���� HP ���ϸ�... 
		BUFF,					// 2��° ���� buff tid => �ش� ������ �ߵ��Ǵ� ��ų�� �����ϸ� �ش� ������ ����
		RACT,					// 2��° ���� ract tid => 
		USECHECK,				// 2��° ��ųtid, 3��° Ƚ��, 4��° ���� �ʱ�ȭ �ð�, 

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
		EnemyOverHP,	// ������ HP �� value ���� Ŭ ���
		EnemyLessHP,	// ������ HP �� value ���� ���� ���
		OwnerOverHP,	// �ڽ��� HP �� value ���� Ŭ ���
		OwnerLessHP,	// �ڽ��� HP �� value ���� ���� ���

		EnemyLessCount,	// ��ų����(skill range) ���� ������ ���� value ���� ������ ���
		EnemyOverCount,	// ��ų����(skill range) ���� ������ ���� value ���� ũ�� ���
		EnemyLessRange,	// Ÿ���� value ���� ������ ������ ���
		EnemyOverRange, // Ÿ���� value ���� �ָ� ���

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

// ��ų ��� Ÿ��
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

// ��ų ����
struct stSkillInfo : IMSerialize
{
	TID_t							skillTid;		// ��ų TID (SkillPlayer_Atk.xml�� tid����)
	SKILL_LEVEL_t					skillLevel;		// ��ų ����

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

// ���� ��ų ����
struct stEquipSkillInfo : IMSerialize
{
	std::list<TID_t>	skillTidList;		// EWRSkillSlotType::Skill_1 ���ͽ����ϴ� Tid ����Ʈ - ������ ��ų TID ����Ʈ (SkillPlayer_Atk.xml�� tid����)

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
	std::list<stSkillInfo>		skillList;			// ��ų ���� ����Ʈ
	std::list<TID_t>			groundEquipSkill;	// ���� ���� ��ų ���� ����Ʈ
	std::list<TID_t>			flyEquipSkill;		// ���� ���� ��ų ���� ����Ʈ

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

// ��ų ������ ����
struct stSkillLevelUpInfo : IMSerialize
{
	TID_t							skillTid;			// ��ų TID (SkillPlayer_Atk.xml�� tid����)		<ex> 4001 </ex>
	int8							increaseLevel;		// ���� ����									<ex> 3 </ex>
		
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

// ��ų ������ �� DB�� �ѱ�� ����
struct stSkillLevelUpInfoDB
{
	TID_t	skillTID;
	int8	increaseLevel;
	int16	use_skill_point;
	stSkillLevelUpInfoDB() : skillTID(INVALID_TID), increaseLevel(INVALID_LEVEL), use_skill_point(0)
	{}
};

// ��ų ��Ʈ ������ ����
struct EWRSkillSetPageType
{
	enum en
	{
		First	= 0,	// 1������
		Second	= 1,	// 2������
		Third	= 2,	// 3������

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

//��ų hit Ÿ��
struct EWRSkillHitType
{
	enum en
	{
		None,
		Normal,	// �Ϲ� ��Ʈó��
		Miss,	// �̽�
		Inv,	// ����
		Guard,	// ����
		Reflect,// �ݻ�
		ReactionImune,	// ���׼� �鿪
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

// � ��ų�� Ư���� Ÿ���� ������� ����ϰ� ������ 
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


// �׷α� ���� Ÿ��
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


// npc ��� �� ���� �ο�
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

// ������ Ÿ��.
struct EWRDamageType
{
	enum en : uint8
	{
		Normal,
		Critical_WithoutEement,		// ġ��Ÿ-�Ӽ����� ����
		Penetrate_WithoutEement,	// ����-�Ӽ����� ����
		Critical_WithEement,		// ġ��Ÿ-�Ӽ����� ����
		Penetrate_WithEement,		// ����-�Ӽ����� ����
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

// �� Ÿ��.
// [Kyoungha] : 2018/04/14 ] : ��Ÿ���� ���� ���ǵ��� �ʾ� �ϴ� Ŭ���̾�Ʈ������ �����ؼ� ���
//								������ �������� �ʿ����� ��� ������ Ǯ� ���!
#ifdef USE_CLIENT_ONLY
struct EWRHealType
{
	enum en : uint8
	{
		Dot,			// ��Ʈ��
		Vampiric,		// ����
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
		Manual,			// �ڵ� ��� ���� ����.
		Auto,			// ������ �ڵ� ���
		PurifyAuto,		// Ư�� ���� �Ͽ� �ڵ� ���.
		Heal,			// ������ ���� �꿡 ���� ���.
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
		SkillObject,		// ���� ��ũ ��ų
		Buff,				// ���� ��ũ ����
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
		ChargeMotes1,	//¡ǥ 1�� ����
		ChargeMotes2,	//¡ǥ 2�� ����
		ChargeMotes3,	//¡ǥ 3�� ����
		MaxChargeMotes = ChargeMotes3,	//�ִ� ¡ǥ ����
		ConsumeMotes,	//¡ǥ�� �Ҹ��ϴ� ��ų

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


#define GROGGY_BUBBLE_COUNT_NORMAL					3				// ������ �׷α� ���� ���� ���� ���� ����Ʈ
#define GROGGY_BUBBLE_COUNT_PARTY					5				// ������ �׷α� ���� ���� ���� ���� ��Ƽ��

#define AVOID_GAUGE_MAX								100.0f			// ��� ������ �ִ밪

#define AVOID_GAUGE_INC_PER_CHECK					14				// �ѹ� äũ�� �� ���� ä���ִ� �� 14
#define AVOID_GAUGE_DEC								20.0f			// ȸ�� �ѹ��� ����ϴ� �������� ��
#define AVOID_GAUGE_CHECK_SEC						2				// 2�ʿ� �ѹ�äũ

#define AVOID_GAUGE_RECOVER_CHECK_TIME_MS			2000			// 2�ʿ� ���� ms �ð� -> update ������ Ȯ���ϹǷ� ������ó��
#define AVOID_GAUGE_COOL_TIME_INIT_TICK				0				// ��� ������ ��Ÿ���� �ʱⰪ

#define FIRST_BUBBLE_TYPE							1				// ù��° ���� Ÿ�� �ε��� ��
#define LAST_BUBBLE_TYPE							5				// ������ ���� Ÿ�� �ε��� ��

#define SKILL_OBJECT_LIFE_TIME_UNLIMIT				-1				// ��ų������Ʈ�� ������� �ʰ� ��� ����

#define LEVEL_UP_SKILL_TID							9001

#define BUFF_UNLIMITED_REMAIN_TIME					-1				// ������ ���� ����
#define FLOOR_HIGHT_CM								1000			// �� ���� z �Ÿ� 10m

#define MOB_GROGGY_BUFF_TID							2007000
#define NOT_REACTION_BUFF_TID						2009500
#define NEXT_DEFAULT_SKILL_INDEX					0
#define NEXT_AWAKEN_SKILL_INDEX						0
#define NEXT_NONE_AWAKEN_SKILL_INDEX				1
#define DISTANCE_2_BACKWARD_MOVE_WHEN_TARGET_ENPC	80.0f
#define DISTANCE_FROM_ACTOR_NO_TARGET_SKILL_OBJECT	500				// Ÿ���� ���� ��ų ������Ʈ�� ������ �Ÿ� (������ �÷��̾ �ٶ󺸴� ��ġ)
#define DEF_HIGHEST_SKILL_COOLTIME					999				// ���� ���ݻ��·� ���� ��Ÿ�ӿ� ���� �ʰ� ���߿� �ְ� �Ǵ� ��ų�� ���� ����ó��

//
//struct stSkillPoint : IMSerialize
//{
//	std::array<SKILL_POINT_t, EWRSkillSetPageType::MAX>		skillPointList;			// ��ų ����Ʈ ����Ʈ
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

// ���� ����
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
	TID_t		buffTid;		// ���� TID (SkillEffectParam_Buff.xml�� tid ����)
	SKILL_SEQ_t skillSeq;		// ������ ������ ��ų uid
	uint8		level;
	uint8		stack;			// ������ ���� ���� (�Ϻ� ������ ������ ��������)
	int32		remain_tm;		// �����ִ� ���� �ð�
#ifdef DEF_PRIEST_MOTES_BY_AUTULN_190102
	bool		buff_tm_run;	// ���� �ð� ����(true : �帧, false : ����)
#endif //DEF_PRIEST_MOTES_BY_AUTULN_190102
	AID_t		caster_aid;		// ������ Actor ID (�������� ������ Ű��)
	bool		isActiveBuff;	// ������ active(Ȱ��) �����ΰ�

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
	AID_t		casterAID;			// ���� ��� ���� ���̵�
	int32		collisionType;		// ��� �ɾ��� ��
	SKILL_SEQ_t skillSeq;			// ��ų ������
	TIME_t		endTick;			// ���� ����
	TID_t		buffTID;			// ���� �������� ��� �ش� ���� ����

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
