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


#pragma once
#include "../Protocol/IMMessage.h"
#include "Define_Type.h"

#ifdef USE_SERVER_ONLY
#else // USE_SERVER_ONLY
	#ifdef USE_PACKET_SAMPLE
		#include "CRandom.h"
	#else
		#include "Kismet/KismetMathLibrary.h"
	#endif // USE_PACKET_SAMPLE
#endif // USE_SERVER_ONLY
#include "Define_System.h"


#define MAX_FELLOW_ELEMENT_EFFECT_FOR_PC	5000	// 펠로우가 PC에게 적용 할 수 있는 최대 속성 저항 값.stat_element_value_forpc


//-----------------------------------------------------
//	최초 공격 유형 
//-----------------------------------------------------
struct EWRFirstAttackType
{
	enum en
	{
		None,			// 없음
		PC_First,		// 플레이어 선공
		NPC_First,		// NPC 선공
		AnyOne,			// 신경쓰지 않음.

		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,		L"*" );
		InsertMap( PC_First,	L"PC_First" );
		InsertMap( NPC_First,	L"NPC_First" );
		InsertMap( AnyOne,		L"AnyOne" );
	}
};


//-----------------------------------------------------
//	게임 내 Stat 옵션
//	... 주석된 항목은 기획 미정리 항목으로 차후 추가될 내용이니 삭제하지 마세요
//	... DB에 공유되는 수치임
//-----------------------------------------------------
struct EWRStatOptionType
{
	enum en
	{
		//	기본 능력
		None = 0,
		AtkAdd,
		DefAdd,
		HpAdd,

		AtkMul,
		DefMul,
		HpMul,

		MAX
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap( None,					L"*" );
		InsertMap( AtkAdd,					L"AtkAdd" );
		InsertMap( DefAdd,					L"DefAdd" );
		InsertMap( HpAdd,					L"HpAdd" );

		InsertMap( AtkMul,					L"AtkMul" );
		InsertMap( DefMul,					L"DefMul" );
		InsertMap( HpMul,					L"HpMul" );
	}

#ifdef USE_SERVER_ONLY
	static bool CheckStatOptionValid( const TID_t stat_option_tid )
	{
		std::wstring s = EWRStatOptionType::GetStr(( EWRStatOptionType::en )stat_option_tid);
		if ( 0 == s.compare(L"Invalid") )
		{
			return false;
		}
		return true;
	}
#endif
};

struct EWRCCState
{
	enum en
	{
		None = 0,
		Weak, // 약점 노출
		MAX,
	};

	DeclareMapTable();
	static inline void BuildMap()
	{
		InsertMap(None, L"0");
		InsertMap(Weak, L"Weak");
	}
};

//-----------------------------------------------------
//	전투 공식 관련 define
//-----------------------------------------------------

typedef		int32							WEAPON_ATK_t;				// 무기 공격력
typedef		int32							TOTAL_ATK_t;				// 종합 공격력
typedef		int32							BASE_DAMAGE_t;				// 기본 피해량

typedef		int32							TOTAL_DEF_t;				// 종합 방어력

typedef		int32							CRITICAL_RATE_t;			// 치명타 확률
//#define     MAX_CRITICAL_RATE				7500						// 치명타 확률 최대값 (만분율)

typedef     int32							CRITICAL_DMG_REVISE_t;		// 치명타 피해량 보정
#define		DEFAULT_CRITICAL_DMG_REVISE		150							// 치명타 피해량 보정 기본값
#define		MIN_CRITICAL_DMG_REVISE			120							// 치명타 피해량 보정 최소값
#define		MAX_CRITICAL_DMG_REVISE			200							// 치명타 피해량 보정 최대값

typedef		int32							ELEMENT_RATE_t;				// 원소 속성 적용 확률

typedef		int32							ELEMENT_DMG_t;				// 원소 속성 피해량 보정
#define		ELEMENT_RATE_REVISE				5000						// 원소 속성 피해량 적용 확률 보정치
#define		DEFAULT_ELEMENT_DMG				2000						// 원소 속성 피해량 보정 기본값
#define		IS_DECREASE_ELEMENT_DMG			-1							// 원소 속성 피해량 감소 적용 여부
#define		DECREASE_ELEMENT_DMG			-20							// 원소 속성 피해량 감소값

typedef		int32							CONTENT_REVISE_t;			// 컨텐츠 보정

typedef		int32							DMG_DECREASE_t;				// 방어자 피해량 감소

typedef		int32							PVP_REVISE_t;				// PVP 보정

typedef		int32							DEF_IGNORE_RATE_t;			// 방어력 무시 확률
#define		MAX_DEF_IGNORE_VALUE			2000						// 방어력 무시 최대 값 (만분율)
#define		MAX_DEF_IGNORE_RROB				7500						// 방어력 무시 최대 확률 (만분율)

