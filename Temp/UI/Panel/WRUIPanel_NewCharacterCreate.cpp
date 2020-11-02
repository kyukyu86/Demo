// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_NewCharacterCreate.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRCharacterCustomizingManager.h"
#include "Manager/WRAccountManager.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRUIManager.h"
#include "WRUIPanel_SubCharacterCreate.h"

#include "Localization/WRLocalizationProcessor.h"

//#define DEFAULT_MALE_START_TID 3
//#define DEFAULT_FEMALE_START_TID 12

static FLinearColor IMAGE_CYBER_FOCUSED = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
//static FLinearColor IMAGE_CYBER_NORMAL = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);
static FLinearColor IMAGE_CYBER_NORMAL = FLinearColor(0.784f, 0.784f, 0.784f, 1.0f);
static FName FONT_CYBER_NORMAL = FName("Title_01");
static FName FONT_CYBER_FOCUSED = FName("Title_02");


void UWRUIPanel_NewCharacterCreate::NativeConstruct()
{
	Super::NativeConstruct();

	SetFocusableInputMode(true, this);
	WRCharacterCustomizingManager::Get()->GetPrePhaseChangedDelegate().AddUObject(this, &UWRUIPanel_NewCharacterCreate::OnPrePhaseChanged);
	WRCharacterCustomizingManager::Get()->GetPostPhaseChangedDelegate().AddUObject(this, &UWRUIPanel_NewCharacterCreate::OnPostPhaseChanged);
	WRCharacterCustomizingManager::Get()->GetPhaseCompletedDelegate().AddUObject(this, &UWRUIPanel_NewCharacterCreate::OnPhaseCompleted);
	InitializeFontProperty();
	InitializeImageColors();
	CanvasPanel_MessageBox->SetVisibility(ESlateVisibility::Collapsed);
	CanvasPanel_ButtonInfo->SetVisibility(ESlateVisibility::Collapsed);
	IMMessageSender::RequestSelectPlayerName();
	//PlayAnimationByName("TopMSG_Loop",0.0f, 1000000);
}

void UWRUIPanel_NewCharacterCreate::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();
	if (Phase == EWRCharacterCustomizingPhase::PartsDetailSetting)
	{
		if (bUpdatePartsNameRequired)
		{
			bool bUpdated = UpdateChildUI(EWRCharacterCustomizingPhase::PartsDetailSetting);
			if (bUpdated)
				bUpdatePartsNameRequired = false;
		}


	}


	
}

void UWRUIPanel_NewCharacterCreate::NativeDestruct()
{
	SetFocusableInputMode(false);

	WRCharacterCustomizingManager::Get()->RemoveCharacters(false);

	Super::NativeDestruct();
}

