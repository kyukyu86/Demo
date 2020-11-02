// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Network/Share/Define/Define_Type.h"
#include "Manager/WRVFXManager.h"
#include "WRStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRStaticMeshActor : public AStaticMeshActor
{
	GENERATED_UCLASS_BODY()

public:
	void SetStaticMesh(UStaticMesh* InMesh);

public:
	template <typename T>
	void SetMaterialParameterValue(FName InParameterName, T InValue, class UMeshComponent* InMeshComponent = nullptr);

protected:
	void SetMaterialParameterValue_Internal(FName InParameterName, float InValue, class UMeshComponent* InMeshComponent = nullptr);
	void SetMaterialParameterValue_Internal(FName InParameterName, FLinearColor InValue, class UMeshComponent* InMeshComponent = nullptr);
	
};

template <typename T>
void AWRStaticMeshActor::SetMaterialParameterValue(FName InParameterName, T InValue, class UMeshComponent* InMeshComponent /*= nullptr*/)
{
	SetMaterialParameterValue_Internal(InParameterName, InValue, InMeshComponent);
}

// UCLASS()
// class WR_API AWRStaticMeshProjectile : public AWRStaticMeshActor
// {
// 	GENERATED_UCLASS_BODY()
// 
// public:
// 
// 	UFUNCTION(BlueprintCallable)
// 	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
// 
// 
// 	virtual void Tick(float DeltaSeconds);	
// 	void SetIdendifier(AID_t InOwnerAID, SKILL_SEQ_t InSkillSeqID);
// 
// 	void Initialize();
// 	void Release();
// 	void TickDistance2Spawner(float DeltaSeconds);
// 
// 	WRDestoryHandle& OnDestroy() { 
// 		return DestoryHandle; 
// 	}
// 
// public:
// 	static FName ProjectileMovementComponentName;
// 	class UProjectileMovementComponent* ProjectileMovement = nullptr;
// 
// private:
// 	AID_t OwnerAID;
// 	SKILL_SEQ_t CombatSeqID;
// 
// 	uint32 UID = 0;
// 
// 	WRDestoryHandle DestoryHandle;
// 	bool bPlayingTakeAnim = false;
// };
