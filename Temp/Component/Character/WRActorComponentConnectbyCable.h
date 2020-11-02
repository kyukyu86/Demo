// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "CustomAsset/WRChainWeaponAsset.h"
#include "Network/Share/Define/Define_BluePrint.h"

/*#include "../../CableComponent/Source/CableComponent/Classes/CableComponent.h"*/

#include "WRActorComponentConnectbyCable.generated.h"


const float const_CutTime = 5.0f;
DECLARE_DELEGATE( FOnCutTime )

class UCableComponent;

enum EWRConnectByCableActor
{
	Chain,
	Caster,
	Target,
};

UENUM(BlueprintType)
enum class EWRConnectCableEvent : uint8
{
	ArrivalToTarget,
	ArrivalToCaster,
};

enum EWRCableMovementState
{
	Ready,
	ToTarget,
	Arrival,
	ToCaster,
};

DECLARE_DELEGATE_OneParam(FWRConnectCableEventDelegate, EWRConnectCableEvent)

struct FWRConnectByCableParams
{
public:

	FWRConnectByCableParams() {}
	FWRConnectByCableParams(const FWRConnectByCableParams& InParam)
	{
		bUseImpactLocation = InParam.bUseImpactLocation;

		Target = InParam.Target;
		WorldPosition = InParam.WorldPosition;

		TargetSocket = InParam.TargetSocket;
		CasterSocket = InParam.CasterSocket;
		HookSocketTitleName = InParam.HookSocketTitleName;

		EndpointShape = InParam.EndpointShape;
		MeshScale = InParam.MeshScale;

		CableLength = InParam.CableLength;
		CableWidth = InParam.CableWidth;
		CableTightValue = InParam.CableTightValue;

		bProjectile = InParam.bProjectile;
		MoveSpeed = InParam.MoveSpeed;

		ChainMaterial = InParam.ChainMaterial;

		CutChainSound = InParam.CutChainSound;
	}

	FWRConnectByCableParams(UWRChainWeaponAsset* InAsset)
	{
		bUseImpactLocation = InAsset->bUseImpactLocation;

		TargetSocket = InAsset->TargetSocket;
		CasterSocket = InAsset->CasterSocket;
		HookSocketTitleName = InAsset->HookSocketTitleName;

		EndpointShape = InAsset->EndpointShape;
		MeshScale = InAsset->MeshScale;

		CableLength = InAsset->CableLength;
		CableWidth = InAsset->CableWidth;
		CableTightValue = InAsset->CableTightValue;

		ChainMaterial = InAsset->ChainMaterial;

		CutChainSound = InAsset->CutChainSound;

		bProjectile = InAsset->bProjectile;
		MoveSpeed = InAsset->MoveSpeed;
	}

	bool bUseImpactLocation = false;

	class AWRCharacter* Target;
	FVector WorldPosition = FVector::ZeroVector;
	
	FName TargetSocket = NAME_None;
	FName CasterSocket = NAME_None;
	FName HookSocketTitleName = NAME_None;
	EWRCollisionType HookPart = EWRCollisionType::None;	// [ 2020-1-13 : kyu ] Take Outter Data (CapsuleBP)

	class UStaticMesh* EndpointShape = nullptr;
	float MeshScale = 1.0f;

	TSoftObjectPtr<class UMaterialInterface>	ChainMaterial;

	float CableLength = 100.0f;
	float CableWidth = 10.0f;
	int32 CableTightValue = 1;

	bool bProjectile = true;
	float MoveSpeed = 3000.0f;

	TSoftObjectPtr<class USoundCue>	CutChainSound;

	FWRConnectCableEventDelegate ConnectCableEventDelegate;
};


struct FWRReservedCut
{
public:
	FWRReservedCut(uint32 InID, class UWRActorComponentConnectbyCable* InConnectCableComponent, UCableComponent* InCableComponent, class AWRStaticMeshActor* InMeshActor, AActor* InImpactLocationActor, class UMaterialInstanceDynamic* InMaterial);

	~FWRReservedCut()
	{
		
	}

	void OnEndCutDirect();

public:

	uint32 ID;
	class UWRActorComponentConnectbyCable* ConnectCableComponent;
	UCableComponent* CableComponent;
	class AWRStaticMeshActor* MeshActor;
	AActor* ImpactLocationActor;
	class UMaterialInstanceDynamic* DynamicMaterial;

	FTimerHandle TimerHandle;
};

/**
 * 
 */ 
