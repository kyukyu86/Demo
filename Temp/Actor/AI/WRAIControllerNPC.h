// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WRAIControllerNPC.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRAIControllerNPC : public AAIController
{
	GENERATED_BODY()
	
public:
	AWRAIControllerNPC();

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	
public:
	void CreatePersistentKey();

private:
	UPROPERTY()
	class UBehaviorTree* BTreeAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;

	//FName SpawnedPosition = FName("SpawnedPositon");

};
