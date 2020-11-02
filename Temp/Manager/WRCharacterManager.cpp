// Fill out your copyright notice in the Description page of Project Settings.

#include "WRCharacterManager.h"
#include "WRInputSystemManager.h"
#include "WRPathManager.h"
#include "WRPopupManager.h"
#include "WRUIManager.h"
#include "WRConsoleManager.h"
#include "WRMapManager.h"
#include "WRItemManager.h"
#include "WRBuffManager.h"
#include "WRQuestManager.h"
#include "WRTriggerManager.h"
#include "WRAccountManager.h"
#include "WREnvInteractionManager.h"
#include "WRSFXManager.h"

#include "Utility/WRActorSpawnUtility.h"
#include "Utility/WRActorUtility.h"
#include "Utility/WRUIUtility.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterIO.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Actor/Character/WRCharacterNPC.h"
#include "Actor/EnvActor/WREnvVehicleBase.h"

#include "Component/Character/WRActorComponentAttack.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRActorComponentManageMaterial.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRObjectTable.h"
#include "Table/WREnpcTable.h"
#include "Table/WREquipmentLookTable.h"
#include "Table/WRCharacterPresetTable.h"
#include "Table/WRItemTable.h"

#include "DrawDebugHelpers.h"
#include "Enum/EWRCharacter.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "Core/WRGameInstance.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/WRGameModeBase.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Actor/StaticMeshActor/WRStaticMeshAbandonedThing.h"
#include "Define/WRPathDefine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/WRGameModeLab.h"
#include "Component/Character/WRActorComponentLadder.h"
#include "Component/Character/WRActorComponentInteraction.h"
#include "WRAsyncLoadUObjectManager.h"
#include "WRAnimManager.h"
#include "Table/WRPlayerTable.h"
#include "Table/WRSkillTable.h"
#include "Table/WRSocialTable.h"
#include "Component/Character/WRActorComponentBuff.h"
#include "WRVehicleManager.h"
#include "Actor/Vehicle/WRVehicle.h"
#include "WheeledVehicleMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Component/Character/WRActorComponentEventAction.h"
#include "Network/Share/Protocol/Protocol_Game_Actor.h"
#include "Network/IMMessageSender.h"
#include "WRVoiceContentsManager.h"
#include "Actor/Character/WRCharacterActionProp.h"


DECLARE_CYCLE_STAT(TEXT("CCharacterManagerWeakPin"), STAT_CCharacterManager_WeakPtrPin, STATGROUP_CCharacterManager);
DECLARE_CYCLE_STAT(TEXT("CCharacterManagerIncreaseCount"), STAT_CCharacterManager_IncreaseCount, STATGROUP_CCharacterManager);
DECLARE_CYCLE_STAT(TEXT("CCharacterManagerSum"), STAT_CCharacterManager_Sum, STATGROUP_CCharacterManager);

// For Cpp
DEFINE_LOG_CATEGORY(CharacterManager)

#define DEF_ABANDON_THING_MAINTAIN_TIME 2.f

WRCharacterManager::WRCharacterManager()
{
}

WRCharacterManager::~WRCharacterManager()
{
}

//====================================================================================

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRCharacterManager::OnInitialize()
{
	for (auto Iter = AsyncSpawnActorKey.CreateIterator(); Iter; ++Iter)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(Iter.Key());
	}

	AsyncSpawnActorKey.Empty();
	PlayerAppearInfo2Spawn.Empty();
	NpcAppearanceInfo2Spawn.Empty();
	IOAppearanceInfo2Spawn.Empty();
	SOAppearanceInfo2Spawn.Empty();
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRCharacterManager::OnShutdown()
{
	// [ 2020-6-17 : animal14 ] 완료되지 않은 Task 정리 및 취소
	for (auto& Task : Tasks)
	{
		Task.Value.Clear();
	}
	Tasks.Empty();

	MyCharacterSpawnedDelegate.Clear();
	this->RemoveAll();

	OnInitialize();
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRCharacterManager::OnLoadLevelStart()
{
	// [ 2020-6-17 : animal14 ] 완료되지 않은 Task 정리 및 취소
	for (auto& Task : Tasks)
	{
		Task.Value.Clear();
	}
	Tasks.Empty();

	AWRCharacterMine* MyCharacter = this->GetMyCharacter();
	if (MyCharacter->IsValidLowLevel() == true)
	{
		this->GetMyCharacter()->SetEnableSyncHMDTick(false);
	}

	MyCharacterSpawnedDelegate.Clear();
	this->RemoveAll();
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRCharacterManager::OnLoadLevelComplete()
{

}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRCharacterManager::OnRenderStart()
{
	OnInitialize();
}

void WRCharacterManager::EndLoadLevelProcess()
{
	// [ 2020-3-31 : magedoga ] 연구소 제외
	AWRGameModeLab* GameModeLab = UWRGameInstance::GetGameInstance()->GetWorld()->GetAuthGameMode<AWRGameModeLab>();
	if (GameModeLab == nullptr)
	{
		AWRCharacterMine* MyCharacter = this->GetMyCharacter();
		if (MyCharacter->IsValidLowLevel() == true)
		{
			this->GetMyCharacter()->SetEnableSyncHMDTick(true);
		}
	}
}

bool WRCharacterManager::OnTick(float InDeltaTime)
{
	//====================================================================================
	// [ 2019-7-18 : magedoga ] 부하 테스트
// 	TWeakPtr<AWRCharacter> WeakCharacter = Characters[FSetElementId::FromInteger(0)];
// 
// 	volatile int32 a = 0;
// 	for (int32 i = 0; i < 10000; ++i)
// 	{
// 		TSharedPtr<AWRCharacter> SharedPtrCharacter = nullptr;
// 		{
// 			SCOPE_CYCLE_COUNTER(STAT_CCharacterManager_WeakPtrPin);
// 			SharedPtrCharacter = WeakCharacter.Pin();
// 		}
// 
// 		SharedPtrCharacter->IncreaseCount();
// 
// 		{
// 			SCOPE_CYCLE_COUNTER(STAT_CCharacterManager_Sum)
// 			++a;
// 		}
// 	}
	//====================================================================================
	TickSpawn(InDeltaTime);
	TickDistanceCulling(InDeltaTime);

	return true;
}
//====================================================================================

void WRCharacterManager::OnCompleteSpawn(const FWRCharacterSpawnParams& InParam, class AWRCharacter* InSpawnedChar, bool bInIsMe)
{
	if (nullptr == InSpawnedChar)
		return;

	if (0 == InParam.ActorID)
	{
		InSpawnedChar->SetActorID(GenerateStandaloneAUID());
	}
	else
	{
		InSpawnedChar->SetActorID(InParam.ActorID);
	}

	if (bInIsMe)
	{
		MyAUID = InSpawnedChar->GetActorID();
	}
	else
	{
		// 		FVector ModifiedLocation = InParam.Location;
		// 		WRActorUtility::ModifyZHeightForClient(InSpawnedChar, InParam.Location, ModifiedLocation.Z);
		// 		InSpawnedChar->SetLocation(ModifiedLocation);
	}

	InSpawnedChar->SetCharacterTableID(InParam.CharacterTableID);
	InSpawnedChar->SetTriggerSpawnTid(InParam.TriggerSpawnID);

	FString VFXPath;
	FString SFXPath;

	EWRCharacterType SpawnedCharacterType = InSpawnedChar->GetCharacterType();
	switch (SpawnedCharacterType)
	{
	case EWRCharacterType::Player:
	{
		// [ 2019-9-10 : vuvle ] : Prototype 임시 Player Data
		InSpawnedChar->SetATK(1);
		InSpawnedChar->SetDEF(1);
		InSpawnedChar->SetMaxHP(10000);
		InSpawnedChar->SetHP(10000);
		InSpawnedChar->SetMoveAnimType(EWRMoveAnimType::BlendSpace);

		AWRCharacterPlayer* PlayerCharacter = Cast<AWRCharacterPlayer>(InSpawnedChar);
		if (PlayerCharacter)
		{
			FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(InParam.CharacterTableID);
			if (PlayerTable)
			{
				VFXPath = PlayerTable->SpawnEffectPath;
				SFXPath = PlayerTable->SpawnSoundPath;
			}
		}
	}
	break;
	case EWRCharacterType::NPC:
	case EWRCharacterType::ENPC:
	{
		AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(InSpawnedChar);
		if (NpcCharacter)
		{
			FWRNPCBaseTable* NpcBaseTable = NpcCharacter->GetNpcBaseTable();
			if (NpcBaseTable)
			{
				InSpawnedChar->SetATK(NpcBaseTable->Atk);
				InSpawnedChar->SetDEF(NpcBaseTable->Def);
				InSpawnedChar->SetMaxHP(NpcBaseTable->MaxHp);
				InSpawnedChar->SetHP(NpcBaseTable->MaxHp);
				InSpawnedChar->SetMoveAnimType(NpcBaseTable->MoveAnimType);

				VFXPath = NpcBaseTable->SpawnEffectPath;
				SFXPath = NpcBaseTable->SpawnSoundPath;
			}
		}
	}
	break;
	case EWRCharacterType::IO:
	{
		AWRCharacterIO* IOCharacter = Cast<AWRCharacterIO>(InSpawnedChar);
		if (IOCharacter)
		{
			FWRInteractionObjectTable* IOTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(InParam.CharacterTableID);
			if (IOTable)
			{
				VFXPath = IOTable->SpawnEffectPath;
				SFXPath = IOTable->SpawnSoundPath;
			}
		}
	}
	break;

	default:
		break;
	}

	// [ 2020-8-10 : animal14 ] 스폰 이팩트
	WRVFXManager::Get()->Spawn(VFXPath, InSpawnedChar, nullptr);

	// [ 2020-8-10 : animal14 ] 스폰 사운드
	WRSFXManager::Get()->SpawnAtLocation(SFXPath, InSpawnedChar->GetActorLocation());

	// [2020-01-31 : firefirefox] Quest에 노티
	WRQuestManager::Get()->OnCharacterSpawned(InSpawnedChar);

	// [ 2020-9-4 : magedoga ] 알려주자 끝났다고
	InSpawnedChar->OnCompleteSpawn();
}

//void WRCharacterManager::Remove(AID_t InAID)
//{
//	AWRCharacter* pChar = FindbyAID(InAID);
//	if (pChar)
//	{
//		Remove(pChar);
//	}
//}
//
//void WRCharacterManager::Remove(class AWRCharacter* InCharacter)
//{
//	this->Remove_Internal(InCharacter);
//}

void WRCharacterManager::Remove(AID_t InAID, bool IsDisplaying /*= false*/)
{
	AWRCharacter* Character = this->FindbyAID(InAID);
	if (Character == nullptr)
	{
		return;
	}
	this->Remove(Character, IsDisplaying);
}

void WRCharacterManager::Remove(class AWRCharacter* InCharacter, bool IsDisplaying /*= false*/)
{
	if (IsDisplaying)
	{
		InCharacter->SelectDeadDisplay();
	}
	else
	{
		AWRCharacterIO* IO = Cast<AWRCharacterIO>(InCharacter);
		if (IO)
		{
			if (IO->GetISN() != INVALID_ISN && IO->IsCompletedRemove() == false)
			{
				IO->RemoveDisplay();
				return;
			}
		}

		this->Remove_Internal(InCharacter);
	}
}

void WRCharacterManager::TickSpawn(float DeltaTime)
{
	if (WRMapManager::Get()->IsLoading())
		return;
}

void WRCharacterManager::TickDistanceCulling(float DeltaTime)
{
	bool IsEnable = WRConsoleManager::Get()->GetBool(EWRConsole::EnableDistanceCulling);
	if (IsEnable == false)
	{
		return;
	}

	float RefreshTime = WRConsoleManager::Get()->GetFloat(EWRConsole::CullingRefreshTime);
	if (CullingRefreshTime > RefreshTime)
	{
		CullingRefreshTime = 0.0F;
	}
	else
	{
		CullingRefreshTime += DeltaTime;
		return;
	}

	AWRCharacter* MyCharacter = this->GetMyCharacter();
	if (MyCharacter == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = MyCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->SkeletalMesh;
	if (SkeletalMesh == nullptr)
	{
		return;
	}

	float SphereRadius = SkeletalMesh->GetBounds().SphereRadius;

#if UE_EDITOR
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}
#else // UE_EDITOR
#endif // UE_EDITOR

	for (auto& Character : Characters)
	{
		if (Character->IsMe())
			continue;

		USkeletalMeshComponent* CullingSkeletalMeshComponent = Character->GetMesh();
		if (CullingSkeletalMeshComponent == nullptr)
			continue;
		USkeletalMesh* CullingSkeletalMesh = CullingSkeletalMeshComponent->SkeletalMesh;
		if (CullingSkeletalMesh == nullptr)
			continue;

		if (Character->GetCharacterType() == EWRCharacterType::NPC)
		{
			AWRCharacterNPC* InCharacterNPC = Cast<AWRCharacterNPC>(Character);
			if (InCharacterNPC != nullptr)
			{
				if (InCharacterNPC->GetNpcCreateType() == EWRNpcCreateType::INDIVIDUAL)
				{
					if (CullingSkeletalMeshComponent->IsVisible())
					{
					}
					else
					{
						SetDistanceCulling(true, Character);
					}
#if UE_EDITOR
					if (Character->IsDebug == false)
					{
						continue;
					}

					DrawDebugSphere(World, CullingSkeletalMeshComponent->Bounds.Origin, CullingSkeletalMesh->GetBounds().SphereRadius, 8, FColor::Red, false, CullingRefreshTime);
#else // UE_EDITOR
#endif // UE_EDITOR
					continue;
				}
			}
		}

		float CullingRadius = CullingSkeletalMesh->GetBounds().SphereRadius;
		float CullingRatio = CullingRadius / SphereRadius;
		float CullingDistance = FVector::Dist(MyCharacter->GetActorLocation(), Character->GetActorLocation());
		float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CullingDistance) * FMath::Clamp(CullingRatio, 1.0F, 2.5F);

		if (CullingDistance < Distance + CullingRadius)
		{
#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
			if (Character->IsVisibilityBits(EWRVisibility::DistanceCulling))
			{
				SetDistanceCulling(false, Character);
			}
			else
			{
			}
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
			USceneComponent* SceneComponent = Character->GetMeshParentComponent();
			if (SceneComponent)
			{
				if (SceneComponent->IsVisible())
				{
				}
				else
				{
					SetDistanceCulling(false, Character);
				}

			}
			else
			{
				if (CullingSkeletalMeshComponent->IsVisible())
				{
				}
				else
				{
					SetDistanceCulling(false, Character);
				}
			}
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
		}
		else
		{
#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
			if (Character->IsVisibilityBits(EWRVisibility::DistanceCulling))
			{
			}
			else
			{
				SetDistanceCulling(true, Character);
			}
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
			USceneComponent* SceneComponent = Character->GetMeshParentComponent();
			if (SceneComponent)
			{
				if (SceneComponent->IsVisible())
				{
					SetDistanceCulling(true, Character);
				}
				else
				{
				}
			}
			else
			{
				if (CullingSkeletalMeshComponent->IsVisible())
				{
					SetDistanceCulling(true, Character);
				}
				else
				{
				}
			}
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
		}

#if UE_EDITOR
		if (Character->IsDebug == false)
		{
			continue;
		}

		DrawDebugSphere(World, CullingSkeletalMeshComponent->Bounds.Origin, CullingSkeletalMesh->GetBounds().SphereRadius, 8, FColor::Red, false, CullingRefreshTime);
#else // UE_EDITOR
#endif // UE_EDITOR
	}
	}

void WRCharacterManager::SetDistanceCulling(const bool IN bInCulling, class AWRCharacter* InCharacter)
{
#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
	if (InCharacter == nullptr)
	{
		return;
	}

	UPawnMovementComponent* PawnMovementComponent = InCharacter->GetMovementComponent();
	if (PawnMovementComponent == nullptr)
	{
		return;
	}

	if (bInCulling)
	{
		PawnMovementComponent->bUpdateOnlyIfRendered = true;

		InCharacter->AddVisibilityBit(EWRVisibility::DistanceCulling);

		InCharacter->SetVisibility(false, true);

		InCharacter->SetActorTickInterval(0.5F);
	}
	else
	{
		PawnMovementComponent->bUpdateOnlyIfRendered = false;

		InCharacter->RemoveVisibilityBit(EWRVisibility::DistanceCulling);

		InCharacter->SetVisibility(true, true);

		InCharacter->SetActorTickInterval(0.0F);
	}
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
	InCharacter->SetCullingState(bInCulling);

	bool Visibility = InCharacter->GetMesh()->IsVisible();

	// [ 2020-08-20 : magedoga ] [bInCulling == true] is Invisible
	InCharacter->SetVisibility(bInCulling ? false : true, true);

	InCharacter->GetMesh()->SetVisibility(Visibility);

	this->SetTickInterval_Internal(bInCulling ? 0.5f : 0.0f, InCharacter);

	// 	if (InCharacter->GetMesh() != nullptr)
	// 	{
	// 		InCharacter->GetMesh()->bEnableUpdateRateOptimizations = bInCulling;
	// 		InCharacter->GetMesh()->VisibilityBasedAnimTickOption = bInCulling ? EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered : EVisibilityBasedAnimTickOption::AlwaysTickPose;
	// 	}

	if (InCharacter->GetMovementComponent() != nullptr)
	{
		InCharacter->GetMovementComponent()->bUpdateOnlyIfRendered = bInCulling;
	}
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
}

void WRCharacterManager::OnPostSpawned(FWRCharacterSpawnParams& SpawnParam, AWRCharacter* InActor, const bool IN InModifyZ /* = true */)
{
	const FString* HashKey = AsyncSpawnActorKey.FindKey(SpawnParam.ActorID);
	if (HashKey)
	{
		AsyncSpawnActorKey.Remove(*HashKey);
	}

	//위치 회전값 세팅
	InActor->SetLocation(SpawnParam.Location, InModifyZ);
	InActor->SetRotation(SpawnParam.Rotation);

	//스케일 적용
	InActor->SetActorScale3D(SpawnParam.Scale);

	//케릭터 마무리
	InActor->OnPostSpawn();

	//스폰 마무리
	OnCompleteSpawn(SpawnParam, InActor, false);

	Characters.Add(InActor);

	if (SpawnParam.TriggerSpawnID != 0 && TriggerSpawnTidCharacters.Contains(SpawnParam.TriggerSpawnID) == false)
		TriggerSpawnTidCharacters.Add(SpawnParam.TriggerSpawnID, InActor);

	FVector Location = InActor->GetActorLocation();


	//스폰 이펙트 세팅
	AWRCharacterIO* IO = Cast<AWRCharacterIO>(InActor);
	if (IO == nullptr)
	{
		return;
	}

	UWRActorComponentManageMaterial* ActorComponentManageMaterial = IO->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial)
	{
		ActorComponentManageMaterial->RestoreMaterialInterface();

		IO->SetMaterialParameterValue("00.SpawnMain", 0.0F, IO->GetCurrentMesh());

		float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::VehicleSpawnDirection);
		ActorComponentManageMaterial->Spawn(EWRManageMaterial::Contruct, Duration);
	}
}

