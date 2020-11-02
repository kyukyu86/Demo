// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_SubCharacterCreate.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRCharacterCustomizingManager.h"
#include "Manager/WRSFXManager.h"
#include "Utility/WREnumUtility.h"
#include "WRUIPanel_NewCharacterCreate.h"
#include "Manager/WRUIManager.h"

#include "Localization/WRLocalizationProcessor.h"

void UWRUIPanel_SubCharacterCreate::NativeConstruct()
{
	Super::NativeConstruct();

	WRCharacterCustomizingManager::Get()->GetPhaseCompletedDelegate().AddUObject(this, &UWRUIPanel_SubCharacterCreate::OnPhaseCompleted);
	WRCharacterCustomizingManager::Get()->GetPrePhaseChangedDelegate().AddUObject(this, &UWRUIPanel_SubCharacterCreate::OnPrePhaseChanged);
	WRCharacterCustomizingManager::Get()->GetPostPhaseChangedDelegate().AddUObject(this, &UWRUIPanel_SubCharacterCreate::OnPostPhaseChanged);

	CanvasPanel_Decision->SetVisibility(ESlateVisibility::Collapsed);
	CanvasPanel_Back->SetVisibility(ESlateVisibility::Hidden);
	WidgetSwitcher_Scene->SetActiveWidgetIndex(UWRUIPanel_SubCharacterCreate::Title);

	//PlayAnimationByName("SubTitleFlyOutAnimation");
	UWRUIPanel_NewCharacterCreate* ParentPanel = WRUIManager::Get()->FindUI<UWRUIPanel_NewCharacterCreate>(EWRUIPanelKind::NewCharacterCreate);
	if (ParentPanel)
		ParentPanel->SetSubCharacterCreatePanel(this);

	WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
	TrunOffAllMarks();
	WRCharacterCustomizingManager::Get()->CompletePhase();
}

void UWRUIPanel_SubCharacterCreate::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUIPanel_SubCharacterCreate::UpdateMark()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	TrunOffAllMarks();

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
		Image_GenderChoice_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CanvasPanel_Decision->SetVisibility(ESlateVisibility::Collapsed);
		CanvasPanel_Back->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		Image_GenderChoice_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_Preset_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CanvasPanel_Decision->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		CanvasPanel_Back->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		Image_SkinColor_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		Image_GenderChoice_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_Preset_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_SkinColor_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_DetailSetting_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_DetailSetting_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EWRCharacterCustomizingPhase::Naming:
		Image_Naming_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;

	default:
		break;
	}
}

void UWRUIPanel_SubCharacterCreate::TrunOffAllMarks()
{
	Image_GenderChoice_ON->SetVisibility(ESlateVisibility::Collapsed);
	Image_Preset_ON->SetVisibility(ESlateVisibility::Collapsed);
	Image_SkinColor_ON->SetVisibility(ESlateVisibility::Collapsed);
	Image_DetailSetting_ON->SetVisibility(ESlateVisibility::Collapsed);
	Image_Naming_ON->SetVisibility(ESlateVisibility::Collapsed);
}

bool UWRUIPanel_SubCharacterCreate::UpdateSubSelectedNameText(const EWRCharacterCustomizingPhase& IN InPhase)
{
	WRCharacterCustomizingManager* const CustomizingManager = WRCharacterCustomizingManager::Get();

	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
	{
		WRCharacterCustomizingManager::Gender SelectedGender = CustomizingManager->GetSelectedGender();
		if (SelectedGender == WRCharacterCustomizingManager::Gender::Male)
		{
			WRTextBlock_SelectedName->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_MaleUpperKey"));
		}
		else if (SelectedGender == WRCharacterCustomizingManager::Gender::Female)
		{
			WRTextBlock_SelectedName->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_FemaleUpperKey"));
		}
	}
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		WRCharacterCustomizingManager::Gender SelectedGender = CustomizingManager->GetSelectedGender();
		if (SelectedGender == WRCharacterCustomizingManager::Gender::Male)
		{
 			FString Gender = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_MaleUpperKey").ToString();
 			FString Preset = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_PresetUpperKey").ToString();
			FString SelectedName = Gender + TEXT(" ") + Preset;
			WRTextBlock_SelectedName->SetText(SelectedName);
		}
		else if (SelectedGender == WRCharacterCustomizingManager::Gender::Female)
		{
			FString Gender = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_FemaleUpperKey").ToString();
			FString Preset = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_PresetUpperKey").ToString();
			FString SelectedName = Gender + TEXT(" ") + Preset;
			WRTextBlock_SelectedName->SetText(SelectedName);
		}
	}
		break; 
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		FText SelectedName = WRTextBlock_SelectedName->GetText();
		FString SelectedPartsName = WRCharacterCustomizingManager::Get()->GetSelectedPartsName();

		if (SelectedPartsName != SelectedName.ToString())
		{
			WRTextBlock_SelectedName->SetText(SelectedPartsName);
			return true;
		}
	}
		break;
	default:
		break;
	}

	return false;
}

