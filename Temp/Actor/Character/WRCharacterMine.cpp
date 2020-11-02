// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacterMine.h"
#include "WRCharacterIO.h"
#include "WRCharacterNPC.h"

#include <HeadMountedDisplayFunctionLibrary.h>
#include <GameFramework/Character.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/InputComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/Actor.h>
#include <WidgetInteractionComponent.h>
#include <AudioCaptureComponent.h>
#include <AudioDeviceManager.h>
#include <Engine/Engine.h>

#include "WidgetComponent/WRWidgetComponentBase.h"
#include "UI/Panel/WRUIPanel_Main.h"
#include "UI/Panel/WRUIPanel_CrossHair.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRItemTable.h"

#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Character/WRActorComponentAttack.h"
#include "Component/Character/WRActorComponentStereoLayerManager.h"
#include "Components/StereoLayerComponent.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRActorComponentRevive.h"
#include "Component/Character/WRActorComponentVR.h"
#include "Component/Character/WRActorComponentChatRecording.h"

#include "Manager/WRConsoleManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRPopupManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRItemManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRTODManager.h"
#include "Manager/WRMediaPlayerManager.h"
#include "Manager/WRDialogueManager.h"
#include "Manager/WRTutorialManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WREnvInteractionManager.h"
#include "Manager/WRCheersManager.h"

#include "CombatSystem/Combat/WRCombatInfo.h"

#include "Network/IMMessageSender.h"

#include "Object.h"
#include "../Animation/WRAnimInstance.h"
#include "../EnvActor/Ladder/WRLadder.h"
#include "../EnvActor/WREnvVehicleBase.h"

#include "Sound/SoundSubmix.h"

#include "Utility/WRActorUtility.h"
#include "Manager/WRVehicleManager.h"
#include "Manager/WRSpawnSimulationManager.h"
#include "Component/Character/WRActorComponentInteraction.h"
#include "AIController.h"
#include "Manager/WRPostProcessMaterialManager.h"


// For Cpp
DEFINE_LOG_CATEGORY(WRCharacterMine)


#define ECC_HookTrace ECC_GameTraceChannel17
#define ECC_EnvGetOnOffTrace ECC_GameTraceChannel17

//#include "UI/Panel/WRUIPanel_Dialogue.h"
//#include "Enum/EWRCharacter.h"

AWRCharacterMine::AWRCharacterMine(const FObjectInitializer& ObjectInitializer)
	: AWRCharacterPlayer(ObjectInitializer)
{
	ThirdPersonSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ThirdPerson");
	//ThirdPersonSkeletalMeshComponent->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	//ThirdPersonSkeletalMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	//ThirdPersonSkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ThirdPersonSkeletalMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	ThirdPersonSkeletalMeshComponent->SetVisibility(false);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AWRCharacterMine::BeginPlay()
{
	Super::BeginPlay();

	// [ 2020-06-16 EZ : 손 Mesh에 Collision이 작동하여 Vehicle 이동시 밀림 현상 및 순간이동 발생하여 collision 해제 ]
	USkeletalMeshComponent* InSkelMesh = this->GetMesh();
	if (InSkelMesh != nullptr)
		InSkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//StereoLayerComponent = Cast<UStereoLayerComponent>(GetDefaultSubobjectByName(TEXT("StereoLayerSystem")));
}

void AWRCharacterMine::OnPostSpawn()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCurrentSkeletalMeshComponent()->BoundsScale = 10.0f;
	bUseControllerRotationYaw = false;
	bReqBeginFallPacket = false;

	InputSystemTargetChangedHandle = WRInputSystemManager::Get()->GetHMDTargetChangedDelegate().AddUObject(this, &AWRCharacterMine::OnInputSystemTargetChanged);

	WRPostProcessMaterialManager::Get()->PostProcessMaterialSetting(Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass())));

	Super::OnPostSpawn();

	//this->CreateReviveComponent();
}

void AWRCharacterMine::OnCompleteSpawn()
{
	this->InitCameraOffsetByPlayerTable();
}

void AWRCharacterMine::OnPreDestroy()
{
	UWidgetInteractionComponent* WidgetInteractionComponent = Cast<UWidgetInteractionComponent>(this->GetComponentByClass(UWidgetInteractionComponent::StaticClass()));
	if (WidgetInteractionComponent->IsValidLowLevel() == true)
	{
		WidgetInteractionComponent->Deactivate();
	}

	if (InputSystemTargetChangedHandle.IsValid())
		WRInputSystemManager::Get()->GetHMDTargetChangedDelegate().Remove(InputSystemTargetChangedHandle);

	if (AudioCaptureComponent->IsValidLowLevel() == true)
	{
		if (AudioCaptureComponent->IsRegistered() == true)
			AudioCaptureComponent->UnregisterComponent();

		this->RemoveOwnedComponent(AudioCaptureComponent);
		AudioCaptureComponent->DestroyComponent();
	}

	if (SphereFlippedNormalComponent)
	{
		SphereFlippedNormalComponent->UnregisterComponent();
		SphereFlippedNormalComponent->RemoveFromRoot();
		this->RemoveOwnedComponent(SphereFlippedNormalComponent);
		SphereFlippedNormalComponent->DestroyComponent();
	}

#if USE_AUDIOMIXER_BP_LIBRARY
	// [ 2020-3-18 : magedoga ] Flush SoundSubmix
	SoundSubmix = nullptr;
#endif // USE_AUDIOMIXER_BP_LIBRARY

	// [ 2020-3-26 : magedoga ] Disable Sync HMD Tick
	this->SetEnableSyncHMDTick(false);

	Super::OnPreDestroy();

	//this->RemoveReviveComponent();
}

void AWRCharacterMine::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWRCharacterMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString MSG = FString::Format(TEXT("Mesh Visible : {0}"), { GetMesh()->IsVisible() == true ? 1 : 0 });
	GEngine->AddOnScreenDebugMessage(-1, 0.016f, FColor::Green, MSG);

	MSG = FString::Format(TEXT("Visible Flag : {0}"), { GetMesh()->GetVisibleFlag() });
	GEngine->AddOnScreenDebugMessage(-1, 0.016f, FColor::Green, MSG);

	UpdateHookTraceCollision();
	UpdateCheckEnvGetOnOff();

	TickRequestMovement(DeltaTime);


	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetWorld()->GetFirstPlayerController();
	if (PlayerController->IsValidLowLevel())
	{
		FVector Empty;
		PlayerController->GetInputMotionState(Empty, Empty, vGyro, Empty);

		WRMaterialParameterCollectionManager::Get()->SetLocationForFoliageInteraction(FName("Location1"), GetActorTransform().GetLocation());
	}

#if UE_EDITOR
	if (WRConsoleManager::Get()->GetInt(EWRConsole::Debug_MyRadius) > 0)
	{
		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
		//UKismetSystemLibrary::DrawDebugCircle(World, GetActorLocation() + FVector(0.f, 0.f, 10), WRConsoleManager::Get()->GetInt(EWRConsole::Debug_MyRadius), 100, FLinearColor::Red, 0.f, 0.f, FVector(1, 0, 0), FVector(0, 1, 0));
		UKismetSystemLibrary::DrawDebugSphere(World, GetActorLocation(), WRConsoleManager::Get()->GetInt(EWRConsole::Debug_MyRadius), 100, FLinearColor::Red);
	}
#else // UE_EDITOR
#endif // UE_EDITOR
}

void AWRCharacterMine::PossessedBy(AController* NewController)
{
	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (PlayerController)
	{
		//====================================================================================
		// [ 2019-8-26 : magedoga ] TODO (When Changed HMD Display Enable State)
		USceneComponent* CameraParentComponent = Cast<USceneComponent>(GetDefaultSubobjectByName(TEXT("CameraParent")));
		if (CameraParentComponent != nullptr)
		{
			CameraParentComponent->SetUsingAbsoluteRotation(true);
#if UE_EDITOR
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() == false && GetWorld()->WorldType == EWorldType::PIE)
			{
				CameraParentComponent->SetUsingAbsoluteRotation(false);
			}
#endif // UE_EDITOR
		}
		//====================================================================================



		//====================================================================================
		// [ 2019-8-26 : magedoga ] Set Rotation Mode
		int32 RotationMode = WRConsoleManager::Get()->GetInt(EWRConsole::Character_RotationMode);
		this->SetRotationMode((EWRCharacterRotationType)RotationMode);
		//====================================================================================
	}
	else
	{
// 		AIController = Cast<AAIController>(NewController);
// 		if (AIController)
// 		{
// 			//AIController->GetPathFollowingComponent()->OnRequestFinished
// 			AIController->ReceiveMoveCompleted.AddDynamic(this, &AWRCharacterMine::OnMoveCompleted);
// 		}
	}

	Super::PossessedBy(NewController);
}

void AWRCharacterMine::UnPossessed()
{
	// [ 2019-8-26 : magedoga ] Delete Character Rotator
	if (CharacterRotator != nullptr)
	{
		CharacterRotator->OnLeaveThis();
		delete CharacterRotator;
		CharacterRotator = nullptr;
	}

	Super::UnPossessed();
}