FReply UWRUIPanel_NewCharacterCreate::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (WRCharacterCustomizingManager::Get()->IsInputAllowed() == false)
		return FReply::Handled();

	if (InKeyEvent.IsRepeat())
	{
		return FReply::Handled();
	}

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	switch (ConvertedInputType)
	{
	case EWRInput::Circle:
		{
			OnCirclePressed();
			return FReply::Handled();
		}
		break;

	case EWRInput::Cross:
		{
			OnCrossPressed();
			return FReply::Handled();
		}
		break;

	case EWRInput::DpadLeft:
		{
			OnDPadLeftPressed();
			return FReply::Handled();
		}
		break;

	case EWRInput::DpadRight:
		{
			OnDPadRightPressed();
			return FReply::Handled();
		}
		break;
	case EWRInput::Triangle:
		{
			OnTriaglePressed();
		return FReply::Handled();
		}

	case EWRInput::Option:
		{
			OnOptionPressed();
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

void UWRUIPanel_NewCharacterCreate::OnPrePhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase)
{
	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		break;
	case EWRCharacterCustomizingPhase::Naming:
		WRTextBlock_CharacterName->SetText(WRCharacterCustomizingManager::Get()->GetCharacterName());
		break;

	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::OnPostPhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase)
{
	InitializeImageColors();
	InitializeFontProperty();

	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
		WRTextBlock_GenderSelect->SetFontRawName(FONT_CYBER_FOCUSED);
		Image_BG_Bar_GenderSelect->SetBrushTintColor(FSlateColor(IMAGE_CYBER_FOCUSED));

		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		WRTextBlock_Preset->SetFontRawName(FONT_CYBER_FOCUSED);
		Image_BG_Bar_Preset->SetBrushTintColor(FSlateColor(IMAGE_CYBER_FOCUSED));
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		WRTextBlock_SkinColor->SetFontRawName(FONT_CYBER_FOCUSED);
		Image_BG_Bar_SkinColor->SetBrushTintColor(FSlateColor(IMAGE_CYBER_FOCUSED));
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		WRTextBlock_PartsDetailSetting->SetFontRawName(FONT_CYBER_FOCUSED);
		Image_BG_Bar_PartsDetailSetting->SetBrushTintColor(FSlateColor(IMAGE_CYBER_FOCUSED));
		break;
	case EWRCharacterCustomizingPhase::Naming:
		CanvasPanel_MessageBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		PlayAnimationByName("MessageBoxAnimation");
		WRTextBlock_Naming->SetFontRawName(FONT_CYBER_FOCUSED);
		Image_BG_Bar_Naming->SetBrushTintColor(FSlateColor(IMAGE_CYBER_FOCUSED));
		break;

	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::OnPhaseCompleted(const EWRCharacterCustomizingPhase& IN InPhase)
{
	switch (InPhase)
	{
	case EWRCharacterCustomizingPhase::None:
	{
		if (Panel_SubCharacterCreate == nullptr)
		{
			Panel_SubCharacterCreate = WRUIManager::Get()->FindUI<UWRUIPanel_SubCharacterCreate>(EWRUIPanelKind::SubCharacterCreate);
			if (Panel_SubCharacterCreate == nullptr)
			{
				ensureMsgf(false, TEXT("SubPanel has not been created."));

			}
		}
	}
		break;
	case EWRCharacterCustomizingPhase::GenderSelect:
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
		break;
	case EWRCharacterCustomizingPhase::SkinColor:
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
		break;
	case EWRCharacterCustomizingPhase::Naming:
		CanvasPanel_MessageBox->SetVisibility(ESlateVisibility::Collapsed);
		break;

	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::InitializeImageColors()
{
	Image_BG_Bar_GenderSelect->SetBrushTintColor(FSlateColor(IMAGE_CYBER_NORMAL));
	Image_BG_Bar_Preset->SetBrushTintColor(FSlateColor(IMAGE_CYBER_NORMAL));
	Image_BG_Bar_SkinColor->SetBrushTintColor(FSlateColor(IMAGE_CYBER_NORMAL));
	Image_BG_Bar_PartsDetailSetting->SetBrushTintColor(FSlateColor(IMAGE_CYBER_NORMAL));
	Image_BG_Bar_Naming->SetBrushTintColor(FSlateColor(IMAGE_CYBER_NORMAL));
}

void UWRUIPanel_NewCharacterCreate::InitializeFontProperty()
{
	WRTextBlock_GenderSelect->SetFontRawName(FName("Title_01"));
	WRTextBlock_Preset->SetFontRawName(FName("Title_01"));
	WRTextBlock_SkinColor->SetFontRawName(FName("Title_01"));
	WRTextBlock_PartsDetailSetting->SetFontRawName(FName("Title_01"));
	WRTextBlock_Naming->SetFontRawName(FName("Title_01"));
}

void UWRUIPanel_NewCharacterCreate::OnDPadLeftPressed()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
	{
		WRCharacterCustomizingManager::Get()->SwapGender(true);
		UpdateChildUI(Phase);
	}
		break;
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		bool SwappedPresetSameAsEquippedPreset = false;
		WRCharacterCustomizingManager::Get()->SwapPresetPart(true, SwappedPresetSameAsEquippedPreset);
		if (SwappedPresetSameAsEquippedPreset == false)
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		}
		else
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		}
		break;
	}
	case EWRCharacterCustomizingPhase::SkinColor:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		bool SwappedItemSameAsEquippedItem = false;
		WRCharacterCustomizingManager::Get()->SwapParts(true, SwappedItemSameAsEquippedItem);
		if (SwappedItemSameAsEquippedItem == false)
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		}
		else
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		}

		UpdateChildUI(Phase);
		break;
	}
	case EWRCharacterCustomizingPhase::Naming:
	{
		break;
	}

	default:
		break;
	}

	
}

void UWRUIPanel_NewCharacterCreate::OnDPadRightPressed()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
	{
		WRCharacterCustomizingManager::Get()->SwapGender(false);
		UpdateChildUI(Phase);
		break;
	}
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		bool SwappedPresetSameAsEquippedPreset = false;
		WRCharacterCustomizingManager::Get()->SwapPresetPart(false, SwappedPresetSameAsEquippedPreset);
		if (SwappedPresetSameAsEquippedPreset == false)
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		}
		else
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		}		
		break;
	}
	case EWRCharacterCustomizingPhase::SkinColor:
	{
		
		break;
	}
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		bool SwappedItemSameAsEquippedItem = false;
		WRCharacterCustomizingManager::Get()->SwapParts(false, SwappedItemSameAsEquippedItem);
		if (SwappedItemSameAsEquippedItem == false)
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_SelectKey").ToString());
		}
		else
		{
			UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		}

		UpdateChildUI(Phase);
		break;
	}
	case EWRCharacterCustomizingPhase::Naming:
	{
		break;
	}

	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::OnCirclePressed()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	WRCharacterCustomizingManager* CustomizingManager = WRCharacterCustomizingManager::Get();
	if (CustomizingManager == nullptr)
		return;

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_OK.UI_OK"));
		CustomizingManager->CompletePhase();
		
		break;
	}
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_OK.UI_OK"));
		if (CustomizingManager->IsEquippedPresetAndSelectedPresetSame())
			CustomizingManager->CompletePhase();
		else
		{
			if (CustomizingManager->EquipPreset())
			{
				UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
			}
		}
			

		break;
	}
	case EWRCharacterCustomizingPhase::SkinColor:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		//if()
		WRSFXManager::Get()->Spawn2D(TEXT("UI_OK.UI_OK"));
		CustomizingManager->SelectPart();
		UpdateChildUI(Phase);
		bUpdatePartsNameRequired = true;