void WRCharacterManager::SpawnMe()
{
	// [2020-01-31 : vuvle] PlayerStart 가 아닌 Server 에서 받은 정보로 Spawn 위치 조정
	/*FVector SpawnLocation;
	FRotator SpawnRotation;
	for (TActorIterator<APlayerStart> It(UWRGameInstance::GetGameInstance()->GetWorld()); It; ++It)
	{
		SpawnLocation = It->GetActorLocation();
		SpawnRotation = It->GetActorRotation();
	}*/

	FVector SpawnLocation = WRAccountManager::Get()->GetMySpawnPosition();
	FRotator SpawnRotation = WRAccountManager::Get()->GetMySpawnRotator();

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(WRAccountManager::Get()->GetMyCharacterPresetTID());
	FString BPPath = NULL_STRING;

	WRTableID CharacterTID = INVALID_RECORD_TID;

	if (PresetTable)
	{
		BPPath = PresetTable->MyPresetBlueprint;
		//BPPath = GetPresetBPPath(PresetTable);
		CharacterTID = PresetTable->CharTableID;
	}

	AID_t MyAID = WRAccountManager::Get()->GetAID();
	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(MyAID, CharacterTID, BPPath, SpawnLocation, SpawnRotation);
	AWRCharacterMine* Player = WRCharacterManager::Get()->Spawn<AWRCharacterMine>(SpawnParam, true, true);

	if (Player != nullptr)
	{
		Player->SetLocationAndRotation(SpawnLocation, SpawnRotation, true);
		Player->SetCharacterName(WRAccountManager::Get()->GetMyCharacterName());
		Player->SetLanguageType(EWRLanguageType::en(WRConsoleManager::Get()->GetInt(EWRConsole::LanguageType)));

		UWRActorComponentEquipmentLook* EquipLook = Player->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook && PresetTable)
		{
#ifdef DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
			for (auto& PresetDefaultPart : PresetTable->PresetDefaultParts)
			{
				WRTableID TID = PresetDefaultPart.Value;
				if (TID == INVALID_TID)
				{
					continue;
				}

				EquipLook->Equip(TID, NAME_None, nullptr);
			}		

			// [ 2020-9-15 : vuvle ] : 본인에게 필요한 장비를 입힌다(Server 에서 받는 데이터 이전에 셋팅하기 위해 임시로 포함한다.
			TArray<WRTableID>& CostumeItemTableArray = WRAccountManager::Get()->GetCustomizingSelectedCostumeItemArray();
			for (int32 i = 0; i < CostumeItemTableArray.Num(); ++i)
			{
				FWRCostumeItemTable* FoundTable = WRTableManager::Get()->FindRow<FWRCostumeItemTable>(CostumeItemTableArray[i]);
				if (FoundTable == nullptr)
					continue;

				EquipLook->Equip(FoundTable->EquipmentLookTID, NAME_None, nullptr);
			}

			std::string NoneItemEquipmentLookString = WRAccountManager::Get()->GetCustomizingSelectedNoneItemEquipmentLookData();
			if (NoneItemEquipmentLookString.empty() == false)
			{
				WRTableID EquipmentLookTID = std::stoi(NoneItemEquipmentLookString);
				EquipLook->Equip(EquipmentLookTID, NAME_None, nullptr);
			}
		
#else // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
			//AsyncLoadResource(Player);
			//EquipLook->Equip(PresetTable->NormalEquipmentLookTID, FWREquipCompleteDelegate::CreateRaw(this, &WRCharacterManager::MergeMesh));
			EquipLook->Equip(PresetTable->NormalEquipmentLookTID);
#endif // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		}

		TArray<WRTableID>& CostumeItemTableArray = WRAccountManager::Get()->GetCustomizingSelectedCostumeItemArray();
		Player->SetEquippedCostume(CostumeItemTableArray);

		WRItemBase* EquippedMainWeapon = WRItemManager::Get()->FindEquippedMainWeapon();
		WRItemBase* EquippedSubWeapon = WRItemManager::Get()->FindEquippedSubWeapon();

		// [ 2020-4-02 : vuvle ] : 해당 부분에 대해 TODO 추가 구현 필요
		if (EquippedMainWeapon && EquippedSubWeapon)
		{
			// [ 2020-4-02 : vuvle ] : 양쪽 무기를 다 장착한 상태에서 EquipWeaponCostume(TID) 함수를 한번에 두개 호출하면 앞에가 씹혀서 양쪽 무기 장착 함수를 따로 호출한다.
			// [ 2020-7-06 : vuvle ] : TODO 추가 구현 필요
			Player->SetEquippedWeaponItemTID(EquippedMainWeapon->GetTID(), EquippedSubWeapon->GetTID(), INVALID_TABLE_TID);
		}
		else if (EquippedMainWeapon)
		{
			Player->SetEquippedWeaponItemTID(EquippedMainWeapon->GetTID());
		}
		else if (EquippedSubWeapon)
		{
			Player->SetEquippedWeaponItemTID(EquippedSubWeapon->GetTID());
		}

		// [ 2020-4-08 : vuvle ] 내 캐릭터 생성을 알리는 Deligate 호출
		if (MyCharacterSpawnedDelegate.IsBound())
		{
			MyCharacterSpawnedDelegate.Broadcast(Player);
		}
	}
}

void WRCharacterManager::DetermineSpawnPlayer(const pt_Game_Actor::Inf_PlayerAppear_G2U& InInform)
{
	if (InInform.vehicleAID != INVALID_AID)
	{
		//탑승상태인 케릭터 인데, 차가 아직 소환이 안된상태면 딜레이 시킴
		AWRVehicle* Vehicle = WRVehicleManager::Get()->FindVehicleByAID(InInform.vehicleAID);
		if (Vehicle)
		{
			SpawnPlayer(InInform);
		}
		else
		{
			PlayereOnBoardingVehicle.Add(InInform.aid, InInform);
		}
	}
	else
	{
		SpawnPlayer(InInform);
	}
}

