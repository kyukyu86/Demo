// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_DanceMode.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRSocialTable.h"
#include "Utility/WRUIUtility.h"
#include "Network/IMMessageSender.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Manager/WRAsyncLoadUObjectManager.h"

#include "Localization/WRLocalizationProcessor.h"

void UWRUIPanel_DanceMode::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Content);
	BIND_WIDGET(UImage, Image_NextStep);
	BIND_WIDGET(UWidgetSwitcher, WidgetSwitcher_Type);

	Init();
}

void UWRUIPanel_DanceMode::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUIPanel_DanceMode::SetFunctionType(const UWRUIPanel_DanceMode::EWRFunctionType IN InFunctionType)
{
	CurFunctionType = InFunctionType;
	if (InFunctionType == EWRFunctionType::None)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (InFunctionType == EWRFunctionType::StartDanceMode)
	{
		CurDanceKindIndex = 0;
		SetDanceKind(SocialAnimationTables[CurDanceKindIndex]);
		WidgetSwitcher_Type->SetActiveWidgetIndex(int32(InFunctionType) -1);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (InFunctionType == EWRFunctionType::NextStep)
	{
		TextBlock_Content->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_NextStepKey"));
		WidgetSwitcher_Type->SetActiveWidgetIndex(int32(InFunctionType) - 1);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWRUIPanel_DanceMode::SetNextStepInput(const EWRInput IN InInput, UTexture2D* IN InIcon)
{
	WRUIUtility::SetResourceObject(Image_NextStep, InIcon);
	NextStepInput = InInput;
	SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::NextStep);
}

void UWRUIPanel_DanceMode::SetNextStepInput(const EWRInput IN InInput, const FString& IN InIconPath)
{
	if (AsyncLoadHashForIconTexture.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadHashForIconTexture);
		AsyncLoadHashForIconTexture.Empty();
	}

	NextStepInput = InInput;
	AsyncLoadHashForIconTexture = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InIconPath, FWRCompleteAsyncLoad::CreateUObject(this, &UWRUIPanel_DanceMode::OnLoadCompletedForIconTexture));
	Image_NextStep->SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUIPanel_DanceMode::OnStartDanceModeClicked()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	// 이동 중이면 댄스 안되게
	if (MyCharacter->IsInputPad())
		return;

	SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::None);
	IMMessageSender::RequestBeginDance(SocialAnimationTables[CurDanceKindIndex]->DanceGroupTID);
}

void UWRUIPanel_DanceMode::OnNextStepClicked()
{
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	UWRActorComponentDanceModeForMine* DanceModeComponent = MyCharacter->GetComponent<UWRActorComponentDanceModeForMine>();
	if (DanceModeComponent == nullptr)
		return;

	SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::None);
	DanceModeComponent->GoNextStep();
}

void UWRUIPanel_DanceMode::NextDanceKind()
{
	if (++CurDanceKindIndex >= SocialAnimationTables.Num())
		CurDanceKindIndex = 0;

	SetDanceKind(SocialAnimationTables[CurDanceKindIndex]);
}

void UWRUIPanel_DanceMode::PrevDanceKind()
{
	if (--CurDanceKindIndex < 0)
		CurDanceKindIndex = SocialAnimationTables.Num() - 1;

	SetDanceKind(SocialAnimationTables[CurDanceKindIndex]);
}

void UWRUIPanel_DanceMode::Init()
{
	UDataTable* SocialAnimationTables_Internal = WRTableManager::Get()->GetTable<FWRSocialAnimationTable>();
	const TMap<FName, uint8*>& SocialAnimationTableMap = SocialAnimationTables_Internal->GetRowMap();
	int32 Count = 0;
	for (auto& Data : SocialAnimationTableMap)
	{
		FWRSocialAnimationTable* SocialAnimationTable = reinterpret_cast<FWRSocialAnimationTable*>(Data.Value);
		if (SocialAnimationTable == nullptr)
			continue;

		if(SocialAnimationTable->SocialType != EWRSocialActionType::Dance)
			continue;

		SocialAnimationTables.Add(SocialAnimationTable);
	}
}

void UWRUIPanel_DanceMode::SetDanceKind(const FWRSocialAnimationTable* IN InSocialAnimationTable)
{
	if (InSocialAnimationTable == nullptr)
		return;
	
	FString LocalKey = WRUIUtility::ConvertFNameToFString(InSocialAnimationTable->Name);
	TextBlock_Content->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, LocalKey));
}

void UWRUIPanel_DanceMode::OnLoadCompletedForIconTexture(UObject* IN InIconTexture, FString IN InHash)
{
	AsyncLoadHashForIconTexture.Empty();
	if (InIconTexture == nullptr)
	{
		Image_NextStep->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	WRUIUtility::SetResourceObject(Image_NextStep, InIconTexture);
	Image_NextStep->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetFunctionType(UWRUIPanel_DanceMode::EWRFunctionType::NextStep);
}