typedef		int32							FINAL_DMG_t;				// 최종 피해량


//-----------------------------------------------------
//	전투용 보정 수치
//-----------------------------------------------------

#define		ICM_BATTLE_REVISE_ATTACK_RATE_BASE_10T		12500.0f		//	공격력 보정% - 기본

#define		ICM_BATTLE_REVISE_DEFENCE_RATE_MIN_10T		0.0f			//	피해량 감소% - 최소
#define		ICM_BATTLE_REVISE_DEFENCE_RATE_MAX_10T		9000.0f			//	피해량 감소% - 최대

//	[SHYOON] [2017/08/11] : 기획 보류
// #define		ICM_BATTLE_REVISE_CONTENT_RATE_MIN		0.0f			//	컨텐츠 보정% - 최소
// #define		ICM_BATTLE_REVISE_CONTENT_RATE_MAX		1.0f			//	컨텐츠 보정% - 최대

#define		ICM_BATTLE_REVISE_ATTRIBUTE_RATE_BASE		0.2f			//	속성 피해량 보정% - 기본

//	치명타
#define		ICM_BATTLE_REVISE_CRITICAL_DMG_BASE_10T		5000.0f			//	치명 피해량 보정% - 기본
#define		ICM_BATTLE_REVISE_CRITICAL_DMG_MIN_10T		2000.0f			//	치명 피해량 보정% - 최소
#define		ICM_BATTLE_REVISE_CRITICAL_DMG_MAX_10T		10000.0f		//	치명 피해량 보정% - 최대

#define		ICM_BATTLE_REVISE_CRITICAL_RATE_MIN_10T		0.0f			//	치명율 보정% - 최소
#define		ICM_BATTLE_REVISE_CRITICAL_RATE_MAX_10T		7500.0f			//	치명율 보정% - 최대

//	방어 무시
#define		ICM_BATTLE_REVISE_PEN_DMG_BASE_10T			2000.0f			//	방어 무시 - 기본
#define		ICM_BATTLE_REVISE_PEN_DMG_MIN_10T			0.0f			//	방어 무시 - 최소
#define		ICM_BATTLE_REVISE_PEN_DMG_MAX_10T			7500.0f			//	방어 무시 - 최대

#define		ICM_BATTLE_REVISE_PEN_RATE_DEFAULT_10T		7000.0f			//	방어 무시 보정% - 기본 적용

#define		ICM_BATTLE_REVISE_PEN_RATE_BASE_10T			2000.0f			//	방어 무시 보정% - 기본
#define		ICM_BATTLE_REVISE_PEN_RATE_MIN_10T			0.0f			//	방어 무시 - 최소
#define		ICM_BATTLE_REVISE_PEN_RATE_MAX_10T			7500.0f			//	방어 무시 - 최대

//#define		ICM_BATTLE_REVISE_PVP_RATE_BASE_10T			4500.0f			//	PVP 보정% -  기본
//#define		ICM_BATTLE_REVISE_PVP_RATE_MIN_10T			3000.0f			//	PVP 보정% - 최소
//#define		ICM_BATTLE_REVISE_PVP_RATE_MIN_10T			1000.0f			//	PVP 보정% - 최소 => 이장원 : 2018.07.17
#define		ICM_BATTLE_REVISE_PVP_RATE_BASE_10T			2000.0f			//	PVP 보정% - 기본 => 이장원 : 2018.07.17
#define		ICM_BATTLE_REVISE_PVP_RATE_MIN_10T			500.0f			//	PVP 보정% - 최소 => 이장원 : 2018.09.12
#define		ICM_BATTLE_REVISE_PVP_RATE_MAX_10T			6000.0f			//	PVP 보정% - 최대

#define		ICM_BATTLE_REVISE_PVP_DMAGE_BY_POWER		250.0f			//	pvp 시 전투력에 대한 보정 (npc 미적용)

// #define		CRI_RATE_MIN					3000.0f						//	치명타 보정 최소
// #define		CRI_RATE_DEFAULT				5000.0f						//	치명타 보정 기본값
// #define		CRI_RATE_MAX					10000.0f					//	치명타 보정 최대
// 

#define		DEFENCE_RATE_REVISE_MIN			1000.0f						//	방어자 피해량 감소 보정

// #define		PVP_RATE_REVISE_MIN				3000.0f						//	PVP 보정 최소
// #define		PVP_RATE_REVISE_DEFAULT			4500.0f						//	PVP 보정 기본
// #define		PVP_RATE_REVISE_MAX				6000.0f						//	PVP 보정 최대

#define		AUTOTHERAPY_HP_RATE				0.02f						//	HP 자연 회복율

#define		ICM_BATTLE_REVISE_BATTLE_POWER_DEVIATION		2000		// 전투력 편차 만분율
#define		ICM_BATTLE_REVISE_BATTLE_POWER_COMP_VALUE		100			// 이장원 전투력 편차 보정