UCLASS()
class WR_API UWRActorComponentConnectbyCable : public UWRActorComponent
{
	GENERATED_BODY()

public:

	//virtual void OnCreateComponent() {};
	virtual void OnDestroyComponent() override;


	// [ 2019-9-27 : magedoga ] Connect
	bool ConnectByAsset(class AWRCharacter* InTarget, class UWRChainWeaponAsset* InAsset, FWRConnectCableEventDelegate InEventDelegate = nullptr, const EWRCollisionType IN InPart = EWRCollisionType::None);
	bool ConnectByAsset(const FVector& InWorldLocation , class UWRChainWeaponAsset* InAsset, FWRConnectCableEventDelegate InEventDelegate = nullptr);
	bool ConnectByParam(const FWRConnectByCableParams& InParams);

	bool ReturnToBase();

	// [ 2019-9-30 : magedoga ] Cut Cable
	void Cut(const bool bInImmediately = false, const bool bInForce = false);

	// [ 2019-9-27 : magedoga ] Get
	FVector GetChainLocation() const;
	// [ 2019-10-2 : kyu ] for Hook
	AWRCharacter* GetCableTarget();
	FVector GetImpactLocation();

	bool AttachAtoB(EWRConnectByCableActor InA, EWRConnectByCableActor InB, FAttachmentTransformRules InAttachRule);
	void DetachComponent(EWRConnectByCableActor InActorType);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// [ 2019-9-30 : magedoga ] Functions for check
	bool IsValidParam(const FWRConnectByCableParams& InParam);
	bool IsReady() const;
	bool IsConnecting() const;
	bool IsWorking() const;
	bool IsMoving() const;
	bool HasTarget() const;
	bool IsConnectingPart(const enum EWRCollisionType IN InPart); // for Part Of NPC

	void EnableCableCollision(AWRCharacter* InOwner);
	void UpdateCableLocationOnVehicle(FVector InLocationToModify);

private:

	bool Connect_Internal(const FWRConnectByCableParams& InParam);

	USceneComponent* GetAttachComponent(EWRConnectByCableActor InKindofActor);
	FName GetAttachSocketName(EWRConnectByCableActor InKindofActor);

	FVector GetDestinationPosition();
	float GetDistanceCasterToMeshActor();

	bool GetHitResultFromTarget(FHitResult& OutHR, const FVector& InStart, const FVector& InEnd, bool bUseMeshComponent = false);

	bool ShouldUseImpactLocation() const;

	void UpdateMaterialTileByLength();
	void UpdateForceByDistance();

	bool TickForMove(float InDeletaTime);	

	static void DestroyObject(AWRCharacter* InOwnerCharacter, UCableComponent* InCableComponent, class AWRStaticMeshActor* InMeshActor, AActor* InImpactLocationActor, class UMaterialInstanceDynamic* InMaterial);

private:

	// [ 2019-9-24 : magedoga ] Spawn Object
	class AWRStaticMeshActor* MeshActor = nullptr;
	class UCableComponent* CableComponent = nullptr;

	FWRConnectByCableParams Param;

	EWRCableMovementState MovementState = EWRCableMovementState::Ready;

	AActor* ImpactLocationActor = nullptr;

	bool bAttachWhenSpawnMeshActorCompleted = false;

	// [ 2019-10-7 : magedoga ] for cut direction
public:
	static uint32 CreateCutDirection(UWRActorComponentConnectbyCable* InConnectCableComponent, UCableComponent* InCableComponent, AWRStaticMeshActor* InMeshActor, AActor* InImpcatLocationActor, class UMaterialInstanceDynamic* InMaterial);
	static void EndCutDirection(uint32 InID);
	static void EmptyCutDirection();

	void RemoveCutDirectionID(const uint32 InID) { CutDirectionIDs.RemoveNode(InID); }
private:
	static TMap<uint32, FWRReservedCut*>	GCutDirection;
	static uint32 GCutDirectionID;

	TDoubleLinkedList<uint32> CutDirectionIDs;

	// [ 2020-1-16 : kyu ] for Cut TIme
public:
	void SetOnCurTime(const bool IN InOn, FOnCutTime IN InDG);
	EWRCollisionType GetCablePart();
private:
	void TickForCut(float InDeletaTime);
	void UpdateChainColor();
private:
	bool bActiveCutTime = false;
	float fDuringTime = 0.f;
	FOnCutTime OnCutTime;
public:
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial = nullptr;
};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(ConnectByCable, Display, All);
DECLARE_LOG_CATEGORY_EXTERN(CableCollision, Display, All);