void WRCharacterManager::SpawnOnBoardingPlayerWhenVehicleSpawnCompleted(AID_t InAID)
{
	if (PlayereOnBoardingVehicle.Contains(InAID))
	{
		pt_Game_Actor::Inf_PlayerAppear_G2U InInform = PlayereOnBoardingVehicle.FindRef(InAID);
		SpawnPlayer(InInform);
	}
}

void WRCharacterManager::SpawnPlayer(const pt_Game_Actor::Inf_PlayerAppear_G2U& InInform)
{
	if (PlayerAppearInfo2Spawn.Contains(InInform.aid))
	{
		return;
	}

	if (FindbyAID(InInform.aid))
	{
		return;
	}
	const FString* AsyncSpawnKey = AsyncSpawnActorKey.FindKey(InInform.aid);
	if (AsyncSpawnKey)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*AsyncSpawnKey);
	}

	PlayerAppearInfo2Spawn.Add(InInform.aid, InInform);

	WRTableID CharacterTID = 1;
	FString BPPath = "Player_Man_E";
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InInform.presetTID);
	if (PresetTable != nullptr)
	{
		BPPath = PresetTable->PresetBlueprint;
		CharacterTID = PresetTable->CharTableID;
	}

	LoadPlayerMontage(InInform);

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterManager::DetermineOnSpawnPlayerComplete);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(BPPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	AsyncSpawnActorKey.Add(AsyncKey, InInform.aid);
}

void WRCharacterManager::LoadPlayerMontage(const pt_Game_Actor::Inf_PlayerAppear_G2U& InInform)
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InInform.presetTID);
	if (PresetTable != nullptr)
	{
		FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(PresetTable->CharTableID);
		if (PlayerTable)
		{
			for (int32 Idx = 0; Idx < PlayerTable->BaseMontageTID.Num(); ++Idx)
			{
				if (WRAnimManager::Get()->Contains(PlayerTable->BaseMontageTID[Idx]))
					continue;

				WRAnimManager::Get()->LoadMontage(PlayerTable->BaseMontageTID[Idx]);
			}

			for (int32 Idx = 0; Idx < PlayerTable->BaseSubMontageTID.Num(); ++Idx)
			{
				if (WRAnimManager::Get()->Contains(PlayerTable->BaseSubMontageTID[Idx]))
					continue;

				WRAnimManager::Get()->LoadMontage(PlayerTable->BaseSubMontageTID[Idx]);
			}
		}
	}

	LoadPlayerWeaponSkillMontage(InInform.rightWeaponTID);
	LoadPlayerWeaponSkillMontage(InInform.leftWeaponTID);

	// vuvle TODO : Temporary TID 변경해야함 (변경할 필요가 없을지도 모르겠음. 건율에게 확인할것)
	//LoadPlayerWeaponSkillMontage(InInform.temporaryTID);

	FWRDanceTable* DanceTable = WRTableManager::Get()->FindRow<FWRDanceTable>(InInform.danceTID);
	if (DanceTable)
	{
		if (WRAnimManager::Get()->Contains(DanceTable->BaseMontageTID) == false)
		{
			WRAnimManager::Get()->LoadMontage(DanceTable->BaseMontageTID);
		}

		if (WRAnimManager::Get()->Contains(DanceTable->BaseSubMontageTID) == false)
		{
			WRAnimManager::Get()->LoadMontage(DanceTable->BaseSubMontageTID);
		}
	}
}

void WRCharacterManager::LoadPlayerWeaponSkillMontage(WRTableID InWeaponTID)
{
	FWRWeaponTable* WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(InWeaponTID);
	if (WeaponTable)
	{
		if (WRAnimManager::Get()->Contains(WeaponTable->BaseMontageTID) == false)
		{
			WRAnimManager::Get()->LoadMontage(WeaponTable->BaseMontageTID);
		}

		//무기 종속 스킬 몽타쥬
		for (int32 Idx = 0; Idx < WeaponTable->SkillTID.Num(); ++Idx)
		{
			FWRPlayerSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRPlayerSkillTable>(WeaponTable->SkillTID[Idx], "SkillPlayer_Atk");
			if (nullptr == SkillTable)
				continue;

			if (WRAnimManager::Get()->Contains(SkillTable->SkillGroupMontageTID) == false)
			{
				WRAnimManager::Get()->LoadMontage(SkillTable->SkillGroupMontageTID);
			}

			if (WRAnimManager::Get()->Contains(SkillTable->SkillGroupSubMontageTID) == false)
			{
				WRAnimManager::Get()->LoadMontage(SkillTable->SkillGroupSubMontageTID);
			}
		}
	}
}

void WRCharacterManager::DetermineOnSpawnPlayerComplete(UObject* InActor, FString InHashKey)
{
	/*
		AID_t ActorID = AsyncSpawnActorKey.FindRef(InHashKey);
		if (PlayerAppearInfo2Spawn.Contains(ActorID) == false)
		{
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("DetermineOnSpawnPlayerComplete"), TEXT("Not found actorID"));
			return;
		}

		pt_Game_Actor::Inf_PlayerAppear_G2U PlayerInfo = PlayerAppearInfo2Spawn.FindRef(ActorID);
		if (INVALID_AID != PlayerInfo.vehicleAID)
		{
		}
		else
		{
			//탑승상태가 아니면 그냥 스폰
			OnSpawnPlayerComplete(InActor, InHashKey);
		}
	*/

	OnSpawnPlayerComplete(InActor, InHashKey);

}

void WRCharacterManager::OnSpawnPlayerComplete(UObject* InActor, FString InHashKey)
{
	AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(InActor);
	if (nullptr == Player)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnSpawnPlayerComplete"), TEXT("UObject is nullptr"));
		return;
	}

	AID_t ActorID = AsyncSpawnActorKey.FindRef(InHashKey);
	if (PlayerAppearInfo2Spawn.Contains(ActorID) == false)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnSpawnPlayerComplete"), TEXT("Not found actorID"));
		return;
	}

	pt_Game_Actor::Inf_PlayerAppear_G2U PlayerInfo = PlayerAppearInfo2Spawn.FindRef(ActorID);

	WRTableID CharacterTID = 1;
	FString BPPath = "Player_Man";
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(PlayerInfo.presetTID);
	if (PresetTable != nullptr)
	{
		BPPath = PresetTable->PresetBlueprint;
		CharacterTID = PresetTable->CharTableID;
	}

	FVector SpawnPosition(PlayerInfo.posX, PlayerInfo.posY, PlayerInfo.posZ);
	FRotator SpawnRotation = FRotator(0.0f, PlayerInfo.angle, 0.0f);
	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(PlayerInfo.aid, CharacterTID, BPPath, SpawnPosition, SpawnRotation);
	OnPostSpawned(SpawnParam, Player);

	if ((EWRChatType::en)PlayerInfo.chatType == EWRChatType::Secret)
	{
		Player->AddVisibilityBit(EWRVisibility::SpecialConversation);
		Player->SetCharacterInvisible(false);

		//[2020 - 07 - 28 EZ : 시크릿대화로 스폰시 상호작용 콜리전 해제 ]
		Player->SetCollisionTargetting(ECollisionEnabled::NoCollision);
	}
	Player->SetChatType((EWRChatType::en)PlayerInfo.chatType);
		
	Player->SetDialogGroupID(PlayerInfo.chatGroupID);

	UWRActorComponentEquipmentLook* EquipLook = Player->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLook)
	{
#ifdef DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		for (auto& PresetDefaultPart : PresetTable->PresetDefaultParts)
		{
			WRTableID TID = PresetDefaultPart.Value;
			if (TID == 0)
			{
				continue;
			}

			EquipLook->Equip(TID, NAME_None, nullptr);	
		}

		// vuvle TODO : 커스터마이징 헤어 적용(다른파츠 추가 변경 필요)
		if (PlayerInfo.customizeInfo.customize.empty() == false)
		{
			WRTableID EquipLookTID = std::stoi(PlayerInfo.customizeInfo.customize);
			EquipLook->Equip(EquipLookTID, NAME_None, nullptr);
		}
#else // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		//AsyncLoadResource(Player);
		//EquipLook->Equip(PresetTable->NormalEquipmentLookTID, FWREquipCompleteDelegate::CreateRaw(this, &WRCharacterManager::MergeMesh));
		EquipLook->Equip(PresetTable->NormalEquipmentLookTID);
#endif // DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820
		
	}

	FVector ModifiedLocation = SpawnPosition;
	WRActorUtility::ModifyZHeightForClient(Player, SpawnPosition, ModifiedLocation.Z);
	Player->SetLocation(ModifiedLocation);

	//Player->SetLocation(SpawnPosition);	

	if (0 == PlayerInfo.hp)
	{
		Player->DetermineFSMSignal(EWRFSM::Die);
	}

	FVector DestPosition(PlayerInfo.dPosX, PlayerInfo.dPosY, PlayerInfo.dPosZ);
	if (DestPosition != FVector::ZeroVector &&
		DestPosition != SpawnPosition)
	{
		Player->ProcessInterpolateDestinationLocationWhenSpawned(DestPosition);
	}
	// [ 2020-2-20 : kyu ] 위치 재설정
	AWREnvVehicleBase* FoundPutOnEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(PlayerInfo.vehicleAID);
	if (FoundPutOnEnvVehicle != nullptr)
	{
		Player->SetEnvVehicleAID(PlayerInfo.vehicleAID);
		Player->SetEnvVehicleTransformOffset(true, FVector(PlayerInfo.localPosX, PlayerInfo.localPosY, PlayerInfo.localPosZ), PlayerInfo.localAngle);
	}
	else
	{
		if (PlayerInfo.vehicleAID != INVALID_AID)
		{
			//ensureMsgf(false, TEXT("vehicleAID is not found"));

			Player->SetEnvVehicleAID(PlayerInfo.vehicleAID);
			Player->SetEnvVehicleTransformOffset(true, FVector(PlayerInfo.localPosX, PlayerInfo.localPosY, PlayerInfo.localPosZ), PlayerInfo.localAngle);
		}
	}

	// 프리셋 설정
	Player->SetPresetTID(PlayerInfo.presetTID);
	// 의상 설정
	TArray<WRTableID> CostumeList;
	std::list<TID_t>::const_iterator iter = PlayerInfo.listCostumeTID.begin();
	std::list<TID_t>::const_iterator iter_end = PlayerInfo.listCostumeTID.end();
	for (; iter != iter_end; ++iter)
	{
		CostumeList.Add(*iter);
	}
	Player->SetEquippedCostume(CostumeList);
	// vuvle TODO : 변경 필요
	Player->SetEquippedWeaponItemTID(PlayerInfo.rightWeaponTID, PlayerInfo.leftWeaponTID, INVALID_TABLE_TID);
	// 버프 설정
	WRBuffManager::Get()->SetBuff(Player, PlayerInfo.lstBuffInfo);
	// 이름 설정
	Player->SetCharacterName(WRUIUtility::ConvertWStringToFName(PlayerInfo.nickName));
	// 건배상태 설정
	Player->SetCheersCondition((EWRCheersConditionType)PlayerInfo.cheersCondition);
	// 국가 설정
	Player->SetLanguageType((EWRLanguageType::en)(PlayerInfo.language));

	//차량에 탑승 상태면 차에 어태치
	AWRVehicle* Vehicle = WRVehicleManager::Get()->FindVehicleByAID(PlayerInfo.vehicleAID);
	if (Vehicle)
	{
		//차량정보 세팅
		Player->SetBoardingVehicleAID(PlayerInfo.myVehicleAID);
		//탑승애니 적용
		Player->DetermineFSMSignal(EWRFSM::DrivingIdle);
		//회전값 적용
		Player->SetActorRotation(Vehicle->GetActorRotation());
		//탑승 기본 세팅
		FString SeatPositionSocketName = "SeatPosition_" + FString::FromInt(PlayerInfo.seatIndex);

		Player->AttachToActor(Vehicle, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *SeatPositionSocketName);

		Vehicle->GetVehicleMovement()->SetOverrideController(Player->GetController());
		UWRAnimInstance* AnimInstance = Player->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->ResetHMDRotation();
		}
	}
	else
	{
		// 전투상태 설정 (Idle을 invoke시켜야 즉시 전투 상태에 맞는 idle을 재생
		Player->SetBattleState(PlayerInfo.BattleState);
		Player->DetermineFSMSignal(EWRFSM::Idle);
	}

	//내가 소유한 차량 정보 세팅
	Player->SetOwnedVehicleAID(PlayerInfo.myVehicleAID);

	// 점유 IO 설정
	// [ 2020-4-21 : kyu ] + 위의 Idle이후로 가야한다. 앉아있는 상태로 만들어야 하기 때문
	AWRCharacterIO* FoundIO = FindbyAID_Casted<AWRCharacterIO>(PlayerInfo.occupyIOAID);
	if (FoundIO != nullptr)
	{
		UWRActorComponentInteractionObject* pInteractionComponent = FoundIO->GetComponent<UWRActorComponentInteractionObject>();
		if (pInteractionComponent != nullptr)
		{
			pInteractionComponent->UpdateToOccupationSync(Player);
		}
	}

	// 사다리 & 클라이밍 설정
	UWRActorComponentLadder* LadderComponent = Player->GetComponent<UWRActorComponentLadder>();
	if (LadderComponent != nullptr)
	{
		LadderComponent->SetLadderOnAppear(PlayerInfo.InHandleIndex, PlayerInfo.InHand);
	}

	// 춤추는 캐릭 처리
	if (PlayerInfo.danceGroupTID != INVALID_TABLE_TID)
	{
		UWRActorComponentDanceMode* DanceModeComponent = Player->GetComponent<UWRActorComponentDanceMode>();
		if (DanceModeComponent)
		{
			DanceModeComponent->StartDanceMode(PlayerInfo.danceGroupTID, PlayerInfo.danceTID);
		}
	}

	//항상 제일 마지막에 호출 되어야 함
	PlayerAppearInfo2Spawn.Remove(ActorID);

	// [2020-07-24 : firefirefox] 화상 채팅에 노티
	WRVoiceContentsManager::Get()->GetVideoChatController().OnPlayerCharacterSpawned(Player);
}