#define		INVALID_REACTION_TID							99999		// 오관용 Damage애님을 하지않는 NotReaction.

#define		TARGET_INIT_EXTRA_AGGRO_POINT 20.0f		// 최초 타겟으로 성정했을 경우 extra(휘발성) 어그로 포인트
#define		DEC_COMMON_AGGRO_RATE 20.0f				// 각 1회 공격했을 경우 감소되는 어그로 퍼센트
#define		DEC_EXTRA_AGGRO_RATE 90.0f				// 각 3회 공격했을 경우 감소되는 어그로 퍼센트

#define		BATTLE_MAINTAIN_SEC		10				//전투 유지 시간(초)


// 어떤 스킬을 특별한 타겟을 대상으로 사용하고 싶을때 
struct EIMTargetParam
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

struct stDamageInfo
{
	TID_t			skillTID;
	AID_t			targetAID;
	SKILL_SEQ_t		battleUID;
	uint8			hitIndex;
	DMG_t			totalDamage;
	bool			bTotalCri;			// 치명타
	bool			bPen;				// 방어무시(강타 - strike)
	bool			bElement;			// 속성 공격

	TID_t			reactionId;
	uint8			acc_reaction_cnt; 

	stDamageInfo() : skillTID(INVALID_TID), targetAID(INVALID_AID), battleUID(INVALID_SKILL_SEQ), hitIndex(0xff), totalDamage(0),
		bTotalCri(false), bPen(false), bElement(false), reactionId(INVALID_TID), acc_reaction_cnt(0)
	{
	}
};


#ifdef USE_SERVER_ONLY
#define WR_MAX(a, b)			max(a, b)
#define WR_RANDOM(a, b)			CRandom::GetRandomValue(a, b)
#else
#define WR_MAX(a, b)			UKismetMathLibrary::Max(a, b)
#define WR_RANDOM(a, b)			UKismetMathLibrary::RandomIntegerInRange(a, b)
#endif




namespace WXR_BattleFunc
{
	//-----------------------------------------------------
	//	<기본 규칙>
	//	- 모든 공식 연산시 최종 피해량을 제외한 나머지 연산에서는 소수점 두번째 자리이하의 수는 반올림 처리
	//-----------------------------------------------------

	//	최종 피해량 = (기본 피해량 * (치명타 피해량 보정 + (1 + 속성 피해량 보정 + 컨텐츠 보정의 합))) * (1 - 방어자 피해량 감소) * PVP보정
	static const DMG_t CalcFinalDamage(	const DMG_t			baseDmg,					// 기본 피해량
										const DMG_RATE_t	criticalReviseDmgRate,		// 치명타 피해량 보정
										const DMG_t			elementReviseDmg,			// 속성 피해량 보정
										const DMG_RATE_t	contentReviseDmgRate,		// 컨텐츠 보정 합
										const DMG_RATE_t	decreaseDmgRate,			// 방어자 피해량 감소
										const DMG_RATE_t	pvpReviseDmgRate,			// pvp 보정
										const DMG_t			maxFixedDamage,				// 최대 고정 데미지
										const DMG_RATE_t	deviationDamageRate,		// 최대 고정 데미지 편차
										const POWER_t		attackerPower,				// 공격자의 전투력
										const DMG_t			skillDmg
		)			
	{
		//	기본 피해량 기준임
		//if( baseDmg <= 0 )
		//{
		//	//	기본 데미지는 1임
		//	return 1;
		//}

		//	각 Rate 항목 0보다 작으면 적용 안함

		DMG_RATE_t	reviseDmgRate	= WXR_PER_FRACTION;

		//	치명타 피해량 보정
		if ( criticalReviseDmgRate > 0 )
			reviseDmgRate += criticalReviseDmgRate;

		//	속성 피해량 보정
		//if ( elementReviseDmgRate > 0 )
		//	reviseDmgRate += elementReviseDmgRate;

		//	컨텐츠 보정 합
		if ( contentReviseDmgRate > 0 )
			reviseDmgRate += contentReviseDmgRate;

		//// pvp 보정 => 장원이가 아래로 내리라고 했음
		//if ( pvpReviseDmgRate > 0 )
		//	reviseDmgRate += pvpReviseDmgRate;

 		//	방어 보정
 		DMG_RATE_t	tmpDecreaseDmgRate = WXR_PER_FRACTION;

		if ( decreaseDmgRate > 0 )
		{
			tmpDecreaseDmgRate = WXR_PER_FRACTION - decreaseDmgRate;
			if ( tmpDecreaseDmgRate <= ICM_BATTLE_REVISE_DEFENCE_RATE_MIN_10T )
			{
				tmpDecreaseDmgRate = ICM_BATTLE_REVISE_DEFENCE_RATE_MIN_10T;
			}
			else if ( tmpDecreaseDmgRate >= ICM_BATTLE_REVISE_DEFENCE_RATE_MAX_10T )
			{
				tmpDecreaseDmgRate = ICM_BATTLE_REVISE_DEFENCE_RATE_MAX_10T;
			}
		}

		DMG_t resultDmg = (DMG_t)( baseDmg * ( reviseDmgRate / WXR_PER_FRACTION ) * ( tmpDecreaseDmgRate / WXR_PER_FRACTION ) + 0.5f );
		//if ( resultDmg <= 0 )
		//{
		//	return 1;
		//}
		if ( resultDmg <= 0 )
		{
			resultDmg = 0;
		}

		// pvp 보정 데미지를 줄인다. => 2018.01.31 이장원과 함께 고침
		if ( pvpReviseDmgRate > 0 )
			resultDmg = (DMG_t)( resultDmg * (pvpReviseDmgRate / float(WXR_PER_FRACTION)) );

		// 고정 데미지 구하기
		DMG_t fixedDamage = 0;

		DMG_t minDamage = (DMG_t)(((DMG_RATE_t)maxFixedDamage * ( WR_MAX(1, (int32)deviationDamageRate)) / WXR_PER_FRACTION ) + 0.5f );//최소 고정 데미지 = 100 * ( 5000 * 0.0001 ) = 50
		if ( minDamage > maxFixedDamage )
		{
			fixedDamage = minDamage;
		}
		else
		{
			fixedDamage = WR_RANDOM(minDamage, maxFixedDamage);
		}

		// 대상이 플레이어 일 경우 " 전투력 * 0.025 * 스킬 계수 " 고정 피해 추가
		fixedDamage = fixedDamage + (DMG_t)( (float)attackerPower * ((float)ICM_BATTLE_REVISE_PVP_DMAGE_BY_POWER/(float)WXR_PER_FRACTION) * ((float)skillDmg/(float)WXR_PER_FRACTION) );

		// 최종 데미지에 고정 데미지를 더한다.
		// + elementReviseDmgRate  => https://docs.google.com/presentation/d/1UOZBvEKXUVXsEN7vPYP9fXgXnSfApxJ0oTlqIYH_ctI/edit#slide=id.g393f3ac47a_0_15
		// 2018.05.03 with 이장원
		resultDmg += (fixedDamage + elementReviseDmg);
		if ( resultDmg <= 0 )
			return 1;

		return resultDmg;
	}


