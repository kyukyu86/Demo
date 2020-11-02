// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class WR_API WRHMDUtility
{
public:

	//====================================================================================

	static bool IsConnected();

	static bool IsDisplayEnable();

	//====================================================================================

	static void ResetRotationAndPosition(const float InYaw);

	//====================================================================================

	static void GetPositionAndRotation(FVector& OutPosition, FRotator& OutRotation);

	static FVector GetHMDPosition();

	static FRotator GetHMDRotation();

};