bool WRCharacterManager::RemovePlayerInfo(AID_t InAID)
{
	bool bResult = false;
	if (PlayerAppearInfo2Spawn.Contains(InAID) ||
		PlayereOnBoardingVehicle.Contains(InAID))
	{
		bResult = true;
	}

	PlayerAppearInfo2Spawn.Remove(InAID);
	PlayereOnBoardingVehicle.Remove(InAID);

	//비동기 로드중인것 삭제
	const FString* FoundKey = AsyncSpawnActorKey.FindKey(InAID);
	if (FoundKey != nullptr)
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*FoundKey);

	return bResult;
}

void WRCharacterManager::ModifyPlayerLocation(AID_t InAID, FVector InLocation)
{
	pt_Game_Actor::Inf_PlayerAppear_G2U* PlayerInfo = PlayerAppearInfo2Spawn.Find(InAID);
	if (PlayerInfo)
	{
		PlayerInfo->posX = InLocation.X;
		PlayerInfo->posY = InLocation.Y;
		PlayerInfo->posZ = InLocation.Z;
	}
}

void WRCharacterManager::ModifyPlayerOffsetTransformInVehicle(AID_t InAID, FVector InOffset, float InAngle)
{
	pt_Game_Actor::Inf_PlayerAppear_G2U* PlayerInfo = PlayerAppearInfo2Spawn.Find(InAID);
	if (PlayerInfo)
	{
		PlayerInfo->localPosX = InOffset.X;
		PlayerInfo->localPosY = InOffset.Y;
		PlayerInfo->localPosZ = InOffset.Z;
		PlayerInfo->localAngle = InAngle;
	}
}

void WRCharacterManager::ModifyOwnedVehicleAID(AID_t InAID, AID_t InVehicleAID)
{
	pt_Game_Actor::Inf_PlayerAppear_G2U* PlayerInfo = PlayerAppearInfo2Spawn.Find(InAID);
	if (PlayerInfo)
	{
		PlayerInfo->vehicleAID = InVehicleAID;
	}
}

void WRCharacterManager::ModifyBoardingVehicleInfo(AID_t InAID, AID_t InVehicleAID, int8 InSeatIndex)
{
	pt_Game_Actor::Inf_PlayerAppear_G2U* PlayerInfo = PlayerAppearInfo2Spawn.Find(InAID);
	if (PlayerInfo)
	{
		PlayerInfo->vehicleAID = InVehicleAID;
		PlayerInfo->seatIndex = InSeatIndex;
	}
}

void WRCharacterManager::SpawnNpc(const pt_Game_Actor::Inf_NpcAppear_G2U& InInform, bool bImmediately /*= false*/)
{
	if (NpcAppearanceInfo2Spawn.Contains(InInform.npcAID))
	{
		return;
	}

	NpcAppearanceInfo2Spawn.Add(InInform.npcAID, InInform);

	if (GetMyCharacter())
	{
		NpcAppearanceInfo2Spawn.ValueSort(
			[](const pt_Game_Actor::Inf_NpcAppear_G2U& A, const pt_Game_Actor::Inf_NpcAppear_G2U& B)
		{
			FVector ALocation = FVector(A.posX, A.posY, A.posZ);
			FVector BLocation = FVector(B.posX, B.posY, B.posZ);

			FVector MyLocationToSort = WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();
			float DistanceToA = (MyLocationToSort - ALocation).Size();
			float DistanceToB = (MyLocationToSort - BLocation).Size();

			return DistanceToA < DistanceToB;
		}
		);
	}

	if (FindbyAID(InInform.npcAID))
	{
		return;
	}
	const FString* AsyncSpawnKey = AsyncSpawnActorKey.FindKey(InInform.npcAID);
	if (AsyncSpawnKey)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*AsyncSpawnKey);
	}

	const FWRNPCBaseTable* NpcBaseTable = FindNpcBaseTable(InInform.npcTID);
	if (nullptr == NpcBaseTable)
	{
		return;
	}

	LoadNpcMontage(NpcBaseTable);

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterManager::OnSpawnNpcCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(NpcBaseTable->BlueprintID, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;

	UE_LOG(CharacterManager, Display, TEXT("WRCharacterManager::AddNpcInfo - Add Key : %s"), *AsyncKey);

	AsyncSpawnActorKey.Add(AsyncKey, InInform.npcAID);
}

void WRCharacterManager::LoadNpcMontage(const FWRNPCBaseTable* InNpcBaseTable)
{
	if (nullptr == InNpcBaseTable)
		return;

	//스킬 및 애니메이션 몽타쥬 로드
	if (WRAnimManager::Get()->Contains(InNpcBaseTable->BaseMontageTID) == false)
	{
		WRAnimManager::Get()->LoadMontage(InNpcBaseTable->BaseMontageTID);
	}


	for (int32 Idx = 0; Idx < InNpcBaseTable->SkillTID.Num(); ++Idx)
	{
		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InNpcBaseTable->SkillTID[Idx]);
		if (nullptr == SkillTable)
			continue;

		if (WRAnimManager::Get()->Contains(SkillTable->SkillGroupMontageTID) == false)
		{
			WRAnimManager::Get()->LoadMontage(SkillTable->SkillGroupMontageTID);
		}
	}
}

void WRCharacterManager::OnSpawnNpcCompleted(UObject* InActor, FString InHashKey)
{
	AWRCharacter* pChar = Cast<AWRCharacter>(InActor);
	if (nullptr == pChar)
	{
		FString MSG = FString::Format(TEXT("OnSpawnNPCCompleted : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("UObject is nullptr"));
		return;
	}

	AID_t ActorID = AsyncSpawnActorKey.FindRef(InHashKey);
	if (NpcAppearanceInfo2Spawn.Contains(ActorID) == false)
	{
		FString MSG = FString::Format(TEXT("OnSpawnNPCCompleted : {0}"), { *InHashKey });
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *MSG, TEXT("Not found actorID"));
		return;
	}

	pt_Game_Actor::Inf_NpcAppear_G2U NpcInfo = NpcAppearanceInfo2Spawn.FindRef(ActorID);
	const FWRNPCBaseTable* NpcBaseTable = FindNpcBaseTable(NpcInfo.npcTID);
	if (nullptr == NpcBaseTable)
	{
		return;
	}

	UE_LOG(CharacterManager, Display, TEXT("WRCharacterManager::OnSpawnNpcCompleted - Remove Key : %s"), *InHashKey);

	FVector SpawnPosition(NpcInfo.posX, NpcInfo.posY, NpcInfo.posZ);
	FRotator SpawnRotation = FRotator(0.0f, NpcInfo.angle, 0.0f);
	FVector ScaleVector = FVector(NpcBaseTable->Scale, NpcBaseTable->Scale, NpcBaseTable->Scale);
	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(NpcInfo.npcAID, NpcInfo.npcTID, NpcBaseTable->BlueprintID, SpawnPosition, SpawnRotation, ScaleVector, NpcInfo.spawnTID);
	OnPostSpawned(SpawnParam, pChar);

	pChar->SetHP(NpcInfo.hp);
	WRBuffManager::Get()->SetBuff(pChar, NpcInfo.lstBuffInfo);
	// 전투상태 설정 (Idle을 invoke시켜야 즉시 전투 상태에 맞는 idle을 재생
	pChar->SetBattleState(NpcInfo.bBattleState);
	if (pChar->IsAlive() == false)
	{
		pChar->DetermineFSMSignal(EWRFSM::Die);
	}
	else
	{
		pChar->DetermineFSMSignal(EWRFSM::Spawn);


		//pChar->DetermineFSMSignal(EWRFSM::Idle);
	}

	FVector DestPosition(NpcInfo.dPosX, NpcInfo.dPosY, NpcInfo.dPosZ);
	if (DestPosition != FVector::ZeroVector &&
		DestPosition != SpawnPosition)
	{
		pChar->ProcessInterpolateDestinationLocationWhenSpawned(DestPosition);
	}

	AWRCharacterNPC* Npc = Cast<AWRCharacterNPC>(pChar);
	if (Npc)
	{
		Npc->bSpawnedByServer = true;
	}

	//항상 제일 마지막에 호출 되어야 함
	NpcAppearanceInfo2Spawn.Remove(ActorID);
}

void WRCharacterManager::RemoveNpcInfo(AID_t InAID)
{
	NpcAppearanceInfo2Spawn.Remove(InAID);

	//비동기 로드중인것 삭제
	const FString* FoundKey = AsyncSpawnActorKey.FindKey(InAID);
	if (FoundKey != nullptr)
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*FoundKey);
}

bool WRCharacterManager::ModifyNpcAIMoveToPosition(const pt_Game_User::Inf_ActorMoveToPos_G2U& InInform)
{
	pt_Game_Actor::Inf_NpcAppear_G2U* NpcInfo = NpcAppearanceInfo2Spawn.Find(InInform.actorID);
	if (NpcInfo)
	{
		NpcInfo->posX = InInform.start_posX;
		NpcInfo->posY = InInform.start_posY;
		NpcInfo->posZ = InInform.start_posZ;
		NpcInfo->dPosX = InInform.target_posX;
		NpcInfo->dPosY = InInform.target_posY;
		NpcInfo->dPosZ = InInform.target_posZ;
		return true;
	}
	return false;
}

void WRCharacterManager::ModifyHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform)
{
	pt_Game_Actor::Inf_NpcAppear_G2U* NpcInfo = NpcAppearanceInfo2Spawn.Find(InInform.targetAID);
	if (NpcInfo)
	{
		NpcInfo->hp = InInform.currentHP;
	}

	pt_Game_Actor::Inf_PlayerAppear_G2U* PlayerInfo = PlayerAppearInfo2Spawn.Find(InInform.targetAID);
	if (PlayerInfo)
	{
		PlayerInfo->hp = InInform.currentHP;
	}
}

void WRCharacterManager::SpawnIO(const pt_Game_Actor::Inf_IOAppear_G2U& InInform)
{
	if (FindbyAID(InInform.ioAID))
	{
		return;
	}
	const FString* AsyncSpawnKey = AsyncSpawnActorKey.FindKey(InInform.ioAID);
	if (AsyncSpawnKey)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*AsyncSpawnKey);
	}

	FWRInteractionObjectTable* IOTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(InInform.ioTID);
	if (nullptr == IOTable)
	{
		return;
	}


	IOAppearanceInfo2Spawn.Add(InInform.ioAID, InInform);

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterManager::OnSpawnIOCompleted);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(IOTable->MainBPPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;
	AsyncSpawnActorKey.Add(AsyncKey, InInform.ioAID);
}

