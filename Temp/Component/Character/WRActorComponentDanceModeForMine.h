// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "WRActorComponentDanceMode.h"
#include "Network/Share/Define/Define_Actor.h"
#include "Define/WRTypeDefine.h"


#include "WRActorComponentDanceModeForMine.generated.h"


/**
 * 
 */
struct FWRDanceGroupTable;
struct FWRDanceTable;
UCLASS()
class WR_API UWRActorComponentDanceModeForMine : public UWRActorComponentDanceMode
{
	GENERATED_BODY()
public:
	enum EWRNextStepType { None, Start, End};
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void GoNextStep();

	bool ShowStartDanceUI();
	void ShowDafaultDanceUI();

	void IncreaseDancePoint(const int32 IN InDancePoint);

	bool CheckDancerAround();	// �ֺ��� ���� �ִ°�?(NPC Job�� Dancer)
	bool CheckDancingPlayerAround();	// �ֺ��� ���ߴ� �÷��̾ �ִ°�?
	void SetEnterDanceAreaByVolume(const bool IN bInEnterDanceAreaByVolume);
	bool IsEnterDanceArea();

	void OnFinishDanceEnded();	// �ǴϽ� ���� ���������� ����Ǿ�����

	void OnAckBeginDanceReceived(const WRTableID IN InDanceGroupTID);
	void OnAckEndDanceReceived();
	void OnAckChangeDanceStepReceived(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID);
private:
	void TickNextStep(const float IN InDeltaTime);

	virtual void StartDanceMode(const WRTableID IN InDanceGroupTID, const bool IN bInRetry = false) override;
	virtual void StopDanceMode(const bool IN bInStopFSM = true) override;

	virtual void SetDanceStep(const WRTableID IN InDanceTID) override;
	void SetNextStepTimer();
private:
	EWRNextStepType NextStepType;
	float FlowTime = 0.f;
	float NextStepStartTime = 0.f;
	float NextStepEndTime = 0.f;
	float PointTime = 0.f;

	bool bEnterDanceAreaByVolume = false;
	float TimeForCheckDanceArea = false;
	FVector Before3rdCharacterOffset = FVector::ZeroVector;
};
