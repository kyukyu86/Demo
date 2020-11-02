// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/SkeletalMeshActor.h"
#include "WRSkeletalMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class WR_API AWRSkeletalMeshActor : public ASkeletalMeshActor
{
	GENERATED_UCLASS_BODY()

public:
	void SetSkeletalMesh(class USkeletalMesh* NewMesh, bool bReinitPose = true);
	
};
