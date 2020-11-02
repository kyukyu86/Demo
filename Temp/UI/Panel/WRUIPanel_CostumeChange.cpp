// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIPanel_CostumeChange.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Network/IMMessageSender.h"
#include "TextBlock.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRPopupManager.h"
#include "Table/WRCharacterPresetTable.h"
#include "Table/Base/WRTableManager.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/PostProcessVolume.h"
#include "Actor/Character/WRCharacter.h"
#include "Actor/EnvActor/Preset/WREnvPresetCharacter.h"
#include <Engine/TextureRenderTarget2D.h>
#include <Engine/Texture.h>
#include "Slate/WidgetRenderer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Image.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "EngineUtils.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"


// 	#define START_MAN_A_PRESET_NUMBER 1
// 	#define START_MAN_B_PRESET_NUMBER 13
// 	#define START_MAN_C_PRESET_NUMBER 16
// 
// 	#define MAX_MAN_A_PRESET_NUMBER 3
// 	#define MAX_MAN_B_PRESET_NUMBER 15
// 	#define MAX_MAN_C_PRESET_NUMBER 18
// 
// 	#define START_WOMAN_A_PRESET_NUMBER 4
// 	#define START_WOMAN_B_PRESET_NUMBER 7
// 	#define START_WOMAN_C_PRESET_NUMBER 10
// 
// 	#define MAX_WOMAN_A_PRESET_NUMBER 6
// 	#define MAX_WOMAN_B_PRESET_NUMBER 9
// 	#define MAX_WOMAN_C_PRESET_NUMBER 12

#define START_MAN_A_PRESET_NUMBER 1
#define START_MAN_B_PRESET_NUMBER 4
#define START_MAN_C_PRESET_NUMBER 7

#define MAX_MAN_A_PRESET_NUMBER 3
#define MAX_MAN_B_PRESET_NUMBER 6
#define MAX_MAN_C_PRESET_NUMBER 9

#define START_WOMAN_A_PRESET_NUMBER 10
#define START_WOMAN_B_PRESET_NUMBER 13
#define START_WOMAN_C_PRESET_NUMBER 16

#define MAX_WOMAN_A_PRESET_NUMBER 12
#define MAX_WOMAN_B_PRESET_NUMBER 15
#define MAX_WOMAN_C_PRESET_NUMBER 18

#define USING_ENVPRESET_NOT_SEPERATE_MODEL

#ifdef USING_ENVPRESET_NOT_SEPERATE_MODEL
#endif // USING_ENVPRESET_SEPERATE_MODEL

#define BUTTON_RE_CLICKABLETIME 1.0f

void UWRUIPanel_CostumeChange::NativeConstruct()
{
	Super::NativeConstruct();

	// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜지더라도 이동과 상관없이 작동되게 변경해달라는 요청으로 Comment화
	//SetFocusableInputMode(true, this);

	BIND_WIDGET(UImage, Image_L1);
	BIND_WIDGET(UImage, Image_R1);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_ChangeButton);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_CancelButton);

	WRPopupManager::Get()->ClosePopup();
}

void UWRUIPanel_CostumeChange::NativeDestruct()
{
	// [ 2020-4-08 : vuvle ] 옷장 UI 가 켜지더라도 이동과 상관없이 작동되게 변경해달라는 요청으로 Comment화
	//SetFocusableInputMode(false);

	Super::NativeDestruct();
}



bool UWRUIPanel_CostumeChange::IsRespawnRequired(const WRTableID IN InCurrentTID, const WRTableID IN InChangingTargetTID)
{
	FWRCharacterPresetTable* CurrentPresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InCurrentTID);
	FWRCharacterPresetTable* TaragetPresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InChangingTargetTID);

	if (CurrentPresetTable && TaragetPresetTable)
	{
		if (CurrentPresetTable->Gender != TaragetPresetTable->Gender)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void UWRUIPanel_CostumeChange::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bButtonClickable == false)
	{
		EnableClickTimeLeft -= InDeltaTime;

		if (EnableClickTimeLeft <= 0.0f)
			bButtonClickable = true;
	}
		
}