	//	기본 피해량 = ( ( (종합 공격력*1.25 ) - 종합 방어력 ) * ( 1 + 공격자 스킬 공격력) )
	static const DMG_t CalcBaseDamage(const ATK_t			totalAtk_Attacker,		//	종합 공격력 ... ( 최소, 최대 범위 이내 랜덤 수치 )
		const DEF_t			totalDef_Defender,		//	방어자 방어력
		const DMG_t			atkDmg,					// 공격 계수 증가
		const DMG_RATE_t	skillDmgRate_Attacker,	//	스킬 증폭률
		const DMG_RATE_t	skillDmgMul,			// 스킬 계수 증가 => 증폭률의 추가 %
		const DMG_RATE_t	penDmg,					//	방어무시 피해 증가
		const DMG_RATE_t	penDmgResi,				//	방어무시 피해 감소
		const bool			bPen)					//	방어무시 적용 여부
	{
		if (totalAtk_Attacker <= 0)
			return 0;

		if (skillDmgRate_Attacker <= 0)
			return 0;

		DMG_t resultDmg = 0;

		if (bPen)
		{
			resultDmg = (DMG_t)(((DMG_RATE_t)totalAtk_Attacker * (ICM_BATTLE_REVISE_ATTACK_RATE_BASE_10T + atkDmg)) - ((DMG_RATE_t)totalDef_Defender * (ICM_BATTLE_REVISE_PEN_RATE_DEFAULT_10T + penDmg - penDmgResi)));
		}
		else
		{
			resultDmg = (DMG_t)(((DMG_RATE_t)totalAtk_Attacker * (ICM_BATTLE_REVISE_ATTACK_RATE_BASE_10T + atkDmg)) - (DMG_RATE_t)totalDef_Defender * WXR_PER_FRACTION);
		}

		if (resultDmg <= 0)
			return 0;

		DMG_RATE_t skill_factor = (skillDmgRate_Attacker / WXR_PER_FRACTION); // 스킬 계수
		resultDmg = (DMG_t)(((DMG_RATE_t)resultDmg / WXR_PER_FRACTION) * ((1 + skillDmgMul / WXR_PER_FRACTION) * skill_factor) + 0.5f);

		return resultDmg;
	}

