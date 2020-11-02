// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRActorComponentRevive.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentRevive : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

public:
	void ReviveOut();
	void ReviveIn();

private:
	void SetReviveVisibility(bool bNewVisibility);	
	void CompleteReviveIn();

private:
	int32 TranslucencySortPriority = 0;

	FVector NewScale3D = FVector::ZeroVector;

	TArray<class UMaterialInterface*> Materials;

	UMaterialInterface* MaterialInterface = nullptr;
};
