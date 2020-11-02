// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRVFXComponentLaser.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRVFXComponentLaser : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetActive(bool bNewActive, bool bReset = false) override;

private:
	FTransform GetActorTransform();

	void Tick4Transform();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXComponentLaser")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXComponentLaser")
	FName BoneName;	

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXComponentLaser")
	//FVector Scale = FVector::OneVector;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXComponentLaser")
	//FRotator Rotation = FRotator::ZeroRotator;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXComponentLaser")
	//FVector Location = FVector::ZeroVector;
};
