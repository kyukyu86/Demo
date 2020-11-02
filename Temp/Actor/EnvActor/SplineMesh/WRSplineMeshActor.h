// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Engine/StaticMesh.h"
#include "WRSplineMeshActor.generated.h"

/**
 *
 */
UCLASS()
class WR_API AWRSplineMeshActor : public AWREnvActor
{
	GENERATED_BODY()
public:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;

private:
	void InitializeSplineMesh();
	void ClearAllSplineMesh();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMovingWalkInfo", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* InStaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMovingWalkInfo", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* InPhysicsMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMovingWalkInfo", meta = (AllowPrivateAccess = "true"))
		bool DrawPhysicsMesh = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRMovingWalkInfo", meta = (AllowPrivateAccess = "true"))
	int32 TranslucencySortPriority = 0;

	UPROPERTY(VisibleAnywhere, Category = "WRSplineMesh")
	class USplineComponent* InRootSplineComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "WRSplineMesh")
	TArray<class USplineMeshComponent*> InSplineMeshComps;


};