// 		if (CustomizingManager->IsEquippedPresetAndSelectedPresetSame())
// 			CustomizingManager->CompletePhase(EWRPhaseMovableBitFlag::UI);
// 		else
// 			CustomizingManager->EquipPreset();
		break;
	}
	case EWRCharacterCustomizingPhase::Naming:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_OK.UI_OK"));
		CustomizingManager->ManufactureAllData();
		IMMessageSender::RequestCreatePlayer(WRTextBlock_CharacterName->GetText().ToString(), WRCharacterCustomizingManager::Get()->GetSelectedPresetTID(), WRAccountManager::Get()->GetCustomizingSelectedNoneItemEquipmentLookData(), WRAccountManager::Get()->GetCustomizingSelectedCostumeItemArray());
		CustomizingManager->CompletePhase();
		break;
	}
		
	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::OnCrossPressed()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));
		WRCharacterCustomizingManager::Get()->CompletePhase(EWRCharacterCustomizingPhaseMove::Previous);
		break;
	}
	case EWRCharacterCustomizingPhase::SkinColor:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));
		WRCharacterCustomizingManager::Get()->BackwardFromDetailPart();
		UpdateChildUI(Phase);
		bUpdatePartsNameRequired = true;
		break;
	}
	case EWRCharacterCustomizingPhase::Naming:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));
		WRCharacterCustomizingManager::Get()->CompletePhase(EWRCharacterCustomizingPhaseMove::Previous);
		
		break;
	}

	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::OnTriaglePressed()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::GenderSelect:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::SkinColor:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		break;
	}
	case EWRCharacterCustomizingPhase::Naming:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));
		IMMessageSender::RequestSelectPlayerName();
		break;
	}

	default:
		break;
	}
}

void UWRUIPanel_NewCharacterCreate::OnAnimationFinished_Implementation(const UWidgetAnimation* IN InAnimation)
{
	Super::OnAnimationFinished_Implementation(InAnimation);

	if (InAnimation->MovieScene->GetFName() == FName("MessageBoxAnimation"))
	{
		WRCharacterCustomizingManager::Get()->ReleaseInput();
	}
}

void UWRUIPanel_NewCharacterCreate::OnOptionPressed()
{
	EWRCharacterCustomizingPhase& Phase = WRCharacterCustomizingManager::Get()->GetPhase();

	switch (Phase)
	{
	case EWRCharacterCustomizingPhase::PresetSelect:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));
		UpdateSelectButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_NextKey").ToString());
		WRCharacterCustomizingManager::Get()->CompletePhase(EWRCharacterCustomizingPhaseMove::End);
		//WRCharacterCustomizingManager::Get()->LockInput();
	}
		break;
	case EWRCharacterCustomizingPhase::PartsDetailSetting:
	{
		WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));
		WRCharacterCustomizingManager::Get()->CompletePhase(EWRCharacterCustomizingPhaseMove::End);
		//WRCharacterCustomizingManager::Get()->LockInput();
	}
		break;
	default:
		break;
	}
}

bool UWRUIPanel_NewCharacterCreate::UpdateChildUI(const EWRCharacterCustomizingPhase& IN InPhase)
{
	bool bResult = false;

	if (Panel_SubCharacterCreate == nullptr)
	{
		Panel_SubCharacterCreate = WRUIManager::Get()->FindUI<UWRUIPanel_SubCharacterCreate>(EWRUIPanelKind::SubCharacterCreate);
		if (Panel_SubCharacterCreate != nullptr)
		{
			bResult = Panel_SubCharacterCreate->UpdateSubSelectedNameText(InPhase);
			if (WRCharacterCustomizingManager::Get()->IsUpperPartsSelecting() == false)
				UpdateBackButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_PartsMenuKey").ToString());
		}
		else
		{
			ensureMsgf(false, TEXT("SubPanel has not been created."));
		}
	}
	else
	{
		bResult = Panel_SubCharacterCreate->UpdateSubSelectedNameText(InPhase);

 		if (WRCharacterCustomizingManager::Get()->IsUpperPartsSelecting() == false)
 			UpdateBackButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_PartsMenuKey").ToString());
		else
			UpdateBackButtonText(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::CommonStringTable, "Common_BackKey").ToString());
	}

	return bResult;
}

void UWRUIPanel_NewCharacterCreate::UpdateSelectButtonText(const FString& IN InStringToChagne)
{
	if (Panel_SubCharacterCreate)
		Panel_SubCharacterCreate->SetSelectButtonText(InStringToChagne);
}

void UWRUIPanel_NewCharacterCreate::UpdateBackButtonText(const FString& IN InStringToChagne)
{
	if (Panel_SubCharacterCreate)
		Panel_SubCharacterCreate->SetBackButtonText(InStringToChagne);
}

void UWRUIPanel_NewCharacterCreate::SetPlayerName(const FString& IN InName)
{
	WRTextBlock_CharacterName->SetText(InName);
}
