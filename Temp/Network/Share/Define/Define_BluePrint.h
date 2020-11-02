//******************************************************************************
//	FileName: 	Define_BluePrint.h
//------------------------------------------------------------------------------
//	Created	:	2019/10/17
//------------------------------------------------------------------------------
//	Author	:	gyutaelee
//------------------------------------------------------------------------------
//	Copyright 2019 WemadeXR Inc. All Rights Reserved.
//******************************************************************************
//	Purpose	:	서버/클라 공용 BluePrintType enum struct 정의
//		- UENUM을 붙여서 사용해야하는 경우에만 정의하여 사용
//		- UENUM이 없어도 되는 경우에는 컨텐츠 별 Define_.h 파일을 생성하여 사용
//		- enum class와 struct 네이밍은 동일하게 사용
//			예) enum class EWRSample, struct EWRSample
//******************************************************************************

#pragma once

static const int32 AVOID_TABLE_TID = 15;


/*----------------------------
 - 인벤토리 카테고리 타입
  : 서버에서 인벤토리 아이템 컨테이너 배열로 사용됨
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRInventoryCategoryType
{
#define class
#define EWRInventoryCategoryType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRInventoryCategoryType : uint8
	{
		Equipment = 0,
		Costume,
		Vehicle,
		Consume,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRInventoryCategoryType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Equipment,	L"Equipment");
		InsertMap(Costume,		L"Costume");
		InsertMap(Vehicle,		L"Vehicle");
		InsertMap(Consume,		L"Consume");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 아이템 장착가능 슬롯 타입
 : DB는 EQUIPPABLE_SLOT_ARMOR_BEGIN ~ EQUIPPABLE_SLOT_COSTUME_END 까지만 사용
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRItemEquippableSlot
{
#define class
#define EWRItemEquippableSlot en
#endif // USE_SERVER_ONLY

#define EQUIPPABLE_SLOT_WEAPON_BEGIN			1
#define EQUIPPABLE_SLOT_ARMOR_BEGIN				11
#define EQUIPPABLE_SLOT_COSTUME_BEGIN			31

	UENUM(BlueprintType)
	enum class EWRItemEquippableSlot : uint8
	{
		None = 0,			// 장착할 수 없는 아이템, 장착중이지 않음

		LeftWeapon = EQUIPPABLE_SLOT_WEAPON_BEGIN,
		RightWeapon,
		Temporary,
		EQUIPPABLE_SLOT_WEAPON_END,

		Armor = EQUIPPABLE_SLOT_ARMOR_BEGIN,
		EQUIPPABLE_SLOT_ARMOR_END,

		Top = EQUIPPABLE_SLOT_COSTUME_BEGIN,
		Bottom,
		Shoes,
		Hat,
		Gloves,
		Glasses,
		Neck,
		Mask,
		Back,
		EQUIPPABLE_SLOT_COSTUME_END,
	
		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRItemEquippableSlot

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(LeftWeapon,		L"LeftWeapon");
		InsertMap(RightWeapon,		L"RightWeapon");
		InsertMap(Temporary,		L"Temporary");
		InsertMap(Armor,			L"Armor");
		InsertMap(Top,				L"Top");
		InsertMap(Bottom,			L"Bottom");
		InsertMap(Shoes,			L"Shoes");
		InsertMap(Hat,				L"Hat");
		InsertMap(Gloves,			L"Gloves");
		InsertMap(Glasses,			L"Glasses");
		InsertMap(Neck,				L"Neck");
		InsertMap(Mask,				L"Mask");
		InsertMap(Back,				L"Back");
	}

	inline static bool IsEquipment(EWRItemEquippableSlot::en eType) { return (eType >= EQUIPPABLE_SLOT_WEAPON_BEGIN && eType < EQUIPPABLE_SLOT_ARMOR_END) ? true : false; }
	inline static bool IsWeapon(EWRItemEquippableSlot::en eType) { return (eType >= EQUIPPABLE_SLOT_WEAPON_BEGIN && eType < EQUIPPABLE_SLOT_WEAPON_END) ? true : false; }
	inline static bool IsArmor(EWRItemEquippableSlot::en eType) { return (eType >= EQUIPPABLE_SLOT_ARMOR_BEGIN && eType < EQUIPPABLE_SLOT_ARMOR_END) ? true : false; }
	inline static bool IsCostume(EWRItemEquippableSlot::en eType) { return (eType >= EQUIPPABLE_SLOT_COSTUME_BEGIN && eType < EQUIPPABLE_SLOT_COSTUME_END) ? true : false; }
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 아이템 메인 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRItemMainType
{
#define class
#define EWRItemMainType en
#endif // USE_SERVER_ONLY

#define ITEM_MAINTYPE_EQUIPMENT_BEGIN	1
#define ITEM_MAINTYPE_CONSUME_BEGIN		21
#define ITEM_MAINTYPE_HOUSING_BEGIN		31
#define ITEM_MAINTYPE_VEHICLE_BEGIN		41

	UENUM(BlueprintType)
	enum class EWRItemMainType : uint8
	{
		None = 0,

		Weapon = ITEM_MAINTYPE_EQUIPMENT_BEGIN,			// 무기
		Armor,										    // 방어구
		Costume,										// 코스튬
		Temporary,										// 임시 아이템
		ITEM_MAINTYPE_EQUIPMENT_END,

		Material = ITEM_MAINTYPE_CONSUME_BEGIN,			// Consume
		Interior,										// Consume		
		Emote,											// Consume		
		Placement,										// Consume
		ITEM_MAINTYPE_CONSUME_END,

		Housing = ITEM_MAINTYPE_HOUSING_BEGIN,			// Housing
		ITEM_MAINTYPE_HOUSING_END,

		Vehicle = ITEM_MAINTYPE_VEHICLE_BEGIN,
		ITEM_MAINTYPE_VEHICLE_END,

		MAX = ITEM_MAINTYPE_VEHICLE_END,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRItemMainType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,			L"None");

		InsertMap(Weapon,		L"Weapon");
		InsertMap(Armor,		L"Armor");
		InsertMap(Costume,		L"Costume");
		InsertMap(Armor,		L"Armor");
		InsertMap(Temporary,	L"Temporary");

		InsertMap(Material,		L"Material");
		InsertMap(Interior,		L"Interior");
		InsertMap(Emote,		L"Emote");
		InsertMap(Placement,	L"Placement");

		InsertMap(Housing,		L"Housing");

		InsertMap(Vehicle,		L"Vehicle");
	}

	inline static bool IsEquipment(EWRItemMainType::en eType)
	{
		switch (eType)
		{
		case EWRItemMainType::Weapon:
		case EWRItemMainType::Armor:
		case EWRItemMainType::Temporary:
			return true;

		default:
			return false;
		}
	}

	inline static bool IsCostume(EWRItemMainType::en eType) { return eType == Costume ? true : false; }
	inline static bool IsVehicle(EWRItemMainType::en eType) { return (eType >= ITEM_MAINTYPE_VEHICLE_BEGIN && eType < ITEM_MAINTYPE_VEHICLE_END) ? true : false; }
	inline static bool IsConsume(EWRItemMainType::en eType) { return (eType >= ITEM_MAINTYPE_CONSUME_BEGIN && eType < ITEM_MAINTYPE_CONSUME_END) ? true : false; }

	inline static EWRInventoryCategoryType::en ConvToInvenCategory(EWRItemMainType::en eMainType)
	{
		switch (eMainType)
		{
		case EWRItemMainType::Weapon:
		case EWRItemMainType::Armor:
		case EWRItemMainType::Temporary:
			return EWRInventoryCategoryType::Equipment;

		case EWRItemMainType::Costume:
		case EWRItemMainType::Material:
		case EWRItemMainType::Interior:
			return EWRInventoryCategoryType::Consume;

		case EWRItemMainType::Vehicle:
			return EWRInventoryCategoryType::Vehicle;

		default:
			return EWRInventoryCategoryType::MAX;
		}
	}
};
#endif // USE_SERVER_ONLY


/*----------------------------
 - 아이템 서브 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRItemSubType
{
#define class
#define EWRItemSubType en
#endif // USE_SERVER_ONLY

#define ITEM_SUBTYPE_WEAPON_BEGIN		1
#define ITEM_SUBTYPE_ARMOR_BEGIN		11
#define ITEM_SUBTYPE_COSTUME_BEGIN		21
#define ITEM_SUBTYPE_TEMPORARY_BEGIN	31

#define ITEM_SUBTYPE_MATERIAL_BEGIN		41
#define ITEM_SUBTYPE_INTERIOR_BEGIN		51
#define ITEM_SUBTYPE_EMOTE_BEGIN		61
#define ITEM_SUBTYPE_PLACEMENT_BEGIN	71

#define ITEM_SUBTYPE_HOUSING_BEGIN		81

#define ITEM_SUBTYPE_VEHICLE_BEGIN		91

	UENUM(BlueprintType)
	enum class EWRItemSubType : uint8
	{
		None = 0,
		
		Weapon = ITEM_SUBTYPE_WEAPON_BEGIN,		// 무기
		Hook,									// 갈고리
		Shield,									// 방패
		Torch,									// 토치
		ITEM_SUBTYPE_WEAPON_END,
		
		Armor = ITEM_SUBTYPE_ARMOR_BEGIN,
		ITEM_SUBTYPE_ARMOR_END,

		Top = ITEM_SUBTYPE_COSTUME_BEGIN,
		Bottom,
		Shoes,
		Hat,
		Gloves,
		Glasses,
		Neck,
		Mask,
		Back,
		ITEM_SUBTYPE_COSTUME_END,

		Drink = ITEM_SUBTYPE_TEMPORARY_BEGIN,	// 음료
		ITEM_SUBTYPE_TEMPORARY_END,

		Recycling = ITEM_SUBTYPE_MATERIAL_BEGIN,
		ITEM_SUBTYPE_MATERIAL_END,

		Floor = ITEM_SUBTYPE_INTERIOR_BEGIN,
		Wall,
		Ceiling,
		ITEM_SUBTYPE_INTERIOR_END,
		
		Emoticon = ITEM_SUBTYPE_EMOTE_BEGIN,
		Motion,
		ITEM_SUBTYPE_EMOTE_END,

		SocialTools = ITEM_SUBTYPE_PLACEMENT_BEGIN,
		ITEM_SUBTYPE_PLACEMENT_END,

		House = ITEM_SUBTYPE_HOUSING_BEGIN,
		ITEM_SUBTYPE_HOUSING_END,

		Car = ITEM_SUBTYPE_VEHICLE_BEGIN,
		ITEM_SUBTYPE_VEHICLE_END,

		MAX = ITEM_SUBTYPE_VEHICLE_END
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRItemSubType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,			L"None");

		InsertMap(Weapon,		L"Weapon");
		InsertMap(Hook,			L"Hook");
		InsertMap(Shield,		L"Shield");
		InsertMap(Torch,		L"Torch");
		
		InsertMap(Armor,		L"Armor");
		
		InsertMap(Top,			L"Top");
		InsertMap(Bottom,		L"Bottom");
		InsertMap(Shoes,		L"Shoes");
		InsertMap(Hat,			L"Hat");
		InsertMap(Gloves,		L"Gloves");
		InsertMap(Glasses,		L"Glasses");
		InsertMap(Neck,			L"Neck");
		InsertMap(Mask,			L"Mask");
		InsertMap(Back,			L"Back");
		
		InsertMap(Drink,		L"Drink");

		InsertMap(Recycling,	L"Recycling");

		InsertMap(Floor,		L"Floor");
		InsertMap(Wall,			L"Wall");
		InsertMap(Ceiling,		L"Ceiling");

		InsertMap(Emoticon,		L"Emoticon");
		InsertMap(Motion,		L"Motion");

		InsertMap(SocialTools,	L"SocialTools");

		InsertMap(House,		L"House");

		InsertMap(Car,			L"Car");
	}

	inline static bool IsEquipment(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_WEAPON_BEGIN && eType < ITEM_SUBTYPE_ARMOR_END) ? true : false; }
	inline static bool IsWeapon(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_WEAPON_BEGIN && eType < ITEM_SUBTYPE_WEAPON_END) ? true : false; }
	inline static bool IsArmor(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_ARMOR_BEGIN && eType < ITEM_SUBTYPE_ARMOR_END) ? true : false; }
	inline static bool IsTemporary(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_TEMPORARY_BEGIN && eType < ITEM_SUBTYPE_TEMPORARY_END) ? true : false; }
	inline static bool IsCostume(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_COSTUME_BEGIN && eType < ITEM_SUBTYPE_COSTUME_END) ? true : false; }
	inline static bool IsMaterial(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_MATERIAL_BEGIN && eType < ITEM_SUBTYPE_MATERIAL_END) ? true : false; }
	inline static bool IsInterior(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_INTERIOR_BEGIN && eType < ITEM_SUBTYPE_INTERIOR_END) ? true : false; }
	inline static bool IsVehicle(EWRItemSubType::en eType) { return (eType >= ITEM_SUBTYPE_VEHICLE_BEGIN && eType < ITEM_SUBTYPE_VEHICLE_END) ? true : false; }
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 퀘스트 시작 조건 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRQuestReadyCoditionType
{
#define class
#define EWRQuestReadyCoditionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRQuestReadyCoditionType : uint8
	{
		None,
		CompletedQuest,				// 선행 퀘스트 완료 여부
		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRQuestReadyCoditionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(CompletedQuest,	L"CompletedQuest");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 퀘스트 동작 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRQuestActionType
{
#define class
#define EWRQuestActionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRQuestActionType : uint8
	{
		None,
		ExcuteTrigger,				// 트리거 동작
		Tutorial,					// 튜토리얼 시작
		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRQuestActionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(ExcuteTrigger,	L"ExcuteTrigger");
		InsertMap(Tutorial,			L"Tutorial");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 퀘스트 태스크 성격
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRQuestTaskStyle
{
#define class
#define EWRQuestTaskStyle en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRQuestTaskStyle : uint8
	{
		None,
		Serial,			// 직렬 진행
		Parallel,		// 병렬 진행
		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRQuestTaskStyle

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,			L"None");
		InsertMap(Serial,		L"Serial");
		InsertMap(Parallel,		L"Parallel");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 퀘스트 태스크 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRQuestTaskType
{
#define class
#define EWRQuestTaskType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRQuestTaskType : uint8
	{
		None,
		MovePosition,		// 목표지점 이동
		ActiveTrigger,		// 트리거 발동
		ClearTutorial,		// 튜토리얼 완수
		ActiveIO,			// 인터렉션 오브젝트 활성
		ActiveIO_Group,		// 인터렉션 오브젝트 활성 그룹
		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRQuestTaskType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(MovePosition,		L"MovePosition");
		InsertMap(ActiveTrigger,	L"ActiveTrigger");
		InsertMap(ClearTutorial,	L"ClearTutorial");
		InsertMap(ActiveIO,			L"ActiveIO");
		InsertMap(ActiveIO_Group,	L"ActiveIO_Group");
	}
};
#endif // USE_SERVER_ONLY


/*----------------------------
 - 스킬 Buff 종류
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRBuffType
{
#define class
#define EWRBuffType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRBuffType : uint8
	{
		None,		// 에러시 대응용
		Buff,       // 버프
		DeBuff,		// 디버프
		All,		// 모든 버프 삭제시 비교를 위해
		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRBuffType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Buff, L"Buff");
		InsertMap(DeBuff, L"DeBuff");
		InsertMap(All, L"All");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 스킬 Buff 그룹
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRBuffGroup
{
#define class
#define EWRBuffGroup en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRBuffGroup : uint8
	{
		None,			// 상태를 체크하기 위해 걸어주는 버프 (예: 잡기가 가능한지 체크)
		Inv,			// 무적 (공격받지 않음)
		Guard,			// 무적 (공격은 받으나 피해를 받지 않고, 공격자에게 효과를 적용가능)
		Stun,			// 행동 불가
		AttackSpd,		// 공격 속도 배율 (기준 10000 = 100%, 1배)
		//Spd,			// 이동 속도 배율 (기준 10000 = 100%, 1배)
		MoveSpd,		// 이동 속도
		Spd,			// 이동 + 공격 속도 파라미터로 사용 가능
		Groggy,			// 그로기		
		Vampire,		// 공격시 hp증가
		Bleeding,		// 공격시 hp감소
		Heal,			// 시간마다 hp회복
		Damage,			// 시간마다 hp감소
		Stat,			// 엑터의 스텟에 영향을 미친다.
		Cure,			// Param에 있는 디버프를 해제
		Dispel,			// Param에 잇는 버프를 해제
		Clear,			// 모든 상태효과 해제 -> 모든 버프/디버프 해제
		Sleep,			// ================> 수면 상태 적용 - 행동 불가 상태에서 피해를 받으면 풀림
		Immune,			// 면역
		Reflect,		// 공격자에게 buff tid 적용
		Purify,			// 정화
		Burn,			// 시간마다 hp감소
		Poison,			// 시간마다 hp감소
		Freeze,			// 빙결
		Shock,			// 감전
		Blind,			// 실명
		Seal,			// 봉인(스킬 사용 금지)
		Ignore,			// 버프 tid 무시
		NotReaction,	// 리액션 무시
		NotReactionJump,	// 리액션 중 점프만 막는것
		SuddenTarget,	// 빈틈 - 그로기 게이지 깎을 수 있는 시점이 되는 버프
		AirBind,		// 공중 속박
		GroundBind,		// 지상 속박
		ALL,			// enpc의 버프 저항 시 사용하는 값. (stActorRecord_ENpc::resist_debuff)
		SealAvoid,		// 회피 봉인
		Condition,		// 조건기를 위한 버프
		Bind,			// 속박 - 이동불가, 스킬사용 가능
		MAX,
	};

	enum purify_sub_enum
	{
		Purify_Clear = 1,
		Purify_DecTm = 2,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRBuffGroup

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"*");
		InsertMap(Stun, L"stun");
		InsertMap(Inv, L"inv");
		InsertMap(Guard, L"guard");
		InsertMap(Sleep, L"sleep");
		InsertMap(AttackSpd, L"attackspd");
		//InsertMap(Spd, L"spd");
		InsertMap(MoveSpd, L"movespd");
		InsertMap(Spd, L"spd");
		InsertMap(Groggy, L"groggy");
		InsertMap(Clear, L"clear");
		InsertMap(Sleep, L"sleep");
		InsertMap(Purify, L"purify");
		InsertMap(Immune, L"immune");
		InsertMap(Reflect, L"reflect");
		InsertMap(Freeze, L"freeze");
		InsertMap(Shock, L"shock");
		InsertMap(Blind, L"blind");
		InsertMap(Seal, L"seal");
		InsertMap(Vampire, L"vampire");
		InsertMap(Bleeding, L"bleeding");
		InsertMap(Heal, L"heal");
		InsertMap(Damage, L"damage");
		InsertMap(Cure, L"cure");
		InsertMap(Dispel, L"dispel");
		InsertMap(Burn, L"burn");
		InsertMap(Poison, L"poison");
		InsertMap(Stat, L"stat");
		InsertMap(Ignore, L"ignore");
		InsertMap(NotReaction, L"notreaction");
		InsertMap(NotReactionJump, L"NotReactionJump");
		InsertMap(SuddenTarget, L"suddentarget");
		InsertMap(AirBind, L"AirBind");
		InsertMap(GroundBind, L"GroundBind");

		InsertMap(ALL, L"ALL");
		InsertMap(SealAvoid, L"SealAvoid");
		InsertMap(Condition, L"Condition");
		InsertMap(Bind, L"Bind");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - Param 버프나 CC의 효과를 정의
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRParamType
{
#define class
#define EWRParamType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRParamType : uint8
	{
		None,				// "*" => 상태를 체크하기 위해 걸어주는 버프 (예: 잡기가 가능한지 체크)
		Inv,				// invincible   - 무적 여부
		Guard,				// 방어
		Stun,				// 스턴 상태
		AttackSpdMul,		// 공격 속도 변경
		MoveSpdMul,			// 이동 속도 변경
		MoveSpdAdd,			// 이동 속도 변경
		//SpdMul,			// 이동 속도 변경
		//SpdAdd,			// 이동 속도 변경

		Groggy,				// 그로기 상태

		VampireMul,			// 공격시 hp흡수
		VampireAdd,			// 공격시 hp흡수
		HealMul,			// dot hp
		HealAdd,			// dot hp

		Cure,				// 모든 Debuff타입 삭제
		Dispel,				// 모든 Buff타입 삭제
		Clear,				// 모든 버프 디버프 해제

		Sleep,				// 수면 상태		
		Reflect,			// 세팅된 상태를 공격한 적에게 버프 적용
		Freeze,				// 빙결
		BlindMul,			// 실명: 지정된 비율만큼의 확률로 공격이 빗나감(만분율)
		Seal,				// 봉인(스킬 사용 금지)

		Poison,				// 중독
		Burn,				// 화상
		Blind,				// 실명
		Shock,				// 감전
		Ignore,				// 버프 tid에 대한 무시

		Immune,				// 면역
		NotReaction,		// 리액션 무시
		NotReactionJump,	// 리액션 중 점프만 막는것
		ALL,				// enpc의 버프 저항 시 사용하는 값. (stActorRecord_ENpc::resist_debuff)
		SealAvoid,			// 회피 봉인

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRParamType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"*");
		InsertMap(Inv, L"Inv");
		InsertMap(Guard, L"Guard");
		InsertMap(Stun, L"Stun");
		InsertMap(AttackSpdMul, L"AttackSpdmul");
		InsertMap(MoveSpdMul, L"MoveSpdMul");
		InsertMap(MoveSpdAdd, L"MoveSpdAdd");
		//InsertMap(SpdMul, L"Spdmul");
		//InsertMap(SpdAdd, L"Spdadd");

		InsertMap(Groggy, L"Groggy");
		InsertMap(VampireMul, L"vampiremul");
		InsertMap(VampireAdd, L"vampireadd");
		InsertMap(HealMul, L"healmul");
		InsertMap(HealAdd, L"healadd");

		InsertMap(Cure, L"cure");
		InsertMap(Dispel, L"dispel");
		InsertMap(Clear, L"clear");

		InsertMap(Sleep, L"Sleep");
		InsertMap(Reflect, L"reflect");
		InsertMap(Freeze, L"freeze");
		InsertMap(BlindMul, L"blindmul");
		InsertMap(Seal, L"seal");

		InsertMap(Poison, L"poison");
		InsertMap(Burn, L"burn");
		InsertMap(Blind, L"blind");
		InsertMap(Shock, L"shock");
		InsertMap(Ignore, L"ignore");
		InsertMap(Immune, L"immune");
		InsertMap(NotReaction, L"notreaction");
		InsertMap(ALL, L"All");
		InsertMap(SealAvoid, L"SealAvoid");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 스킬 Buff 적용시점
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRApplyReactionType
{
#define class
#define EWRApplyReactionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRApplyReactionType : uint8
	{
		None,
		FirstHit,	// 첫 타에 적용
		AllHit,		// 매 히트틱마다 적용
		LastHit,	// 마지막 히트에만 적용
		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRApplyReactionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(AllHit, L"*");
		InsertMap(FirstHit, L"FirstHit");
		InsertMap(AllHit, L"AllHit");
		InsertMap(LastHit, L"LastHit");
	}
};
#endif // USE_SERVER_ONLY


/*----------------------------
 - 갈고리 전용
	오브젝트가 갈고리에 어떻게 적용 될지 결정
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRHookType
{
#define class
#define EWRHookType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRHookType : uint8
	{
		None,					//	갈고리 걸리지 않음
		Activate,				//	갈고리 동작
		Dash,					//	갈고리 대쉬
		Pull,					//	갈고리 당기기

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRHookType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"none");
		InsertMap(Activate, L"Activate");
		InsertMap(Dash, L"Dash");
		InsertMap(Pull, L"Pull");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 피격 부위
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRCollisionType
{
#define class
#define EWRCollisionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRCollisionType : uint8
	{
		None,
		RightAttack,
		LeftAttack,
		Bite,
		Shield,
		Head,
		LeftArm,
		RightArm,
		Body,
		LeftLeg,
		RightLeg,
		Tail,
		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRCollisionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"none");
		InsertMap(RightAttack, L"RightAttack");
		InsertMap(LeftAttack, L"LeftAttack");
		InsertMap(Bite, L"Bite");
		InsertMap(Shield, L"Shield");
		InsertMap(Head, L"Head");
		InsertMap(LeftArm, L"LeftArm");
		InsertMap(RightArm, L"RightArm");
		InsertMap(Body, L"Body");
		InsertMap(LeftLeg, L"LeftLeg");
		InsertMap(RightLeg, L"RightLeg");
		InsertMap(Tail, L"Tail");
		InsertMap(MAX, L"MAX");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 스킬 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRSkillType
{
#define class
#define EWRSkillType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRSkillType : uint8
	{
		None,
		Fast,
		Strong,
		Defense,
		Parrying,
		Dash,
		Bash,
		ChainLightning,
		Projectile,
		SuicideBomb,
		ChainProjectile,

		Hook,
		Heal,
		Avoid,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRSkillType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Fast, L"Fast");
		InsertMap(Strong, L"Strong");
		InsertMap(Defense, L"Defense");
		InsertMap(Parrying, L"Parrying");
		InsertMap(Dash, L"Dash");
		InsertMap(Bash, L"Bash");
		InsertMap(ChainLightning, L"ChainLightning");
		InsertMap(Projectile, L"Projectile");
		InsertMap(SuicideBomb, L"SuicideBomb");
		InsertMap(ChainProjectile, L"ChainProjectile");
		InsertMap(Hook, L"Hook");
		InsertMap(Heal, L"Heal");
		InsertMap(Avoid, L"Avoid");
	}

	static bool IsBattleType(EWRSkillType::en skillType)
	{
		switch (skillType)
		{
		case EWRSkillType::None:
		case EWRSkillType::Dash:
		{
			return false;
		}
		break;
		}

		return true;
	}

	static bool DistanceCheckType(EWRSkillType::en skillType)
	{
		switch (skillType)
		{
		case EWRSkillType::None:
		case EWRSkillType::Dash:
		case EWRSkillType::Avoid:
		case EWRSkillType::Defense:
		{
			return false;
		}
		break;
		}

		return true;
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 어택 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRAttackType
{
#define class
#define EWRAttackType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRAttackType : uint8
	{
		None,
		Throw,
		Pull,
		Dash,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRAttackType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Throw, L"Throw");
		InsertMap(Pull, L"Pull");
		InsertMap(Dash, L"Dash");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 스킬 범위 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRSkillAreaType
{
#define class
#define EWRSkillAreaType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRSkillAreaType : uint8
	{
		Normal	= 0,		// 범위가 아닌 일반 공격
		Laser	= 1,		// 직사각형 범위로 순차적으로 Hit 되는 타입
		Fan		= 2,		// 부채꼴
		Square	= 3,		// 직사각형 범위
		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRSkillAreaType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Normal, L"Normal");
		InsertMap(Laser, L"Laser");
		InsertMap(Fan, L"Fan");
		InsertMap(Square, L"Square");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- SO 포지션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRSOPosType
{
#define class
#define EWRSOPosType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRSOPosType : uint8
	{
		None = 0,		// 초기값
		Caster = 1,		// 스킬을 발동하는 객체의 위치로 부터
		Target = 2,		// 타켓의 위치로 부터
		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRSOPosType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Caster, L"Caster");
		InsertMap(Target, L"Target");
	}
};
#endif // USE_SERVER_ONLY


/*----------------------------
- FSM State
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRFSM
{
#define class
#define EWRFSM en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRFSM : uint8
	{
		None,
		AnimationEnd,	//애니메이션이 끝났을때 Invoke되는 Type. IMFSMBase::NextFSMWhenAnimationEnd에 세팅된 값을 실제 Invoke시킴
		Idle,		// idle
		Move,       // 이동
		Attack,     // 평타
		Skill,		// 스킬
		MaintainingSkill,	//dynamicMove(etc) 가 끝나는 시점에 애니가 종료 되어야 하는 스킬
		Damage,     // 데미지
		Die,        // 죽음(일반)
		Dead,       // 시체
		Stand,      // Down 에서 일어나는 중
		Interact,	// 상호작용중
		Revival,	// 부활
		Spawn,		// 스폰
		Down,		// 누워있는 상태
		CC,			// 군중제어상태(Crowd Control)
		Emotion,	// 감정표현
		Ride,		// 탑승
		Dismount,	// 탈것 내리기
		Acting,     // 연출용 애니메이션 재생 > 이후 Idle 로 변경
		BeingTamed,		// 테이밍 당하는 상태	
		Taming,			// 테이밍 시도중
		Defense,		// 방어상태
		BeBlocked,		// 공격이 막혔을때
		Parrying,		// 공격을 쳐내는 상태
		BeParried,		// 공격이 쳐내짐 당한상태
		Avoiding,		// 회피상태
		WeaponChange,	// 무기 교체
		Arm,			// 무기 장착
		Disarm,			// 무기 해제
		Talk,				// 다이얼로그 대화
		SocialAnimation,	// 소셜 애니
		EventAction,		// 이벤트 액션
		Ladder,				// 사다리
		Dance,				// 춤추기
		Zipline,            // 집라인탑승자세
		EnterVehicle,	// 차량탑승
		ExitVehicle,	// 차량하차
		DrivingIdle,	// 운전상태
		VehicleInteraction,	// 차량 상호작용
		VoiceChatMotion,	// 음성 대화중 모션


		//미구현
		Airborne,	// 공중에 떠있는 상태

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRFSM

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(AnimationEnd, L"AnimationEnd");
		InsertMap(Idle, L"Idle");
		InsertMap(Move, L"Move");
		InsertMap(Attack, L"Attack");
		InsertMap(Skill, L"Skill");
		InsertMap(MaintainingSkill, L"MaintainingSkill");
		InsertMap(Damage, L"Damage");
		InsertMap(Die, L"Die");
		InsertMap(Dead, L"Dead");
		InsertMap(Stand, L"Stand");
		InsertMap(Interact, L"Interact");
		InsertMap(Revival, L"Revival");
		InsertMap(Spawn, L"Spawn");
		InsertMap(Down, L"Down");
		InsertMap(CC, L"CC");
		InsertMap(Emotion, L"Emotion");
		InsertMap(Ride, L"Ride");
		InsertMap(Dismount, L"Dismount");
		InsertMap(Acting, L"Acting");
		InsertMap(BeingTamed, L"BeingTamed");
		InsertMap(Taming, L"Taming");
		InsertMap(Defense, L"Defense");
		InsertMap(BeBlocked, L"BeBlocked");
		InsertMap(Parrying, L"Parrying");
		InsertMap(BeParried, L"BeParried");
		InsertMap(Avoiding, L"Avoiding");
		InsertMap(WeaponChange, L"WeaponChange");
		InsertMap(Talk, L"Talk");
		InsertMap(Airborne, L"Airborne");
		InsertMap(SocialAnimation, L"SocialAnimation");
		InsertMap(Ladder, L"Ladder");
		InsertMap(Dance, L"Dance");
		InsertMap(Zipline, L"Zipline");
		InsertMap(MAX, L"MAX");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 팩션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRFactionRelation
{
#define class
#define EWRFactionRelation en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRFactionRelation : uint8
	{
		Neutral,
		Friendly,
		Hostile,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRFactionRelation

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Neutral, L"Neutral");
		InsertMap(Friendly, L"Friendly");
		InsertMap(Hostile, L"Hostile");
	}
};
#endif // USE_SERVER_ONLY


//자신의 팩션 성향(대상과의 팩션 관계에 의해 호의적, 적대적 구분)
//
//- 1 ( faction_a ) : 이그니스 연합군 세력의 PC와 동일한 성향일 때
//- 2 ( faction_b ) : 에어리스 정예군 세력의 PC와 동일한 성향일 때
//- 3 ( faction_c ) : 모든 PC, (E)NPC와 전투를 하지 않도록 할 때
//- 4 ( faction_d ) : 모든 PC와 전투를 하지 않고 특정(E)NPC와 전투를 하고자 할 때
//- 5 ( faction_e ) : 모든 PC와 특정(E)NPC와 전투를 하고자 할 때
//- 6 ( faction_f ) : 시리우스 세력장
//- 7 ( faction_g ) : 에이프스 세력장

/*----------------------------
- 팩션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRCharacterFactionType
{
#define class
#define EWRCharacterFactionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRCharacterFactionType : uint8
	{
		None,

		FactionA,
		FactionB,
		FactionC,
		FactionD,
		FactionE,
//		FactionF,
//		FactionG,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRCharacterFactionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(FactionA, L"factiona");
		InsertMap(FactionB, L"factionb");
		InsertMap(FactionC, L"factionc");
		InsertMap(FactionD, L"factiond");
		InsertMap(FactionE, L"factione");
//		InsertMap(FactionF, L"factionf");
//		InsertMap(FactionG, L"factiong");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- dynamic Movement type
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRDynamicMovementType
{
#define class
#define EWRDynamicMovementType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRDynamicMovementType : uint8
	{
		None,

		//시전자가 이동
		FrontMoveToPosition,	//설정된 거리만큼 앞으로 이동
		FrontMoveToActor,		//대상에서 설정된거리만큼 떨어진 위치로 이동
		BackMoveToPosition,		//설정된 거리만큼 뒤로 이동
		BackMoveToActor,		//대상에서 설정된 거리만큼 뒤로 이동 (해당 거리보다 멀 경우 제자리에 있음)

		//대상을 이동
		PushedToPosition,		//대상을 설정된 거리만큼 민다
		PulledToActor,			//대상을 설정된 거리만큼 떨어진 위치로 당긴다

		//내 자신이 이동
		MoveToInputDirection,	//입력받은 방향으로 이동, 입력이 없을경우 정면이동

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRDynamicMovementType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(FrontMoveToPosition, L"FrontMoveToPosition");
		InsertMap(FrontMoveToActor, L"FrontMoveToActor");
		InsertMap(BackMoveToPosition, L"BackMoveToPosition");
		InsertMap(BackMoveToActor, L"BackMoveToActor");
		InsertMap(PushedToPosition, L"PushedToPosition");
		InsertMap(PulledToActor, L"PulledToActor");
		InsertMap(MoveToInputDirection, L"MoveToInputDirection");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- dynamic Movement type
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRAttackHeightType
{
#define class
#define EWRAttackHeightType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRAttackHeightType : uint8
	{
		None = 0,

		Upper = 1 << 1,
		Middle = 1 << 2,
		Lower = 1 << 3,

		Pitch = 1 << 4,	// player 전용

		MAX,
	};
#ifdef USE_SERVER_ONLY
#else // USE_SERVER_ONLY
	ENUM_CLASS_FLAGS(EWRAttackHeightType);
#endif // USE_SERVER_ONLY


#ifdef USE_SERVER_ONLY
#undef class
#undef EWRAttackHeightType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Upper, L"Upper");
		InsertMap(Middle, L"Middle");
		InsertMap(Lower, L"Lower");
		InsertMap(Pitch, L"Pitch");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 탈것 이동 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRVehicleMoveType
{
#define class
#define EWRVehicleMoveType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRVehicleMoveType : uint8
	{
		None,

		Shuttle,		// 왕복
		Circulation,	// 순환
		Warp,			// 순간이동형

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRVehicleMoveType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None, L"None" );
		InsertMap( Shuttle, L"Shuttle" );
		InsertMap( Circulation, L"Circulation" );
		InsertMap( Warp, L"Warp" );
	}

	static const bool HasStation( const EWRVehicleMoveType::en enVehicleMoveType )
	{
		switch ( enVehicleMoveType )
		{
			case EWRVehicleMoveType::Shuttle:		return true;
			case EWRVehicleMoveType::Circulation:	return true;
			case EWRVehicleMoveType::Warp:			return false;
		}

		return false;
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 소셜 액션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRSocialActionType
{
#define class
#define EWRSocialActionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRSocialActionType : uint8
	{
		None = 0,

		Animation,
		Dance,
		SecretDialog,
		FocusTalk,
		Translation,
		VideoChatList,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRSocialActionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(Animation,		L"Animation");
		InsertMap(Dance,			L"Dance");
		InsertMap(SecretDialog,		L"SecretDialog");
		InsertMap(FocusTalk,		L"FocusTalk");
		InsertMap(Translation,		L"Translation");
		InsertMap(VideoChatList,	L"VideoChatList");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - NPC 직업 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRNpcJobType
{
#define class
#define EWRNpcJobType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRNpcJobType : uint8
	{
		None = 0,

		Dancer,			// 댄스 NPC
		Drinker,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRNpcJobType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,		L"None");
		InsertMap(Dancer,	L"Dancer");
		InsertMap(Drinker,	L"Drinker");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - NPC 생성 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRNpcCreateType
{
#define class
#define EWRNpcCreateType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRNpcCreateType : uint8
	{
		UNIQUE,			// 기존 1:1 스폰
		INDIVIDUAL,		// 개인별 고유
		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRNpcCreateType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(UNIQUE, L"UNIQUE");
		InsertMap(INDIVIDUAL, L"INDIVIDUAL");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 건배 상태 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRCheersConditionType
{
#define class
#define EWRCheersConditionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRCheersConditionType : uint8
	{
		None = 0,

		Suggest,			// 건배 제의 상태 (요청자)
		WaitingAnswer,		// 건배 응답 대기 상태 (응답자)
		Answer,				// 건배 응답 상태 (응답자)
		WaitingBegin,		// 건배 대기 상태 (요청자, 응답자)

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRCheersConditionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(Suggest,			L"Suggest");
		InsertMap(WaitingAnswer,	L"WaitingAnswer");
		InsertMap(Answer,			L"Answer");
		InsertMap(WaitingBegin,		L"WaitingBegin");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 재화 카테고리
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRCurrencyCategory
{
#define class
#define EWRCurrencyCategory en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRCurrencyCategory : uint8
	{
		None = 0,
	
		Game_Money,			// 인게임 재화
		Cash_Money,			// 캐쉬로 구매하는 재화
		Point,				// 컨텐츠를 통해 획득하거나 사용 되는 포인트
		Ticket,				// 입장 시 사용 되는 포인트
	
		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRCurrencyCategory

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,			L"None");
		InsertMap(Game_Money,	L"Game_Money");
		InsertMap(Cash_Money,	L"Cash_Money");
		InsertMap(Point,		L"Point");
		InsertMap(Ticket,		L"Ticket");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
 - 재화 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRCurrencyType
{
#define class
#define EWRCurrencyType en
#endif // USE_SERVER_ONLY

#define CATEGORY_GAME_MONEY_BEGIN		1
#define CATEGORY_CASH_MONEY_BEGIN		11

	UENUM(BlueprintType)
	enum class EWRCurrencyType : uint8
	{
		None = 0,
	
		Gold			= CATEGORY_GAME_MONEY_BEGIN,
		CATEGORY_GAME_MONEY_END,

		Cash			= CATEGORY_CASH_MONEY_BEGIN,
		CATEGORY_CASH_MONEY_END,
	
		MAX = CATEGORY_CASH_MONEY_END
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRCurrencyType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,			L"None",		L"");
		InsertMap(Gold,			L"Gold",		L"god");
		InsertMap(Cash,			L"Cash",		L"cah");
	}

	inline static bool IsGameMoney(EWRCurrencyType::en enValue)
	{
		if (enValue >= CATEGORY_GAME_MONEY_BEGIN && enValue < CATEGORY_GAME_MONEY_END)
			return true;

		return false;
	}

	inline static bool IsCashMoney(EWRCurrencyType::en enValue)
	{
		if (enValue >= CATEGORY_CASH_MONEY_BEGIN && enValue < CATEGORY_CASH_MONEY_END)
			return true;

		return false;
	}
};
#endif // USE_SERVER_ONLY


/*----------------------------
 - 아이템 슬롯 타입
  : 서버/DB는 REGISTRATION_SLOT_LEFT_WEAPON_BEGIN ~ REGISTRATION_SLOT_RIGHT_WEAPON_END 까지만 사용
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRRegistrationSlotType
{
#define class
#define EWRRegistrationSlotType en
#endif // USE_SERVER_ONLY

#define REGISTRATION_SLOT_LEFT_WEAPON_BEGIN			1
#define REGISTRATION_SLOT_RIGHT_WEAPON_BEGIN		21

	UENUM(BlueprintType)
	enum class EWRRegistrationSlotType : uint8
	{
		None				= 0,

		LeftWeapon_1		= REGISTRATION_SLOT_LEFT_WEAPON_BEGIN,
		LeftWeapon_2,
		LeftWeapon_3,
		REGISTRATION_SLOT_LEFT_WEAPON_END,

		RightWeapon_1		= REGISTRATION_SLOT_RIGHT_WEAPON_BEGIN,
		RightWeapon_2,
		RightWeapon_3,
		REGISTRATION_SLOT_RIGHT_WEAPON_END,

		Armor,

		Hat,
		Glasses,
		Mask,
		Neck,
		Top,
		Bottom,
		Gloves,
		Shoes,
		Back,

		MAX,
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRRegistrationSlotType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,					L"None");
		InsertMap(LeftWeapon_1,			L"LeftWeapon_1");
		InsertMap(LeftWeapon_2,			L"LeftWeapon_2");
		InsertMap(LeftWeapon_3,			L"LeftWeapon_3");
		InsertMap(RightWeapon_1,		L"RightWeapon_1");
		InsertMap(RightWeapon_2,		L"RightWeapon_2");
		InsertMap(RightWeapon_3,		L"RightWeapon_3");
		InsertMap(Armor,				L"Armor");
		InsertMap(Hat,					L"Hat");
		InsertMap(Glasses,				L"Glasses");
		InsertMap(Mask,					L"Mask");
		InsertMap(Neck,					L"Neck");
		InsertMap(Top,					L"Top");
		InsertMap(Bottom,				L"Bottom");
		InsertMap(Gloves,				L"Gloves");
		InsertMap(Shoes,				L"Shoes");
		InsertMap(Back,					L"Back");
	}

	inline static bool IsWeapon(EWRRegistrationSlotType::en eType) { return (IsLeftWeapon(eType) && IsRightWeapon(eType)) ? true : false; }
	inline static bool IsLeftWeapon(EWRRegistrationSlotType::en eType) { return (eType >= REGISTRATION_SLOT_LEFT_WEAPON_BEGIN && eType < REGISTRATION_SLOT_LEFT_WEAPON_END) ? true : false; }
	inline static bool IsRightWeapon(EWRRegistrationSlotType::en eType) { return (eType >= REGISTRATION_SLOT_RIGHT_WEAPON_BEGIN && eType < REGISTRATION_SLOT_RIGHT_WEAPON_END) ? true : false; }
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 성별정의
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRGender
{
#define class
#define EWRGender en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRGender : uint8
	{
		Male,
		Female,

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRGender

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Male,		L"Male");
		InsertMap(Female,	L"Female");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 다이얼로그 컨디션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRDialogueSetCoditionType
{
#define class
#define EWRDialogueSetCoditionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRDialogueSetCoditionType : uint8
	{
		None = 0,

		CompletedQuest,			// 선행 퀘스트 완료 여부
		ProgressingQuest,		// 해당퀘스트가 진행중인지
		HaveItem,				// 아이템 보유 여부

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRDialogueSetCoditionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,					L"None");
		InsertMap(CompletedQuest,		L"CompletedQuest");
		InsertMap(ProgressingQuest,		L"ProgressingQuest");
		InsertMap(HaveItem,				L"HaveItem");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 다이얼로그 액션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRDialogueActionType
{
#define class
#define EWRDialogueActionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRDialogueActionType : uint8
	{
		None = 0,

		ExcuteTrigger,			// 트리거 동작
		Tutorial,				// 튜토리얼 시작
		PlayAni,				// 애니 재생
		AcceptQuest,			// 퀘스트 지급
		ActiveLifeEvent,		// 일상 이벤트 아이템 삭제

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRDialogueActionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,					L"None");
		InsertMap(ExcuteTrigger,		L"ExcuteTrigger");
		InsertMap(Tutorial,				L"Tutorial");
		InsertMap(PlayAni,				L"PlayAni");
		InsertMap(AcceptQuest,			L"AcceptQuest");
		InsertMap(ActiveLifeEvent,		L"ActiveLifeEvent");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 다이얼로그 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRDialogueType
{
#define class
#define EWRDialogueType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRDialogueType : uint8
	{
		None = 0,

		MainQuest,
		NpcEventTalk,
		FeedbackTalk,
		TriggerNpcTalk,
		NpcIdleSpeech,
		NpcTalk,

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRDialogueType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,					L"None");
		InsertMap(MainQuest,			L"MainQuest");
		InsertMap(NpcEventTalk,			L"NpcEventTalk");
		InsertMap(FeedbackTalk,			L"FeedbackTalk");
		InsertMap(TriggerNpcTalk,		L"TriggerNpcTalk");
		InsertMap(NpcTalk,				L"NpcTalk");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 보상 스타일
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRRewardStyle
{
#define class
#define EWRRewardStyle en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRRewardStyle : uint8
	{
		None = 0,

		Variable,			// 변동 보상 (세카이 시스템 적용에따른...)
		Fixed,				// 고정 보상

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRRewardStyle

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,					L"None");
		InsertMap(Variable,				L"Variable");
		InsertMap(Fixed,				L"Fixed");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 보상 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRRewardType
{
#define class
#define EWRRewardType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRRewardType : uint8
	{
		None = 0,

		Item,
		Currency,

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRRewardType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(Item,				L"Item");
		InsertMap(Currency,			L"Currency");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 일상 이벤트 액션 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRLifeEventActionType
{
#define class
#define EWRLifeEventActionType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRLifeEventActionType : uint8
	{
		None = 0,

		DeleteItem,

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRLifeEventActionType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None,				L"None");
		InsertMap(DeleteItem,		L"DeleteItem");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- 상점 진열대 타입
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRShopDisplayStandType
{
#define class
#define EWRShopDisplayStandType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
	enum class EWRShopDisplayStandType : uint8
	{
		Sell = 0,		// 판매대
		Buy,			// 구매대

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRShopDisplayStandType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(Sell,				L"Sell");
		InsertMap(Buy,				L"Buy");
	}
};
#endif // USE_SERVER_ONLY

/*----------------------------
- IO Sub Type
------------------------------*/
#ifdef USE_SERVER_ONLY
struct EWRIOSubType
{
#define class
#define EWRIOSubType en
#endif // USE_SERVER_ONLY

	UENUM(BlueprintType)
		enum class EWRIOSubType : uint8
	{
		None = 0,
		Mic,

		MAX
	};

#ifdef USE_SERVER_ONLY
#undef class
#undef EWRIOSubType

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"None");
		InsertMap(Mic, L"Mic");
	}
};
#endif // USE_SERVER_ONLY