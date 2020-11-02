
// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_CheatRightListElement.h"

#include "UI/CustomWidget/WRFocusButton.h"

#include "Button.h"
#include "Image.h"

#include "Utility/WRUIUtility.h"
#include "Define/WRUIDefine.h"

#include "Table/Base/WRTableRow.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRItemTable.h"
#include "Table/WREnpcTable.h"
#include "Table/WRObjectTable.h"

#include "Manager/WRNoticeManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRItemManager.h"

#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRActorUtility.h"
#include "Network/Share/Define/Define_Actor.h"
#include "TextBlock.h"
#include "Network/IMMessageSender.h"
#include "UI/Panel/WRUIPanel_Cheat.h"
#include "Actor/PreviewActor/WRPreviewStudio.h"
#include "WidgetPath.h"

#include "Localization/WRLocalizationProcessor.h"




void UWRUISlot_CheatRightListElement::NativeConstruct()
{
	Super::NativeConstruct();
	
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Name, FocusButton_Name->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_CheatRightListElement::OnClickedName););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Minus, FocusButton_Minus->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_CheatRightListElement::OnClickedMinus););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Plus, FocusButton_Plus->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_CheatRightListElement::OnClickedPlus););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Preview, FocusButton_Preview->GetButton()->OnClicked.AddDynamic(this, &UWRUISlot_CheatRightListElement::OnClickedPreview););
	BIND_WIDGET(UTextBlock, TextBlock_Count);

	LastFocusedButton = FocusButton_Name;
}

void UWRUISlot_CheatRightListElement::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWRUISlot_CheatRightListElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UWRUISlot_CheatRightListElement::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	switch (ConvertedInputType)
	{
	case EWRInput::Circle:
	{
		LastFocusedButton->OnClickedButton();
		return FReply::Handled();
	}
	break;
	}

	return FReply::Unhandled();
}

void UWRUISlot_CheatRightListElement::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	UWRFocusButton* PrevFocusedButton = nullptr;

	for (auto EachWidget : PreviousFocusPath.Widgets)
	{
		if (EachWidget.IsValid() == false)
			continue;

		UWidget* FoundWidget = GetWidgetHandle(EachWidget.Pin().ToSharedRef());
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				PrevFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
		}
	}

	for (auto EachWidget : NewWidgetPath.Widgets.GetInternalArray())
	{
		UWidget* FoundWidget = GetWidgetHandle(EachWidget.Widget);
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				LastFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
		}
	}
}

void UWRUISlot_CheatRightListElement::OnClickedName()
{
	WRTableID TID = FCString::Atoi(*TableData->GetRowName().ToString());

	switch (eListType)
	{
	case EWRCheatRightListType::Item:
	{
		IMMessageSender::RequestCheat(FString::Printf(TEXT("!additem %d %d"), TID, nCount));
	}
	break;
	case EWRCheatRightListType::Monster:
	{
// 		FString strCheat = FString::Printf(TEXT("c_actor %d,%d,%d"), EWRActorType::Npc, TID, nCount);
// 		IMMessageSender::RequestCheat(strCheat);
// 
// 		return;

		FTransform tf = GetMyFrontTransform();

		FWREnpcTable* CastedTable = static_cast<FWREnpcTable*>(TableData);
		if (CastedTable != nullptr)
		{
			FWRCharacterSpawnParams SpawnParam(0, TID, CastedTable->BlueprintID, tf.GetLocation(), tf.GetRotation().Rotator());
			AWRCharacter* SpawnChar = WRCharacterManager::Get()->Spawn<AWRCharacter>(SpawnParam);
			if (SpawnChar != nullptr)
			{
				SpawnChar->SetLocation(tf.GetLocation(), true);
			}
		}
	}
	break;
	case EWRCheatRightListType::IO:
	{
		FTransform tf = GetMyFrontTransform();

		FWRInteractionObjectTable* CastedTable = static_cast<FWRInteractionObjectTable*>(TableData);
		if (CastedTable != nullptr)
		{
// 			FString strCheat = FString::Printf(TEXT("c_actor %d,%d,%d"), EWRActorType::InteractionObject, TID, nCount);
// 			IMMessageSender::RequestCheat(strCheat);
// 
// 			return;

			FWRCharacterSpawnParams SpawnParam(0, TID, CastedTable->MainBPPath, tf.GetLocation(), tf.GetRotation().Rotator());
			AWRCharacterIO* SpawnIO = WRCharacterManager::Get()->Spawn<AWRCharacterIO>(SpawnParam);
			if (SpawnIO != nullptr)
			{
				SpawnIO->OnSpawnComplete(CastedTable, EWRInteractionState::Usable);
				SpawnIO->SetCheatedObject(true);

				// Set Location
				FVector SpawnPosition = tf.GetLocation();
// 				FNavLocation OutNavLocation;
// 				bool bResult = WRActorUtility::ProjectPointToNavigation(SpawnIO, tf.GetLocation(), OutNavLocation);
// 				if (bResult)
// 				{
// 					float OutHeight = 0.0f;
// 					if (WRActorUtility::ModifyZHeightForClient(SpawnIO, OutNavLocation.Location, OutHeight))
// 					{
// 						SpawnPosition.Z = OutHeight;
// 					}
// 				}
				SpawnIO->SetLocation(SpawnPosition);
			}
		}		
	}
	break;
	case EWRCheatRightListType::Character:
	{
		FTransform tf = GetMyFrontTransform();

		FWRNpcTable* CastedTable = static_cast<FWRNpcTable*>(TableData);
		if (CastedTable != nullptr)
		{
			FWRCharacterSpawnParams SpawnParam(0, TID, CastedTable->BlueprintID, tf.GetLocation(), tf.GetRotation().Rotator());
			AWRCharacter* SpawnChar = WRCharacterManager::Get()->Spawn<AWRCharacter>(SpawnParam);
			if (SpawnChar != nullptr)
			{
				FVector SpawnPosition = tf.GetLocation();
				
				SpawnChar->SetLocation(SpawnPosition, true);
			}
		}
	}
	break;
	}
}

