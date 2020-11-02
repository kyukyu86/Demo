// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"


#include "WRActorComponentNpcSocial.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentNpcSocial : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void OnLoadingCompleted();
	bool IsAvailableNpcSocial();
	void ProgressNpcSocial();
private:	
	void ActiveIcon();
	void DeActiveIcon();
private:
	bool bActive = false;
	float CheckTime = 0.f;;
};