void WRCharacterManager::OnSpawnIOCompleted(UObject* InActor, FString InHashKey)
{
	AWRCharacterIO* IO = Cast<AWRCharacterIO>(InActor);
	if (nullptr == IO)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnSpawnIOComplete"), TEXT("UObject is nullptr"));
		return;
	}

	AID_t ActorID = AsyncSpawnActorKey.FindRef(InHashKey);
	if (IOAppearanceInfo2Spawn.Contains(ActorID) == false)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnSpawnIOComplete"), TEXT("Not found actorID"));
		return;
	}

	pt_Game_Actor::Inf_IOAppear_G2U IOInfo = IOAppearanceInfo2Spawn.FindRef(ActorID);
	FWRInteractionObjectTable* IOTable = WRTableManager::Get()->FindRow<FWRInteractionObjectTable>(IOInfo.ioTID);
	if (nullptr == IOTable)
	{
		return;
	}

	FVector SpawnPosition(IOInfo.posX, IOInfo.posY, IOInfo.posZ);
	FRotator SpawnRotation = FRotator(0.0f, IOInfo.angle, 0.0f);
	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(IOInfo.ioAID, IOInfo.ioTID, IOTable->MainBPPath, SpawnPosition, SpawnRotation, FVector::OneVector, IOInfo.spawnTID);
	OnPostSpawned(SpawnParam, IO, false);

	IO->OnSpawnComplete(IOInfo, IOTable);


	//항상 제일 마지막에 호출 되어야 함
	IOAppearanceInfo2Spawn.Remove(ActorID);
}


AWRCharacter* WRCharacterManager::SpawnPresetCharacters(const WRTableID IN InTableID)
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InTableID);

	if (PresetTable == nullptr)
		return nullptr;

	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(0, 1, PresetTable->PresetBlueprint, PresetTable->SpawnPosition, FRotator(0.0f, PresetTable->SpawnAngle, 0.0f), FVector(1.2f, 1.2f, 1.2f));
	AWRCharacter* SpawnedCharacter = Spawn<AWRCharacter>(SpawnParam);
	if (SpawnedCharacter != nullptr)
	{
		// 		// [ 2020-4-10 : vuvle ] : Z 를 땅에 붙임 (좌표 문제가 있어서 사용 하지 않음. 확인할 것)
		// 		SpawnedCharacter->SetLocation(PresetTable->SpawnPosition, true);

				// [ 2020-2-18 : vuvle ] : Preset 이 Spawn 되는 Lab 은 광원이 없어 강제로 Light Channel 을 변경한다.
				// [ 2020-2-19 : vuvle ] : Light 채널 변경을 하지 않고 광원 설정 변경하는것으로 변경
				/*USkeletalMeshComponent* MeshComponent = SpawnedCharacter->GetMesh();
				if (MeshComponent != nullptr)
				{
					MeshComponent->LightingChannels.bChannel1 = true;
				}*/

				// [ 2020-2-18 : vuvle ] : Spawn 시 반드시 Equip 을 해야 다음 연출이 정상적으로 나옴.
		UWRActorComponentEquipmentLook* EquipLook = SpawnedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
		}

		// [ 2020-3-23 : vuvle ] : Equip을 다시 해야 연출이 발생된다. 기획 변경으로 첫 캐릭터 부터 연출이 나와야 하는 상황이라 Equip 을 다시 한다.
		// [ 2020-6-01 : vuvle ] : EquipmentLook Component 구조 변경으로 인해 Equip을 한번 더 하지 않아도 정상 연출이 나오는걸 확인하여 삭제
// 		if (EquipLook)
// 		{
// 			EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
// 		}

		AWRCharacterPlayer* CastedSpawnedCharacter = Cast<AWRCharacterPlayer>(SpawnedCharacter);
		if (CastedSpawnedCharacter->IsValidLowLevel() == true)
		{
			CastedSpawnedCharacter->SetUsingSyncHMDHead(false);
		}
	}

	return SpawnedCharacter;
}

void WRCharacterManager::ChangeMyPreset(const WRTableID IN InTableID, const bool bUsingHologram)
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InTableID);

	if (PresetTable == nullptr)
	{
		ensureMsgf(false, TEXT("Could not find Preset TID"));
		return;
	}

	FWRCharacterPresetTable* OriginPresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(WRAccountManager::Get()->GetMyCharacterPresetTID());

	if (OriginPresetTable == nullptr)
	{
		ensureMsgf(false, TEXT("Could not find Preset TID"));
		return;
	}

	// [ 2020-4-17 : vuvle ] 술병을 들고있으면 일단 버리고 시작한다.
	WRItemManager::Get()->ThrowTemporaryItemAway(EWRReasonForThrowTemporaryItemAway::None);

	// [ 2020-2-18 : vuvle ] 원래 입고 있던 성별과 같은 Preset이라면 
	if (PresetTable->Gender == OriginPresetTable->Gender)
	{
		AWRCharacterMine* MyCharacter = GetMyCharacter();

		if (MyCharacter == nullptr)
		{
			ensureMsgf(false, TEXT("My character is nullptr"));
			return;
		}

		UWRActorComponentEquipmentLook* EquipLook = MyCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			if (bUsingHologram == true)
			{
				EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
			}
			else
			{
				EquipLook->Equip(PresetTable->NormalEquipmentLookTID);
			}
		}
	}
	// [ 2020-2-18 : vuvle ] 다른 성별의 Preset 이라면 Respawn 을 시켜야 한다.
	else
	{
		AWRCharacterMine* PreviousMyCharacter = GetMyCharacter();

		if (PreviousMyCharacter == nullptr)
			return;

		FVector SpawnLocation = PreviousMyCharacter->GetActorLocation();
		FRotator SpawnRotation = PreviousMyCharacter->GetActorRotation();

		WRTriggerManager::Get()->RemoveCollisionActor(PreviousMyCharacter);
		//WRTriggerManager::Get()->RemoveFollowerID(PreviousMyCharacter->GetActorID());

		if (PreviousMyCharacter != nullptr)
			Remove(PreviousMyCharacter);

		FString BPPath = NULL_STRING;
		WRTableID CharacterTID = INVALID_RECORD_TID;

		BPPath = GetPresetBPPath(PresetTable);
		CharacterTID = PresetTable->CharTableID;

		AID_t MyAID = WRAccountManager::Get()->GetAID();
		FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(MyAID, CharacterTID, BPPath, SpawnLocation, SpawnRotation);
		AWRCharacterMine* Player = WRCharacterManager::Get()->Spawn<AWRCharacterMine>(SpawnParam, true, true);
		Player->SetEnableSyncHMDTick(true);

		if (Player)
		{
			UWRActorComponentEquipmentLook* EquipLook = Player->GetComponent<UWRActorComponentEquipmentLook>();
			if (EquipLook && PresetTable)
			{
				//AsyncLoadResource(Player);
				//EquipLook->Equip(PresetTable->NormalEquipmentLookTID, FWREquipCompleteDelegate::CreateRaw(this, &WRCharacterManager::MergeMesh));
				EquipLook->Equip(PresetTable->NormalEquipmentLookTID);
			}

			WRTriggerManager::Get()->SetCollisionActor(GetMyCharacter());

			// [ 2020-4-08 : vuvle ] 내 캐릭터 생성을 알리는 Deligate 호출
			MyCharacterSpawnedDelegate.Broadcast(Player);
		}

		WRItemBase* EquippedMainWeapon = WRItemManager::Get()->FindEquippedMainWeapon();
		WRItemBase* EquippedSubWeapon = WRItemManager::Get()->FindEquippedSubWeapon();

		if (EquippedMainWeapon && EquippedSubWeapon)
		{
			// [ 2020-4-02 : vuvle ] : 양쪽 무기를 다 장착한 상태에서 모델 변경이 이루어질경우 EquipWeaponCostume(TID) 함수를 한번에 두개 호출하면 앞에가 씹혀서 양쪽 무기 장착 함수를 따로 호출한다.
			Player->SetEquippedWeaponItemTID(EquippedMainWeapon->GetTID(), EquippedSubWeapon->GetTID(), INVALID_TABLE_TID);
		}
		else if (EquippedMainWeapon)
		{
			Player->SetEquippedWeaponItemTID(EquippedMainWeapon->GetTID());
		}
		else if (EquippedSubWeapon)
		{
			Player->SetEquippedWeaponItemTID(EquippedSubWeapon->GetTID());
		}
	}

	WRAccountManager::Get()->SetMyCharacterPresetTID(InTableID);
	WRVFXManager::Get()->Spawn(EFFECT_PATH_4_CHARACTER_CHANGE, WRCharacterManager::Get()->GetMyCharacter(), nullptr);
}

void WRCharacterManager::ChangePreset(const AID_t IN InAID, const WRTableID IN InTableID, const bool IN bUsingHologram)
{
	AWRCharacterPlayer* FoundCharacter = Cast<AWRCharacterPlayer>(FindbyAID(InAID));
	if (FoundCharacter == nullptr)
	{
		ensureMsgf(false, TEXT("Character cannot be found by AID"));
		return;
	}

	FWRCharacterPresetTable* OriginPresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(FoundCharacter->GetPresetTID());
	if (OriginPresetTable == nullptr)
	{
		ensureMsgf(false, TEXT("Could not find Preset TID"));
		return;
	}

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InTableID);

	if (PresetTable == nullptr)
	{
		ensureMsgf(false, TEXT("Could not find Preset TID"));
		return;
	}

	// 원래 입고 있던 성별과 같은 Preset이라면 
	if (PresetTable->Gender == OriginPresetTable->Gender)
	{
		UWRActorComponentEquipmentLook* EquipLook = FoundCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			if (bUsingHologram == true)
			{
				EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
			}
			else
			{
				EquipLook->Equip(PresetTable->NormalEquipmentLookTID);
			}
		}
	}
	// 다른성별이라면 Respawn 한다
	else
	{
		// vuvle TODO : 서버에서 Disappear & Appear 를 해야만 한다.
		// [ 2020-3-9 : vuvle ] 기존 캐릭터 정보 저장
		FString BPPath = PresetTable->PresetBlueprint;
		WRTableID SpawnCharacterTID = PresetTable->CharTableID;
		FVector SpawnPosition = FoundCharacter->GetActorLocation();
		FRotator SpawnRotation = FoundCharacter->GetActorRotation();
		AID_t SpawnCharacterAID = FoundCharacter->GetActorID();
		WRTableID SpawnCharacterMainWeaponItemTID = FoundCharacter->GetRightWeaponItemTID();
		WRTableID SpawnCharacterSubWeaponItemTID = FoundCharacter->GetLeftWeaponItemTID();
		WRTableID SpawnCharacterTemporaryItemTID = FoundCharacter->GetTemporaryItemTID();
		FName SpawnCharacterName = FoundCharacter->GetCharacterName();

		Remove(FoundCharacter);

		FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(SpawnCharacterAID, SpawnCharacterTID, BPPath, SpawnPosition, SpawnRotation);
		AWRCharacterPlayer* SpawnedCharacter = Spawn<AWRCharacterPlayer>(SpawnParam);
		if (SpawnedCharacter == nullptr)
			return;

		UWRActorComponentEquipmentLook* EquipLook = SpawnedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			EquipLook->Equip(PresetTable->NormalEquipmentLookTID);
		}

		// 프리셋 설정
		SpawnedCharacter->SetPresetTID(InTableID);
		// 외형 설정
		SpawnedCharacter->SetEquippedWeaponItemTID(SpawnCharacterMainWeaponItemTID, SpawnCharacterSubWeaponItemTID, SpawnCharacterTemporaryItemTID);
		// 버프 설정
		//SpawnedCharacter->SetBuff(InInform.lstBuffInfo);
		// 이름 설정
		SpawnedCharacter->SetCharacterName(SpawnCharacterName);

		WRVFXManager::Get()->Spawn(EFFECT_PATH_4_CHARACTER_CHANGE, SpawnedCharacter, nullptr);
	}
}

FString WRCharacterManager::GetPresetBPPath(const FWRCharacterPresetTable* IN InPresetTable)
{
	if (InPresetTable == nullptr)
		return NULL_STRING;

	if (InPresetTable)
	{
		if (InPresetTable->Gender == EWRCharacterGender::Male_A)
		{
			return "Player_Man_FirstPerson";
		}
		else if (InPresetTable->Gender == EWRCharacterGender::Male_B)
		{
			return "Player_Man_B_FirstPerson";
		}
		else if (InPresetTable->Gender == EWRCharacterGender::Male_C)
		{
			return "Player_Man_D_FirstPerson";
		}
		else if (InPresetTable->Gender == EWRCharacterGender::Female_A)
		{
			return "Player_Woman_FirstPerson";
		}
		else if (InPresetTable->Gender == EWRCharacterGender::Female_B)
		{
			return "Player_Woman_B_FirstPerson";
		}
		else if (InPresetTable->Gender == EWRCharacterGender::Female_C)
		{
			return "Player_Woman_D_FirstPerson";
		}
		else
		{
			return "Player_Man_FirstPerson";
		}
	}
#ifdef UE_EDITOR
	else
	{
		return "Player_Man_FirstPerson";
	}
#else//UE_EDITOR
#endif//UE_EDITOR

}