	//	종합 공격력 = ( PC 공격력 + 무기 공격력+ 아이템1 + 스킬1 ) * ( 1 + 아이템 2 + 스킬2 )
	static const ATK_t CalcTotalAtk( const ATK_t		atk_Attacker,			//	공격력 합
									 const DMG_RATE_t	atkRate_Attacker )		//	공격력 증폭률 합 ... 만분율 ( body + item )
	{
		DMG_RATE_t	atkDmgRate	= (DMG_RATE_t)( atkRate_Attacker + WXR_PER_FRACTION ) / WXR_PER_FRACTION;

		return (ATK_t)( (float)atk_Attacker * atkDmgRate + 0.5f );
	}

	//	종합 방어력 = (방어자 기본 방어력 + 아이템1 + 스킬1) * (1 + 아이템2 + 스킬2)
	static const DEF_t CalcTotalDef( const DEF_t		def_Defender,				//	방어력 합 ... ( body + item ...  )
									 const DMG_RATE_t	defRate_Defender )			//	방어력 증폭률 합 ... 만분율 ( body + item )
	{
		DMG_RATE_t	defDmgRate	= (DMG_RATE_t)( defRate_Defender + WXR_PER_FRACTION ) / (DMG_RATE_t)WXR_PER_FRACTION;

		return (DEF_t)( (float)def_Defender * defDmgRate + 0.5f );
	}

	//	최종 체력 :	( 체력 * ( 1 + ( 체력% * 0.0001 ) ) ) * ( 1 + 컨텐츠 보정% )
	static const DEF_t CalcTotalHP( const HP_t		hp_Defender,				//	체력 합 ... ( body + item ...  )
									const HP_RATE_t	hpRate_Defender )			//	체력 증폭률 합 ... 만분율 ( body + item )
	{
		DMG_RATE_t	hpDmgRate = (DMG_RATE_t)( hpRate_Defender + WXR_PER_FRACTION ) / (DMG_RATE_t)WXR_PER_FRACTION;

		return (DEF_t)( (float)hp_Defender * hpDmgRate + 0.5f );
	}

	//	방어 무시 연산(penetrate) =	( 2000 + ( 공격자 방어 무시 - 방어자 방어 무시 저항 ) ) * 0.0001
	static const bool IsCalcPenRate(	const PEN_RATE_t	penRate_Attacker,
										const PEN_RESI_t	penRateResi_Defender )
	{
		CRI_RATE_t penRate = (CRI_RATE_t)( penRate_Attacker - penRateResi_Defender );

		if ( penRate <= ICM_BATTLE_REVISE_PEN_RATE_MIN_10T )	// min 값 0%
		{
			penRate = (CRI_RATE_t)ICM_BATTLE_REVISE_PEN_RATE_MIN_10T;
		}
		else if ( penRate >= ICM_BATTLE_REVISE_PEN_RATE_MAX_10T )	// max 값 75%
		{
			penRate = (CRI_RATE_t)ICM_BATTLE_REVISE_PEN_RATE_MAX_10T;
		}

		int32 randPenRate = WR_RANDOM( (int)0, (int)WXR_PER_FRACTION );
		if (randPenRate <= penRate)
		{
			return true;
		}

		return false;
	}


	//	방어 무시 = ( ( 방어무시 피해량 * ( 1 + 컨텐츠 보정% ) ) * ( 1 - 방어자 피해량 감소% ) ) * ( 1 - PvP 보정% )
	static DMG_RATE_t CalcPenDmgRate(	const DMG_t penDmg_Attacker,				//	방어무시 피해량 ( body + item ... )
										const DMG_t skillPenDmg_Attacker,			//	스킬 방어무시 피해량
										const DMG_t penDmgResi_Defender )			//	방어자 방어무시 감소 ( body + item ...  )
	{
		DMG_RATE_t tmpDmg = (DMG_RATE_t)( ( penDmg_Attacker + skillPenDmg_Attacker ) * ( WXR_PER_FRACTION - penDmgResi_Defender ) );

		if ( ICM_BATTLE_REVISE_PEN_DMG_MIN_10T >= tmpDmg )
		{
			return ICM_BATTLE_REVISE_PEN_DMG_MIN_10T;
		}
		else if ( ICM_BATTLE_REVISE_PEN_DMG_MAX_10T <= tmpDmg )
		{
			return ICM_BATTLE_REVISE_PEN_DMG_MAX_10T;
		}

		//	결과 : 10000.0f 형태
		return tmpDmg;
	}

