// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"
#include "Enum/EWREnvObject.h"
#include "WRMovingwalkComponent.generated.h"

/**
 * 
 */

UCLASS()
class WR_API UWRMovingwalkComponent : public USplineMeshComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, Category = "WRMovingWalkInfo")
	EWREnvSplineInfo LaneInfo = EWREnvSplineInfo::None;
	UPROPERTY(VisibleAnywhere, Category = "WRMovingWalkInfo")
	class AWREnvMovingWalkway* MovingwalkPtr = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "WRMovingWalkInfo")
	int32 NodeIndex = INDEX_NONE;

private:
	TMap<int32, AActor*> OverlappedActors;

public:
	void RegisterSplineMesh(int32 InNodeIndex, class USplineComponent* InRootSplineComp, class UStaticMesh* InStaticMesh, TEnumAsByte<ESplineMeshAxis::Type> InForwardAxis, EWREnvSplineInfo InLaneInfo, int32 InTranslucencySortPriority, const FVector & InStartLocation, const FVector & InStartTangent, const FVector & InEndLocation, const FVector & InEndTangent);// , bool bEnableMove);
	
private:
	void UpdateOverlappedActors(AActor* OverlappedActor);

	
public:
	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy);

	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool IsOverlappedActor(const int32 InUID) { return OverlappedActors.Contains(InUID); }
	FORCEINLINE TMap<int32, AActor*>& GetOverlappedActors() { return OverlappedActors; }
	FORCEINLINE EWREnvSplineInfo GetSplineInfo() { return LaneInfo; }
public:

	UFUNCTION()
		virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	
	UFUNCTION()
		virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
