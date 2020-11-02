// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Define/WRTypeDefine.h"
#include "Define/WRUIDefine.h"

/**
*
*/

// 

class AActor;

struct FWRCompassMarkInfoBase
{
public:
	enum EWRCompassMarkType { None, Actor, Position };
	enum EWRCompassMarkUIType { Question, Exclamation };

	FWRCompassMarkInfoBase() {}
	FWRCompassMarkInfoBase(const WRTableID IN InMapTID) : MapTID(InMapTID) {}
	FWRCompassMarkInfoBase(const WRTableID IN InMapTID, const EWRCompassMarkUIType IN InMarkUIType) : MapTID(InMapTID), MarkUIType(InMarkUIType){}
	virtual ~FWRCompassMarkInfoBase() {}

public:
	bool bWaitCreateUI = false;
	WRUIHandle UIHandle = INVALID_UI_HANDLE;
	WRTableID MapTID = INVALID_TABLE_TID;
	EWRCompassMarkUIType MarkUIType = EWRCompassMarkUIType::Question;
	virtual FWRCompassMarkInfoBase::EWRCompassMarkType GetCompassMarkType() const { return FWRCompassMarkInfoBase::EWRCompassMarkType::None; }
};


struct FWRCompassMarkInfoForActor : public FWRCompassMarkInfoBase
{
	FWRCompassMarkInfoForActor(const WRTableID IN InMapTID, AActor* IN InTargetActor, const FVector& IN InOffset, const FName& IN InSocketName, const EWRCompassMarkUIType IN InMarkUIType)
		: FWRCompassMarkInfoBase(InMapTID, InMarkUIType), TargetActor(InTargetActor), Offset(InOffset), SocketName(InSocketName) {}

	AActor* TargetActor;
	FVector Offset;
	FName SocketName;

public:
	virtual FWRCompassMarkInfoBase::EWRCompassMarkType GetCompassMarkType() const { return FWRCompassMarkInfoBase::EWRCompassMarkType::Actor; }
};


struct FWRCompassMarkInfoForPosition : public FWRCompassMarkInfoBase
{
	FWRCompassMarkInfoForPosition(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset, const EWRCompassMarkUIType IN InMarkUIType)
		: FWRCompassMarkInfoBase(InMapTID, InMarkUIType), TargetPosition(InTargetPosition), ZOffset(InZOffset) {}
	FVector TargetPosition;
	int16 ZOffset;

public:
	virtual FWRCompassMarkInfoBase::EWRCompassMarkType GetCompassMarkType() const { return FWRCompassMarkInfoBase::EWRCompassMarkType::Position; }
};


class WR_API WRCompassManager : public WRSingleton<WRCompassManager>
{
	CDECLARE_MANAGER(WRCompassManager);
	
private:
	WRCompassManager();
	virtual ~WRCompassManager();
	
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
	//====================================================================================
public:
	void InitCompassUI();

	WRCompassMarkHandle AddMark(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset);
	WRCompassMarkHandle AddMark(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType);
	WRCompassMarkHandle AddMark(const WRTableID IN InMapTID, AActor* IN InActor, const FVector& IN InOffset);
	WRCompassMarkHandle AddMark(const WRTableID IN InMapTID, AActor* IN InActor, const FVector& IN InOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType);
	WRCompassMarkHandle AddMark(const WRTableID IN InMapTID, AActor* IN InActor, const FName& IN InSocketName, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType);
	void RemoveMark(const WRCompassMarkHandle IN InCompassMarkHandle);
	void StartIgnoreDistChecking(const float IN InIgnoreDistCheckingTime);

	const TMap<WRCompassMarkHandle, FWRCompassMarkInfoBase*>* GetCompassList() { return &CompassMarkMap; }
private:
	void Clear();
	WRCompassMarkHandle GenerateCompassMarkHandle();

	WRCompassMarkHandle AddMark_Internal(const WRTableID IN InMapTID, const FVector& IN InTargetPosition, const int16 IN InZOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType);
	WRCompassMarkHandle AddMark_Internal(const WRTableID IN InMapTID, AActor* IN InActor, const FName& IN InSocketName, const FVector& IN InOffset, const FWRCompassMarkInfoBase::EWRCompassMarkUIType IN InCompassMarkUIType);

	void CheckDisplay3DMark();
	void CheckDisplay3DMarkForPosition(const WRCompassMarkHandle IN InCompassMarkHandle, FWRCompassMarkInfoBase* IN InCompassMarkInfoBase);
	void CheckDisplay3DMarkForActor(const WRCompassMarkHandle IN InCompassMarkHandle, FWRCompassMarkInfoBase* IN InCompassMarkInfoBase);
private:
	TMap<WRCompassMarkHandle, FWRCompassMarkInfoBase*> CompassMarkMap;
	float IgnoreDistCheckingTime;
};