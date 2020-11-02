// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_EquipNavigator.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRMapManager.h"
#include "UI/CustomWidget/WRProgressBar.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRSFXManager.h"



void UWRUIPanel_EquipNavigator::NativeConstruct()
{
	Super::NativeConstruct();
	
	WRTextBlock_Title->SetVisibility(ESlateVisibility::Collapsed);
	WRTextBlock_OK->SetVisibility(ESlateVisibility::Collapsed);;
	WRTextBlock_Complete->SetVisibility(ESlateVisibility::Collapsed);;

	PlayAnimationByName("OpenAnimation", 0.f, 1, EUMGSequencePlayMode::Forward, OpenAnimationSpeed);
}

void UWRUIPanel_EquipNavigator::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_EquipNavigator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	WRTextBlock_Title->Tick_TypingEffect(InDeltaTime);
	WRTextBlock_OK->Tick_TypingEffect(InDeltaTime);
	WRTextBlock_Complete->Tick_TypingEffect(InDeltaTime);
	WRProgressBar_Gauge->OnTick(InDeltaTime);
}

void UWRUIPanel_EquipNavigator::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation->MovieScene->GetFName() == FName("OpenAnimation"))
	{
		WRTextBlock_Title->PlayTypingEffect(TypingEffectSpeed, FWROnTypingEffectCompleted::CreateUObject(this, &UWRUIPanel_EquipNavigator::OnTypingEffectCompletedForTitle));
	}
	else if (Animation->MovieScene->GetFName() == FName("TextBottomAnimation"))
	{
		WRProgressBar_Gauge->SetTimer(GaugeTime, FWROnProgressCompleted::CreateUObject(this, &UWRUIPanel_EquipNavigator::OnPrgressCompleted)/*, GaugeStepNum*/);		
		WRMapManager::Get()->SetMidpointPauseWhenDirecting(false);
		WRSFXManager::Get()->Spawn2D(TEXT("SFX/gauge_01_Cue.gauge_01_Cue"));
	}
	else if (Animation->MovieScene->GetFName() == FName("CloseAnimation"))
	{
		WRUIManager::Get()->CloseUI(UIHandle);
	}
}

void UWRUIPanel_EquipNavigator::OnTypingEffectCompletedForTitle()
{
	WRTextBlock_OK->PlayTypingEffect(TypingEffectSpeed, FWROnTypingEffectCompleted::CreateUObject(this, &UWRUIPanel_EquipNavigator::OnTypingEffectCompletedForOK));
}

void UWRUIPanel_EquipNavigator::OnTypingEffectCompletedForOK()
{
	PlayAnimationByName("TextBottomAnimation", 0.f, 1, EUMGSequencePlayMode::Forward, TextBottomAnimationSpeed);
}

void UWRUIPanel_EquipNavigator::OnTypingEffectCompletedForComplete()
{
	PlayAnimationByName("CloseAnimation", 0.f, 1, EUMGSequencePlayMode::Forward, CloseAnimationSpeed);
}

void UWRUIPanel_EquipNavigator::OnPrgressCompleted()
{
	WRProgressBar_Gauge->Clear();
	WRProgressBar_Gauge->SetPercent(0.f);
	WRTextBlock_Complete->PlayTypingEffect(TypingEffectSpeed, FWROnTypingEffectCompleted::CreateUObject(this, &UWRUIPanel_EquipNavigator::OnTypingEffectCompletedForComplete));
}
