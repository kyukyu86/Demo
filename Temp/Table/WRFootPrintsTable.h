// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Base/WRTableRow.h"
#include "Engine/EngineTypes.h"
#include "Materials/MaterialInstance.h"

#include "WRFootPrintsTable.generated.h"


USTRUCT(BlueprintType)
struct FWRFootPrintsTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
	
public:	
	static FString GetFileName() { return TEXT("CharData_FootPrints"); };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FootPrint)
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FootPrint)
	int32 CharacterTID;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FootPrint)
	FString DecalFootPrintMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FootPrint)
		float FootPrintSize = 50.f;
};
