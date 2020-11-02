// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGameViewportClient.h"
#include "Engine/World.h"
#include "Manager/WRUIManager.h"

void UWRGameViewportClient::LostFocus(FViewport* InViewport)
{

	Super::LostFocus(InViewport);
}

void UWRGameViewportClient::ReceivedFocus(FViewport* InViewport)
{
	Super::ReceivedFocus(InViewport);

//  #if WITH_EDITOR || PLATFORM_WINDOWS
//     	if (WRUIManager::IsValid() == true)
//     	{
//     		WRUIManager::Get()->FocusSavedUI();
//     	}
//  #endif // WITH_EDITOR

}
