// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Define/WRTypeDefine.h"

#include "WRActorComponentAcceptQuest.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentAcceptQuest : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Init();
	void OnQuestStateChanged();
private:
	void UpdateReadyQuest();	// 수락 가능한 퀘스트가 있는지 확인

private:
	WRCompassMarkHandle CompassMarkHandle = INVALID_COMPASS_MARK_HANDLE;
};
