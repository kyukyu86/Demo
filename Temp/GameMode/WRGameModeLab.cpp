// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGameModeLab.h"

#include "Actor/Character/WRCharacterMine.h"

#include "Core/WRGameInstance.h"

#include "Define/WRPathDefine.h"

#include "Engine/PostProcessVolume.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"

#include "Manager/WRMapManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterCustomizingManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRMapTable.h"

#include "UI/Panel/WRUIPanel_MonitorInLab.h"

#include "Utility/WRActorUtility.h"

#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include "Network/IMMessageSender.h"


void AWRGameModeLab::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//====================================================================================
}

void AWRGameModeLab::StartPlay()
{
	Super::StartPlay();

	//====================================================================================

	// ..
	FVector SpawnLocation;
	FRotator SpawnRotation;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		SpawnLocation = It->GetActorLocation();
		SpawnRotation = It->GetActorRotation();
	}

	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(0, 1, "PC/Player_Man_FirstPerson.Player_Man_FirstPerson", SpawnLocation, SpawnRotation);
	AWRCharacterMine* Player = WRCharacterManager::Get()->Spawn<AWRCharacterMine>(SpawnParam, true, true);

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

	if (Player->IsValidLowLevel() == true)
	{
		Player->SetActorHiddenInGame(true);
		Player->SetEnableSyncHMDTick(false);
		Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	WRConsoleManager::Get()->SetEngineConsole(TEXT("a.URO.Enable"), 0);

//	Hash = WRVFXManager::Get()->AsyncSpawn(EFFECT_PATH_4_NIAGARA_PATH_4_TITLE_APPEAR, 0, 0, 0, EWRVFX::None, FRotator::ZeroRotator, WRAsyncHandle::CreateUObject(this, &AWRGameModeLab::OnAsyncLoadCompleted));
//	if (Hash == WRASYNCLOADMANAGER_INVALID)
//	{
//#if UE_EDITOR
//		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Hash is invalid."), TEXT("AWRGameModeLab::StartPlay()"));
//#else // UE_EDITOR
//#endif // UE_EDITOR
//	}

	AWRVFX* VFX = CreateVFX(EFFECT_PATH_4_NIAGARA_PATH_4_TITLE_APPEAR);
	if (VFX == nullptr)
	{
		return;
	}

	UNiagaraComponent* NiagaraComponent = VFX->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}

	NiagaraComponent->OnSystemFinished.AddUniqueDynamic(this, &AWRGameModeLab::OnAppearDirectionCompleted);

	//FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Title, EWRUIWidgetCreationBitFlag::IgnoreDepth);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Title, FTransform(FRotator(0.f, 180.f, 0.f), FVector(172.09, -98.5, 200), FVector::OneVector), EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);

	FTimerHandle TimerHandle;
	WRActorUtility::SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AWRGameModeLab::SpawnMaintainVFX), 4.5f, false);

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		FPostProcessSettings* Settings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);
		if (Settings)
			Settings->DepthOfFieldFstop = 1;
		break;
	}

	WRMaterialParameterCollectionManager::Get()->SetScalarParameterValue(EWRMPC::Monitor, "EmissiveValue", 0);

	WRInputSystemManager::Get()->SetLocationLock(true);
}

void AWRGameModeLab::StartToLeaveMap()
{
	WRConsoleManager::Get()->SetEngineConsole(TEXT("a.URO.Enable"), 1);
}

void AWRGameModeLab::OnPressed(const uint32 InPSN)
{
	PSN = InPSN;

	// [ 2020-8-12 : animal14 ] 루핑 이팩트 삭제
	WRVFXManager::Get()->Remove(VFXUID, false, true);
	VFXUID = 0;

	// [ 2020-8-12 : animal14 ] 
	AWRVFX* VFX = CreateVFX(EFFECT_PATH_4_NIAGARA_PATH_4_TITLE_DISAPPEAR);
	if (VFX == nullptr)
	{
		return;
	}

	UNiagaraComponent* NiagaraComponent = VFX->GetCastedComponent<UNiagaraComponent>(EWRVFXComponent::FX);
	if (NiagaraComponent == nullptr)
	{
		return;
	}

	NiagaraComponent->OnSystemFinished.AddUniqueDynamic(this, &AWRGameModeLab::OnDisappearDirectionCompleted);
}

void AWRGameModeLab::SpawnMaintainVFX()
{
	CreateVFX(EFFECT_PATH_4_NIAGARA_PATH_4_TITLE_MAINTAIN);
}

void AWRGameModeLab::OnAppearDirectionCompleted(class UNiagaraComponent* InComponent)
{
	if (InComponent == nullptr)
	{
		return;
	}

	InComponent->OnSystemFinished.RemoveAll(this);

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Title, EWRUIWidgetCreationBitFlag::IgnoreDepth);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

void AWRGameModeLab::OnDisappearDirectionCompleted(class UNiagaraComponent* InComponent)
{
	if (InComponent == nullptr)
	{
		return;
	}

	InComponent->OnSystemFinished.RemoveAll(this);

	if (PSN == 0)
	{
		
#ifdef DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		WRCharacterCustomizingManager::Get()->StartCharacterCreationProcess();

// 		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::NewCharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
// 		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 		WidgetInfo = FWRWidgetInfo(EWRUIPanelKind::SubCharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
// 		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		
#else // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
#endif // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		
	}
	else
	{
#ifdef DEF_USING_CHARACTER_SELECT_UI_BY_VUVLE_200211
		WRAccountManager::Get()->SetPSN(Packet.psn);

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
#else // DEF_USING_CHARACTER_SELECT_UI_BY_VUVLE_200211
		IMMessageSender::RequestDeletePlayer(PSN);

	
#ifdef DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		WRCharacterCustomizingManager::Get()->StartCharacterCreationProcess();

// 		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::NewCharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
// 		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 		WidgetInfo = FWRWidgetInfo(EWRUIPanelKind::SubCharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
// 		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
#else // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::CharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
#endif // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		
#endif // DEF_USING_CHARACTER_SELECT_UI_BY_VUVLE_200211
	}
}

AWRVFX* AWRGameModeLab::CreateVFX(const FString& InPath)
{
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(InPath, nullptr, nullptr);
	if (VFX == nullptr)
	{
		return nullptr;
	}

	VFXUID = VFX->GetUniqueID();

	FVector NewLocation = FVector::ZeroVector;
	NewLocation.X = -125.0f;
	NewLocation.Y = 15.0f;
	NewLocation.Z = 80.0f;

	FRotator NewRotation = FRotator::ZeroRotator;
	NewRotation.Yaw = 180.0f;

	VFX->SetActorLocationAndRotation(NewLocation, NewRotation);

	return VFX;
}