void UWRUISlot_CheatRightListElement::OnClickedMinus()
{
	if (nCount > 1)
	{
		--nCount;		
		WRUIUtility::SetTextSafe(TextBlock_Count, FString::FormatAsNumber(nCount));
	}
}

void UWRUISlot_CheatRightListElement::OnClickedPlus()
{
	if (nCount < 999)
	{
		++nCount;
		WRUIUtility::SetTextSafe(TextBlock_Count, FString::FormatAsNumber(nCount));
	}
}

void UWRUISlot_CheatRightListElement::OnClickedPreview()
{
	if (TableData == nullptr)
		return;

	WRTableID TID = FCString::Atoi(*TableData->GetRowName().ToString());
	UWRUIPanel_Cheat* CheatPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Cheat>(EWRUIPanelKind::Cheat);
	if (CheatPanel == nullptr)
		return;


	switch (eListType)
	{
	case EWRCheatRightListType::Item:
	{
		FWRItemTableBase* CastedTable = static_cast<FWRItemTableBase*>(TableData);
			
		if (CastedTable)
		{
			FWREquipmentLookTable* FoundEquipLookTable = nullptr;

			if (CastedTable->MainType == EWRItemMainType::Weapon)
			{
				FWRWeaponItemTable* CastedItemTable = static_cast<FWRWeaponItemTable*>(CastedTable);
				FoundEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(CastedItemTable->EquipmentLookTID);
			}			
			else if (CastedTable->MainType == EWRItemMainType::Costume)
			{
				FWRCostumeItemTable* CastedItemTable = static_cast<FWRCostumeItemTable*>(CastedTable);
				FoundEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(CastedItemTable->EquipmentLookTID);
			}
			else if (CastedTable->MainType == EWRItemMainType::Temporary)
			{
				FWRTemporaryItemTable* CastedItemTable = static_cast<FWRTemporaryItemTable*>(CastedTable);
				FoundEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(CastedItemTable->EquipmentLookTID);
			}
			else if (CastedTable->MainType == EWRItemMainType::Vehicle)
			{
// 				FWRVehicleItemTable* CastedItemTable = static_cast<FWRVehicleItemTable*>(CastedTable);
// 				FoundEquipLookTable = WRTableManager::Get()->FindRow<FWRVehicleTable>(CastedItemTable->VehicleTID);
			}

			if (FoundEquipLookTable == nullptr)
				return;

			if (FoundEquipLookTable->StaticMesh.IsEmpty() == false)
			{
				FWRPreviewInfo PreviewInfo;
				PreviewInfo.Type = EWRPreviewType::Static;
				PreviewInfo.BPPath = FoundEquipLookTable->StaticMesh;

				CheatPanel->ShowPreview(PreviewInfo);
			}
			else if (FoundEquipLookTable->SkeletalMesh.IsEmpty() == false)
			{
				FWRPreviewInfo PreviewInfo;
				PreviewInfo.Type = EWRPreviewType::Skeletal;
				PreviewInfo.BPPath = FoundEquipLookTable->SkeletalMesh;

				CheatPanel->ShowPreview(PreviewInfo);
			}
		}
	}
	break;
	case EWRCheatRightListType::Monster:
	{
		FWREnpcTable* CastedTable = static_cast<FWREnpcTable*>(TableData);
		if (CastedTable != nullptr)
		{
			FWRPreviewInfo PreviewInfo;
			PreviewInfo.Type = EWRPreviewType::DefaultActor;
			PreviewInfo.BPPath = CastedTable->BlueprintID;

			CheatPanel->ShowPreview(PreviewInfo);
		}
	}
	break;
	case EWRCheatRightListType::IO:
	{
		FWRInteractionObjectTable* CastedTable = static_cast<FWRInteractionObjectTable*>(TableData);
		if (CastedTable != nullptr)
		{
			FWRPreviewInfo PreviewInfo;
			PreviewInfo.Type = EWRPreviewType::DefaultActor;
			PreviewInfo.BPPath = CastedTable->MainBPPath;

			CheatPanel->ShowPreview(PreviewInfo);
		}
	}
	break;
	case EWRCheatRightListType::Character:
	{
		FWRNpcTable* CastedTable = static_cast<FWRNpcTable*>(TableData);
		if (CastedTable != nullptr)
		{
			FWRPreviewInfo PreviewInfo;
			PreviewInfo.Type = EWRPreviewType::CustomActor;
			PreviewInfo.BPPath = CastedTable->BlueprintID;

			CheatPanel->ShowPreview(PreviewInfo);
		}
	}
	break;
	}
}

