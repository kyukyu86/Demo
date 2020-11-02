// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRUIEnum.h"
#include "Define/WRTypeDefine.h"
#include "Delegate.h"


class AActor;

struct FWRTargetLocationInfoBase
{
public:
	enum EWRTargetLocationInfoType { None, Actor, Position };
public:
	FWRTargetLocationInfoBase() {}
	virtual ~FWRTargetLocationInfoBase() {}
	FWRTargetLocationInfoBase(const EWRTargetLocationInfoType IN InType, const EWRUISlotKind IN InUISlotKind, const WRTableID IN InMapTID, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist = 0.f)
	: Type(InType), UISlotKind(InUISlotKind), MapTID(InMapTID), OnAutoRemovedDelegate(InOnAutoRemovedDelegate), bDeleteWhenEnterSearchingArea(bInDeleteWhenEnterSearchingArea), ShowDist(InShowDist){}
public:
	EWRTargetLocationInfoType Type;
	EWRUISlotKind UISlotKind;
	WRTableID MapTID;
	FSimpleDelegate OnAutoRemovedDelegate;	// 내부 로직에 의해 자동으로 삭제 된 경우의 콜백
	bool bDeleteWhenEnterSearchingArea = false;	// 서칭 범위에 들어가면 삭제해야 하는가?
	bool bShowUI = false;
	float ShowDist = 0.f;
	float SearchedEffectCoolTime;

public:
	virtual FVector GetTargetLocation() const { return FVector::ZeroVector; }
};

struct FWRTargetLocationInfoForActor : public FWRTargetLocationInfoBase
{
	virtual ~FWRTargetLocationInfoForActor() {}
	FWRTargetLocationInfoForActor(AActor* IN InTargetActor, const EWRUISlotKind IN InUISlotKind, const WRTableID IN InMapTID, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist = 0.f, const FVector& IN InOffset = FVector::ZeroVector)
		: FWRTargetLocationInfoBase(EWRTargetLocationInfoType::Actor, InUISlotKind, InMapTID, InOnAutoRemovedDelegate, bInDeleteWhenEnterSearchingArea, InShowDist), TargetActor(InTargetActor), Offset(InOffset){}

	AActor* TargetActor;
	FVector Offset;
public:
	virtual FVector GetTargetLocation() const override;
};

struct FWRTargetLocationInfoForPosition : public FWRTargetLocationInfoBase
{
	virtual ~FWRTargetLocationInfoForPosition() {}
	FWRTargetLocationInfoForPosition(const FVector& IN InTargetPosition, const EWRUISlotKind IN InUISlotKind, const WRTableID IN InMapTID, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist = 0.f)
		: FWRTargetLocationInfoBase(EWRTargetLocationInfoType::Position, InUISlotKind, InMapTID, InOnAutoRemovedDelegate, bInDeleteWhenEnterSearchingArea, InShowDist), TargetPosition(InTargetPosition) {}

	FVector TargetPosition;
	virtual FVector GetTargetLocation() const override;
};

class WR_API WRTargetLocationGuideManager : public WRSingleton<WRTargetLocationGuideManager>
{
	CDECLARE_MANAGER(WRTargetLocationGuideManager);

private:
	WRTargetLocationGuideManager();
	virtual ~WRTargetLocationGuideManager();

public:
	//====================================================================================
	// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;

	virtual void EndLoadLevelProcess() override;
	//====================================================================================
public:
	WRTargetLocationGuideHandle AddTargetLocationInfo(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist = 0.f);
	WRTargetLocationGuideHandle AddTargetLocationInfo(const WRTableID IN InMapTID, AActor* IN InTargetActor, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist = 0.f, const FVector& IN InOffset = FVector::ZeroVector);
	void RemoveTargetLocationInfo(const WRTargetLocationGuideHandle IN InHandle);
private:
	void Clear();

	WRTargetLocationGuideHandle GenerateTargetLocationGuideHandle();

	WRTargetLocationGuideHandle AddTargetLocationInfo_Internal(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist);
	WRTargetLocationGuideHandle AddTargetLocationInfo_Internal(const WRTableID IN InMapTID, AActor* IN InTargetActor, const EWRUISlotKind IN InUISlotKind, const FSimpleDelegate& IN InOnAutoRemovedDelegate, const bool IN bInDeleteWhenEnterSearchingArea, const float IN InShowDist, const FVector& IN InOffset);

	bool CheckEnteredSearchingArea();
	bool CheckEnteredSearchingAreaForActor(const FWRTargetLocationInfoForActor* IN InTargetLocationInfoForActor);
	bool CheckEnteredSearchingAreaForPosition(const FWRTargetLocationInfoForPosition* IN InTargetLocationInfoForPosition);
	bool CheckVisibilityDist(const FWRTargetLocationInfoBase* IN InTargetLocationInfoBase);	// 보여지는 거리안에 있는지 검사
	bool CheckVisibilityDistForActor(const FWRTargetLocationInfoForActor* IN InTargetLocationInfoForActor);
	bool CheckVisibilityDistForPosition(const FWRTargetLocationInfoForPosition* IN InTargetLocationInfoForPosition);	

	void OnSearched(const WRTargetLocationGuideHandle IN InHandle, FWRTargetLocationInfoBase* IN InTargetLocationInfoBase);	// Searching 영역에 들어갔을때
private:
	TMap<WRTargetLocationGuideHandle, FWRTargetLocationInfoBase*> TargetLocationInfoMap;
	TDoubleLinkedList<WRTargetLocationGuideHandle> ReservedRemoveHandleList;
	float FlowCheckingTime = 0.f;
};