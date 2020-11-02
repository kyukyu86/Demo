// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "WRSlicedActor.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRSlicedActor : public AWREnvActor
{
	GENERATED_BODY()
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginDestroy() override;
	virtual bool CheckStillInWorld();
private:
	class UProceduralMeshComponent* InProcMeshComp = nullptr;
	WRDestoryHandle DestoryHandle;

	TArray<class UMaterialInstanceDynamic*> MIDs;

	float DissolveDuration = 0.f;
	float DissolveDeltaTime = 0.f;

	FVector PreviousLocation = FVector::ZeroVector;

	float AccDeltaTime = 0.0F;

	bool IsDissolvePlay = false;
	bool IsDeadPlay = true;
public:
	void Initialize();

	void SpawnVFXForProcMeshComp();
	void CompleteDissolve();

	void CreateDynamicMaterials();

	FORCEINLINE UProceduralMeshComponent* GetProcMesh() { return InProcMeshComp; }

	WRDestoryHandle& OnDestroy() {
		return DestoryHandle;
	}

private:
	void ProcessRemoveDisplay(float InDeltaTime);

	void MakeRemoveDisplay();

	void SetDynamicMaterialValue(float InDeltaTime);
};
