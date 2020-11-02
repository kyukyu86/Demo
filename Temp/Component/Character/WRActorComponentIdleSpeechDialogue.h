// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Define/WRTypeDefine.h"

#include "WRActorComponentIdleSpeechDialogue.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentIdleSpeechDialogue : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void SetData(const int32 IN InSpeechRange, const TArray <WRTableID>& IN InDialogueTIDs);

	void OnMyCharacterMoveStarted();
	void OnMyCharacterMoveStoped();
private:
	void RegisterDialogue();

private:
	float LoopTime;
	int32 SpeechRange;
	TArray<WRTableID> DialogueTIDs;

	FDelegateHandle MyCharacterStartMoveDelegateHandle;
	FDelegateHandle MyCharacterStopMoveDelegateHandle;
};
