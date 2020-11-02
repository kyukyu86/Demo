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
	FSimpleDelegate OnAutoRemovedDelegate;	// ���� ������ ���� �ڵ����� ���� �� ����� �ݹ�
	bool bDeleteWhenEnterSearchingArea = false;	// ��Ī ������ ���� �����ؾ� �ϴ°�?
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
	// [ 2019-8-14 : hansang88 ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
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
	bool CheckVisibilityDist(const FWRTargetLocationInfoBase* IN InTargetLocationInfoBase);	// �������� �Ÿ��ȿ� �ִ��� �˻�
	bool CheckVisibilityDistForActor(const FWRTargetLocationInfoForActor* IN InTargetLocationInfoForActor);
	bool CheckVisibilityDistForPosition(const FWRTargetLocationInfoForPosition* IN InTargetLocationInfoForPosition);	

	void OnSearched(const WRTargetLocationGuideHandle IN InHandle, FWRTargetLocationInfoBase* IN InTargetLocationInfoBase);	// Searching ������ ������
private:
	TMap<WRTargetLocationGuideHandle, FWRTargetLocationInfoBase*> TargetLocationInfoMap;
	TDoubleLinkedList<WRTargetLocationGuideHandle> ReservedRemoveHandleList;
	float FlowCheckingTime = 0.f;
};