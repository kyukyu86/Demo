// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_QuestMarkForTargetLocationGuide.h"
#include "Manager/WRTargetLocationGuideManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Define/WRUIDefine.h"

void UWRUISlot_QuestMarkForTargetLocationGuide::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UWRTextBlock, WRTextBlock_Dist);
}

void UWRUISlot_QuestMarkForTargetLocationGuide::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Tick_UpdateDistText();
}

void UWRUISlot_QuestMarkForTargetLocationGuide::Tick_UpdateDistText()
{
	if (TargetLocationInfoBase == nullptr)
		return;
		
	FVector Dir = TargetLocationInfoBase->GetTargetLocation() - WRInputSystemManager::Get()->GetHMDLocation();
	float Dist = Dir.Size();

	FString DistStr = FString::Printf(TEXT("%d m"), int32(Dist * 0.01f));
	WRTextBlock_Dist->SetText(FText::FromString(DistStr));
}