/*
1. 대상이 npc
2. 대상이 플레이어

OverlappedComp : 때릴때 부딪힌 내 컴포넌트
Other : 내가 공격한 대상
OtherComp : 내가 공격한 대상의 부위

//플레이어에 대한 공격은 맞은 대상이 처리해야 하므로 여기서 처리 하지 않음

*/
void AWRCharacterMine::OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlapCapsuleAttacking(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (this == Other)
		return;

	UWRCapsuleCollisionComponent* HitComponent = Cast<UWRCapsuleCollisionComponent>(OtherComp);
	if (nullptr == HitComponent)
		return;

	if (HitComponent->HitCollisionType < EWRCollisionType::Head ||
		EWRCollisionType::MAX <= HitComponent->HitCollisionType)
	{
		return;
	}

	WRCombatInfo* CombatInfo = nullptr;
	AWRVFX* VFX = Cast<AWRVFX>(OverlappedComp->GetOwner());
	if (VFX && Other->GetRootComponent()) //충돌한게 투사체일때
	{
		ECollisionChannel eChannel = Other->GetRootComponent()->GetCollisionObjectType();
		switch (eChannel)
		{
		case ECollisionChannel::ECC_WorldStatic:
		case ECollisionChannel::ECC_WorldDynamic:
			//case ECollisionChannel::ECC_Pawn:
		case ECollisionChannel::ECC_PhysicsBody:
		case ECollisionChannel::ECC_Vehicle:
		case ECollisionChannel::ECC_Destructible:
		{
			//내가 발사한 투사체가 지형에 부딪혔을때
			//서버로 위치 전송하고 리턴
			IMMessageSender::ReqestProjectileHitResult(this, VFX->GetActorLocation(), VFX->GetVFXCombatSeqID());

			return;
		}
		break;
		default:
			break;
		}

		CombatInfo = WRCombatManager::Get()->FindBySeqID(GetActorID(), VFX->GetVFXCombatSeqID());
	}
	else
	{
		CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	}

	AWRCharacter* OtherCharacter = Cast<AWRCharacter>(Other);
	if (nullptr == OtherCharacter)
		return;

	//팩션 체크
	EWRFactionRelation FactionRelation = GetRelationToTarget(OtherCharacter);
	if (FactionRelation != EWRFactionRelation::Hostile)
		return;

	//플레이어에 대한 공격은 맞은 대상이 처리해야 하므로 여기서 처리 하지 않음
	if (OtherCharacter->IsPlayer())
		return;

	if (CombatInfo)
	{
		CombatInfo->DetermineHit(OtherCharacter->GetActorID(), HitComponent);
	}
}

/*
1. 엔피씨가 때릴때
2. 플레이어가 때릴때

OverlappedComp : 내가 맞은 부위
Other : 나를 공격한 대상
OtherComp : 나를 공격한 대상의 부위
*/
void AWRCharacterMine::OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlapCapsuleHitting(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (this == Other)
		return;

	if (IsAlive() == false)
		return;

	AWRCharacter* OtherCharacter = nullptr;
	WRCombatInfo* CombatInfo = nullptr;
	AWRVFX* VFX = Cast<AWRVFX>(OtherComp->GetOwner());
	if (VFX)	//원거리 공격인지 체크
	{
		if (this == VFX->GetSpawner())
			return;

		OtherCharacter = Cast<AWRCharacter>(VFX->GetSpawner());
		if (nullptr == OtherCharacter)
			return;

		CombatInfo = WRCombatManager::Get()->FindBySeqID(OtherCharacter->GetActorID(), VFX->GetVFXCombatSeqID());
	}
	else
	{
		OtherCharacter = Cast<AWRCharacter>(Other);
		if (nullptr == OtherCharacter)
		{
			OtherCharacter = Cast<AWRCharacter>(Other->GetOwner());
			if (nullptr == OtherCharacter)
				return;

			if (this == OtherCharacter)
				return;
		}

		//팩션 체크
		EWRFactionRelation FactionRelation = GetRelationToTarget(OtherCharacter);
		if (FactionRelation != EWRFactionRelation::Hostile)
			return;

		CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(OtherCharacter->GetActorID());
	}

	if (CombatInfo)
	{
		UWRCapsuleCollisionComponent* HitComponent = Cast<UWRCapsuleCollisionComponent>(OverlappedComp);
		CombatInfo->DetermineHit(GetActorID(), HitComponent);
	}
}

void AWRCharacterMine::BeginFSMAttack(const WRFSMInfo& InFSM)
{
	Super::BeginFSMAttack(InFSM);

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	if (CombatInfo)
	{
		CombatInfo->SetReceivedServerSeqIDFlag(false);
	}

	UWRActorComponentAttack* AttackComponent = GetComponent<UWRActorComponentAttack>();
	if (AttackComponent)
	{
		AttackComponent->OnProcessExecuteSkill(InFSM.SkillTableID);
	}

	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InFSM.SkillTableID, GetSkillTableFileName());
	if (SkillTable == nullptr)
	{
		return;
	}

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::Dash:
	{
		if (WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable))
		{
			this->DashOut();
		}
		else
		{
			float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::Dash_Duration);
			WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::Dash, Duration);

		}
	}
	break;
	case EWRSkillType::ChainProjectile:
	{
	}
	break;
	case EWRSkillType::Defense:
	{
		LastHeightType = WRInputSystemManager::Get()->GetAttackHeightType();
	}
	break;
	case EWRSkillType::Hook:
	{
		AttackComponent->SetCoolTime(InFSM.OwnerCharacter, InFSM.SkillTableID);
	}
	break;
	default:
		break;
	}
}

void AWRCharacterMine::TickFSMAttack(float InDeltaTime)
{
	Super::TickFSMAttack(InDeltaTime);

	switch (GetCurrentFSMType())
	{
	case EWRFSM::Defense:
	{
		if (LastHeightType != WRInputSystemManager::Get()->GetAttackHeightType())
		{
			WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
			if (CombatInfo)
			{
				UseSkillParam UseParam;
				UseParam.Caster = this;
				UseParam.SetSkillTID(CombatInfo->GetCombatSeqParam().SkillTID, GetSkillTableFileName());
				AWRCharacter* pTarget = WRCharacterManager::Get()->FindbyAID(GetTargetAID());
				if (pTarget)
				{
					UseParam.TargetList.Add(pTarget->GetActorID());
				}

				FString SectionName = FString::FromInt(CombatInfo->GetCombatSeqParam().SkillTID);
				SectionName += "_";
				SectionName += WREnumUtility::EnumToString("EWRAttackHeightType", UseParam.AttackHeightType);
				PlayAnimJumpToSection(GetCurrentFSMType(), SectionName);

				IMMessageSender::RequestUseSkillBegin(UseParam);
			}


			LastHeightType = WRInputSystemManager::Get()->GetAttackHeightType();
		}
	}
	break;
	default:
		break;
	}
}

void AWRCharacterMine::EndFSMAttack(const WRFSMInfo& InFSM)
{
	UWRActorComponentAttack* AttackComponent = GetComponent<UWRActorComponentAttack>();
	if (AttackComponent)
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
		if (CombatInfo)
		{
			FWRCombatParam CombatParam = CombatInfo->GetCombatSeqParam();
			AttackComponent->OnProcessNormalAttackInitialization(CombatParam.SkillTID);
			FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(CombatParam.SkillTID, GetSkillTableFileName());
			if (SkillTable == nullptr)
			{
				return;
			}

			switch (SkillTable->SkillType)
			{
			case EWRSkillType::Hook:
			{

			}
			break;
			default:
			{
				AttackComponent->SetCoolTime(InFSM.OwnerCharacter, CombatParam.SkillTID);
			}
			break;
			}
		}
	}

	LastHeightType = EWRAttackHeightType::None;

	Super::EndFSMAttack(InFSM);
}

void AWRCharacterMine::BeginFSMDie(const WRFSMInfo& InFSM)
{
	Super::BeginFSMDie(InFSM);
}

void AWRCharacterMine::EndFSMDie(const WRFSMInfo& InFSM)
{
	Super::EndFSMDie(InFSM);

	bReqBeginFallPacket = false;
}

UWRAnimInstance* AWRCharacterMine::GetAnimInstance()
{
	if (IsShowing3rdCharacter())
	{
		if (nullptr == ThirdPersonSkeletalMeshComponent)
			return nullptr;

		return Cast<UWRAnimInstance>(ThirdPersonSkeletalMeshComponent->GetAnimInstance());
	}
	else
	{
		if (GetMesh() == nullptr)
			return nullptr;

		return Cast<UWRAnimInstance>(GetMesh()->GetAnimInstance());
	}
}

void AWRCharacterMine::Falling()
{
	UWRActorComponentDynamicMovement* DynamicMove = GetComponent<UWRActorComponentDynamicMovement>();
	if (DynamicMove && DynamicMove->IsValidDynamicMovement())
		return;

	Super::Falling();

	DetermineFallPacket();
}

void AWRCharacterMine::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bReqBeginFallPacket)
	{
		IMMessageSender::RequestEndFall(this);
		bReqBeginFallPacket = false;
	}
}

void AWRCharacterMine::DetermineTerminateSomethingWhenChangeFSM(const WRFSMInfo& InFSMInfo)
{
	switch (InFSMInfo.FSMType)
	{
	case EWRFSM::Move:
	case EWRFSM::Idle:
	{
	}
	break;
	default:
	{
		WRInputSystemManager::Get()->CancelHMDPicking();
	}
	break;
	}
}

