// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "Enum/EWRVFX.h"
#include "WRVFXComponentAttach.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRVFXComponentAttach : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SetActive(bool bNewActive, bool bReset = false) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#else // WITH_EDITOR
#endif // WITH_EDITOR

private:
	FTransform GetActorTransform(class AActor* InActor);
	FTransform GetComponentTransform(class AActor* InActor);

	void Transform(const FTransform& InTransform, class AActor* InActor);

	void Transform4Spawner();
	void Transform4Target();
	void Transform4Parent();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	EWRVFXAttach Attach = EWRVFXAttach::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	EWRVFXComponentToAttach ComponentToAttach = EWRVFXComponentToAttach::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	bool IsLooping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	bool IsComponentSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	bool IsAttachScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	bool IsAttachRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	bool IsAttachLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	bool IsSnapFloor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRAttachComponent")
	FVector FloorLocation = FVector::ZeroVector;

private:
	DECLARE_DELEGATE(WRTick)
	TArray<WRTick> Ticks;
};