FString WRCharacterManager::GetMyCharacterBPPath()
{
	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(WRAccountManager::Get()->GetMyCharacterPresetTID());
	if (PresetTable == nullptr)
	{
		return NULL_STRING;
	}

	FString BPPath = NULL_STRING;

	if (PresetTable)
	{
		BPPath = GetPresetBPPath(PresetTable);
	}

	//FString BPFullPath = FPackageName::IsValidObjectPath(BPPath) ? BPPath : WRPathManager::Get()->GetPath(EWRPath::Blueprint_Character) + BPPath;
	FString BPFullPath = FPackageName::IsValidObjectPath(BPPath) ? BPPath : WRPathManager::Get()->GetPath(EWRPath::Character) + BPPath;
	FString ResultPath = WRActorSpawnUtility::ResolvePath(BPFullPath);

	return ResultPath;
}

void WRCharacterManager::SpawnSO(const pt_Game_Actor::Inf_SOAppear_G2U& InInform)
{
	if (FindbyAID(InInform.apAID))
		return;

	const FWRNPCBaseTable* IOTable = FindNpcBaseTable(InInform.apTID);
	if (nullptr == IOTable)
		return;

	FVector SpawnPosition(InInform.posX, InInform.posY, InInform.posZ);
	FRotator SpawnRotation = FRotator(0.0f, InInform.angle, 0.0f);

	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(InInform.apAID, InInform.apTID, IOTable->BlueprintID, SpawnPosition, SpawnRotation, FVector::OneVector, InInform.spawnTID);
	AWRCharacter* SO = Spawn<AWRCharacter>(SpawnParam);
	if (nullptr == SO)
		return;

	SO->SetLocation(SpawnPosition, true);
}

void WRCharacterManager::SpawnAP(const pt_Game_Actor::Inf_APAppear_G2U& InInform)
{
	if (FindbyAID(InInform.apAID))
	{
		return;
	}

	const FString* AsyncSpawnKey = AsyncSpawnActorKey.FindKey(InInform.apAID);
	if (AsyncSpawnKey)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*AsyncSpawnKey);
	}

	FWRActionPropTable* APTable = WRTableManager::Get()->FindRow<FWRActionPropTable>(InInform.apTID);
	if (nullptr == APTable)
	{
		return;
	}

	APAppearanceInfo2Spawn.Add(InInform.apAID, InInform);

	FWRCompleteAsyncLoad AsyncLoadDele = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterManager::OnSpawnAPComplete);
	FString AsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(APTable->MainBPPath, AsyncLoadDele);
	if (AsyncKey == WRASYNCLOADMANAGER_INVALID)
		return;
	AsyncSpawnActorKey.Add(AsyncKey, InInform.apAID);
}

void WRCharacterManager::OnSpawnAPComplete(UObject* InActor, FString InHashKey)
{
	AWRCharacterActionProp* AP = Cast<AWRCharacterActionProp>(InActor);
	if (nullptr == AP)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnSpawnAPComplete"), TEXT("UObject is nullptr"));
		return;
	}

	AID_t ActorID = AsyncSpawnActorKey.FindRef(InHashKey);
	if (APAppearanceInfo2Spawn.Contains(ActorID) == false)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("OnSpawnAPComplete"), TEXT("Not found actorID"));
		return;
	}

	pt_Game_Actor::Inf_APAppear_G2U APInfo = APAppearanceInfo2Spawn.FindRef(ActorID);
	FWRActionPropTable* APTable = WRTableManager::Get()->FindRow<FWRActionPropTable>(APInfo.apTID);
	if (nullptr == APTable)
	{
		return;
	}

	FVector SpawnPosition(APInfo.posX, APInfo.posY, APInfo.posZ);
	FRotator SpawnRotation = FRotator(0.0f, APInfo.angle, 0.0f);
	FWRCharacterSpawnParams SpawnParam = FWRCharacterSpawnParams(APInfo.apAID, APInfo.apTID, APTable->MainBPPath, SpawnPosition, SpawnRotation, FVector::OneVector, APInfo.spawnTID);
	OnPostSpawned(SpawnParam, AP);

	AP->OnSpawnComplete(APInfo, APTable);


	//항상 제일 마지막에 호출 되어야 함
	APAppearanceInfo2Spawn.Remove(ActorID);
}

void WRCharacterManager::OnChangeAPState(const pt_Game_Actor::Inf_ChangeAPState_G2U& InInform)
{
	AWRCharacterActionProp* AP = FindbyAID_Casted<AWRCharacterActionProp>(InInform.apAID);
	if (nullptr == AP)
		return;

	AP->OnChangeState((EWRAPState::en)InInform.apState);
}

class AWRCharacter* WRCharacterManager::FindbyIndex(const int32 InIndex)
{
	if (Characters.GetMaxIndex() < InIndex)
		return nullptr;

	return Characters[FSetElementId::FromInteger(InIndex)];
}

class AWRCharacter* WRCharacterManager::FindbyAID(AID_t InAID)
{
	if (InAID == INVALID_AID)
		return nullptr;

	if (Characters.Contains(InAID))
	{
		return *Characters.Find(InAID);
	}

	return nullptr;
}

class AWRCharacter* WRCharacterManager::FindByTID(const WRTableID IN InCharacterTID)
{
	if (InCharacterTID == INVALID_TABLE_TID)
		return nullptr;

	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		if (CharacterTemp->GetCharacterTableID() != InCharacterTID)
			continue;

		return CharacterTemp;
	}

	return nullptr;
}

void WRCharacterManager::FindAllByTID(const WRTableID IN InCharacterTID, TDoubleLinkedList<class AWRCharacter*>& OUT OutCharacterList)
{
	OutCharacterList.Empty();

	if (InCharacterTID == INVALID_TABLE_TID)
		return;

	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		if (CharacterTemp->GetCharacterTableID() != InCharacterTID)
			continue;

		OutCharacterList.AddTail(CharacterTemp);
	}
}

AWRCharacter* WRCharacterManager::FindCharacterBySpawnTid(const int32 InSpawnTid)
{
	AWRCharacter** Result = nullptr;
	Result = TriggerSpawnTidCharacters.Find(InSpawnTid);
	return Result != nullptr ? *Result : nullptr;
}

const FWRNPCBaseTable* WRCharacterManager::FindNpcBaseTable(const WRTableID IN InTableID)
{
	if (DEF_START_VALUE_FOR_NPC_TID_BANDWIDTH <= InTableID && InTableID < DEF_START_VALUE_FOR_NPC_TID_BANDWIDTH + DEF_STANDARD_VALUE_NPC_TID_BANDWIDTH)
	{
		return WRTableManager::Get()->FindRow<FWRNpcTable>(InTableID);
	}
	else if (DEF_START_VALUE_FOR_ENPC_TID_BANDWIDTH <= InTableID && InTableID < DEF_START_VALUE_FOR_ENPC_TID_BANDWIDTH + DEF_STANDARD_VALUE_NPC_TID_BANDWIDTH)
	{
		return WRTableManager::Get()->FindRow<FWREnpcTable>(InTableID);
	}
	else if (DEF_START_VALUE_FOR_SO_TID_BANDWIDTH <= InTableID && InTableID < DEF_START_VALUE_FOR_SO_TID_BANDWIDTH + DEF_STANDARD_VALUE_NPC_TID_BANDWIDTH)
	{
		return WRTableManager::Get()->FindRow<FWRSOTable>(InTableID);
		//ensureMsgf(false, TEXT("Invalid InTableID"));
	}

	return nullptr;
}

class AWRCharacterIO* WRCharacterManager::FindIOByResultAttachID(const int32 InID)
{
	if (InID == INVALID_TABLE_TID)
		return nullptr;

	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		if (CharacterTemp->IsIO() == false)
			continue;

		AWRCharacterIO* CastedIO = Cast<AWRCharacterIO>(CharacterTemp);
		if (CastedIO == nullptr)
			continue;

		if (CastedIO->GetResultAttachMyID() == InID)
			return CastedIO;
	}

	return nullptr;
}

void WRCharacterManager::FindByNpcJobType(const EWRNpcJobType IN InNpcJobType, TDoubleLinkedList<AWRCharacterNPC*>& OUT OutNpcCharacter)
{
	OutNpcCharacter.Empty();

	if (InNpcJobType == EWRNpcJobType::None)
		return;

	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		if (CharacterTemp->GetCharacterType() != EWRCharacterType::NPC)
			continue;

		AWRCharacterNPC* NPCCharacter = Cast<AWRCharacterNPC>(CharacterTemp);
		if (NPCCharacter == nullptr)
			continue;

		const FWRNpcTable* NPCTable = static_cast<const FWRNpcTable*>(NPCCharacter->GetNpcBaseTable());
		if (NPCTable == nullptr)
			continue;

		if (NPCTable->JobType != InNpcJobType)
			continue;

		OutNpcCharacter.AddTail(NPCCharacter);
	}
}


void WRCharacterManager::GetActorListBasedOnFactionRelation(class AWRCharacter* InFinder, EWRFactionRelation InRelation, TArray<class AWRCharacter*>& InoutResult)
{
	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		EWRFactionRelation FactionRelation = InFinder->GetRelationToTarget(CharacterTemp);
		if (InRelation != FactionRelation)
			continue;

		InoutResult.Add(CharacterTemp);
	}
}

void WRCharacterManager::GetNearByCharacters(TDoubleLinkedList<class AWRCharacter *>& OUT OutList, const float IN InDist)
{
	AWRCharacterMine* pMine = GetMyCharacter();

	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		if (CharacterTemp->IsMe())
			continue;

		if (FVector::Dist(pMine->GetActorLocation(), CharacterTemp->GetActorLocation()) > InDist)
			continue;

		OutList.AddTail(CharacterTemp);
	}
}

void WRCharacterManager::GetActorListInnerSector(TDoubleLinkedList<class AWRCharacter*>& OUT OutList, const FTransform IN InStandardTF, const float IN InMin, const float IN InMax, const float IN InAngle)
{
	for (auto& Element : Characters)
	{
		AWRCharacter* CharacterTemp = Element;
		if (CharacterTemp == nullptr)
			continue;

		if (CharacterTemp->IsMe())
			continue;

		if (WRActorUtility::CheckInnerRange_Sector(CharacterTemp, InStandardTF, InMin, InMax, InAngle) == false)
			continue;

		OutList.AddTail(CharacterTemp);
	}

	// for Debug
// 	AWRCharacterMine* pMine = GetMyCharacter();
// 	WRActorUtility::DrawSector(pMine->GetWorld(), InStandardTF.GetLocation(), InStandardTF.GetRotation().GetForwardVector(), InMin, InMax, InAngle, DEF_CHEERS_SUGGEST_LIMIT_TIME/1000.f);
}

AID_t WRCharacterManager::GenerateStandaloneAUID()
{
	if (MAX_STANDALONE_AUID <= ActorUID4StandAlone)
	{
		ActorUID4StandAlone = INITIALIZE_STANDALONE_AUID;
	}
	else
	{
		ActorUID4StandAlone += INCREASE_STANDALONE_UID;
	}

	return ActorUID4StandAlone;
}

class AWRCharacterMine* WRCharacterManager::GetMyCharacter()
{
	return Cast<AWRCharacterMine>(FindbyAID(MyAUID));
}

bool WRCharacterManager::IsMyCharacter(class AWRCharacter* InChar) const
{
	// 	if (INVALID_AID == MyAUID)
	// 		return false;

	if (InChar == nullptr)
		return false;

	if (MyAUID == InChar->GetActorID())
		return true;

	return false;
}

const AWRCharacter* WRCharacterManager::GetMyTarget()
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe != nullptr)
	{
		return pMe->GetTargetCharacter();
	}
	return nullptr;
}

bool WRCharacterManager::IsEquippedHook()
{
	// [ 2019-10-2 : kyu ] *** 나중에 ItemData로 바꿔야한다.

	if (GetEquippedSubWeaponAbility() == EWRAdditionalAbility::Hook)
		return true;

	return false;
}

