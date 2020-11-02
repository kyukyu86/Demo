// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Define/WRTypeDefine.h"
#include "Manager/WRObjectInteractionManager.h"
#include "TimerManager.h"

#include "WRActorComponentInteraction.generated.h"


/**
 *		for Player
 */
UCLASS()
class WR_API UWRActorComponentInteraction : public UWRActorComponent
{
	GENERATED_BODY()
	
private:
	// Recognition
	float fRecognitionDistanceShowDelay = 0.0f; // for debug kyu

	// Occupation
	class AWRCharacterIO* OccupiedIO = nullptr;
	float fSrcGravityScale = 1.f;
	bool bSrcGravityEnable = true;
	FTimerDelegate OcccupyTimerDelegate;
	FTimerHandle OccupyTimer;

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
	// Interaction Object
	void OnInteractionStart(class AWRCharacterIO* IN InIO);
	void OnInteractionStop();
	void OnInteractionEnd();

	// Recognition
	void Debug_UpdateRecognitionIO(float IN InDeltaTime); // for debug kyu		

	// Occupation
	FORCEINLINE class AWRCharacterIO* GetOccupiedIO() { return OccupiedIO; }
	void SetOccupiedIO(class AWRCharacterIO* IN InNewIO, const bool IN InIODestroy = false);
	bool IsOccupiedIO(const AID_t IN InAID);
	void OnOccupyTimerFinished();

	void OnOccupationActivate();
	void OnOccupationDeactivate();

private:
	class AWRCharacterPlayer* GetOwnerPlayer();
	void SetOwnerGravity(const bool IN InSet);

	// Occupation
	void TransformToOccupationEndPosition();
};