void UWRUIPanel_SubCharacterCreate::SetSelectButtonText(const FString& IN InString)
{
	WRTextBlock_Select->SetText(InString);
}

void UWRUIPanel_SubCharacterCreate::SetBackButtonText(const FString& IN InString)
{
	WRTextBlock_Back->SetText(InString);
}

void UWRUIPanel_SubCharacterCreate::OnAnimationFinished_Implementation(const UWidgetAnimation* IN InAnimation)
{
	Super::OnAnimationFinished_Implementation(InAnimation);

	if (InAnimation->MovieScene->GetFName() == FName("TitleFlyInAnimation"))
	{
		WidgetSwitcher_Scene->SetActiveWidgetIndex(UWRUIPanel_SubCharacterCreate::Contents);
		UpdateMark();
		WRCharacterCustomizingManager::Get()->LockInput();
		PlayAnimationByName("SubTitleFlyInAnimation");
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
	}
	else if (InAnimation->MovieScene->GetFName() == FName("SubTitleFlyOutAnimation"))
	{
		WidgetSwitcher_Scene->SetActiveWidgetIndex(UWRUIPanel_SubCharacterCreate::Title);
		//WRCharacterCustomizingManager::Get()->MoveToNextPhase();
		WRCharacterCustomizingManager::Get()->AddPhaseMoveBit(EWRPhaseMovableBitFlag::UIAnimation);
	}
	else if (InAnimation->MovieScene->GetFName() == FName("SubTitleFlyInAnimation"))
	{
		WRCharacterCustomizingManager::Get()->ReleaseInput();
	}
}

void UWRUIPanel_SubCharacterCreate::OnPhaseCompleted(const EWRCharacterCustomizingPhase& IN InPhase)
{
	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
		PlayAnimationByName("SubTitleFlyOutAnimation");
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		PlayAnimationByName("SubTitleFlyOutAnimation");
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		PlayAnimationByName("SubTitleFlyOutAnimation");
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		PlayAnimationByName("SubTitleFlyOutAnimation");
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		break;
	case EWRCharacterCustomizingPhase::Naming:
		WidgetSwitcher_Scene->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PlayAnimationByName("SubTitleFlyOutAnimation");
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		break;
	default:
		break;
	}
}

void UWRUIPanel_SubCharacterCreate::OnPrePhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase)
{
	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
		WRTextBlock_Title->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_SubCharacterCreateSelectGenderKey"));
		//WRTextBlock_SubTitle->SetText(FName("GENDER OF YOUR CHOICE"));
		Image_GenderChoice_ON->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		UpdateSubSelectedNameText(InPhase);
		StopAnimationByName("TitleFlyInAnimation");
		PlayAnimationByName("TitleFlyInAnimation", 0.0f);
		SetSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		SetBackButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_BackKey").ToString());
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		WRCharacterCustomizingManager::Get()->LockPhase();
		WRCharacterCustomizingManager::Get()->LockInput();
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		WRTextBlock_Title->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_SubCharacterCreateSelectPresetKey"));
		//WRTextBlock_SubTitle->SetText(FName("PRESET OF YOUR CHOICE"));
		
		UpdateSubSelectedNameText(InPhase);
		StopAnimationByName("TitleFlyInAnimation");
		PlayAnimationByName("TitleFlyInAnimation", 0.0f);
		SetSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		SetBackButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_BackKey").ToString());
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		WRCharacterCustomizingManager::Get()->LockPhase();
		WRCharacterCustomizingManager::Get()->LockInput();
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		WRTextBlock_Title->SetText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_SubCharacterCreateSelectPartKey"));
		UpdateSubSelectedNameText(InPhase);
		StopAnimationByName("TitleFlyInAnimation");
		PlayAnimationByName("TitleFlyInAnimation", 0.0f);
		SetSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		SetBackButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_BackKey").ToString());
		WRSFXManager::Get()->Spawn2D(TEXT("CharacterCreateUIOPEN_Cue.CharacterCreateUIOpen_Cue"));
		WRCharacterCustomizingManager::Get()->LockPhase();
		WRCharacterCustomizingManager::Get()->LockInput();
		
		break;
	case EWRCharacterCustomizingPhase::Naming:
		WidgetSwitcher_Scene->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		break;
	}
}

void UWRUIPanel_SubCharacterCreate::OnPostPhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase)
{
	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		break;
	case EWRCharacterCustomizingPhase::Naming:
		break;

	default:
		break;
	}


	
}
