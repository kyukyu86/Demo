// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVehicleAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

void UWRVehicleAnimInstance::SetSteering(float InValue)
{
	UKismetMathLibrary::Clamp(InValue, -1.0f, 1.0f);

	SteeringValue = InValue;
}