void AWRCharacterMine::BuildCustomComponents()
{
	// 부모인 AWRCharacterPlayer쪽에서 별도의 처리를 위해 일부러 Super 위에서 Create 해주었습니다.
	CreateComponent<UWRActorComponentDanceModeForMine>(EWRCharacterComponent::DanceMode, true);

	Super::BuildCustomComponents();

	CreateComponent<UWRActorComponentVR>(EWRCharacterComponent::VR, true);
	CreateComponent<UWRActorComponentStereoLayerManager>(EWRCharacterComponent::StereoLayerManager, true);


	if (AudioCaptureComponent->IsValidLowLevel() == false)
	{
		AudioCaptureComponent = NewObject<UAudioCaptureComponent>(this);
		AudioCaptureComponent->RegisterComponent();
		this->AddInstanceComponent(AudioCaptureComponent);
		this->AddOwnedComponent(AudioCaptureComponent);

#if USE_AUDIOMIXER_BP_LIBRARY
		SoundSubmix = NewObject<USoundSubmix>(this);
#else
		USoundSubmix* SoundSubmix = NewObject<USoundSubmix>(this);
#endif // USE_AUDIOMIXER_BP_LIBRARY
		if (SoundSubmix != nullptr)
		{
			FAudioDeviceManager* AudioDeviceManager = (GEngine ? GEngine->GetAudioDeviceManager() : nullptr);
			if (AudioDeviceManager != nullptr)
			{
				AudioDeviceManager->RegisterSoundSubmix(SoundSubmix);;
			}
			AudioCaptureComponent->SoundSubmix = SoundSubmix;
		}
	}

	if (SphereFlippedNormalComponent == nullptr)
	{
		SphereFlippedNormalComponent = NewObject<UStaticMeshComponent>(this);
		if (SphereFlippedNormalComponent == nullptr)
		{
			return;
		}

		SphereFlippedNormalComponent->AddToRoot();
		SphereFlippedNormalComponent->RegisterComponent();
		this->AddInstanceComponent(SphereFlippedNormalComponent);
		this->AddOwnedComponent(SphereFlippedNormalComponent);

		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/Asset/Character/UE4_Mannequin/Mesh/SM_Sphere_FlippedNormals.SM_Sphere_FlippedNormals"));
		if (StaticMesh == nullptr)
		{
			return;
		}

		SphereFlippedNormalComponent->SetStaticMesh(StaticMesh);

		UCameraComponent* CameraComponent = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
		if (CameraComponent == nullptr)
		{
			return;
		}

		SphereFlippedNormalComponent->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SphereFlippedNormalComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereFlippedNormalComponent->SetVisibility(false);
		//SphereFlippedNormalComponent->AddRelativeRotation(FRotator(0.0f, -80.0f, 0.0f));
	}

	CreateComponent<UWRActorComponentRevive>(EWRCharacterComponent::Revive);
	CreateComponent<UWRActorComponent360Movie>(EWRCharacterComponent::Play360Movie);
	CreateComponent<UWRActorComponentChatRecording>(EWRCharacterComponent::ChatRecording);
}

void AWRCharacterMine::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AWRCharacterMine::MoveForward);		// [ 2019-8-21 : kyu ] W, S
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AWRCharacterMine::MoveRight);			// [ 2019-8-21 : kyu ] A, D
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AWRCharacterMine::TurnRight);			// [ 2019-8-21 : kyu ] <-, ->
	PlayerInputComponent->BindAxis(TEXT("PitchKey"), this, &AWRCharacterMine::PitchKey);			// [ 2019-8-21 : kyu ] ↑, ↓
	PlayerInputComponent->BindAxis(TEXT("PitchPad"), this, &AWRCharacterMine::PitchPad);			// [ 2019-8-21 : kyu ] R3 위, 아래

	WRInputSystemManager::Get()->SetUpInputBind(PlayerInputComponent);
}

void AWRCharacterMine::ChangeWeaponWithoutAnimation(const EWRChangeEquipment IN InType)
{
	WREquipItemBase* pWeaponData = nullptr;

	WRTableID EquipmentLookTID = INVALID_TABLE_TID, CharacterWeaponTID = INVALID_TABLE_TID;
	switch (InType)
	{
	case EWRChangeEquipment::MainWeapon:
	{
		pWeaponData = WRItemManager::Get()->FindEquippedMainWeapon();
		SetEquipPartVisibility(EWREquipPart::RightHand, true);

		if (pWeaponData == nullptr)
			return;

		FWRWeaponItemTable* pItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, pWeaponData->GetTID());
		if (pItemTable == nullptr)
			return;

		EquipmentLookTID = pItemTable->EquipmentLookTID;
		CharacterWeaponTID = pItemTable->CharWeaponTID;
	}
	break;

	case EWRChangeEquipment::SubWeapon:
	{
		pWeaponData = WRItemManager::Get()->FindEquippedSubWeapon();
		SetEquipPartVisibility(EWREquipPart::LeftHand, true);

		if (pWeaponData == nullptr)
			return;

		FWRWeaponItemTable* pItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, pWeaponData->GetTID());
		if (pItemTable == nullptr)
			return;

		EquipmentLookTID = pItemTable->EquipmentLookTID;
		CharacterWeaponTID = pItemTable->CharWeaponTID;
	}
	break;

	case EWRChangeEquipment::Temporary:
	{
		pWeaponData = WRItemManager::Get()->FindEquippedTemporaryWeapon();
		SetEquipPartVisibility(EWREquipPart::RightHand, true);

		if (pWeaponData == nullptr)
			return;

		FWRTemporaryItemTable* pItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, pWeaponData->GetTID());
		if (pItemTable == nullptr)
			return;

		EquipmentLookTID = pItemTable->EquipmentLookTID;
		CharacterWeaponTID = pItemTable->CharWeaponTID;
	}
	break;

	default:
		return;
	}

	// [ 2019-11-14 : kyu ] 변경된 데이터 기반으로 외형을 다시 설정한다
	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent)
	{
		if (EquipmentLookTID != INVALID_TABLE_TID)
		{
			FWREquipmentLookTable* FoundEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(EquipmentLookTID);
			if (FoundEquipLookTable != nullptr)
			{
				FString AttachSocketName = FoundEquipLookTable->AttachSocket.ToString();
				switch (InType)
				{
				case EWRChangeEquipment::MainWeapon:
				{
					AttachSocketName += "_R";
				}
				break;
				case EWRChangeEquipment::SubWeapon:
				{
					AttachSocketName += "_L";
				}
				break;
				case EWRChangeEquipment::Temporary:
				{
					AttachSocketName += "_R";
				}
				break;
				}

				EquipmentLookComponent->Equip(EquipmentLookTID, *AttachSocketName);
			}
		}		
	}

	UWRActorComponentAttack* AttackComponent = GetComponent<UWRActorComponentAttack>();
	if (AttackComponent)
	{
		AttackComponent->GenerateSkillInfo(CharacterWeaponTID);
	}

	// [ 2019-12-17 : kyu ] 장착 장비에 따른 처리 추가
	switch (InType)
	{
	case EWRChangeEquipment::MainWeapon:
	{
		FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(EWRUIPanelKind::CrossHair);
		if (CombinedPanelData)
		{
			if (pWeaponData->GetSubType() == EWRItemSubType::Hook)
			{
				UWRUIPanel_CrossHair* Panel_CrossHair = Cast<UWRUIPanel_CrossHair>(CombinedPanelData->UIPanel);
				if (Panel_CrossHair)
					Panel_CrossHair->ShowHookCrossHair();
			}
			else
			{
				UWRUIPanel_CrossHair* Panel_CrossHair = Cast<UWRUIPanel_CrossHair>(CombinedPanelData->UIPanel);
				if (Panel_CrossHair)
					Panel_CrossHair->HideHookCrossHair();
			}
		}
	}
	break;
	}
}

bool AWRCharacterMine::AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType /*= EWRActionType::en::RUN*/)
{
	if (Super::AIMoveTo(InDestLocation, InMoveType))
	{
// 		if (CharacterRotator != nullptr)
// 		{
// 			CharacterRotator->SetSyncRotationByHMD_Once();
// 		}

		WRInputSystemManager::Get()->CreateHMDPickingVFX(InDestLocation, false);

		return true;
	}

	return false;
}

void AWRCharacterMine::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult)
{
	Super::OnMoveCompleted(RequestID, MovementResult);

	switch (MovementResult)
	{
	case EPathFollowingResult::Success:
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::Aborted:
	{
		IMMessageSender::RequestPlayerMoveStop(this);

		WRInputSystemManager::Get()->RemoveHMDPickingVFX();
	}
	break;
	case EPathFollowingResult::OffPath:
	default:
	{

	}
	break;
	}
}

void AWRCharacterMine::SetRotationMode(EWRCharacterRotationType InType)
{
	WRPlayerCharacterRotator* NewRotator = WRPlayerCharacterRotator::CreateRotator(InType, this);
	if (NewRotator == nullptr)
		return;

	if (CharacterRotator != nullptr)
	{
		CharacterRotator->OnLeaveThis();
		delete CharacterRotator;
	}

	CharacterRotator = NewRotator;
	CharacterRotator->OnEnterThis();
}