FReply UWRUIPanel_CostumeChange::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
// 	EWRInput Input = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
// 	if(AnyKeys.Contains(Input))

	if (OKkeys.Contains(InKeyEvent.GetKey()))
	{
		OnOKKeyClicked();
	}
	else if (CancelKeys.Contains(InKeyEvent.GetKey()))
	{
		OnCancelKeyClicked();
	}
	else if (LeftShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnL1KeyClicked();
	}
	else if (RightShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnR1KeyClicked();
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

FReply UWRUIPanel_CostumeChange::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (OKkeys.Contains(InKeyEvent.GetKey()))
	{
		OnOKKeyPressed();
	}
	else if (CancelKeys.Contains(InKeyEvent.GetKey()))
	{
		OnCancelKeyPressed();
	}
	else if (LeftShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnL1KeyPressed();
	}
	else if (RightShiftKeys.Contains(InKeyEvent.GetKey()))
	{
		OnR1KeyPressed();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UWRUIPanel_CostumeChange::OnOKKeyClicked()
{
	CanvasPanel_ChangeButton->SetRenderOpacity(1);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_OK.UI_OK"));
	IMMessageSender::RequestChangePreset(CurrentPresetTID);
}

void UWRUIPanel_CostumeChange::OnCancelKeyClicked()
{
	CanvasPanel_CancelButton->SetRenderOpacity(1);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_CANCEL.UI_CANCEL"));
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::CostumeChange);
}

void UWRUIPanel_CostumeChange::OnL1KeyClicked()
{
	Image_L1->SetRenderOpacity(1.0);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));

	if (SpawnedPresetCharacter == nullptr)
		return;

	if (bButtonClickable == false)
		return;

	if (ChangeCostume(PreviousOrder) == true)
	{
		bButtonClickable = false;
		EnableClickTimeLeft = BUTTON_RE_CLICKABLETIME;
	}
}

bool UWRUIPanel_CostumeChange::ChangeCostume(const EWRChangeOrder IN InChangeOrder)
{
	WRTableID TargetPresetTID = INVALID_TABLE_TID;

	if (InChangeOrder == NextOrder)
	{
		TargetPresetTID = CalculateNextPresetTID();
	}
	else if (InChangeOrder == PreviousOrder)
	{
		TargetPresetTID = CalculatePreviousPresetTID();
	}
	else
	{
		ensureMsgf(false, TEXT("Not defined order entered."));
	}

	if (ParentPreset->IsProducing() == true)
	{
		//UE_LOG(WRLog, Display, TEXT("UWRUIPanel_CostumeChange::Costume Changing."));
		return false;
	}

	UWRActorComponentEquipmentLook* EquipLook = SpawnedPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLook == nullptr)
		return false;

	if (EquipLook->IsDirecting())
		return false;
			
	if (IsRespawnRequired(CurrentPresetTID, TargetPresetTID))
	{
		CurrentPresetTID = TargetPresetTID;
		SaveChangedPresetTIDToParent(CurrentPresetTID);
		ParentPreset->SwitchPresetCharacter();

		/*FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(CurrentPresetTID);

		if (PresetTable != nullptr)
		{
			UWRActorComponentEquipmentLook* EquipLook = SpawnedPresetCharacter->GetComponent<UWRActorComponentEquipmentLook>();
			if (EquipLook)
			{
				EquipLook->Equip(0);
			}
		}*/
	}
	else
	{
		FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(TargetPresetTID);

		if (PresetTable != nullptr)
		{			
			if(EquipLook->Equip(PresetTable->HoloEquipmentLookTID) == true)
			{
				//UE_LOG(WRLog, Warning, TEXT("Equipped return true."));
				CurrentPresetTID = TargetPresetTID;
				SaveChangedPresetTIDToParent(CurrentPresetTID);
			}
			else
			{
				return false;
				//UE_LOG(WRLog, Warning, TEXT("Equipped return false."));
			}
		}
	}

	return true;
}

void UWRUIPanel_CostumeChange::OnR1KeyClicked()
{
	Image_R1->SetRenderOpacity(1.0);
	WRSFXManager::Get()->Spawn2D(TEXT("UI_SELECT.UI_SELECT"));

	if (SpawnedPresetCharacter == nullptr)
		return;

	if (bButtonClickable == false)
		return;

	if (ChangeCostume(NextOrder) == true)
	{
		bButtonClickable = false;
		EnableClickTimeLeft = BUTTON_RE_CLICKABLETIME;
	}
}

void UWRUIPanel_CostumeChange::OnOKKeyPressed()
{
	CanvasPanel_ChangeButton->SetRenderOpacity(0.5);
}

void UWRUIPanel_CostumeChange::OnCancelKeyPressed()
{
	CanvasPanel_CancelButton->SetRenderOpacity(0.5);
}

