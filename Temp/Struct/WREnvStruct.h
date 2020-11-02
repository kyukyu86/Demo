// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Network/Share/Define/Define_Type.h"
#include "WREnvStruct.generated.h"



USTRUCT(BlueprintType)
struct FWRSecretDialogInfo
{
	GENERATED_USTRUCT_BODY()
public:
	AID_t PortalAID = INVALID_AID;
	int32 GroupID = 0;
	TArray<AID_t> TalkPlayerList;
	FVector PortalLocation = FVector::ZeroVector;
};