void AWRCharacterMine::Show3rdCharacter(const bool bInShow, FVector InCameraOffset /*= FVector::ZeroVector*/)
{
	if (bShow3rdCharacter == bInShow)
		return;

	bShow3rdCharacter = bInShow;

	// [ 2020-1-7 : magedoga ] 1st Skeletal Mesh Component
	this->SetEquipPartVisibility(EWREquipPart::Body, !bShow3rdCharacter);

	// [ 2020-1-7 : magedoga ] 3rd Skeletal Mesh Component
	ThirdPersonSkeletalMeshComponent->SetVisibility(bShow3rdCharacter);

	// [ 2020-1-7 : magedoga ] 무조건 EquipmentComponent에 통보해줘야한다.
	// [ 2020-1-7 : magedoga ] 장착돼있는 아이템을 옮겨야 하기 때문에.
	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
	EquipmentLookComponent->UpdateEquipment(this);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	EquipmentLookComponent->Update_AttachItemToSuitableMesh(this);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

	auto SetCameraParentOffset = [&](const FVector& InOffset, const bool bInAdd)
	{
		TArray<UActorComponent*> ActorComponents; GetComponents(USceneComponent::StaticClass(), ActorComponents);
		for (int32 i = 0; i < ActorComponents.Num(); ++i)
		{
			if (ActorComponents[i]->GetName().Contains("CameraParent") == false)
				continue;

			USceneComponent* CameraParent = Cast<USceneComponent>(ActorComponents[i]);
			FTransform RelativeTR = CameraParent->GetRelativeTransform();
			FVector CalculatedLocation = bInAdd == true ? RelativeTR.GetTranslation() + InOffset : RelativeTR.GetTranslation() - InOffset;
			CameraParent->SetRelativeLocation(CalculatedLocation);
		}
	};

	// [ 2020-1-20 : magedoga ] 카메라 Offset
// 	float OffsetX = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetX);
// 	float OffsetZ = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_3rdCharacterCameraOffsetZ);
// 	FVector OffsetXZ = (FVector::ForwardVector * OffsetX) + (FVector::UpVector * OffsetZ);

	if (bShow3rdCharacter == true)
	{
		SetCameraParentOffset(InCameraOffset, true);
		CameraOffset += InCameraOffset;
		GetCurrentSkeletalMeshComponent()->BoundsScale = 1.5f;	//기본은1이지만...혹시몰라서...또 애니 안할까봐...
	}
	else
	{
		SetCameraParentOffset(InCameraOffset, false);
		CameraOffset -= InCameraOffset;
		GetCurrentSkeletalMeshComponent()->BoundsScale = 10.0f;
	}

	//FSM정보 이전
	TransplantFSMWhenChangeViewpoint(bInShow);
}

void AWRCharacterMine::TransplantFSMWhenChangeViewpoint(bool bInShow)
{
	if (EWRFSM::Idle == GetCurrentFSMType())
		return;

	if ((nullptr == ThirdPersonSkeletalMeshComponent) || (GetMesh() == nullptr))
		return;

	UWRAnimInstance* CurrentAnimInstance = nullptr;
	UWRAnimInstance* NextAnimInstance = nullptr;
	if (bInShow)
	{
		CurrentAnimInstance = Cast<UWRAnimInstance>(GetMesh()->GetAnimInstance());
		NextAnimInstance = Cast<UWRAnimInstance>(ThirdPersonSkeletalMeshComponent->GetAnimInstance());
	}
	else
	{
		CurrentAnimInstance = Cast<UWRAnimInstance>(ThirdPersonSkeletalMeshComponent->GetAnimInstance());
		NextAnimInstance = Cast<UWRAnimInstance>(GetMesh()->GetAnimInstance());
	}

	if (nullptr == CurrentAnimInstance || nullptr == NextAnimInstance)
	{
		return;
	}

	//재생할 몽타주 정보 생성
	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = GetCurrentFSMType();
	FSMInfo.OwnerCharacter = this;
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	if (CombatInfo)
	{
		FSMInfo.SkillTableID = CombatInfo->GetCombatSeqParam().SkillTID;
	}
	FWRMontageInfo MontageInfo;
	WRFSMBase::GetMontageInfoForPlaying(FSMInfo, MontageInfo);

	//새로운 몽타주 재생
	FAnimMontageInstance* CurrentMontageInstance = CurrentAnimInstance->GetActiveMontageInstance();
	if (CurrentMontageInstance)
	{
		//현재 재생중인 몽타주 재생정보
		FString CurrentSectionName = CurrentMontageInstance->GetCurrentSection().ToString();
		float CurrentMontagePosition = CurrentMontageInstance->GetPosition();

		//새로운 메쉬로 몽타주 재생
		NextAnimInstance->PlayMontage(MontageInfo.MontageID, CurrentSectionName);
		FAnimMontageInstance* NextMontageInstance = NextAnimInstance->GetActiveMontageInstance();
		if (NextMontageInstance)
		{
			//재생이 성공하면 이전 재생포인트로 이동
			NextMontageInstance->SetPosition(CurrentMontagePosition);
		}

		//이전 mesh에서 사용하던 몽타주는 재생정지
		if (CurrentMontageInstance->Montage)
			CurrentMontageInstance->Stop(CurrentMontageInstance->Montage->BlendOut);
	}

}

bool AWRCharacterMine::Set3rdSkeletalMesh(USkeletalMesh* InSkeletalMesh)
{
	if (InSkeletalMesh->IsValidLowLevel() == false)
		return false;

	checkf(ThirdPersonSkeletalMeshComponent != nullptr, TEXT("AWRCharacterMine::Set3rdSkeletalMesh - null  3rdSkeletalMeshComponent"));
	ThirdPersonSkeletalMeshComponent->EmptyOverrideMaterials();
	ThirdPersonSkeletalMeshComponent->SetSkeletalMesh(InSkeletalMesh, false);

	OnAnimInitializedDele4Me();

	return true;
}

USkeletalMeshComponent* AWRCharacterMine::GetCurrentSkeletalMeshComponent() const
{
	return this->IsShowing3rdCharacter() == false ? GetMesh() : ThirdPersonSkeletalMeshComponent;
}

void AWRCharacterMine::UseAidaHeal()
{
	AWRCharacter* Character = WRCharacterManager::Get()->FindByTID(DEF_AIDA_TABLIE_ID);
	if (Character == nullptr)
		return;

	UseSkillParam UseParam;
	UseParam.Caster = Character;
	UseParam.SetSkillTID(DEF_AIDA_HEAL_SKILL_TABLE_ID, GetSkillTableFileName());
	UseParam.TargetList.Add(GetActorID());

	IMMessageSender::RequestUseSkill(UseParam);
}

void AWRCharacterMine::DetermineFallPacket()
{
	//dynamicMove가 끝나는 경우에도 호출되는 경우가 있으므로 현재 좌표에서 바닥까지의 높이를 한번 체크 하고 패킷처리 함
	FVector Location = GetActorLocation();
	WRActorUtility::ModifyZHeightForServer(this, GetActorLocation(), Location.Z);
	if (WRActorUtility::IsOnNavigation(GetWorld(), Location))
	{
		FVector NavLocation = WRActorUtility::ProjectPointToNavigation(this);
		float ActorHeightFromNav = UKismetMathLibrary::Abs(NavLocation.Z - GetActorLocation().Z);
		float CapsuleHalfHeight = WRActorUtility::GetActorCapsuleHalfHeight(this);

		float CapsuleBottomZFromNav = UKismetMathLibrary::Abs(ActorHeightFromNav - CapsuleHalfHeight);
		if (CapsuleBottomZFromNav < 200.0f)
		{
			return;
		}
	}

	IMMessageSender::RequestBeginFall(this);
	bReqBeginFallPacket = true;
}

void AWRCharacterMine::SetWhiteOut(const FVector InLocation, const FRotator InRotation)
{
	Super::SetWhiteOut(InLocation, InRotation);

	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::WhiteOutIn_Duration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::WhiteOut, Duration);

	WarpLocation = InLocation;
	WarpRatation = InRotation;

	// [ 2019-11-22 : animal14 ] 이벤트 바인딩
	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::WhiteOut).AddUObject(this, &AWRCharacterMine::SetWhiteIn);
}

void AWRCharacterMine::SetWhiteIn()
{
	Super::SetWhiteIn();

	// [ 2019-11-22 : animal14 ] 이벤트 언바인딩
	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::WhiteOut).RemoveAll(this);

	this->SetActorLocationAndRotation(WarpLocation, WarpRatation);
	WRInputSystemManager::Get()->ResetHMDOrientation();

	// [ 2019-11-8 : animal14 ] 
	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::WhiteOutIn_Duration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::WhiteIn, Duration);

	SetUsingWarp(false);
}

//====================================================================================
// [ 2020-3-26 : magedoga ] Sync HMD

void AWRCharacterMine::SetEnableSyncHMDTick(const bool bInEnable)
{
	// [ 2020-3-26 : magedoga ] On
	if (bInEnable)
	{
		//현재 hmd 동기화
		IMMessageSender::RequestSyncHMD(this);

		//다음틱 계산을 위해 싱크된 회전값 저장
		LastSyncHMDRotation = WRInputSystemManager::Get()->GetHMDRotation();

		//특정 범위 이상을 움직이지 않더라도 주기적으로 hmd위치를 동기화 해줌
		SyncTimerDelegate = FTimerDelegate::CreateUObject(this, &AWRCharacterMine::CallbackSyncHMD);
		WRActorUtility::SetTimer(SyncHMDTimer, SyncTimerDelegate, 2.0f, true);

		//Tick 시작
		TickSyncHMDHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AWRCharacterMine::TickSyncHMD), 0.1f);
	}
	else
	{
		FTicker::GetCoreTicker().RemoveTicker(TickSyncHMDHandle);
		TickSyncHMDHandle.Reset();

		WRActorUtility::ClearTimer(SyncHMDTimer);
		SyncTimerDelegate.Unbind();

		LastSyncHMDRotation = FRotator::ZeroRotator;
	}
}