	//	치명타 발생 여부 = (공격자 치명타율 + 아이템2 + 스킬2) –(방어자 치명타 저항 + 아이템2 + 스킬2)
	static const bool IsCalcCriRate(	const CRI_RATE_t atkCriRate_Attacker,			//	공격 치타율
										const CRI_RATE_t skillCriRate_Attacker,		//	스킬 치타율
										const CRI_RESI_t atkCriResi_Defender )			//	공격 치타 저항율
	{
		CRI_RATE_t criRate = (CRI_RATE_t)( ( atkCriRate_Attacker + skillCriRate_Attacker ) - atkCriResi_Defender );
		//ICM_BATTLE_REVISE_CRITICAL_RATE_BASE_10T
		if( criRate <= ICM_BATTLE_REVISE_CRITICAL_RATE_MIN_10T )
		{
			criRate = (CRI_RATE_t)ICM_BATTLE_REVISE_CRITICAL_RATE_MIN_10T;
		}
		else if ( criRate > ICM_BATTLE_REVISE_CRITICAL_RATE_MAX_10T )
		{
			criRate = (CRI_RATE_t)ICM_BATTLE_REVISE_CRITICAL_RATE_MAX_10T;
		}

		int32 randCriRate = WR_RANDOM( (int)0, (int)WXR_PER_FRACTION );

		if (randCriRate <= criRate)
		{
			return true;
		}

		return false;
	}


	//	치명타 피해량 보정 = 0.5 + ((공격자 치명타 피해 증가 + 아이템2 + 스킬2) – (방어자 치명타 피해 감소 + 아이템2 + 스킬2))
	static DMG_RATE_t CalcCriDmgRate(	const DMG_RATE_t atkCriDmgRate_Attacker,				//	공격 치타 데미지 ( body + item ... )
										const DMG_RATE_t skillCriDmgRate_Attacker,			//	스킬 치타 데미지
										const DMG_RATE_t atkCriDmgResiRate_Defender )		//	공격 치타 데미지 저항 ( body + item ...  )
	{
		DMG_RATE_t tmpDmgRate = ICM_BATTLE_REVISE_CRITICAL_DMG_BASE_10T + ( atkCriDmgRate_Attacker + skillCriDmgRate_Attacker ) - atkCriDmgResiRate_Defender;

		if ( ICM_BATTLE_REVISE_CRITICAL_DMG_MIN_10T >= tmpDmgRate )
		{
			return ICM_BATTLE_REVISE_CRITICAL_DMG_MIN_10T;
		}
		else if ( ICM_BATTLE_REVISE_CRITICAL_DMG_MAX_10T <= tmpDmgRate )
		{
			return ICM_BATTLE_REVISE_CRITICAL_DMG_MAX_10T;
		}

		//	결과 : 10000.0f 형태
		return tmpDmgRate;
	}

	//	PVP 보정 = 0.45 + ( 공격자 PVP공격력의 합 - 방어자 PVP방어력의 합 )
	static DMG_RATE_t CalcPvpRate(	const DMG_RATE_t pvpDmgAdd,					//	PVP 공격력
									const DMG_RATE_t pvpDmgResi)					//	PVP 방어율
	{
		DMG_RATE_t resultDmg = (DMG_RATE_t)ICM_BATTLE_REVISE_PVP_RATE_BASE_10T + pvpDmgAdd - pvpDmgResi;
		
		if( resultDmg <= ICM_BATTLE_REVISE_PVP_RATE_MIN_10T )
		{
			return ICM_BATTLE_REVISE_PVP_RATE_MIN_10T;
		}
		else if ( resultDmg >= ICM_BATTLE_REVISE_PVP_RATE_MAX_10T )
		{
			return ICM_BATTLE_REVISE_PVP_RATE_MAX_10T;
		}

		//	결과 : 10000.0f 형태
		return resultDmg;
	}
	
	//	1회 자연 회복 HP
	static HP_t CalcAutotherapy( const HP_t mapHP, const int32 autotherapyhp)
	{
		if ( 0 >= mapHP )
			return 0;

		//return static_cast<HP_t>( (float)( mapHP * AUTOTHERAPY_HP_RATE ) + 0.5f );
		return static_cast<HP_t>( (float)( mapHP * autotherapyhp / WXR_PER_FRACTION ) + 0.5f );
	}
};


//-----------------------------------------------------
//	 기본 공방, hp (혼합)
//-----------------------------------------------------
struct stStatBasic
{
	HP_t					maxHP;					//	최대 체력
	ATK_t					atk;					//	공격력
	DEF_t					def;					//	방어력

	stStatBasic()
	:	maxHP( 0 )
	,	atk( 0 )
	,	def( 0 )
	{
	}

	void Clear()
	{
		maxHP	= 0;
		atk		= 0;
		def		= 0;
	}

	void Copy( stStatBasic* pStatBasic )
	{
		maxHP				= pStatBasic->maxHP;
		atk					= pStatBasic->atk;
		def					= pStatBasic->def;
	}

	void Add( stStatBasic* pStatBasic )
	{
		maxHP				+= pStatBasic->maxHP;
		atk					+= pStatBasic->atk;
		def					+= pStatBasic->def;
	}