void UWRUIPanel_CostumeChange::OnL1KeyPressed()
{
	Image_L1->SetRenderOpacity(0.5);
}

void UWRUIPanel_CostumeChange::OnR1KeyPressed()
{
	Image_R1->SetRenderOpacity(0.5);
}

void UWRUIPanel_CostumeChange::SaveChangedPresetTIDToParent(const WRTableID IN InTID)
{
	if (ParentPreset != nullptr)
	{
		ParentPreset->SetDefaultPresetTID(CurrentPresetTID);
	}
}

WRTableID UWRUIPanel_CostumeChange::CalculateNextPresetTID()
{
	WRTableID NextPresetTID = CurrentPresetTID + 1;

	int8 MaxNumberDependingOnGender = 0;
	int8 StartNumberDependingOnGender = 0;

	GetPresetStartEndTID(StartNumberDependingOnGender, MaxNumberDependingOnGender);

	if (NextPresetTID > MaxNumberDependingOnGender)
	{
		NextPresetTID = StartNumberDependingOnGender;
	}

	return NextPresetTID;
}

WRTableID UWRUIPanel_CostumeChange::CalculatePreviousPresetTID()
{
	WRTableID PreviousPresetTID = CurrentPresetTID - 1;

	int8 MaxNumberDependingOnGender = 0;
	int8 StartNumberDependingOnGender = 0;

	GetPresetStartEndTID(StartNumberDependingOnGender, MaxNumberDependingOnGender);

	if (PreviousPresetTID < StartNumberDependingOnGender)
	{
		PreviousPresetTID = MaxNumberDependingOnGender;
	}

	return PreviousPresetTID;
}

void UWRUIPanel_CostumeChange::GetPresetStartEndTID(int8& OUT OutStartTID, int8& OUT OutEndTID)
{


#ifdef USING_ENVPRESET_NOT_SEPERATE_MODEL
	if (Gender == EWRCharacterGender::Male_A || Gender == EWRCharacterGender::Male_B || Gender == EWRCharacterGender::Male_C)
	{
		OutStartTID = START_MAN_A_PRESET_NUMBER;
	}
	else if (Gender == EWRCharacterGender::Female_A || Gender == EWRCharacterGender::Female_B || Gender == EWRCharacterGender::Female_C)
	{
		OutStartTID = START_WOMAN_A_PRESET_NUMBER;
	}

	OutEndTID = LastPresetTID;
#else // // USING_ENVPRESET_NOT_SEPERATE_MODEL
	if (Gender == EWRCharacterGender::Male_A)
	{
		OutStartTID = START_MAN_A_PRESET_NUMBER;
		OutEndTID = MAX_MAN_A_PRESET_NUMBER;
	}
	else if (Gender == EWRCharacterGender::Male_B)
	{
		OutStartTID = START_MAN_B_PRESET_NUMBER;
		OutEndTID = MAX_MAN_B_PRESET_NUMBER;
	}
	else if (Gender == EWRCharacterGender::Male_C)
	{
		OutStartTID = START_MAN_C_PRESET_NUMBER;
		OutEndTID = MAX_MAN_C_PRESET_NUMBER;
	}
	else if (Gender == EWRCharacterGender::Female_A)
	{
		OutStartTID = START_WOMAN_A_PRESET_NUMBER;
		OutEndTID = MAX_WOMAN_A_PRESET_NUMBER;
	}
	else if (Gender == EWRCharacterGender::Female_B)
	{
		OutStartTID = START_WOMAN_B_PRESET_NUMBER;
		OutEndTID = MAX_WOMAN_B_PRESET_NUMBER;
	}
	else if (Gender == EWRCharacterGender::Female_C)
	{
		OutStartTID = START_WOMAN_C_PRESET_NUMBER;
		OutEndTID = MAX_WOMAN_C_PRESET_NUMBER;
	}
#endif // USING_ENVPRESET_NOT_SEPERATE_MODEL
}

void UWRUIPanel_CostumeChange::SetLastTIDFromPresetCharacter()
{
	if (ParentPreset)
	{
		LastPresetTID = ParentPreset->GetLastPresetTID();
	}
	else
	{
		ensureMsgf(false, TEXT("Parent preset character is not set."));
	}
}

void UWRUIPanel_CostumeChange::SetCurrentPresetTID(const WRTableID IN InTID)
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InTID);

	if (PresetTable == nullptr)
		return;

	Gender = PresetTable->Gender;
	CurrentPresetTID = InTID;
}
