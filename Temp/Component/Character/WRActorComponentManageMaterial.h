// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Actor/Animation/WRAnimInstance.h"

#include "CoreMinimal.h"
#include "Component/Base/WRBaseComponentManageMaterial.h"
#include "WRActorComponentManageMaterial.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentManageMaterial : public UWRBaseComponentManageMaterial
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

public:
	void RestoreMaterialInterface();
	void SetMaterial(int32 ElementIndex, class UMaterialInterface* Material);

private:
	void SetHeavyAttack(float InRatio);
	//void SetSkillAttack(float InRatio);
	void SetWeakness(float InRatio);
	void SetHit(float InRatio);
	void SetSpawn(float InRatio);
	void SetDissolve(float InRatio);
	void SetConstruct(float InRatio);
	void SetChangeCloth(float InRatio);
private:
	void SetMaterialnterface();
	void ClearMaterialInterface();

private:
	TArray<class UMaterialInterface*> MaterialInterfaces;
};