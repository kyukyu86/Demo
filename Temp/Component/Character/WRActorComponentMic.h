// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRActorComponentMic.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentMic : public UWRActorComponent
{
	GENERATED_BODY()
	
private:
	bool MicActivating = false;
	AID_t MicAID = INVALID_AID;

	FTimerHandle RandomAnimationTimerHandle;

private:
	bool CheckActivateConditions();

protected:
	void PlayRandomAnimation();

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

	void ActivateMic(const AID_t IN InMicAID);
	void DeactivateMic(const AID_t IN InMicAID);
	bool IsActivating() { return MicActivating; }

	bool OnAnimation(const TID_t IN InAnimationTID);
};
