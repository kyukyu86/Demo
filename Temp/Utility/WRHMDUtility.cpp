// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRHMDUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"

bool WRHMDUtility::IsConnected()
{
	return UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
}

bool WRHMDUtility::IsDisplayEnable()
{
	return UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
}


void WRHMDUtility::ResetRotationAndPosition(const float InYaw)
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(InYaw);
}

void WRHMDUtility::GetPositionAndRotation(FVector& OutPosition, FRotator& OutRotation)
{
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(OutRotation, OutPosition);
}

FVector WRHMDUtility::GetHMDPosition()
{
	FVector Position;FRotator Rotation;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(Rotation, Position);
	return Position;
}

FRotator WRHMDUtility::GetHMDRotation()
{
	FVector Position; FRotator Rotation;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(Rotation, Position);
	return Rotation;
}
