// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "WRGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:

	virtual void LostFocus(FViewport* InViewport) override;
	virtual void ReceivedFocus(FViewport* InViewport) override;
	
};
