// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Component/Base/WRActorComponent.h"
#include "Manager/WRBuffManager.h"
#include "Define/WRTypeDefine.h"
#include "Enum/EWRCharacter.h"
#include "Network/Share/Define/Define_BluePrint.h"

#include "WRActorComponentBuff.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentBuff : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool AddBuff(const struct stBuffInfo& IN InBuffInfo);
	void RemoveBuff(const WRTableID IN InBuffTID);
	void ChangeBuffStack(const WRTableID IN InBuffTID, const int32 IN InStack);

	bool HaveBuff(const WRTableID IN InBuffTID);
	float GetRemainTime(const WRTableID IN InBuffTID);

public:
	TArray<FWRBuff*>* GetBuffs(const EWRBuffType IN InType);

private:
	TMap<EWRBuffType, TArray<FWRBuff*>> Buffs;
};
