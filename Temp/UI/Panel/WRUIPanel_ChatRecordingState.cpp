// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_ChatRecordingState.h"

void UWRUIPanel_ChatRecordingState::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimationByName("LoopAnimation", 0.F, 0);
}
