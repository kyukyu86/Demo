// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Enum/EWRCharacter.h"
#include "WRActorComponentInteractionObject.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRCapsuleCollisionComponent.generated.h"

/**
 * 
 */
//UCLASS()
UCLASS(meta = (BlueprintSpawnableComponent))
class WR_API UWRCapsuleCollisionComponent : public UCapsuleComponent
{
	GENERATED_BODY()
public:
	UWRCapsuleCollisionComponent(const FObjectInitializer& ObjectInitializer);
	UWRCapsuleCollisionComponent();

public:
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy);
	class AWRCharacter* GetOwnerCharacter();

public:
	virtual void SetActive(bool bNewActive, bool bReset = false) override;

public:
	void RegistAttackingSignature();
	void RegistHittingSignature();
	// [ 2020-1-10 : kyu ] Hook
	bool IsHookable();
	void SetHookTargeted(const bool IN InSet);
	void UpdateCollisionWidget(class UWRUIPanel* InPanel = nullptr);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRCollision")
	EWRCollisionType CollisionType = EWRCollisionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRCollision")
	EWRCollisionType HitCollisionType = EWRCollisionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRCollision Hook Widget")
	FName CapsuleAttachSocket;

	bool bReservedUIOpen = false;
	WRUIHandle UIHandle = 0;
};
