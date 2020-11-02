#pragma once
#include "../Character/WRCharacter.h"
#include "Enum/EWRCharacter.h"

struct FWRMontageInfo
{
	FWRMontageInfo()
	{
		SectionName = "";
		MontageID = INVALID_TABLE_TID;
	}

	FString SectionName;
	int32 MontageID;
};

class WRFSMBase
{
public:
	WRFSMBase(EWRFSM InType, EWRFSM InLinkedFsmWhenAnimationEnd, bool InbLayeredBlendPerBone = false);
	virtual ~WRFSMBase();

public:
	virtual void Begin(const WRFSMInfo& FSMInfo) = 0;
	virtual void Tick(AWRCharacter* OwnerCharacter, float DeltaTime) = 0;
	virtual void End(const WRFSMInfo& InReservedFSM) = 0;
	virtual void ProcessWhenAnimationEnd(AWRCharacter* OwnerCharacter) = 0;		//fsm시간이 끝나서 상태가 변환될때
	virtual void ProcessWhenAnimationInterrupted(AWRCharacter* OwnerCharacter) = 0;	//fsm이 강제로 변환될때
	virtual EWRFSM GetNextFSMWhenAnimationEnd(AWRCharacter* InChar);

public:	
	virtual bool IsHighPriorityThanCurrentFSM(const WRFSMInfo& InFSMInfo) = 0;

public:
	static FString GetBaseSectionName(EWRFSM InFSMType);
	static FString GetAttackHeightSuffix(EWRAttackHeightType InType);
	static void GetMontageInfoForPlaying(const WRFSMInfo& InFSMInfo, FWRMontageInfo& OutMontageInfo);

public:
	void ProcessAnimation(AWRCharacter* InCharacter, int32 InMontageID, FString InAnimSection = "Default", float InAnimTime = 0, float InAnimScale = 1.0f);
	void ProcessAnimation(const WRFSMInfo& InFSMInfo);
	bool IsLayeredBlendFSM(AWRCharacter* InCharacter);
	
	EWRFSM GetFSMType();

//private:
	bool ProcessAnimationAsMontage(AWRCharacter* InCharacter, int32 MontageID, FString InSectionName, float InAnimScale);
	
protected:
	EWRFSM CurrentFSMType;
	EWRFSM LinkedFSMWhenAnimationEnd;

	bool bLayeredBlendperBone = false;
};

