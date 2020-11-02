// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_TargetLocationSearched.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRUIManager.h"


void UWRUIPanel_TargetLocationSearched::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimationByName(FName("AppearAnimation"));
}

void UWRUIPanel_TargetLocationSearched::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation->MovieScene->GetFName() == FName("AppearAnimation"))
	{
		WRUIManager::Get()->CloseUI(UIHandle);
	}
}
