// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVehicleMovementComponent4W.h"
#include "Kismet/KismetMathLibrary.h"

void UWRVehicleMovementComponent4W::UpdateDrag(float DeltaTime)
{
	AController* Controller = GetController();
	if (Controller)
	{
		Super::UpdateDrag(DeltaTime);
	}
}

void UWRVehicleMovementComponent4W::ClearAllInputValue()
{
	ClearAllInput();
	ClearInput();
	ServerUpdateState(0.f, 0.f, 0.f, 0.f, GetCurrentGear());
}

float UWRVehicleMovementComponent4W::GetSteeringInput()
{
	return SteeringInput;
}

float UWRVehicleMovementComponent4W::GetThrottleInput()
{
	return ThrottleInput;
}

float UWRVehicleMovementComponent4W::GetRowSteeringInput()
{
	return RawSteeringInput;
}

float UWRVehicleMovementComponent4W::GetRowThrottleInput()
{
	return RawThrottleInput;
}

bool UWRVehicleMovementComponent4W::GetHandBrakeInput()
{
	return bRawHandbrakeInput;
}

int32 UWRVehicleMovementComponent4W::GetSpeedByKM()
{
	float ForwardSpeed = GetForwardSpeed();
	return UKismetMathLibrary::Abs_Int(ForwardSpeed * 0.036f);
}

void UWRVehicleMovementComponent4W::SetIdleBrakeInput(float InValue)
{
	IdleBrakeInput = InValue;
}

void UWRVehicleMovementComponent4W::SetStopThreshold(float InValue)
{
	StopThreshold = InValue;
}