void AWRCharacterMine::CallbackSyncHMD()
{
	if (LastSyncHMDRotation.Equals(HMDRotationToUpdate) == false)
	{
		IMMessageSender::RequestSyncHMD(this);
	}
}

bool AWRCharacterMine::TickSyncHMD(float InDeltaTime)
{
	if (IsUsingSyncHMDHead() == false)
		return false;

	//현재 hmd 저장. 이건 내 케릭터 세팅을 위해서 하는거임
	HMDRotationToUpdate = WRInputSystemManager::Get()->GetHMDRotation();

	//이하 다른 사람과의 동기화를 위해
	//이전 싱크 위치와 현재 위치가 어느정도 차이가 나면 동기화
	bool bReqSyncHMDPkt = false;
	float PitchGap = HMDRotationToUpdate.Pitch - LastSyncHMDRotation.Pitch;
	float YawGap = HMDRotationToUpdate.Yaw - LastSyncHMDRotation.Yaw;

	float BaseDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::Character_SyncHMDDist);
	if (BaseDistance < UKismetMathLibrary::Abs(PitchGap) || BaseDistance < UKismetMathLibrary::Abs(YawGap))
	{
		bReqSyncHMDPkt = true;
	}

	if (LastHeightType != WRInputSystemManager::Get()->GetAttackHeightType())
	{
		//defense 상태에서는 TickFSMAttack 에서 처리 한다
		if (GetCurrentFSMType() != EWRFSM::Defense)
		{
			LastHeightType = WRInputSystemManager::Get()->GetAttackHeightType();
		}
		bReqSyncHMDPkt = true;
	}

	if (bReqSyncHMDPkt)
	{
		//동기화 시킬 회전 저장
		LastSyncHMDRotation = WRInputSystemManager::Get()->GetHMDRotation();

		//서버와 동기화
		IMMessageSender::RequestSyncHMD(this);

		//동기화 하면 다음 입력이 없을때를 위한 시간에 따른 동기화 재시작
		if (SyncTimerDelegate.IsBound() == false)
		{
			SyncTimerDelegate = FTimerDelegate::CreateUObject(this, &AWRCharacterMine::CallbackSyncHMD);
		}
		WRActorUtility::SetTimer(SyncHMDTimer, SyncTimerDelegate, 2.0f, true);
	}

	return true;
}
//====================================================================================

void AWRCharacterMine::OnMediaOpened(FString OpenedURL)
{
	WRMediaPlayerManager::Get()->OnMediaOpened(OpenedURL);
}

void AWRCharacterMine::TeleportUsingVoiceRecognition(const WRTableID IN InCoordinateTID)
{
	SetUsingWarp(true);
	IMMessageSender::RequestEndVoiceCommand();
	IMMessageSender::RequestPlayerWarp(InCoordinateTID);

}

void AWRCharacterMine::CallAida()
{
	AWRCharacter* Character = WRCharacterManager::Get()->FindByTID(DEF_AIDA_TABLIE_ID);
	if (Character == nullptr)
		return;

	IMMessageSender::RequestCallADA(Character->GetActorID(), GetActorLocation());
}

void AWRCharacterMine::SetUsingWarp(const bool IN bInUsingWarp)
{
	bUsingWarp = bInUsingWarp;
	if (bUsingWarp && bGetOnEnvVehicle)
	{
		WREnvInteractionManager::Get()->Send_GetOffEnvVehicle(this);
	}
}

void AWRCharacterMine::OnBeginOverlap_EnvRecognitionArea(class AWREnvVehicleBase* IN InEnvVehicleBase)
{
	bCheckEnvGetOnOff = true;
}

void AWRCharacterMine::OnEndOverlap_EnvRecognitionArea(class AWREnvVehicleBase* IN InEnvVehicleBase)
{
	bCheckEnvGetOnOff = false;
}

void AWRCharacterMine::UpdateCheckEnvGetOnOff()
{
	// 탑승물의 인지영역에 들어갔을 때만 동작함
	if (bCheckEnvGetOnOff == false)
		return;

	if (bUsingWarp)
		return;

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + FVector(0, 0, -100.f);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FHitResult Hit(1.0f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_EnvGetOnOffTrace, CollisionParams) == true)
	{
		AWREnvVehicleBase* CastedEnvVehicle = Cast<AWREnvVehicleBase>(Hit.GetComponent()->GetOwner());
		if (CastedEnvVehicle == nullptr)
			return;

		if (CastedEnvVehicle->GetAID() == GetEnvVehicleAID())
			return;

		bGetOnEnvVehicle = true;
		// GetOn
		WREnvInteractionManager::Get()->Send_GetOnEnvVehicle(this, CastedEnvVehicle);


		//		UKismetSystemLibrary::DrawDebugPoint(this, Hit.ImpactPoint, 10.f, FLinearColor::Green, 2.f);
	}
	else
	{
		if (bGetOnEnvVehicle)
		{
			bGetOnEnvVehicle = false;
			// GetOff
			WREnvInteractionManager::Get()->Send_GetOffEnvVehicle(this);
		}
	}
}

bool AWRCharacterMine::IsEquippedBottle()
{
	if (GetTemporaryItemTID() == INVALID_TABLE_TID)
		return false;

	FWRTemporaryItemTable* pItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, GetTemporaryItemTID());
	if (pItemTable == nullptr)
		return false;

	if (pItemTable->MainType == EWRItemMainType::Temporary && pItemTable->SubType == EWRItemSubType::Drink)
		return true;

	return false;
}

void AWRCharacterMine::DashOut()
{
	//if (this->MoveForwardValue == 0.0F && this->MoveRightValue == 0.0F)
	//{
	//}
	//else
	//{
	//	return;
	//}

	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::Dash_Duration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::DashOut, Duration);

	// [ 2020-2-21 : animal14 ] 이벤트 바인딩
	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::DashOut).AddUObject(this, &AWRCharacterMine::DashIn);
}

void AWRCharacterMine::DashIn()
{
	if (this->IsMe() == false)
	{
		return;
	}

	// [ 2019-11-22 : animal14 ] 이벤트 언바인딩
	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::DashOut).RemoveAll(this);

	//if (this->MoveForwardValue == 0.0F && this->MoveRightValue == 0.0F)
	//{
	//}
	//else
	//{
	//	return;
	//}

	// [ 2019-11-8 : animal14 ] 
	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::Dash_Duration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::DashIn, Duration);
}

bool AWRCharacterMine::IsDanceModeOn()
{
	UWRActorComponentDanceMode* DanceModeComponent = GetComponent<UWRActorComponentDanceMode>();
	if (DanceModeComponent == nullptr)
		return false;

	return DanceModeComponent->IsDanceModeOn();
}

bool AWRCharacterMine::Play360Movie(class UWRPlay360VideoAsset* InAsset, bool b3DUse/* = false*/)
{
	UWRActorComponent360Movie* Play360MovieComponent = GetComponent<UWRActorComponent360Movie>();
	if (Play360MovieComponent->IsValidLowLevel() == false)
	{
		UE_LOG(WRCharacterMine, Display, TEXT("AWRCharacterMine::Play360Movie - Null 360MovieComponent."));
		return false;
	}

	WRMaterialParameterCollectionManager::Get()->SetScalarParameterValue(EWRMPC::Movie, "360_3DMovieUse", b3DUse);
	return Play360MovieComponent->Play(InAsset, FWR360MovieDelegate::CreateUObject(this, &AWRCharacterMine::OnChange360MovieState));
}

void AWRCharacterMine::Stop360Movie()
{
	UWRActorComponent360Movie* Play360MovieComponent = GetComponent<UWRActorComponent360Movie>();
	if (Play360MovieComponent->IsValidLowLevel() == false)
	{
		UE_LOG(WRCharacterMine, Display, TEXT("AWRCharacterMine::Stop360Movie - Null 360MovieComponent."));
		return;
	}

	Play360MovieComponent->Stop();
}


void AWRCharacterMine::OnChange360MovieState(EWR360MovieEvent InState)
{
	//bool bShow = InState == EWR360MovieEvent::EndPlay;
	bool bPlaying = InState == EWR360MovieEvent::StartPlay;

	// [ 2020-3-26 : magedoga ] Hide my body part. if, Started 360 movie.
	UWRActorComponentEquipmentLook* EquipmentLookComponent = this->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent->IsValidLowLevel() == true)
	{
		EquipmentLookComponent->SetVisibility(EWREquipPart::Body, !bPlaying);
	}

	static bool bEnableBlackOutInt;
	IConsoleVariable* ConsoleVariable = WRConsoleManager::Get()->GetConsoleVariable(EWRConsole::BlackOutIn_Enable);
	if (ConsoleVariable != nullptr)
	{
		if (bPlaying == true)
		{
			bEnableBlackOutInt = ConsoleVariable->GetBool();
			ConsoleVariable->Set(false);
		}
		else
		{
			ConsoleVariable->Set(bEnableBlackOutInt);
		}

	}




	// [ 2020-3-26 : magedoga ] Show/Hide UI.
	if (bPlaying == false)
		WRUIManager::Get()->ShowAllWidgetComponents();
	else
		WRUIManager::Get()->HideAllWidgetComponents();


	// [ 2020-4-8 : magedoga ] Lock Move
