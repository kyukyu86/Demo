// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../WREnvActor.h"
#include "Component/InstancedFoliage/WRFoliageInstancedStaticMeshComponent.h"
#include "WRDynamicFoliage.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRDynamicFoliage : public AWREnvActor
{
	GENERATED_BODY()
private:
	class UMaterialInstanceDynamic* DynamicMaterial = nullptr;
//protected:
//	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void InitializeFoliage();

	void SetInstanceFoliageAndIndex(UWRFoliageInstancedStaticMeshComponent* InInstanceFoliagePtr, const int32& InItemIndex)
	{
		InstanceFoliagePtr = InInstanceFoliagePtr;
		ItemIndex = InItemIndex;
	}

	FORCEINLINE UWRFoliageInstancedStaticMeshComponent* GetInstanceFoliage() { return InstanceFoliagePtr; }

private:
	void AttenuationGlow();
	void IncreaseGlow();

private:
	int32 ItemIndex = -1;
	UWRFoliageInstancedStaticMeshComponent* InstanceFoliagePtr = nullptr;

	float CurrentGlow;
	float StandardGlow;

	bool IsFoliageInitialized = false;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float MinLength = 360;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float MaxLength = 600;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float MaxGlow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float IncreaseGlowSize = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FoliageInfo)
		float AttenuationRatio = 0.5f;


};