	void Mul( stStatBasic* pStatBasic )
	{
		if( pStatBasic->maxHP > 0 )
		{	
			maxHP += (HP_t)( ( (double)( maxHP * pStatBasic->maxHP ) / WXR_PER_FRACTION) + 0.5f );
		}

		if( pStatBasic->atk	> 0 )
		{	
			atk += (ATK_t)( ( (double)(	atk * pStatBasic->atk ) / WXR_PER_FRACTION ) + 0.5f );
		}

		if( pStatBasic->def > 0 )
		{	
			def += (DEF_t)( ( (double)( def * pStatBasic->def ) / WXR_PER_FRACTION ) + 0.5f );
		}
	}

	bool Equal( stStatBasic* pStatBasic )
	{
		if( maxHP != pStatBasic->maxHP )
			return false;
		if (atk != pStatBasic->atk)
			return false;
		if (def != pStatBasic->def)
			return false;
		return true;
	}
};

//-----------------------------------------------------
//	보조 스탯 ( +, 10000 )
//-----------------------------------------------------
struct stStatAssist
{
	STAT_t stat_value[EWRStatOptionType::MAX];

	stStatAssist()
	{
		for (int32 i = 0; i < (int32)EWRStatOptionType::MAX; ++i)
			stat_value[i] = 0;
	}

	void Clear()
	{
		for (int32 i = 0; i < (int32)EWRStatOptionType::MAX; ++i)
			stat_value[i] = 0;
	}

	void Copy( stStatAssist* pStatAssist )
	{
		for (int32 i = 0; i < (int32)EWRStatOptionType::MAX; ++i)
			stat_value[i] = pStatAssist->stat_value[i];
	}
	
	void Add( stStatAssist* pStatAssist )
	{
		for (int32 i = 0; i < (int32)EWRStatOptionType::MAX; ++i)
			stat_value[i] += pStatAssist->stat_value[i];
	}

	bool Equal( stStatAssist* pStatAssist )
	{
		for (int32 i = 0; i < (int32)EWRStatOptionType::MAX; ++i)
		{
			if (stat_value[i] != pStatAssist->stat_value[i])
				return false;
		}

		return true;
	}

	STAT_t GetOptionValue(EWRStatOptionType::en option)
	{
		if (option >= EWRStatOptionType::MAX)
			return 0;

		if (option < EWRStatOptionType::AtkAdd)
			return 0;

		return stat_value[option];
	}
};


//-----------------------------------------------------
//	 이동, 거리 등 StatOption 테이블과 관계없는 항목
//-----------------------------------------------------
struct stStatMovement
{
	SPEED_t					walkSpeed;				//	이동속도 - 걷기
	SPEED_t					runSpeed;				//	이동속도 - 뛰기
	SPEED_t					backSpeed;				//	후퇴 속도

	stStatMovement()
	:	walkSpeed( 0 )
	,	runSpeed( 0 )
	,	backSpeed( 0 )
	{
	}

	void Clear()
	{
		walkSpeed			= 0;
		runSpeed			= 0;
		backSpeed			= 0;
	}

	void Copy( stStatMovement* pStatMovement )
	{
		walkSpeed			= pStatMovement->walkSpeed;
		runSpeed			= pStatMovement->runSpeed;
		backSpeed			= pStatMovement->backSpeed;
	}

	void Add( stStatMovement* pStatMovement )
	{
		walkSpeed			+= pStatMovement->walkSpeed;
		runSpeed			+= pStatMovement->runSpeed;
		backSpeed			+= pStatMovement->backSpeed;
	}

	bool Equal(stStatMovement* pStatMovement)
	{
		if (walkSpeed != pStatMovement->walkSpeed)
			return false;
		if (runSpeed != pStatMovement->runSpeed)
			return false;
		if (backSpeed != pStatMovement->backSpeed)
			return false;
		return true;
	}
};

//-----------------------------------------------------
//	스탯 기본 정보
//-----------------------------------------------------
struct stStatGroupBase
{
	stStatBasic				statBasic;				//	기본
	stStatAssist			statAssist;				//	보조
	stStatMovement			statMovement;			//	이동 관련

	stStatGroupBase()
	{
		Clear();
	}

	void	Clear()
	{
		statBasic.Clear();
		statAssist.Clear();
		statMovement.Clear();
	}

	void	Copy( stStatGroupBase* pStatGroupBase )
	{
		statBasic.Copy( &(pStatGroupBase->statBasic) );
		statAssist.Copy( &(pStatGroupBase->statAssist) );
		statMovement.Copy( &(pStatGroupBase->statMovement) );
	}

	void	Add( stStatGroupBase* pStatGroupBase )
	{
		statBasic.Add( &pStatGroupBase->statBasic );
		statAssist.Add( &pStatGroupBase->statAssist );
		statMovement.Add( &pStatGroupBase->statMovement );
	}

	bool Equal( stStatGroupBase* pStatGroupBase )
	{
		if (statBasic.Equal(&pStatGroupBase->statBasic)		== false)	return false;
		if (statAssist.Equal(&pStatGroupBase->statAssist)	== false)	return false;
		if (statMovement.Equal(&pStatGroupBase->statMovement) == false)	return false;
		return true;
	}
};


