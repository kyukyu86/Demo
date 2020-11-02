// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRBaseComponentManageMaterial.h"
#include "WRVehicleComponentManageMaterial.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRVehicleComponentManageMaterial : public UWRBaseComponentManageMaterial
{
	GENERATED_BODY()

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

public:
	void RestoreMaterialInterface();
	void SetMaterial(int32 ElementIndex, class UMaterialInterface* Material);

private:
	void SetSpawn(float InRatio);

private:
	void SetMaterialnterface();
	void ClearMaterialInterface();

private:
	TArray<class UMaterialInterface*> MaterialInterfaces;
};
