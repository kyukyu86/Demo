// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "CoreMinimal.h"
#include "Define/WRTypeDefine.h"
#include "Table/WRSkillEffectParamTable.h"
#include "Network/Share/Define/Define_Type.h"
#include "Network/Share/Define/Define_Skill.h"


struct FWRBuff
{
	FWRBuff() : 
		Activate(false), 
		BuffTID(INVALID_TABLE_TID), 
		pBuffTable(nullptr), 
		Stack(0), 
		MaxStack(0), 
		RemainTime(0.f),
		SkillSeq(INVALID_SKILL_SEQ), 
		Level(0), 
		CasterAID(INVALID_AID)
	{

	}
	FWRBuff(const stBuffInfo& v) :
		Activate(v.isActiveBuff),
		BuffTID(v.buffTid),
		pBuffTable(nullptr),
		Stack(v.stack), 
		MaxStack(0),
		RemainTime(v.remain_tm),
		SkillSeq(v.skillSeq),
		Level(v.level),
		CasterAID(v.caster_aid)
	{

	}
	bool operator == (const FWRBuff& v) const
	{
		return BuffTID == v.BuffTID;
	}

public:
	bool Activate;

	WRTableID BuffTID;
	FWRBuffTable* pBuffTable;
	int32 Stack;
	int32 MaxStack;
	float RemainTime;

	SKILL_SEQ_t SkillSeq;
	uint8 Level;
	AID_t CasterAID;
};



class WR_API WRBuffManager : public WRSingleton<WRBuffManager>
{
	CDECLARE_MANAGER(WRBuffManager);
	
private:
	WRBuffManager();
	virtual ~WRBuffManager();
	
public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================

	// to Global
	void SetBuff(const AID_t IN InAID, const std::list<stBuffInfo>& IN InBuffList);
	void SetBuff(class AWRCharacter* IN InChar, const std::list<stBuffInfo>& IN InBuffList);

	void AddBuff(const AID_t IN InAID, const stBuffInfo& IN InBuffInfo);
	void AddBuff(class AWRCharacter* IN InChar, const stBuffInfo& IN InBuffInfo);

	void RemoveBuff(const AID_t IN InAID, const WRTableID IN InBuffTID);	
	void RemoveBuff(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID);

	void ChangeBuffStack(const AID_t IN InAID, const WRTableID IN InBuffTID, const int32 IN InStack);
	void ChangeBuffStack(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID, const int32 IN InStack);

	bool HaveBuff(const AID_t IN InAID, const WRTableID IN InBuffTID);
	bool HaveBuff(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID);

	float GetBuffRemainTime(const AID_t IN InAID, const WRTableID IN InBuffTID);
	float GetBuffRemainTime(class AWRCharacter* IN InChar, const WRTableID IN InBuffTID);

	// Make
	bool MakeBuff(const struct stBuffInfo& IN InBuffInfo, FWRBuff*& OUT OutBuff);
};