// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <Components/StereoLayerComponent.h>

#include "WRStereoLayerComponent.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRStereoLayerComponent : public UStereoLayerComponent
{
	GENERATED_BODY()

public:
	
#if UE_4_25
	void SetStereoLayerShape(EStereoLayerShape InType) { }
	void SetStereoLayerType(EStereoLayerType InType) {  }
	void SetStereoLayerPriority(int32 InPriority) {  }
#else // UE_4_25
	void SetStereoLayerShape(EStereoLayerShape InType) { StereoLayerShape = InType; }
	void SetStereoLayerType(EStereoLayerType InType) { StereoLayerType = InType; }
	void SetStereoLayerPriority(int32 InPriority) { Priority = InPriority; }
#endif // UE_4_25
};