//-----------------------------------------------------
//	(최종 결합 전) 아이템 결합
//-----------------------------------------------------
struct stStatGroupSupply
{
	stStatBasic				statBasicAdd;			//	착용 아이템 능력치 총합 ( + , 1 )
	stStatBasic				statBasicMul;			//	착용 아이템 능력치 총합 ( x , 10000 )
	stStatBasic				statBasicTotal;			//	Worked 경우 최종 결산 ... 다른 경우 사용 안함

	stStatAssist			statAssist;				//	보조 ( + , 10000 )
	stStatMovement			statMovement;			//	이동 관련

	stStatGroupSupply()
	{
		Clear();
	}

	void	Clear()
	{
		statBasicAdd.Clear();
		statBasicMul.Clear();
		statBasicTotal.Clear();

		statAssist.Clear();
		statMovement.Clear();
	}

	void	Copy( stStatGroupSupply* pStatGroupSrc )
	{
#ifdef	USE_SERVER_ONLY
		VERIFY_RETURN( pStatGroupSrc );
#endif	//	USE_SERVER_ONLY

		statBasicAdd.Copy( &( pStatGroupSrc->statBasicAdd ) );
		statBasicMul.Copy( &( pStatGroupSrc->statBasicMul ) );
		statBasicTotal.Copy( &( pStatGroupSrc->statBasicTotal ) );

		statAssist.Copy( &( pStatGroupSrc->statAssist ) );
 		statMovement.Copy( &( pStatGroupSrc->statMovement ) );
	}

	bool Equal( stStatGroupSupply* pStatGroupSrc )
	{
		if ( statBasicAdd.Equal( &( pStatGroupSrc->statBasicAdd ) ) == false )		return false;
		if ( statBasicMul.Equal( &( pStatGroupSrc->statBasicMul ) ) == false )		return false;
		if ( statBasicTotal.Equal( &( pStatGroupSrc->statBasicTotal ) ) == false )	return false;
		if ( statAssist.Equal( &( pStatGroupSrc->statAssist ) ) == false )			return false;
		if ( statMovement.Equal( &(pStatGroupSrc->statMovement) ) == false)			return false;

		return true;
	}

	void	AddBody( stStatGroupBase* pStatGroupBody )
	{
		if ( nullptr == pStatGroupBody )
			return;

		statBasicAdd.Add( &( pStatGroupBody->statBasic ) );
		statAssist.Add( &( pStatGroupBody->statAssist ) );
 		statMovement.Add( &( pStatGroupBody->statMovement ) );
	}

	void	AddSupply( stStatGroupSupply* pStatGroupSupply )
	{
		if ( nullptr == pStatGroupSupply )
			return;

		statBasicAdd.Add( &( pStatGroupSupply->statBasicAdd ) );
		statBasicMul.Add( &( pStatGroupSupply->statBasicMul ) );
		statBasicTotal.Add( &( pStatGroupSupply->statBasicTotal ) );

		statAssist.Add( &( pStatGroupSupply->statAssist ) );
 		statMovement.Add( &( pStatGroupSupply->statMovement ) );
	}

	const stStatBasic&	MergeForPower( stStatGroupBase* pStatGroupBody, std::vector< stStatGroupSupply* >& rListStatGroup )
	{
		Clear();
		AddBody( pStatGroupBody );

		for( auto itr = rListStatGroup.begin(); itr != rListStatGroup.end(); ++itr )
		{
			AddSupply( *itr );
		}

		ApplyBasicTotal();

		return statBasicTotal;
	}

	void	ApplyBasicTotal()
	{
		statBasicTotal.atk		= WXR_BattleFunc::CalcTotalAtk(statBasicAdd.atk, (DMG_RATE_t)statBasicMul.atk);
		statBasicTotal.def		= WXR_BattleFunc::CalcTotalDef(statBasicAdd.def, (DMG_RATE_t)statBasicMul.def);
		statBasicTotal.maxHP	= WXR_BattleFunc::CalcTotalHP(statBasicAdd.maxHP, (DMG_RATE_t)statBasicMul.maxHP);
	}

	void	MergeTotal( stStatGroupSupply* pStatFightingPower, stStatGroupSupply* pStatGroupBuff )
	{
		if( nullptr != pStatFightingPower )
		{
			//	이 값이 있는 경우 pStatFightingPower + pStatGroupBuff 개념
			//	없을 경우 현재 구조체 + pStatGroupBuff 개념
			Clear();
		}

		AddSupply(pStatFightingPower);
		AddSupply(pStatGroupBuff);

		//	결산
		//statBasicTotal.Copy(&statBasicAdd);

		ApplyBasicTotal();

		//if(nullptr != pStatFightingPower )
		//	pStatFightingPower->statBasicTotal.Copy(&statBasicTotal);

	}
};