bool WRCharacterManager::IsHookConnected()
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe != nullptr)
	{
		UWRActorComponentConnectbyCable* CableComponent = pMe->GetComponent<UWRActorComponentConnectbyCable>();
		if (CableComponent == nullptr)
			return false;

		if (CableComponent->IsWorking())
			return true;
	}
	return false;
}

bool WRCharacterManager::IsHookableRange(class AWRCharacter* IN InChar)
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe != nullptr)
	{
		return pMe->IsHookableRange(InChar);
	}
	return false;
}

bool WRCharacterManager::IsCableTarget(AWRCharacter* IN InChar)
{
	AWRCharacter* pCableTarget = GetCableTarget();
	if (pCableTarget == InChar)
		return true;

	return false;
}

bool WRCharacterManager::IsCableTargetPart(class AWRCharacter* IN InChar, const EWRCollisionType IN InPart)
{
	if (IsCableTarget(InChar) == false)
		return false;

	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe != nullptr)
	{
		UWRActorComponentConnectbyCable* CableComponent = pMe->GetComponent<UWRActorComponentConnectbyCable>();
		if (CableComponent == nullptr)
			return false;

		if (CableComponent->IsWorking() == false)
			return false;

		return CableComponent->IsConnectingPart(InPart);
	}
	return false;
}

AWRCharacter* WRCharacterManager::GetCableTarget()
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe != nullptr)
	{
		UWRActorComponentConnectbyCable* CableComponent = pMe->GetComponent<UWRActorComponentConnectbyCable>();
		if (CableComponent == nullptr)
			return nullptr;

		return CableComponent->GetCableTarget();
	}
	return nullptr;
}

EWRAdditionalAbility WRCharacterManager::GetEquippedSubWeaponAbility()
{
	// [ 2019-10-2 : kyu ] *** 나중에 ItemData로 바꿔야한다.

	AWRCharacterMine* pMainChar = WRCharacterManager::Get()->GetMyCharacter();
	if (pMainChar != nullptr)
	{
		FWRWeaponTable* pWeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(pMainChar->GetLeftWeaponTID());
		if (pWeaponTable != nullptr)
		{
			FWREquipmentLookTable* pEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(pWeaponTable->WeaponLookTid);
			if (pEquipLookTable != nullptr)
			{
				return pEquipLookTable->AdditionalAbility;
			}
		}
	}
	return EWRAdditionalAbility::None;
}

void WRCharacterManager::SpawnAbandonedThing(FString& IN InObjectPath, class AWRCharacter* IN InChar, const WRTableID IN InItemTID)
{
	if (InObjectPath.IsEmpty())
		return;

	if (InChar == nullptr)
		return;

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World != nullptr)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParams.Owner = InChar;

		// Calc Location
		FTransform tf;
		FVector vCharLocation = InChar->GetActorLocation();
		vCharLocation.Z += InChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector vCharForward = InChar->GetActorForwardVector();
		FVector vLocation = vCharLocation + vCharForward * 100.0f;
		tf.SetLocation(vLocation);
		// Spawn
		auto AsyncLoadComplete = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString strKey)
		{
			AWRStaticMeshAbandonedThing* AbandonThingActor = World->SpawnActor<AWRStaticMeshAbandonedThing>(AWRStaticMeshAbandonedThing::StaticClass(), tf, spawnParams);
			if (AbandonThingActor == nullptr)
				return;

			UStaticMesh* StaticMesh = Cast<UStaticMesh>(InObject);
			// Set Static Mesh
			AbandonThingActor->SetStaticMesh(StaticMesh);
			// Static Mesh Physics On
			AbandonThingActor->SetPhysics(vCharForward, 500.f);
			// Set Timer
			AbandonThingActor->SetMaintainTimer(DEF_ABANDON_THING_MAINTAIN_TIME);
			// Set Info
			AbandonThingActor->SetItemTID(InItemTID);
		});
		FString HashKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InObjectPath, AsyncLoadComplete);
	}
}

bool WRCharacterManager::IsMoving()
{
	AWRCharacterPlayer* pMe = GetMyCharacter();
	if (pMe == nullptr)
		return false;

	if (pMe->MoveForwardValue != 0.f || pMe->MoveRightValue != 0.f)
		return true;

	return false;
}

void WRCharacterManager::Remove_Internal(class AWRCharacter* InCharacter, bool bInRemoveFromContainer /*= true*/)
{
	if (InCharacter->IsValidLowLevel() == false)
		return;

	const FString* FoundKey = AsyncSpawnActorKey.FindKey(InCharacter->GetActorID());
	if (FoundKey != nullptr)
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*FoundKey);

	// [ 2020-6-18 : animal14 ] 진행중인 Task를 삭제 및 취소
	if (Tasks.Contains(InCharacter))
	{
#if UE_EDITOR
		TArray<FStringFormatArg> StringArgs;
		(Tasks[InCharacter].IsCompleted == false) ? StringArgs.Add(TEXT("It isn't completed to load resources.")) : StringArgs.Add(TEXT("It is failed to merge meshes asynchronously."));

		FString Text = FString::Format(TEXT("{0}"), StringArgs);

		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Text, TEXT("WRCharacterManager::Remove_Internal()"));
#else // UE_EDITOR
#endif // UE_EDITOR

		Tasks[InCharacter].Clear();
	}
	Tasks.Remove(InCharacter);

	// [ 2020-3-04 : vuvle ] : 임시 Look IK의 Target으로 설정된 캐릭터가 삭제되면 Container 를 돌면서 초기화 시킨다.
	for (auto& Element : Characters)
	{
		if (Element->GetLookIKTarget() == InCharacter)
		{
			Element->SetLookIKTarget(nullptr);
		}
	}

	FString CharacterName = InCharacter->GetName();

	// [ 2020-7-23 : kyu ] 누구든 점유중인 IO가 삭제 시 해제를 한다
	if (InCharacter->IsIO())
	{
		AWRCharacterIO* CastedChar = Cast<AWRCharacterIO>(InCharacter);
		if (CastedChar && CastedChar->GetOccupiedPlayer() != nullptr && CastedChar->GetOccupiedPlayer()->IsValidLowLevel())
		{
			CastedChar->GetOccupiedPlayer()->DetermineFSMSignal(EWRFSM::Idle);
		}
	}
	
	InCharacter->RemoveFromRoot();
	InCharacter->OnPreDestroy();

#if UE_EDITOR
	WRActorUtility::IsRooted(InCharacter);
#else // UE_EDITOR
#endif // UE_EDITOR

	InCharacter->Destroy();

	if (InCharacter->GetActorID() == MyAUID)
	{
		MyAUID = 0;
	}

	if (bInRemoveFromContainer == true)
	{
		Characters.Remove(InCharacter->GetActorID());
		DeadCharacters.Remove(InCharacter);
		if (InCharacter->GetTriggerSpawnTid() && TriggerSpawnTidCharacters.Contains(InCharacter->GetTriggerSpawnTid()) == true)
		{
			TriggerSpawnTidCharacters.Remove(InCharacter->GetTriggerSpawnTid());
		}
	}
}

void WRCharacterManager::RemoveAll()
{
	for (auto& Element : Characters)
	{
		this->Remove_Internal(Element, false);
	}

	Characters.Empty();
	TriggerSpawnTidCharacters.Empty();
	DeadCharacters.Empty();
	PlayerAppearInfo2Spawn.Empty();
	NpcAppearanceInfo2Spawn.Empty();

	AsyncSpawnActorKey.Empty();
	IOAppearanceInfo2Spawn.Empty();
	SOAppearanceInfo2Spawn.Empty();
}

void WRCharacterManager::SetTickInterval_Internal(float InInterval, AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
		return;

	InCharacter->SetActorTickInterval(InInterval);
}

void WRCharacterManager::CancelAsync(const AID_t InAID)
{
	const FString* Key = AsyncSpawnActorKey.FindKey(InAID);
	if (Key == nullptr)
	{
		return;
	}

	WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(*Key);
}

void WRCharacterManager::MergeMesh(AActor* InActor, EWREquipPart InPart)
{
	if (InPart != EWREquipPart::Body)
	{
#ifdef UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Not Body"), TEXT("WRCharacterManager::MergeMesh()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	FWRSkeletalMeshMergeParameters Parameters;
	Parameters.Skeleton = LoadObject<USkeleton>(nullptr, TEXT("/Game/Asset/Character/NPC/LP287_P1/SK_FemaleBase_Skeleton.SK_FemaleBase_Skeleton"));

	auto LoadResource = [&](const FString& InPath) {
		USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *InPath);
		if (SkeletalMesh == nullptr)
		{
			return;
		}
		Parameters.MeshesToMerge.Add(SkeletalMesh);
	};

	LoadResource(TEXT("/Game/Asset/Character/NPC/LP287_P1/Separates/SK_TopBody_A.SK_TopBody_A"));
	LoadResource(TEXT("/Game/Asset/Character/NPC/LP287_P1/Separates/SK_Head.SK_Head"));

	// [ 2020-6-16 : animal14 ] 스켈레탈 메시를 머징
	USkeletalMesh* SkeletalMesh = WRActorUtility::MergeMeshes(Parameters);
	if (SkeletalMesh == nullptr)
	{
		return;
	}

	// [ 2020-6-16 : animal14 ] 머징된 스켈레탈 메시로 초기화
	AWRCharacter* Character = Cast<AWRCharacter>(InActor);
	if (Character == nullptr)
	{
		return;
	}

	UWRActorComponentEquipmentLook* ActorComponentEquipmentLook = Character->GetComponent<UWRActorComponentEquipmentLook>();
	if (ActorComponentEquipmentLook == nullptr)
	{
		return;
	}
	//ActorComponentEquipmentLook->SetMesh(InPart, SkeletalMesh);

	// [ 2020-6-16 : animal14 ] 피직스 애셋을 명시적으로 설정
	UPhysicsAsset* PhysicsAsset = LoadObject<UPhysicsAsset>(nullptr, TEXT("/Game/Asset/Character/NPC/LP287_P1/SK_FemaleBase_PhysicsAsset.SK_FemaleBase_PhysicsAsset"));
	if (PhysicsAsset == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}
	SkeletalMeshComponent->SetPhysicsAsset(PhysicsAsset);
}