//	WRInputSystemManager::Get()->SetMoveLock(bPlaying);
}

void AWRCharacterMine::InitCameraOffsetByPlayerTable()
{
	UWRActorComponentVR* VRComponent = this->GetComponent<UWRActorComponentVR>();
	if (VRComponent->IsValidLowLevel() == true)
	{
		VRComponent->InitCameraOffsetByPlayerTable();
	}
}

void AWRCharacterMine::RequestMove()
{
	switch (this->GetMoveState())
	{
	case EWRMoveStateType::OnFoot: IMMessageSender::RequestPlayerMove(this);
		break;
	case EWRMoveStateType::OnTransport:IMMessageSender::RequestPlayerMoveInTransportation(this);
		break;
	case EWRMoveStateType::OnMovingwalk:IMMessageSender::RequestPlayerMoveInMW(this);
		break;
	default:
		IMMessageSender::RequestPlayerMove(this);
		break;
	}
}

void AWRCharacterMine::RequestStop()
{
	switch (this->GetMoveState())
	{
	case EWRMoveStateType::OnFoot: IMMessageSender::RequestPlayerMoveStop(this);
		break;
	case EWRMoveStateType::OnTransport:IMMessageSender::RequestPlayerStopInTransportation(this);
		break;
	case EWRMoveStateType::OnMovingwalk:IMMessageSender::RequestPlayerStopInMW(this);
		break;
	default:
		IMMessageSender::RequestPlayerMoveStop(this);
		break;
	}
}

void AWRCharacterMine::MoveForward(float InScale)
{
	if (WRSpawnSimulationManager::Get()->GetSimulationState() != EWRSpawnSimulationState::None)
	{
		MoveForwardValue = 0.f;
		MoveForward4SpawnSimulation(InScale);
	}
	else
	{
		MoveForward4CharacterMovement(InScale);
	}
}

void AWRCharacterMine::MoveRight(float InScale)
{
	if (WRSpawnSimulationManager::Get()->GetSimulationState() != EWRSpawnSimulationState::None)
	{
		MoveRightValue = 0.f;
		MoveRightForSpawnSimulation(InScale);
	}
	else
	{
		MoveRightForCharacterMovement(InScale);
	}
}

bool AWRCharacterMine::IsMovable()
{
	//DM중이면 이동 안됨
	UWRActorComponentDynamicMovement* DynamicMoveComponent = GetComponent<UWRActorComponentDynamicMovement>();
	if (DynamicMoveComponent)
	{
		if (DynamicMoveComponent->IsValidDynamicMovement())
			return false;
	}

	//블랜드 불가능한 fsm상태일때
	if (GetCurrentFSM()->IsLayeredBlendFSM(this) == false)
	{
		UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
		if (FSMComponent)
		{
			//move가 우선순위에 있는 fsm마저 아니면
			if (FSMComponent->IsPossibleEndCurrentFSM(EWRFSM::Move) == false)
				return false;
		}
	}

	return true;
}

void AWRCharacterMine::MoveForward4SpawnSimulation(float InScale)
{
	if (InScale != 0)
	{
		WRSpawnSimulationManager::Get()->InputEvent(EWRInput::LStickX, EInputEvent::IE_Repeat, InScale);
	}
}

void AWRCharacterMine::MoveForward4CharacterMovement(float InScale)
{
	if (WRInputSystemManager::Get()->IsLocationLock())
	{
		MoveForwardValue = 0.f;
		return;
	}

	// [ 2019-12-3 : kyu ] 튜토리얼시 Movable 체크
	if (WRTutorialManager::Get()->IsTutorialMovableBlock())
	{
		MoveForwardValue = 0.f;
		return;
	}

	if (IsMovable() == false)
	{
		MoveForwardValue = 0.f;
		return;
	}

	//방향이 바뀌면 즉시 서버로 위치 동기화 요청
	float ReverseMove = MoveForwardValue * InScale;
	if (ReverseMove < 0)
	{
		RequestMove();
	}

	// [ 2019-12-10 : animal14 ] 해당 멤버 변수로 애니메이션 블렌드
	MoveForwardValue = InScale;
	if (0.0f == InScale)
		return;

	AddMovementInput(GetActorForwardVector(), InScale);

	// [ 2020-2-5 : kyu ] 이동시 기타 처리
	UpdateHookMoving();
	WRObjectInteractionManager::Get()->OnMoved();
}

void AWRCharacterMine::MoveRightForSpawnSimulation(float InScale)
{
	if (InScale != 0)
	{
		WRSpawnSimulationManager::Get()->InputEvent(EWRInput::LStickY, EInputEvent::IE_Repeat, InScale);
	}
}

void AWRCharacterMine::MoveRightForCharacterMovement(float InScale)
{
	if (WRInputSystemManager::Get()->IsLocationLock())
	{
		MoveRightValue = 0.f;
		return;
	}

	// [ 2019-12-3 : kyu ] 튜토리얼시 Movable 체크
	if (WRTutorialManager::Get()->IsTutorialMovableBlock())
	{
		MoveRightValue = 0.f;
		return;
	}

	if (IsMovable() == false)
	{
		MoveRightValue = 0.f;
		return;
	}

	//방향이 바뀌면 즉시 서버로 위치 동기화 요청
	float ReverseMove = MoveRightValue * InScale;
	if (ReverseMove < 0)
	{
		RequestMove();
	}

	// [ 2019-12-10 : animal14 ] 해당 멤버 변수로 애니메이션 블렌드
	MoveRightValue = InScale;
	if (0.0f == InScale)
		return;

	AddMovementInput(GetActorRightVector(), InScale);

	// [ 2020-2-5 : kyu ] 이동시 기타 처리
	UpdateHookMoving();
	WRObjectInteractionManager::Get()->OnMoved();
}

void AWRCharacterMine::PitchKey(float InScale)
{
#if WITH_EDITOR || PLATFORM_WINDOWS
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false /*&& GetWorld()->WorldType == EWorldType::PIE*/)
	{
		UCameraComponent* CameraComponent = Cast<UCameraComponent>(this->GetComponentByClass(UCameraComponent::StaticClass()));
		if (CameraComponent != nullptr)
		{
			float DeltaSeconds = this->GetWorld()->GetDeltaSeconds();
			float PitchAngle = WRConsoleManager::Get()->GetFloat(EWRConsole::Camera_MaxPitchAngleWhenHMDOff);
			float DeltaPitchPerSecond = WRConsoleManager::Get()->GetFloat(EWRConsole::Camera_DeltaPitchPerSecond);

			if (WRInputSystemManager::Get()->IsPressedLShift())
				DeltaPitchPerSecond *= 0.1f;

			FRotator Rotation = CameraComponent->GetComponentRotation();
			Rotation.Pitch += DeltaPitchPerSecond * InScale * DeltaSeconds;

			if (Rotation.Pitch >= -PitchAngle && Rotation.Pitch <= PitchAngle)
				CameraComponent->SetWorldRotation(Rotation);
		}
	}
#endif
}

void AWRCharacterMine::PitchPad(float InScale)
{
	// [ 2020-9-3 : magedoga ] Rotation 180 
	if (InScale < -0.95f && CharacterRotator->IsLockRotation180() == false)
	{
		CharacterRotator->Rotation180();
	}
	else if (InScale > -0.15f && CharacterRotator->IsLockRotation180() == true)
	{
		CharacterRotator->UnlockRotation180();
	}
}

void AWRCharacterMine::TurnRight(float InScale)
{
	if (WRSpawnSimulationManager::Get()->GetSimulationState() != EWRSpawnSimulationState::None)
	{
		TurnRight4SpawnSimulation(InScale);
	}
	else
	{
		TurnRight4CharacterRotation(InScale);
	}
}

void AWRCharacterMine::TurnRight4SpawnSimulation(float InScale)
{
	if (InScale != 0)
	{
		WRSpawnSimulationManager::Get()->InputEvent(EWRInput::RStickY, EInputEvent::IE_Repeat, InScale);
	}
}

void AWRCharacterMine::TurnRight4CharacterRotation(float InScale)
{
	if (nullptr == CharacterRotator)
		return;
	   
	//if (InScale != 0.0f)
	{
		if (WRInputSystemManager::Get()->IsRotationLock())
		{
			//return;
		}
		else
		{
			CharacterRotator->Rotation(InScale);

#if WITH_EDITOR || PLATFORM_WINDOWS
			// [ 2019-8-20 : kyu ] HMD Off 시 키매핑에 의한 Yaw 회전 수정 적용 추가
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false /*&& GetWorld()->WorldType == EWorldType::PIE*/)
			{
				UCameraComponent* CameraComponent = Cast<UCameraComponent>(this->GetComponentByClass(UCameraComponent::StaticClass()));
				if (CameraComponent != nullptr)
				{
					FRotator OwnerRotation = CameraComponent->GetComponentRotation();
					OwnerRotation.Yaw = this->GetActorRotation().Yaw;
					CameraComponent->SetWorldRotation(OwnerRotation);
				}
			}
#endif // WITH_EDITOR
		}

// 		if (InScale != 0.0f)
// 		{
// 			//		ClearAIMoveTo();
// 			if (IsAIMoveTo())
// 			{
// 				RequestStop();
// 			}
// 		}
	}
}

