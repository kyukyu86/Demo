// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"


#include "WRActorComponentCommonNpcTalk.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentCommonNpcTalk : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void OnLoadingCompleted();

	void OnMyCharacterMoveStarted();
	void OnMyCharacterMoveStoped();
	void OnDialogueOpend();
	void OnDialogueFinished();
	void OnMyCharacterTargetChanged();
	bool IsAvailableCommonNpcTalk();
	void ProgressCommonNpcTalk(); // NPC와 대화를 실행하라
private:
	void UpdateLoading();
	void UpdateIcon();
	bool CheckRange();	// 대화 가능 거리 체크및 UI처리
	void StartLoading();
	void StopLoading();
	void ActiveIcon();
	void DeActiveIcon();
private:
	bool bLoading = false;
	bool bActive = false;
	float CheckTime = 0.f;

	FDelegateHandle MyCharacterStartMoveDelegateHandle;
	FDelegateHandle MyCharacterStopMoveDelegateHandle;
	FDelegateHandle MyCharacterTargetChangedDelegateHandle;
};
