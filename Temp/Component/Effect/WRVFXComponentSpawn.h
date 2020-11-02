// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRVFXComponentSpawn.generated.h"

USTRUCT(BlueprintType)
struct FWRVFXSpawns
{
	GENERATED_BODY()

public:
	bool Tick(float DeltaTime);

public:
	void Reset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	TSubclassOf<AWRVFX> GeneratedClass;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	//FVector Scale = FVector::OneVector;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	//FRotator Rotation = FRotator::ZeroRotator;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	//FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	bool IsSpawnWhenCreate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	bool IsSpawnWhenCollide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent", meta = (EditCondition = "!IsSpawnWhenCreate"))
	float Delay = 1.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WRSpawnComponent")
	float AccDelay = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	FRotator AdditionalAngle;

	bool IsSpawned = false;
};

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRVFXComponentSpawn : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetActive(bool bNewActive, bool bReset = false) override;

public:
	void OnComponentBeginOverlap();

private:
	void LoadTable();

#if UE_EDITOR
	void OnComplete(AWRVFX* InVFX, const FRotator& InAdditionalAngle);
#else // UE_EDITOR
#endif // UE_EDITOR

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRSpawnComponent")
	TArray<FWRVFXSpawns> VFXSpawns;
};
