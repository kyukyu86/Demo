// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"


#include "WRActorComponentPlayerSocial.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentPlayerSocial : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void OnLoadingCompleted();
	bool IsAvailablePlayerSocial();
	void ProgressPlayerSocial();
private:	
	void ActiveIcon();
	void DeActiveIcon();
private:
	bool bActive = false;
	float CheckTime = 0.f;;
};
