// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WRChainWeaponAsset.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRChainWeaponAsset : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Socket")
	bool bUseImpactLocation = false;

	UPROPERTY(EditAnywhere, Category="Socket", meta = (EditCondition="!bUseImpactLocation"))
	FName TargetSocket = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Socket")
	FName CasterSocket = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Socket")
	FName HookSocketTitleName = NAME_None;

	/*If Set, Will use this static mesh for chain head. but, Will use equipped static mesh when non set*/
	UPROPERTY(EditAnywhere, Category = "Render")
	class UStaticMesh* EndpointShape = nullptr;

	UPROPERTY(EditAnywhere, Category = "Render")
	float MeshScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Render")
	TSoftObjectPtr<class UMaterialInterface>	ChainMaterial;

	UPROPERTY(EditAnywhere, Category ="Cable", meta=(ClampMin = "0.0", UIMin = "0.0", UIMax = "1000.0"))
	float CableLength = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Cable", meta = (ClampMin = "0.0", UIMin = "1.0", UIMax = "10.0"))
	float CableWidth = 10.0f;

	// [ 2019-10-10 : magedoga ] Set tight value when cut chain
	UPROPERTY(EditAnywhere, Category = "Cable", meta = (ClampMin = "0.0", UIMin = "1.0", UIMax = "16.0"))
	int32 CableTightValue = 1;

	/* If true, this chain weapon have movement (Caster to Target) */
	UPROPERTY(EditAnywhere, Category = "Projectile")
	bool bProjectile = true;

	/* MPS (Meter Per Second)*/
	UPROPERTY(EditAnywhere, Category = "Projectile", meta=(EditCondition = "bProjectile", ClampMin= "100.0", ClampMax= "10000.0"))
	float MoveSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TSoftObjectPtr<class USoundCue>	CutChainSound;
};
