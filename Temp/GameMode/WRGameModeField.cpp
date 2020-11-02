// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGameModeField.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInventoryManager.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Component/Character/WRActorComponentVR.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Enum/EWRCharacter.h"
#include "GameFramework/DefaultPawn.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRMapTable.h"
#include "Table/WRCharacterPresetTable.h"
#include "Manager/WRTriggerManager.h"

#ifdef DEF_ITEM_OBTAIN_WHEN_CHARACTER_SPAWNED
#include "Table/WRItemTable.h"
#endif // DEF_ITEM_OBTAIN_WHEN_CHARACTER_SPAWNED

#include "Manager/SingletonClass/WRSingletonManager.h"
#include "Manager/WRMapManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRAccountManager.h"
#include "Manager/WRAnimManager.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "VoiceInterface.h"

void AWRGameModeField::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//====================================================================================

}

void AWRGameModeField::StartPlay()
{
	Super::StartPlay();

	//====================================================================================

	// ..
	WRCharacterManager::Get()->SpawnMe();
#ifdef DEF_ITEM_OBTAIN_WHEN_CHARACTER_SPAWNED
	if (Player)
	{
		// 임시 Item 설정
		WRTableID MainWeaponItemTID = 1, SubWeaponItemTID = 3;
		FWRItemTable* MainWeaponItemTable = WRTableManager::Get()->FindRow<FWRItemTable>(MainWeaponItemTID);
		FWRItemTable* SubWeaponItemTable = WRTableManager::Get()->FindRow<FWRItemTable>(SubWeaponItemTID);
		if (MainWeaponItemTable && SubWeaponItemTable)
		{
			WRInventoryManager::Get()->AddItem(MainWeaponItemTID);
			WRInventoryManager::Get()->AddItem(SubWeaponItemTID);
			Player->SetEquippedWeaponItemTID(MainWeaponItemTID, SubWeaponItemTID, INVALID_TABLE_TID);
		}
	}
#endif // DEF_ITEM_OBTAIN_WHEN_CHARACTER_SPAWNED
	WRTableID MapTid = WRMapManager::Get()->GetReservedMapTID();
	FWRMapTableBase* MapTable = WRTableManager::Get()->FindRowInMapTable(MapTid);
	if (MapTable)
	{
		WRTriggerManager::Get()->LoadTrigger(MapTable->LevelName.ToString());

	}

// 	static bool bbbbb = true;
// 	FTimerHandle FH;
// 	GetWorld()->GetTimerManager().SetTimer(FH, FTimerDelegate::CreateLambda([&]() {
// 		WRMapManager::Get()->LoadLevelByTable(WRTableManager::Get()->FindRow<FWRMapFieldTable>(bbbbb == true ? 1 : 2));
// 	}), 2.0f, false);
// 	bbbbb = !bbbbb;

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	WidgetInfo = FWRWidgetInfo(EWRUIPanelKind::CrossHair, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);


#if PLATFORM_PS4
	IOnlineSubsystem* PS4Subsystem = Online::GetSubsystem(GetWorld());
	IOnlineVoicePtr VoiceInt = PS4Subsystem->GetVoiceInterface();
	VoiceInt->RegisterLocalTalker(0);
#endif //PLATFORM_PS4
}