void AWRCharacterMine::SetSyncRotationByHMD(const bool bInEnable)
{
// 	int32 RotationMode = WRConsoleManager::Get()->GetInt(EWRConsole::Character_RotationMode);
// 	if (RotationMode != (int32)EWRCharacterRotationType::ThumbStickPerAction2)
// 		return;

	if (CharacterRotator == nullptr)
		return;

	CharacterRotator->SetSyncRotationByHMD(bInEnable);
}

void AWRCharacterMine::OnFixRotation()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AWRCharacterMine::OffFixRotation()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

WRTableID AWRCharacterMine::GetRightWeaponTID()
{
	FWRWeaponItemTable* ItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, GetRightWeaponItemTID());
	if (ItemTable)
	{
		return ItemTable->CharWeaponTID;
	}
	return INVALID_TABLE_TID;
}

WRTableID AWRCharacterMine::GetLeftWeaponTID()
{
	FWRWeaponItemTable* ItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, GetLeftWeaponItemTID());
	if (ItemTable)
	{
		return ItemTable->CharWeaponTID;
	}
	return INVALID_TABLE_TID;
}

WRTableID AWRCharacterMine::GetTemporaryTID()
{
	FWRTemporaryItemTable* ItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, GetTemporaryItemTID());
	if (ItemTable)
	{
		return ItemTable->CharWeaponTID;
	}
	return INVALID_TABLE_TID;
}

WRTableID AWRCharacterMine::GetRightWeaponItemTID()
{
	WREquipItemBase* EquippedItem = WRItemManager::Get()->FindEquippedMainWeapon();
	if (EquippedItem)
	{
		return EquippedItem->GetTID();
	}
	return INVALID_TABLE_TID;
}

WRTableID AWRCharacterMine::GetLeftWeaponItemTID()
{
	WREquipItemBase* EquippedItem = WRItemManager::Get()->FindEquippedSubWeapon();
	if (EquippedItem)
	{
		return EquippedItem->GetTID();
	}
	return INVALID_TABLE_TID;
}

WRTableID AWRCharacterMine::GetTemporaryItemTID()
{
	WREquipItemBase* EquippedItem = WRItemManager::Get()->FindEquippedTemporaryWeapon();
	if (EquippedItem)
	{
		return EquippedItem->GetTID();
	}
	return INVALID_TABLE_TID;
}

void AWRCharacterMine::Rotation180()
{
	CharacterRotator->Rotation180();
}

bool AWRCharacterMine::IsLockRotation180() const
{
	return CharacterRotator->IsLockRotation180();
}

void AWRCharacterMine::UnlockRotate180()
{
	CharacterRotator->UnlockRotation180();
}

void AWRCharacterMine::TickRequestMovement(float DeltaTime)
{
	if (0.0f == MoveForwardValue && 0.0f == MoveRightValue)
	{
		//키 입력으로 이동중이다가 이동을 멈췄을때
		if (bInputPad)
		{
			bInputPad = false;
			WRCharacterManager::Get()->NotifyMyCharacterStopMoveDelegate();

			this->RequestStop();			

			if (GetCurrentFSMType() == EWRFSM::Move)
			{
				DetermineFSMSignal(EWRFSM::Idle);
			}

			if (WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable))
			{
				float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::BlackOutIn_Duration);
				WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::BlackOut, Duration);
			}
		}
	}
	else
	{
		//키 입력으로 이동을 시작할때
		if (!bInputPad)
		{
			bInputPad = true;
			WRCharacterManager::Get()->NotifyMyCharacterStartMoveDelegate();

			ElapsedTime2RequestMovement = 0.2f;

			if (WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable))
			{
				float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::BlackOutIn_Duration);
				WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::BlackOut, Duration);
			}

// 			if (CharacterRotator != nullptr)
// 			{
// 				CharacterRotator->SetSyncRotationByHMD_Once();
// 			}

			DetermineFSMSignal(EWRFSM::Move);

			// [SeungYeop 20/08/25] : 기획 의도 변경으로 주석처리(Feat 장원님)
			/*if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::SocialMenu))
			{
				WRUIManager::Get()->CloseUI(EWRUIPanelKind::SocialMenu);
			}*/

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
			if (IsAIMoveTo())
			{
				ClearAIMoveTo();
			}
#endif // DEF_FIX_CLEARAIMOVETO
		}

		ElapsedTime2RequestMovement += DeltaTime;
		if (0.1f <= ElapsedTime2RequestMovement)
		{
			ElapsedTime2RequestMovement = 0.0f;

			this->RequestMove();
		}
	}
}

//====================================================================================
void AWRCharacterMine::OnAckGuardBreak(const pt_Game_User::Ack_GuardBreak_G2U& InInform)
{

}

void AWRCharacterMine::OnAckSuccessToShield(const pt_Game_User::Ack_SuccessToShield_G2U& InInform)
{

}

void AWRCharacterMine::OnAckSuccessAvoid(const pt_Game_User::Ack_SuccessToAvoid_G2U& InInform)
{

}

void AWRCharacterMine::OnAckSyncDynamicMoveLocation(const pt_Game_User::Ack_SyncDynamicMoveLocation_G2U& InInform)
{

}

void AWRCharacterMine::OnAckDynamicMove(const pt_Game_User::Ack_DynamicMove_G2U& InInform)
{

}

void AWRCharacterMine::OnAckHitResult(const pt_Game_User::Ack_HitResult_G2U& InInform)
{

}

void AWRCharacterMine::OnAckSpawnVehicle(const pt_Game_Vehicle::Ack_SpawnVehicle_G2U& InAck)
{

}

void AWRCharacterMine::OnAckGetOnVehicle(const pt_Game_Vehicle::Ack_GetOnVehicle_G2U& InAck)
{
	if (InAck.result == EWRError::Success)
	{
		int32 aa = 0;
	}
	else
	{
	}
}

void AWRCharacterMine::OnAckGetOffVehicle(const pt_Game_Vehicle::Ack_GetOffVehicle_G2U& InAck)
{
	if (InAck.result == EWRError::Success)
	{
		int32 aa = 0;
	}
	else
	{
	}
}

void AWRCharacterMine::OnAckBeginBrake(const pt_Game_Vehicle::Ack_BeginBrake_G2U& InAck)
{

}

void AWRCharacterMine::OnAckEndBrake(const pt_Game_Vehicle::Ack_EndBrake_G2U& InAck)
{

}

void AWRCharacterMine::OnAckAcceleration(const pt_Game_Vehicle::Ack_Acceleration_G2U& InAck)
{

}

void AWRCharacterMine::OnAckSteering(const pt_Game_Vehicle::Ack_Steering_G2U& InAck)
{

}

void AWRCharacterMine::OnAckAgreeToBoard(const pt_Game_Vehicle::Ack_AgreeToBoard_G2U& IN InPacket)
{
	if (InPacket.agree)
	{
		IMMessageSender::RequestGetOnVehicle(this, InPacket.seatIndex, InPacket.vehicleAID);
	}
	else
	{
		WRUIManager::Get()->CloseUI(EWRUIPanelKind::BoardingRequest);
	}
}

void AWRCharacterMine::OnAckUseSkill(const pt_Game_User::Ack_UseSkill_G2U& InInform)
{
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(GetActorID(), InInform.tempSeq);
	if (nullptr == CombatInfo)
		return;

	CombatInfo->ModifyCombatSeqID(InInform.skillSeq);
}

void AWRCharacterMine::OnAckBeginSkill(const pt_Game_User::Ack_BeginSkill_G2U& InInform)
{
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(GetActorID(), InInform.tempSeq);
	if (nullptr == CombatInfo)
		return;

	CombatInfo->ModifyCombatSeqID(InInform.skillSeq);
}

void AWRCharacterMine::OnAckEndSkill(const pt_Game_User::Ack_EndSkill_G2U& InInform)
{
	//아무것도 안함
}

void AWRCharacterMine::OnAckBeginFall(const pt_Game_User::Ack_BeginFall_G2U& InInform)
{
	//떨어지는 애니가 있으면 여기서 재생
}

void AWRCharacterMine::OnAckEndFall(const pt_Game_User::Ack_EndFallen_G2U& InInform)
{
	if (InInform.curHP <= 0)
	{
		DetermineFSMSignal(EWRFSM::Die);
	}
}

void AWRCharacterMine::OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform)
{
	//나 자신은 공격을 이미 처리 했기에 아무것도 하지 않는다
	WRActorUtility::DrawDebug(this, EWRDrawDebugType::Skill, FVector(InInform.casterPosX, InInform.casterPosY, InInform.casterPosZ));
}

void AWRCharacterMine::OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InInform.skillTID, GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return;

	// 	switch (SkillRecord->SkillType)
	// 	{
	// 	case EWRSkillType::ChainProjectile:
	// 	{
	// 		UWRActorComponentEquipmentLook* EquipComponent = GetComponent<UWRActorComponentEquipmentLook>();
	// 		if (EquipComponent)
	// 		{
	// 			EquipComponent->SetWeaponVisibility(EWREquipPart::LeftHand, false);
	// 		}
	// 	}
	// 	break;
	// 	}

		//나 자신은 공격을 이미 처리 했기에 아무것도 하지 않는다
	WRActorUtility::DrawDebug(this, EWRDrawDebugType::Skill, FVector(InInform.casterPosX, InInform.casterPosY, InInform.casterPosZ));
}

void AWRCharacterMine::OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform)
{
	//아무것도 안함
}