void WRCharacterManager::MergeMeshAsynchronously(AActor* InActor, EWREquipPart InPart)
{
	if (InPart != EWREquipPart::Body)
	{
#ifdef UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Not Body"), TEXT("WRCharacterManager::MergeMeshAsynchronously()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	if (Tasks.Contains(InActor) == false)
	{
#ifdef UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("It wasn't requested to load resource asynchronously."), TEXT("WRCharacterManager::MergeMeshAsynchronously()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	if (Tasks[InActor].IsCompleted == false)
	{
#ifdef UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("The task wasn't completed."), TEXT("WRCharacterManager::MergeMeshAsynchronously()"));
#else // UE_EDITOR
#endif // UE_EDITOR
		return;
	}

	// [ 2020-6-16 : animal14 ] 머징된 스켈레탈 메시로 초기화
	AWRCharacter* Character = Cast<AWRCharacter>(InActor);
	if (Character == nullptr)
	{
		return;
	}

	// [ 2020-6-16 : animal14 ] 피직스 애셋을 명시적으로 설정
	UPhysicsAsset* PhysicsAsset = Tasks[InActor].PhysicsAsset;
	if (PhysicsAsset == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}
	//SkeletalMeshComponent->SetPhysicsAsset(PhysicsAsset);

	// [ 2020-6-17 : animal14 ] 완료된 Task를 정리
	Tasks[InActor].Clear();
	Tasks.Remove(InActor);
}

void WRCharacterManager::AsyncLoadResource(AActor* InCharacter)
{
	FWRSkeletalMeshMergeTask MergeTask;
	Tasks.Add(InCharacter, MergeTask);

	auto LoadResource = [&](const FString& InPath, AActor* InCharacter) {
		FString Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(InPath, FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterManager::OnAsyncLoadComplete));

		Tasks[InCharacter].Objects.Add(Hash, nullptr);
	};

	LoadResource(TEXT("/Game/Asset/Character/CTest/Custom_test.Custom_test"), InCharacter);
	LoadResource(TEXT("/Game/Asset/Character/CTest/Custom_test_hair.Custom_test_hair"), InCharacter);
	LoadResource(TEXT("/Game/Asset/Character/CTest/Custom_test_Skeleton.Custom_test_Skeleton"), InCharacter);
	LoadResource(TEXT("/Game/Asset/Character/CTest/Custom_test_PhysicsAsset.Custom_test_PhysicsAsset"), InCharacter);

	//LoadResource(TEXT("/Game/Asset/Character/NPC/LP287_P1/Separates/SK_TopBody_A.SK_TopBody_A"), InCharacter);
	//LoadResource(TEXT("/Game/Asset/Character/NPC/LP287_P1/Separates/SK_Head.SK_Head"), InCharacter);
	//LoadResource(TEXT("/Game/Asset/Character/NPC/LP287_P1/SK_FemaleBase_Skeleton.SK_FemaleBase_Skeleton"), InCharacter);
	//LoadResource(TEXT("/Game/Asset/Character/NPC/LP287_P1/SK_FemaleBase_PhysicsAsset.SK_FemaleBase_PhysicsAsset"), InCharacter);
}

void WRCharacterManager::OnAsyncLoadComplete(UObject* InAsset, FString InHash)
{
	bool IsContaining = false;

	for (auto& Task : Tasks)
	{
		if (Task.Value.Objects.Contains(InHash))
		{
			IsContaining = true;

			InAsset->AddToRoot();

			USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(InAsset);
			if (SkeletalMesh)
			{
				Task.Value.MeshesToMerge.Add(SkeletalMesh);
			}

			USkeleton* Skeleton = Cast<USkeleton>(InAsset);
			if (Skeleton)
			{
				Task.Value.Skeleton = Skeleton;
			}

			UPhysicsAsset* PhysicsAsset = Cast<UPhysicsAsset>(InAsset);
			if (PhysicsAsset)
			{
				Task.Value.PhysicsAsset = PhysicsAsset;
			}

			Task.Value.Objects[InHash] = InAsset;

			Task.Value.IsCompleted = WRAsyncLoadUObjectManager::IsLoadCompleteTasks(Task.Value.Objects);
			if (Task.Value.IsCompleted)
			{
				FWRSkeletalMeshMergeParameters Parameters;
				Parameters.MeshesToMerge = Task.Value.MeshesToMerge;
				Parameters.Skeleton = Task.Value.Skeleton;

				// [ 2020-6-16 : animal14 ] 스켈레탈 메시를 머징
				USkeletalMesh* MergedSkeletalMesh = WRActorUtility::MergeMeshes(Parameters);
				if (MergedSkeletalMesh == nullptr)
				{
					continue;
				}

				AWRCharacterPlayer* Character = Cast<AWRCharacterPlayer>(Task.Key);
				if (Character == nullptr)
				{
					continue;
				}

				UWRActorComponentEquipmentLook* ComponentEquipmentLook = Character->GetComponent<UWRActorComponentEquipmentLook>();
				if (ComponentEquipmentLook == nullptr)
				{
					continue;
				}

				// [ 2020-6-18 : animal14 ] 임시코드 : 내 캐릭터는 프리셋 정보가 따로 존재
				//FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(Character->GetPresetTID());
				//if (PresetTable == nullptr)
				//{
				//	continue;
				//}

//#ifdef ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
				// [ 2020-6-22 : animal14 ] 머징된 메시로 연출이 들어가야하기 때문에 머징한 스켈레탈 메시로 초기화
				ComponentEquipmentLook->Equip(11, NAME_None, FWREquipmentComplete::CreateRaw(this, &WRCharacterManager::MergeMeshAsynchronously), MergedSkeletalMesh, MergedSkeletalMesh);
				//#else // ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
				//#endif // ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
				break;
			}
		}
	}

	if (IsContaining == false)
	{
#ifdef UE_EDITOR
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("It wasn't requested to load resource asynchronously."), TEXT("WRCharacterManager::OnAsyncLoadComplete()"));
#else // UE_EDITOR
#endif // UE_EDITOR
	}
	}

FDelegateHandle WRCharacterManager::AddMyCharacterStartMoveDelegate(const FSimpleDelegate& IN InDelegateToNotify)
{
	return StartMoveDelegate.Add(InDelegateToNotify);
}

FDelegateHandle WRCharacterManager::AddMyCharacterStopMoveDelegate(const FSimpleDelegate& IN InDelegateToNotify)
{
	return StopMoveDelegate.Add(InDelegateToNotify);
}

FDelegateHandle WRCharacterManager::AddMyCharacterTargetChangedDelegate(const FSimpleDelegate& IN InDelegateToNotify)
{
	return TargetChangedDelegate.Add(InDelegateToNotify);
}

void WRCharacterManager::RemoveMyCharacterStartMoveDelegate(const FDelegateHandle& IN InDelegateHandle)
{
	StartMoveDelegate.Remove(InDelegateHandle);
}

void WRCharacterManager::RemoveMyCharacterStopMoveDelegate(const FDelegateHandle& IN InDelegateHandle)
{
	StopMoveDelegate.Remove(InDelegateHandle);
}

void WRCharacterManager::RemoveMyCharacterTargetChangedDelegate(const FDelegateHandle& IN InDelegateHandle)
{
	TargetChangedDelegate.Remove(InDelegateHandle);
}

void WRCharacterManager::NotifyMyCharacterStartMoveDelegate()
{
	StartMoveDelegate.Broadcast();
}

void WRCharacterManager::NotifyMyCharacterStopMoveDelegate()
{
	StopMoveDelegate.Broadcast();
}

void WRCharacterManager::NotifyMyCharacterTargetChangedDelegate()
{
	TargetChangedDelegate.Broadcast();
}

void WRCharacterManager::SetVisibleAll(const TArray<AID_t>& IgnoreAIDList, const bool InIsVisible)
{
	for (AWRCharacter* It : Characters)
	{
		if (It == nullptr)
			continue;

		if (IgnoreAIDList.Contains(It->GetActorID()) == false)
		{
			It->SetCharacterInvisible(InIsVisible);
		}
	}
}

void WRCharacterManager::SetVisibleByList(const TArray<AID_t>& InAIDList, const bool InIsVisible)
{
	for (AID_t InAID : InAIDList)
	{
		AWRCharacter* InCharacter = this->FindbyAID(InAID);
		if(InCharacter == nullptr)
			continue;

		InCharacter->SetCharacterInvisible(InIsVisible);
	}
}

void WRCharacterManager::SetWRTargettingCollisionEnableAll(const ECollisionEnabled::Type InType, const TArray<AID_t>& InIgnoreList)
{
	for (auto& Element : Characters)
	{
		if (InIgnoreList.Contains(Element->GetActorID()) == false)
		{
			Element->SetCollisionTargetting(InType);
		}
	}
}

void WRCharacterManager::SetWRTargettingCollisionEnableByList(const TArray<AID_t>& InAIDList, const ECollisionEnabled::Type InType)
{
	for (AID_t InAID : InAIDList)
	{
		AWRCharacter* InCharacter = this->FindbyAID(InAID);
		if (InCharacter == nullptr)
			continue;

		InCharacter->SetCollisionTargetting(InType);
	}
}

void WRCharacterManager::AddVisibilityBitByAIDList(const TArray<AID_t>& InAIDList, const EWRVisibility InBit)
{
	for (AID_t InAID : InAIDList)
	{
		AWRCharacter* InCharacter = this->FindbyAID(InAID);
		if (InCharacter == nullptr)
			continue;

		InCharacter->AddVisibilityBit(InBit);
	}
}

void WRCharacterManager::RemoveVisibilityBitByAIDList(const TArray<AID_t>& InAIDList, const EWRVisibility InBit)
{
	for (AID_t InAID : InAIDList)
	{
		AWRCharacter* InCharacter = this->FindbyAID(InAID);
		if (InCharacter == nullptr)
			continue;

		InCharacter->RemoveVisibilityBit(InBit);
	}
}

void WRCharacterManager::SetEnableDisplayURODebug(const bool bInEnable)
{
	for (auto& Element : Characters)
	{
		Element->SetEnableDisplayURODebug(bInEnable);
	}
}

void WRCharacterManager::ResetVRRotateOrientation()
{
	AWRCharacterMine* MyCharacter = GetMyCharacter();
	if (MyCharacter == nullptr)
		return;

	FRotator MyCharacterRotator = MyCharacter->GetActorRotation();
	MyCharacter->GetRotator()->RotateCharacterWithHMD(MyCharacterRotator);
}

//====================================================================================


void WRCharacterManager::GetCharactersByType(EWRCharacterType InType, TArray<AWRCharacter*>& OutResult)
{
	for (auto& Element : Characters)
	{
		if (Element->GetCharacterType() != InType)
			continue;

		OutResult.Add(Element);
	}
}

void WRCharacterManager::GetAllActor(TArray<AActor*>& InoutActor)
{
	for (auto& Element : Characters)
	{
		InoutActor.Add(Element);
	}
}

void WRCharacterManager::OnChangedConsole_IOCollisionShow(const bool IN InShow)
{
	for (auto& Element : Characters)
	{
		if (Element->IsValidLowLevel() && Element->IsIO())
		{
			AWRCharacterIO* CastedChar = Cast<AWRCharacterIO>(Element);
			if (CastedChar != nullptr)
			{
				CastedChar->SetShowCollisions(InShow);
			}
		}
	}
}

void WRCharacterManager::CutOffMyHookChain(const bool bInImmediately /* = false */, const bool IN InIdle /* = false */)
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe == nullptr)
		return;

	pMe->CutOffHookChain(bInImmediately, nullptr, true, InIdle);
}

void WRCharacterManager::HookCutAllRelatedTarget(AWRCharacter* IN InTarget, const bool IN InByMe)
{
	for (auto& Element : Characters)
	{
		AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Element);
		if (CastedPlayer == nullptr)
			continue;

		CastedPlayer->CutOffHookChain(true, InTarget, InByMe);
	}
}

void WRCharacterManager::SetMyCheersCondition(const EWRCheersConditionType IN InConditionType)
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe == nullptr)
		return;

	pMe->SetCheersCondition(InConditionType);
}

EWRCheersConditionType WRCharacterManager::GetMyCheersCondition()
{
	AWRCharacterMine* pMe = GetMyCharacter();
	if (pMe == nullptr)
		return EWRCheersConditionType::None;

	return pMe->GetCheersCondition();
}

void WRCharacterManager::SetCheersCondition(const AID_t IN InAID, const EWRCheersConditionType IN InConditionType)
{
	AWRCharacter* pChar = FindbyAID(InAID);
	if (pChar == nullptr)
		return;

	pChar->SetCheersCondition(InConditionType);
}

EWRCheersConditionType WRCharacterManager::GetCheersCondition(const AID_t IN InAID)
{
	AWRCharacter* pChar = FindbyAID(InAID);
	if (pChar == nullptr)
		return EWRCheersConditionType::None;

	return pChar->GetCheersCondition();
}

void WRCharacterManager::OnAddItemReceived()
{
	TArray<AWRCharacter*> NpcCharacters;
	GetCharactersByType(EWRCharacterType::NPC, NpcCharacters);
	for (int32 i = 0; i < NpcCharacters.Num(); ++i)
	{
		AWRCharacter* NpcCharacter = NpcCharacters[i];
		if (NpcCharacter == nullptr)
			continue;

		UWRActorComponentEventAction* EventActionComponent = NpcCharacter->GetComponent<UWRActorComponentEventAction>();
		if (EventActionComponent == nullptr)
			continue;

		EventActionComponent->UpdateEventActoinCodition();
	}
}

void WRCharacterManager::OnUpdateItemCountReceived()
{
	TArray<AWRCharacter*> NpcCharacters;
	GetCharactersByType(EWRCharacterType::NPC, NpcCharacters);
	for (int32 i = 0; i < NpcCharacters.Num(); ++i)
	{
		AWRCharacter* NpcCharacter = NpcCharacters[i];
		if (NpcCharacter == nullptr)
			continue;

		UWRActorComponentEventAction* EventActionComponent = NpcCharacter->GetComponent<UWRActorComponentEventAction>();
		if (EventActionComponent == nullptr)
			continue;

		EventActionComponent->UpdateEventActoinCodition();
	}
}

void WRCharacterManager::OnDeleteItemReceived()
{
	TArray<AWRCharacter*> NpcCharacters;
	GetCharactersByType(EWRCharacterType::NPC, NpcCharacters);
	for (int32 i = 0; i < NpcCharacters.Num(); ++i)
	{
		AWRCharacter* NpcCharacter = NpcCharacters[i];
		if (NpcCharacter == nullptr)
			continue;

		UWRActorComponentEventAction* EventActionComponent = NpcCharacter->GetComponent<UWRActorComponentEventAction>();
		if (EventActionComponent == nullptr)
			continue;

		EventActionComponent->UpdateEventActoinCodition();
	}
}