class UButton* UWRUISlot_CheatRightListElement::GetFirstButton()
{
	return FocusButton_Name->GetButton();
}

void UWRUISlot_CheatRightListElement::SetData(EWRCheatRightListType IN InType, FWRTableRow* IN InTable)
{
	eListType = InType;

	switch (InType)
	{
	case EWRCheatRightListType::Item:
	{
		FWRItemTableBase* CastedTable = static_cast<FWRItemTableBase*>(InTable);
		if (CastedTable != nullptr)
		{
			TableData = InTable;
			FString LocalKey = CastedTable->NameLocalKey.ToString();
			FocusButton_Name->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, LocalKey).ToString());
		}
	}
	break;
	case EWRCheatRightListType::Monster:
	{
		FWREnpcTable* CastedTable = static_cast<FWREnpcTable*>(InTable);
		if (CastedTable != nullptr)
		{
			TableData = InTable;
			FString LocalKey = CastedTable->NameLocalkey.ToString();
			FocusButton_Name->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::NPCStringTable, LocalKey).ToString());
		}
	}
	break;
	case EWRCheatRightListType::IO:
	{
		FWRInteractionObjectTable* CastedTable = static_cast<FWRInteractionObjectTable*>(InTable);
		if (CastedTable != nullptr)
		{
			TableData = InTable;
			FocusButton_Name->SetContents(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::ItemStringTable, CastedTable->NameLocalkey).ToString());
		}
	}
	break;
	case EWRCheatRightListType::Character:
	{
		FWRNpcTable* CastedTable = static_cast<FWRNpcTable*>(InTable);
		if (CastedTable != nullptr)
		{
			TableData = InTable;
			FocusButton_Name->SetContents(CastedTable->NameLocalkey.ToString());
		}
	}
	break;
	}
}

FTransform UWRUISlot_CheatRightListElement::GetMyFrontTransform()
{
	FTransform tf;

	FVector vPlayerForward = WRCharacterManager::Get()->GetMyCharacter()->GetActorForwardVector();
	FVector vOffsetLocation = vPlayerForward * 400.0f;
	FVector vPlayerLocation = WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();

	FVector vIOSpawnLocation = vPlayerLocation + vOffsetLocation;
	FRotator rIOSpawnRotation = (vPlayerLocation - vIOSpawnLocation).Rotation();

	tf.SetLocation(vIOSpawnLocation);
	tf.SetRotation(FQuat(rIOSpawnRotation));

	return tf;
}