void AWRCharacterMine::OnInformRevive(const pt_Game_Actor::Inf_Revive_G2U& InInform)
{
	Super::OnInformRevive(InInform);

	UWRActorComponentRevive* ActorComponentRevive = this->GetComponent<UWRActorComponentRevive>();
	if (ActorComponentRevive == nullptr)
	{
		return;
	}

	ActorComponentRevive->ReviveIn();
}

void AWRCharacterMine::OnInformChangeHP(const pt_Game_Actor::Inf_ChangeHP_G2U& InInform)
{
	Super::OnInformChangeHP(InInform);

	float HPDuration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::UI, "PCHitBlood_FB_Time");
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::HP, HPDuration);
}

void AWRCharacterMine::OnInputSystemTargetChanged(ACharacter* IN InTarget)
{
	if (InTarget == nullptr)
	{
		SetTargetAID(INVALID_AID);
		return;
	}

	AWRCharacter* CastTarget = Cast<AWRCharacter>(InTarget);
	if (CastTarget == nullptr)
	{
		SetTargetAID(INVALID_AID);
		return;
	}

	SetTargetAID(CastTarget->GetActorID());
}

void AWRCharacterMine::OnTargetLockChanged(const bool IN bInLock, const ACharacter* IN InTarget)
{
	if (InTarget == nullptr)
		return;

	const AWRCharacter* CastTarget = Cast<const AWRCharacter>(InTarget);
	if (CastTarget)
	{
		CastTarget->OnTargetLockChanged(bInLock);
	}
}

void AWRCharacterMine::OnHookEventToCaster()
{
	// [ 2019-11-6 : kyu ] 케릭터의 타겟이 아닌 케이블의 타겟으로 수정
	UWRActorComponentConnectbyCable* CableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
		return;

	AWRCharacter* pTarget = CableComponent->GetCableTarget();
	if (pTarget == nullptr)
		return;

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	if (nullptr == CombatInfo)
		return;

	FWRSkillTable* SkillTable = CombatInfo->GetSkillTable();
	if (nullptr == SkillTable)
		return;

	AWRCharacterPlayer::OnHookEventToCaster();

	pTarget->OnHookEventToTarget(this, CableComponent->GetCablePart()); // 대상의 발동 (대상의 동작 횟수 등 처리를 위함)
}

bool AWRCharacterMine::IsRecognizableRange(const ACharacter* IN InTarget)
{
	if (InTarget == nullptr)
		return false;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = InTarget->GetActorLocation();
	float fDist = FVector::Dist(MyLocation, TargetLocation);
	float fRecognitionDist = GetRecognitionDistance();

	if (fDist <= fRecognitionDist)
		return true;

	return false;
}

bool AWRCharacterMine::IsHookableRange(const AWRCharacter* IN InTarget)
{
	if (InTarget == nullptr)
		return false;

	UWRActorComponentAttack* AttackComponent = GetComponent<UWRActorComponentAttack>();
	if (AttackComponent)
	{
		WRTableID SkillTID = AttackComponent->GetCurrentHookTID(InTarget);
		if (SkillTID == INVALID_TABLE_TID)
			return false;

		FWRSkillTable* pSkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(SkillTID, GetSkillTableFileName());
		if (pSkillTable == nullptr)
			return false;

		if (pSkillTable->SkillType != EWRSkillType::Hook)
			return false;

		FVector MyLocation = GetActorLocation();
		FVector TargetLocation = InTarget->GetActorLocation();
		float fDist = FVector::Dist(MyLocation, TargetLocation);

		FWRPlayerSkillTable* pPlayerSkillTable = static_cast<FWRPlayerSkillTable*>(pSkillTable);
		if (pPlayerSkillTable == nullptr)
			return false;

		if (fDist >= pPlayerSkillTable->MinRange && fDist <= pPlayerSkillTable->Range)
			return true;
	}
	return false;
}

void AWRCharacterMine::SetTargetAID(const AID_t IN InTargetAID)
{
	if (TargetAID == InTargetAID)
		return;

	AID_t BeforeTargetAID = TargetAID;

	Super::SetTargetAID(InTargetAID);

	AWRCharacter* BeforeTargetCharacter = WRCharacterManager::Get()->FindbyAID(BeforeTargetAID);
	if (BeforeTargetCharacter != nullptr)
	{
		BeforeTargetCharacter->ReleasedTarget();
	}

	AWRCharacter* TargetCharacter = WRCharacterManager::Get()->FindbyAID(TargetAID);
	if (TargetCharacter != nullptr)
	{
		TargetCharacter->OnTargeted();
	}
	else
	{
		WRInputSystemManager::Get()->SetHMDStartPositionForAvoid(false);
	}

	UWRActorComponentChatRecording* ChatRecordingComponent = GetComponent<UWRActorComponentChatRecording>();
	if (ChatRecordingComponent)
		ChatRecordingComponent->OnMyCharacterTargetChanged(TargetCharacter);

	WRCharacterManager::Get()->NotifyMyCharacterTargetChangedDelegate();
}

void AWRCharacterMine::SaveCurrentHookPart()
{
	if (pTargetedHookCollision != nullptr)
	{
		SetHookPart(pTargetedHookCollision->HitCollisionType);
	}
}

void AWRCharacterMine::OnChainCutTime()
{
	// [ 2020-1-16 : kyu ] 몬스터 대상으로 갈고리를 사용시, 5초 후에 끊긴다. UWRActorComponentConnectbyCable::TickForCut에서 Delegate로 날아옴
	CutOffHookChain(true, nullptr, true, true);

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	if (CombatInfo)
	{
		IMMessageSender::RequestUseSkillEnd(this, CombatInfo->GetCombatSeqID());
	}
}

EWRCollisionType AWRCharacterMine::GetCurrentTargetedHookPart()
{
	if (pTargetedHookCollision != nullptr)
	{
		return pTargetedHookCollision->HitCollisionType;
	}
	return EWRCollisionType::None;
}

AWRCharacter* AWRCharacterMine::GetHookTargetCharacter()
{
	UWRActorComponentConnectbyCable* CableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
		return nullptr;

	if (CableComponent->IsWorking() == false)
		return nullptr;

	AWRCharacter* pTarget = CableComponent->GetCableTarget();

	return pTarget;
}

void AWRCharacterMine::UpdateHookTraceCollision()
{
	// [ 2020-1-13 : kyu ] 여기서는 타겟팅에 의해 UI가 뜨고 꺼짐을 제어한다.
	if (WRConsoleManager::Get()->GetBool(EWRConsole::HookAnywhere) == false
		&& WRMapManager::Get()->GetCurrentMapType() == EWRMapType::Field)
		return;

	if (WRCharacterManager::Get()->IsEquippedHook() == false)
	{
		if (pTargetedHookCollision != nullptr)
		{
			pTargetedHookCollision->SetHookTargeted(false);
			pTargetedHookCollision = nullptr;
		}
		return;
	}
	else
	{
		if (pTargetedHookCollision != nullptr)
		{
			if (pTargetedHookCollision->IsHookable())
			{
				pTargetedHookCollision->SetHookTargeted(true);
			}
			else
			{
				pTargetedHookCollision->SetHookTargeted(false);
				//pTargetedHookCollision = nullptr;
			}
		}
	}

	FHitResult Hit(1.0f);
	UCameraComponent* CamComponet = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
	if (CamComponet == nullptr)
		return;

	float RayLength = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingRayLength);
	FVector HMDForward = CamComponet->GetForwardVector();
	FVector StartLocation = CamComponet->GetComponentLocation();
	FVector EndLocation = StartLocation + (HMDForward * RayLength);
	//float	SphereRadius = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingSphereRadius);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	TArray<AWRCharacter*> DeadCharacters = WRCharacterManager::Get()->GetDeadCharacters();
	for (int i = 0; i < DeadCharacters.Num(); ++i)
		CollisionParams.AddIgnoredActor(DeadCharacters[i]);

	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_HookTrace, CollisionParams) == true)
	{
		// 이미 같은 대상 패스
		if (pTargetedHookCollision == Hit.GetComponent())
			return;

		UWRCapsuleCollisionComponent* NewHookCollision = Cast<UWRCapsuleCollisionComponent>(Hit.GetComponent());
		if (NewHookCollision == nullptr)
			return;

		if (IsRecognizableRange(NewHookCollision->GetOwnerCharacter()) == false)
			return;

		if (NewHookCollision->IsHookable() == false)
			return;

		// 이전 콜리전 타겟팅 해제
		if (pTargetedHookCollision != nullptr)
		{
			pTargetedHookCollision->SetHookTargeted(false);
		}
		// 다음 콜리전 타겟팅 셋팅
		pTargetedHookCollision = NewHookCollision;
		pTargetedHookCollision->SetHookTargeted(true);

		/*

		+ 갈고리 사용시 실제 발동될 타겟 Part는 UWRActorComponentAttack::DetermineHookEvent()에서 저장한다.

		+ 갈고리 발사시 AWRCharacterPlayer::OnHookThrow()에서 이 Part정보를 전달.

		+ 갈고리 연결시 CableAsset의 HookTitle과 Part의 조합으로 연결될 Socket을 찾는다. (Hook_RightLeg_01)
		  없으면 일반적인 NearBone으로 셋팅.

		*/
	}
	else
	{
		if (pTargetedHookCollision != nullptr)
		{
			pTargetedHookCollision->SetHookTargeted(false);
			pTargetedHookCollision = nullptr;
		}
	}
}

//====================================================================================



