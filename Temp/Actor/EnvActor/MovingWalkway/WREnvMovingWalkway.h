// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Manager/WRCharacterManager.h"
#include "Enum/EWREnvObject.h"
#include "WREnvMovingWalkway.generated.h"

/**
 *
 */
class UWRMovingwalkComponent;

UCLASS()
class WR_API AWREnvMovingWalkway : public AWREnvActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		TMap<EWREnvSplineInfo, TSoftObjectPtr<UStaticMesh>> SplineMeshs;
	

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis = ESplineMeshAxis::X;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		FString SoundEffectPath;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		int32 TranslucencySortPriority = 0;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		float DistanceToSpawnSound = -1.f;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		float MoveMeterPerSpeed = 12.f;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		bool IsCantMoveInput = false;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		bool IsDrawPhysicsCollision = false;

	UPROPERTY(EditAnywhere, Category = "WRMovingWalkInfo")
		bool IsDrawOverlapCollision = false;

private:
	UPROPERTY(VisibleAnywhere, Category = "WRMovingWalkInfo")
	TArray<class UWRMovingwalkComponent*> InMovingwalkLanes;
	UPROPERTY(VisibleAnywhere, Category = "WRMovingWalkInfo")
		TArray<class UWRMovingwalkComponent*> InMovingwalkComps;
	UPROPERTY(VisibleAnywhere, Category = "WRMovingWalkInfo")
		USplineComponent* InRootSplineComp = nullptr;
private:
	int32 InMovingwalkSoundUID = INDEX_NONE;
	bool IsZipLineUsing = false;
public:
	// Sets default values for this actor's properties
	AWREnvMovingWalkway();

	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE float GetMoveMeterPerSpeed() { return MoveMeterPerSpeed; }
	FORCEINLINE float GetDistanceToStart() { return DistanceToSpawnSound; }
	FORCEINLINE bool GetCantMoveInput() { return IsCantMoveInput; }
	FORCEINLINE bool GetDrawPhysicsCollision() { return IsDrawPhysicsCollision; }
	FORCEINLINE bool GetDrawOverlapCollision() { return IsDrawOverlapCollision; }
	FORCEINLINE int32 GetNumberOfSplinePoints()
	{
		if (InRootSplineComp != nullptr)
		{
			return InRootSplineComp->GetNumberOfSplinePoints();
		}
		return INDEX_NONE;
	}

private:
	void InitializeMovingWalkway();

	void ModifySplineMeshs();

	void RegisterNodes();

	void CreateNode(int32 InNodeIndex);

	void DestroyNode(USplineMeshComponent* InNode);

	void ClearAllNode();

	void SoundProcess();
public:
	void SpawnSoundEffect(const AActor& InActor);

	void StartRideZipline(AWRCharacter* InCharacter);
	void EndRideZipline(AActor* InActor);

	void MovingWalkProcess(float DeltaTime);

	bool IsBoardingMovingwalk(AWRCharacter* InCharacter);
	class UWRMovingwalkComponent* GetOverlappingComp(AWRCharacter* InCharacter);
private:
	TMap<AID_t, stSmallDateTime> OverlappedActors;
	
public:
	void AddOverlappedActors(const AID_t& aid, const stSmallDateTime IN InStartTime);
	FORCEINLINE void RemoveOverlappedActors(const AID_t& aid)
	{
		if (OverlappedActors.Contains(aid) == true)
		{
			OverlappedActors.Remove(aid);
			OverlappedActors.Compact();
		}
	};
	FORCEINLINE TMap<AID_t, stSmallDateTime>& GetOverlappedActors() { return OverlappedActors; };

	FORCEINLINE bool IsOverlappedActor(const AID_t& aid) { return OverlappedActors.Contains(aid); };
};
