// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Network/Share/Define/Define_Actor.h"


#include "WRActorComponentDanceMode.generated.h"


/**
 * 
 */
struct FWRDanceGroupTable;
struct FWRDanceTable;
UCLASS()
class WR_API UWRActorComponentDanceMode : public UWRActorComponent
{
	GENERATED_BODY()
public:
	enum EWRNextStepType { None, Start, End};
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void StartDanceMode(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID);
	bool IsDanceModeOn() const;

	void OnInformChangeDanceStepReceived(const WRTableID IN InDanceGroupTID, const WRTableID IN InDanceTID);
protected:
	virtual void StartDanceMode(const WRTableID IN InDanceGroupTID, const bool IN bInRetry = false);
	virtual void StopDanceMode(const bool IN bInStopFSM = true);

	virtual void SetDanceStep(const WRTableID IN InDanceTID);
	
protected:
	const FWRDanceGroupTable* CurDanceGroupTable;
	const FWRDanceTable* CurDanceTable;
	int32 CurIndex;	// 그룹댄스 안에서 현재 어느 댄스중인지에 대한 인덱스

	bool bRetry;
};
