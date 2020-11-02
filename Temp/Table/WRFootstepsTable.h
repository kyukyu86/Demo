// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Base/WRTableRow.h"
#include "Engine/EngineTypes.h"
#include "Enum/EWRSFX.h"
#include <Sound/SoundCue.h>


#include "WRFootstepsTable.generated.h"


USTRUCT(BlueprintType)
struct FWRFootstepsTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
	
public:	
	static FString GetFileName() { return TEXT("CharData_Footsteps"); };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Footstep)
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Footstep)
	int32 CharacterTID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Footstep)
	EWRShoesMaterialType ShoesMaterialType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Footstep)
	FString StepSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Footstep)
	FString StepSoundInRaining;
};
 