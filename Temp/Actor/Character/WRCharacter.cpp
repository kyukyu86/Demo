// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacter.h"

#include "Enum/EWRCharacter.h"

#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRActorComponentAttack.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponentVFX.h"
#include "Component/Character/WRActorComponentSFX.h"
//#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Character/WRActorComponentBuff.h"

#include <AIController.h>
#include <LogMacros.h>
#include "../Animation/WRAnimInstance.h"
#include "../FSM/WRFSMBase.h"

#include "Manager/WRCharacterManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRAnimManager.h"
#include "Manager/WRSkillManager.h"
#include "Manager/WREnvInteractionManager.h"
#include "Manager/WRItemManager.h"
#include "Manager/WRTODManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRTutorialManager.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRVoiceContentsManager.h"

#include "Table/WREnpcTable.h"
#include "Table/WRSkillTable.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRWeaponTable.h"
#include "Table/WRDynamicMovementTable.h"
#include "Table/WRItemTable.h"
#include "Table/WREffectPathTable.h"
#include "../../Table/WRPlayerTable.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WREnumUtility.h"
#include "Utility/WRKismetMeshSliceLibrary.h"
#include "Utility/WRHMDUtility.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Define/WRPathDefine.h"

#include <Animation/AnimMontage.h>
#include "Animation/AnimSequenceBase.h"

#include "UI/Panel/WRUIPanel_TargetUI.h"

#include <Kismet/KismetMathLibrary.h>
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "CombatSystem/Combat/WRCombatInfo.h"

#include "WRCharacterNPC.h"
#include "WRCharacterMine.h"

#include "Network/Share/Define/Define_Battle.h"
#include "Network/Share/Define/Define_Skill.h"

#include "Sound/ReverbEffect.h"

#include "Component/Destructible/WRDestructibleComponent.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Network/IMMessageSender.h"
#include "Animation/AnimInstance.h"
#include "Manager/WRVehicleManager.h"
#include "../Vehicle/WRVehicle.h"
#include "WheeledVehicleMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/SphereComponent.h"



#define DEF_AIDA_TABLIE_ID 20000007

AWRCharacter::AWRCharacter(const FObjectInitializer& ObjectInitializer)
	: ACharacter(ObjectInitializer)
	, TargetAID(INVALID_AID)
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	MeshParent = CreateDefaultSubobject<USceneComponent>("MeshParent");
	MeshParent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MeshComponent->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshParent->SetRelativeTransform(MeshComponent->GetRelativeTransform());
	MeshComponent->SetRelativeTransform(FTransform::Identity);

	if (MeshComponent)
	{
		MeshComponent->OnAnimInitialized.AddDynamic(this, &ThisClass::OnAnimInitializedDele);
		MeshComponent->OnAnimInitialized.AddDynamic(this, &ThisClass::OnAnimInitializedDele4Me);

	}

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	// [ 2020-8-20 : magedoga ] URO
	if (BaseVisibleDistanceFactorThesholds.Num() > 0)
	{
		BaseVisibleDistanceFactorThesholds.Empty();
	}

	// [ 2020-8-20 : magedoga ] Screen에서 차지하는 비율에 맞게 Frame skip
	BaseVisibleDistanceFactorThesholds.Add(0.02);
	BaseVisibleDistanceFactorThesholds.Add(0.01);
	BaseVisibleDistanceFactorThesholds.Add(0.0);

	MeshComponent->OnAnimUpdateRateParamsCreated.BindUObject(this, &AWRCharacter::OnAnimUpdateRateParameterCreated);
}


// Sets default values
AWRCharacter::AWRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWRCharacter::BeginPlay()
{
	Super::BeginPlay();

	// [ 2019-11-18 : animal14 ] 
	WRTODManager::Get()->OnSpawn().AddUObject(this, &AWRCharacter::SpawnTOD);
	WRTODManager::Get()->OnRemove().AddUObject(this, &AWRCharacter::RemoveTOD);

	//--------------------일부러 nullptr 체크 안함--------------------
	CapsuleOriginalCollisionProfileName = GetCapsuleComponent()->GetCollisionProfileName();
	CapsuleOriginalCollisionEnabled = GetCapsuleComponent()->GetCollisionEnabled();

	MeshOriginalCollisionProfileName = GetMesh()->GetCollisionProfileName();
	MeshOriginalCollisionEnabled = GetMesh()->GetCollisionEnabled();
	//--------------------일부러 nullptr 체크 안함--------------------
}

// Called every frame
void AWRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->ProcessRemoveDisplay(DeltaTime);

	this->SetRotationForAwayStep();

	this->ProcessRotationLerp();
	//WRUIManager::Get()->OpenDebugUI(this, MakeDebugString(), FVector(0, 0, 100));
/*
	if (GetCharacterTableID() == 20000007)
	{
		static bool aa = false;
		if (aa)
		{
			FVector ff(51668.675781, 44320.777344, 9466.807617);
			AIMoveTo(ff);
		}
	}
*/
}

// Called to bind functionality to input
void AWRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AWRCharacter::OnPostSpawn()
{
	this->BuildCustomComponents();

	// [ 2019-11-13 : animal14 ] HP 연출 설정
	this->SetMaterialParameterValue("BloodRandom", FMath::FRandRange(0.0F, 1.0F));

	this->MakeSpawnDisplay();

	//SetRole(ENetRole::ROLE_Authority);
	bBattleState = false;


	USkeletalMeshComponent* SkeletalMeshComponent = this->GetMesh();
	if (SkeletalMeshComponent->IsValidLowLevel() == true)
	{
		// [ 2020-8-20 : magedoga ] Animation 최적화 방법 변경 AlwaysTickPoseAndRefreshBones으로 변경하고 AnimUpdateParameter를 이용하여 URO 타이트하게 적용
		SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

		SkeletalMeshLocalTransform = SkeletalMeshComponent->GetRelativeTransform();

		SkeletalMeshComponent->bReceivesDecals = false;

		TArray<USceneComponent*> AttachedComponent = SkeletalMeshComponent->GetAttachChildren();
		for (int32 i = 0; i < AttachedComponent.Num(); ++i)
		{
			USkeletalMeshComponent* ChildSkeletalMeshComponent = Cast<USkeletalMeshComponent>(AttachedComponent[i]);
			if (ChildSkeletalMeshComponent != nullptr)
			{
				ChildSkeletalMeshComponent->bReceivesDecals = false;
			}
		}
	}

	// 	if (GetCharacterMovement()->IsValidLowLevel() == true)
	// 	{
	// 		GetCharacterMovement()->bUpdateOnlyIfRendered = true;
	// 	}

	GetCharacterMovement()->bEnablePhysicsInteraction = 0;
}


void AWRCharacter::OnCompleteSpawn()
{

}

void AWRCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeCombatCollisionCapsule();
	//WRTargetting

	//[2020 - 07 - 28 - EZ : 상호작용 온오프를 위한 WRTargetting 확인 ]
	TArray<UCapsuleComponent*> InCapsuleCompArr;

	this->GetComponents(InCapsuleCompArr, true);

	for (UCapsuleComponent* InComp : InCapsuleCompArr)
	{
		if (InComp != nullptr)
		{
			if (InComp->GetName() == TEXT("WRTargetting"))
			{
				WRTargettingCapsule = InComp;
				break;
			}
		}
	}


	//WRTargettingCapsule = this->GetComponentsByTag()
}

void AWRCharacter::OnPreDestroy()
{
	//UE_LOG(WRCharacter, Display, TEXT("AWRCharacter::OnPreDestroy - Called"));

	// [2020-01-31 : firefirefox] Quest에 노티
	WRQuestManager::Get()->OnCharacterUnSpawned(this);

	if (this->IsLookingTarget() == true)
		SetLookIKTarget(nullptr);

	// [ 2019-8-30 : kyu ] Combined UI Destroy
	RemoveCombinedUI();

	// [ 2020-07-07 : SeungYeop]
	WRVoiceContentsManager::Get()->OnCharacterDestroyed(GetActorID());

	if (DestoryHandle.ExecuteIfBound(this))
	{
		DestoryHandle.Unbind();
	}

	this->RemoveComponentAll();

	CharacterType = EWRCharacterType::None;
	//ActorID = 0;

	mapAttackCapsuleCollisionComponent.Reset();
	mapHitCapsuleCollisionComponent.Reset();
	EquippedItemList.Empty();
	CharacterTableID = 0;

	ReceivedDestPos2AIMove = FVector::ZeroVector;
	ReceivedRotateWhenStop = FRotator::ZeroRotator;
	OriginalMoveRequestGoalLocation = FVector::ZeroVector;


	// [ 2019-11-18 : animal14 ] 
	WRTODManager::Get()->OnSpawn().RemoveAll(this);
	WRTODManager::Get()->OnRemove().RemoveAll(this);

	//WRActorUtility::ClearTimer(RestraintHandle);
	FinishRestraint();
	//WRActorUtility::ClearTimer(DrunkHandle);
	FinishDrunk();

	WRCharacterManager::Get()->GetMyCharacterSpawnedDelegate().Remove(MyCharacterSpawnedDelegatehandle);
}

void AWRCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AWRCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{

}

void AWRCharacter::BeginFSMAttack(const WRFSMInfo& InFSM)
{
	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(InFSM.SkillTableID, GetSkillTableFileName());
	if (nullptr == SkillTable)
		return;

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::Hook:
	{
	}
	break;
	default:
	{
		UWRActorComponentDynamicMovement* DynamicMoveComponent = GetComponent<UWRActorComponentDynamicMovement>();
		if (DynamicMoveComponent)
		{
			DynamicMoveComponent->DetermineDynamicMovement(SkillTable->OwnerDynamicMovementTID, this);
		}
	}
	break;
	}

	FWRCombatParam CombatParam;
	CombatParam.AttackerUID = GetActorID();
	CombatParam.SkillTID = InFSM.SkillTableID;
	CombatParam.CombatSeqID = InFSM.CombatSeqID;
	CombatParam.PreviousFSMType = InFSM.PreviousFSMType;
	WRCombatManager::Get()->CreateInfo(CombatParam);
}

void AWRCharacter::TickFSMAttack(float InDeltaTime)
{

}

void AWRCharacter::EndFSMAttack(const WRFSMInfo& InFSM)
{
	if (nullptr == InFSM.OwnerCharacter)
		return;

	WRCombatManager::Get()->UnregisterCurrentCombat(InFSM.OwnerCharacter->GetActorID());
}

void AWRCharacter::BeginFSMDamage(const WRFSMInfo& InFSM)
{
}

void AWRCharacter::EndFSMDamage(const WRFSMInfo& InFSM)
{

}

void AWRCharacter::BeginFSMDie(const WRFSMInfo& InFSM)
{
	bBattleState = false;
}

void AWRCharacter::EndFSMDie(const WRFSMInfo& InFSM)
{

}

//====================================================================================
// [ 2019-7-17 : magedoga ] Component

void AWRCharacter::BuildCustomComponents()
{
	//UE_LOG(WRCharacter, Display, TEXT("AWRCharacter::BuildCustomComponents - Called"));

	// [ 2019-7-19 : magedoga ] Add Common ActorComponent
	CreateComponent<UWRActorComponentFSM>(EWRCharacterComponent::FSM, true);
	CreateComponent<UWRActorComponentVFX>(EWRCharacterComponent::VFX);
	CreateComponent<UWRActorComponentSFX>(EWRCharacterComponent::SFX);
	CreateComponent<UWRActorComponentManageMaterial>(EWRCharacterComponent::ManageMaterial);
	CreateComponent<UWRActorComponentDynamicMovement>(EWRCharacterComponent::DynamicMovement);
	// [ 2020-4-6 : magedoga ] 기본 생성에서 제거
	//CreateComponent<UWRActorComponentBuff>(EWRCharacterComponent::Buff, true);
	//..
}

void AWRCharacter::OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (this == Other)
		return;

	WREnvInteractionManager::Get()->BeginEnvHittedInteraction(this, SweepResult);
}

void AWRCharacter::OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (this == Other)
		return;

	WREnvInteractionManager::Get()->BeginEnvOverlapInteraction(this, SweepResult);

	//투사체의 경우 AWRStaticMeshProjectile::OnBeginOverlap 여기서 처리 한다
// 	AWRCharacter* OtherChar = Cast<AWRCharacter>(Other);
// 	if (OtherChar)
// 	{
// 		if (IsMe() && OtherChar->IsPlayer())	//내가 맞았는데 때린애가 플레이어 일때
// 		{
// 			return;
// 		}
// 
// 		if (IsPlayer() && OtherChar->IsMe())
// 		{
// 			return;
// 		}
// 
// 		if (IsMe() || OtherChar->IsMe())	//내가 맞거나, 내가 때린애일때
// 		{
// 			WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(OtherChar->GetActorID());
// 			if (CombatInfo)
// 			{
// 				CombatInfo->DetermineHit(GetActorID(), OtherComp);
// 			}
// 		}
// 	}
}

void AWRCharacter::OnEndOverlapHittingCapsule(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWRCharacter::OutsideWorldBounds()
{
	this->PopupForOutofBound();

	WRCharacterManager::Get()->Remove(GetActorID());
	Super::OutsideWorldBounds();
}

void AWRCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	this->PopupForOutofBound();

	WRCharacterManager::Get()->Remove(GetActorID());
	Super::FellOutOfWorld(dmgType);
}

void AWRCharacter::Falling()
{
	Super::Falling();
}

void AWRCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

void AWRCharacter::SetVisibility(bool bNewVisibility, bool bPropagateToChildren /*= false*/)
{
#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
	if (bNewVisibility)
	{
		// [ 2020-9-10 : animal14 ] 플래그 있음
		if (WREnumUtility::IsNone((uint64)VisibilityBits) == false)
		{
			return;
		}
	}
	else
	{
		// [ 2020-9-10 : animal14 ] 플래그 없음
		if (WREnumUtility::IsBit((uint64)VisibilityBits, (uint64)EWRVisibility::None))
		{
			return;
		}
	}

	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent == nullptr)
	{
		return;
	}

	if (bPropagateToChildren)
	{
		EquipmentLookComponent->SetBodyVisibility();
	}
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_

	RootComponent->SetVisibility(bNewVisibility, bPropagateToChildren);

#ifdef REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
	if (bPropagateToChildren)
	{
		EquipmentLookComponent->GetBodyVisibility();
	}
#else // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
#endif // REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_
}

bool AWRCharacter::IsVisibility()
{
	return RootComponent->IsVisible();
}

void AWRCharacter::SpawnDefaultController()
{
	if (Controller != nullptr || GetNetMode() == NM_Client)
	{
		return;
	}

	if (AIControllerClass != nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.OverrideLevel = GetLevel();
		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save AI controllers into a map
		AController* NewController = GetWorld()->SpawnActor<AController>(AIControllerClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
		if (NewController != nullptr)
		{
			// if successful will result in setting this->Controller 
			// as part of possession mechanics
			NewController->Possess(this);

			AIController = Cast<AAIController>(NewController);
			if (AIController)
			{
				//AIController->GetPathFollowingComponent()->OnRequestFinished
				AIController->ReceiveMoveCompleted.AddDynamic(this, &AWRCharacter::OnMoveCompleted);
			}
		}
	}
}

void AWRCharacter::PopupForOutofBound()
{
	if (WRHMDUtility::IsDisplayEnable() == true)
		return;

	FVector CurrentLocation = this->GetActorLocation();
	CurrentLocation.Z += 1000.0f;
	this->SetActorLocation(CurrentLocation);

	if (bProcessingOutsideWorldBounds == false)
	{
		bProcessingOutsideWorldBounds = true;

		FVector ActorLocation = this->GetActorLocation();

		FString MSG = FString::Format(TEXT("Name : {0}\n")TEXT("Type : {1}\n")TEXT("X,Y : {2}, {3}"),
			{ this->GetName(), WREnumUtility::EnumToString("EWRCharacterType", this->GetCharacterType()), ActorLocation.X, ActorLocation.Y });

		FText Description = FText::FromString(MSG);
		FText Title = FText::FromString("PendingKill");
		FMessageDialog::Open(EAppMsgType::Ok, Description, &Title);
	}
}


//====================================================================================
// [ 2019-9-19 : magedoga ] MID

#define WRCHARACTER_SETMATERIAL_PARAMETER_VALUE(ParamName, ParamType, Value, Function, MeshComponent)						\
UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();					\
if (EquipmentLookComponent != nullptr)																						\
{																															\
	EquipmentLookComponent->SetMaterialParameterValue<ParamType>(ParamName, Value);											\
}																															\
																															\
TArray<UMaterialInstanceDynamic*>	MIDs;																					\
WRActorUtility::GetMeshMaterialInstanceDynamic(MeshComponent, MIDs);														\
for (int32 i = 0; i < MIDs.Num(); ++i)																						\
{																															\
	UMaterialInstanceDynamic* MID = MIDs[i];																				\
	if (MID != nullptr)																										\
		MID->Function(ParamName, Value);																					\
}

void AWRCharacter::SetMaterialParameterValue_Internal(FName InParamName, float InValue, UMeshComponent* InMeshComponent /*= nullptr*/)
{
	WRCHARACTER_SETMATERIAL_PARAMETER_VALUE(InParamName, float, InValue, SetScalarParameterValue, InMeshComponent == nullptr ? GetMesh() : InMeshComponent)
}

void AWRCharacter::SetMaterialParameterValue_Internal(FName InParamName, FLinearColor InValue, UMeshComponent* InMeshComponent /*= nullptr*/)
{
	WRCHARACTER_SETMATERIAL_PARAMETER_VALUE(InParamName, FLinearColor, InValue, SetVectorParameterValue, InMeshComponent == nullptr ? GetMesh() : InMeshComponent)
}
//====================================================================================

void AWRCharacter::InitializeCombatCollisionCapsule()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh)
	{
		TArray<USceneComponent*> CapsuleComponents = SkeletalMesh->GetAttachChildren();
		for (int32 Index = 0; Index < CapsuleComponents.Num(); ++Index)
		{
			UWRCapsuleCollisionComponent* CapsuleCollision = Cast<UWRCapsuleCollisionComponent>(CapsuleComponents[Index]);
			if (nullptr == CapsuleCollision)
				continue;

			switch (CapsuleCollision->CollisionType)
			{
			case EWRCollisionType::RightAttack:
			case EWRCollisionType::LeftAttack:
			case EWRCollisionType::Bite:
			case EWRCollisionType::Shield:
			{
				//두가지 타입의 경우 몽타주에서 animNotifyState가 begin될때 껏다가 end될때 꺼준다			
				CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				CapsuleCollision->RegistAttackingSignature();
				mapAttackCapsuleCollisionComponent.Add(CapsuleCollision->CollisionType, CapsuleCollision);
			}
			break;
			}

			switch (CapsuleCollision->HitCollisionType)
			{
			case EWRCollisionType::None:
			case EWRCollisionType::MAX:
			{
				continue;
			}
			break;
			default:
			{
				CapsuleCollision->RegistHittingSignature();
				mapHitCapsuleCollisionComponent.Add(CapsuleCollision->HitCollisionType, CapsuleCollision);
			}
			break;
			}

		}
	}
}


void AWRCharacter::RemoveCapsuleCollisionComponentInfo(const EWRCollisionType& InType)
{
	mapAttackCapsuleCollisionComponent.Remove(InType);
	if (mapAttackCapsuleCollisionComponent.Num() == 0)
	{
		mapAttackCapsuleCollisionComponent.Reset();
	}

	mapHitCapsuleCollisionComponent.Remove(InType);
	if (mapHitCapsuleCollisionComponent.Num() == 0)
	{
		mapHitCapsuleCollisionComponent.Reset();
	}
}

UWRCapsuleCollisionComponent* AWRCharacter::FindCapsuleCollisionComponent(const EWRCollisionType& InType)
{
	if (mapAttackCapsuleCollisionComponent.Contains(InType))
	{
		return mapAttackCapsuleCollisionComponent.FindRef(InType);
	}

	if (mapHitCapsuleCollisionComponent.Contains(InType))
	{
		return mapHitCapsuleCollisionComponent.FindRef(InType);
	}

	return nullptr;
}

bool AWRCharacter::DetermineFSMSignal(EWRFSM FSMType, bool bInvokeImmediately /*= false*/)
{
	return DetermineFSMSignal(WRFSMInfo(FSMType, 0, 0, 0, bInvokeImmediately));
}

bool AWRCharacter::DetermineFSMSignal(WRFSMInfo FSMInfo, bool bInvokeImmediately /*= false*/)
{
	UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
	if (FSMComponent)
	{
		FSMComponent->EmptySequenceFSMInfo();

		return FSMComponent->InvokeFSM(FSMInfo);
	}

	return false;
}

void AWRCharacter::RegistSequenceFSMSignal(EWRFSM InFSMType)
{
	RegistSequenceFSMSignal(WRFSMInfo(InFSMType, 0, 0, 0, false));
}

void AWRCharacter::RegistSequenceFSMSignal(WRFSMInfo InFSMType)
{
	UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
	if (FSMComponent)
	{
		FSMComponent->RegistSequenceFSM(InFSMType);
	}
}

void AWRCharacter::RegistSequenceFSMSignal(const TArray<EWRFSM>& InFSMArr, bool bImmediatelyInvokeFirstIndexFSM)
{
	TArray<WRFSMInfo> arrFSMInfo;
	for (int32 Index = 0; Index < InFSMArr.Num(); ++Index)
	{
		arrFSMInfo.Add(WRFSMInfo(InFSMArr[Index], 0, 0, 0, false));
	}

	RegistSequenceFSMSignal(arrFSMInfo, bImmediatelyInvokeFirstIndexFSM);
}

void AWRCharacter::RegistSequenceFSMSignal(const TArray<WRFSMInfo>& InFSMArr, bool bImmediatelyInvokeFirstIndexFSM)
{
	UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
	if (FSMComponent)
	{
		FSMComponent->RegistSequenceFSM(InFSMArr, bImmediatelyInvokeFirstIndexFSM);
	}
}

bool AWRCharacter::IsAlive()
{
	if (0 < GetHP())
	{
		return true;
	}

	return false;
}

AWRCharacter* AWRCharacter::GetParent()
{
	return this;
}

AWRCharacter* AWRCharacter::GetChild()
{
	return this;
}

bool AWRCharacter::SetLocation(const FVector& NewLocation, bool bModifyZ, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	FVector ModifiedPosition = NewLocation;
	if (bModifyZ)
	{
		WRActorUtility::ModifyZHeightForClient(this, NewLocation, ModifiedPosition.Z);
	}

	return SetActorLocation(ModifiedPosition, bSweep, OutSweepHitResult, Teleport);
}

bool AWRCharacter::SetRotation(FRotator NewRotation, ETeleportType Teleport /*= ETeleportType::None*/)
{
	return SetActorRotation(NewRotation, Teleport);
}

bool AWRCharacter::SetLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bModifyZ, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	FVector ModifiedPosition = NewLocation;
	if (bModifyZ)
	{
		WRActorUtility::ModifyZHeightForClient(this, NewLocation, ModifiedPosition.Z);
	}

	return SetActorLocationAndRotation(ModifiedPosition, NewRotation, bSweep, OutSweepHitResult, Teleport);
}

bool AWRCharacter::SetActorLocation(const FVector& NewLocation, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	if (IsMe())
	{
		int32 aa = 0;
	}
	else if (IsPlayer())
	{
		int32 aa = 0;
	}
	else
	{
		int32 aa = 0;
	}


	return Super::SetActorLocation(NewLocation, bSweep, OutSweepHitResult, Teleport);
}

bool AWRCharacter::SetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep /*= false*/, FHitResult* OutSweepHitResult /*= nullptr*/, ETeleportType Teleport /*= ETeleportType::None*/)
{
	if (IsMe())
	{
		int32 aa = 0;
	}
	else if (IsPlayer())
	{
		int32 aa = 0;
	}
	else
	{
		int32 aa = 0;
	}

	return Super::SetActorLocationAndRotation(NewLocation, NewRotation, bSweep, OutSweepHitResult, Teleport);
}

class AAIController* AWRCharacter::GetAIController()
{
	if (AIController)
	{
		return AIController;
	}
	else
	{
		return Cast<AAIController>(GetController());
	}
}

bool AWRCharacter::PlayAnimationMontage(int32 MontageID, FString StartSectionName /* = ""*/, float InPlayRate /* = 1.0f */)
{
	UWRAnimInstance* AnimInstance = GetAnimInstance();
	if (nullptr == AnimInstance)
		return false;

	float AnimMontageLength = AnimInstance->PlayMontage(MontageID, StartSectionName, InPlayRate);
	if (AnimMontageLength <= 0.0f)
	{
		return false;
	}

	return true;
}

void AWRCharacter::StopCurrentAnimMontage()
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (AnimInstance)
	{
		UAnimMontage* MontageToStop = AnimInstance->GetCurrentActiveMontage();
		bool bShouldStopMontage = AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);
		if (bShouldStopMontage)
		{
			AnimInstance->Montage_Stop(MontageToStop->BlendOut.GetBlendTime(), MontageToStop);
		}
	}
}

void AWRCharacter::PlayAnimationSequence(class UAnimationAsset* NewAnimToPlay, bool bLooping /*= false*/)
{
	if (GetMesh() == nullptr)
		return;

	GetMesh()->PlayAnimation(NewAnimToPlay, bLooping);
}

void AWRCharacter::StopAnimationSequence()
{
	if (GetMesh() == nullptr)
		return;

	GetMesh()->Stop();
}

void AWRCharacter::PlaySlotAnimationAsDynamicMontage(class UAnimSequenceBase* AnimSeq, const FName IN InSlotName)
{
	UWRAnimInstance* pAnimInst = GetAnimInstance();
	if (pAnimInst == nullptr)
		return;

	UAnimMontage* pMontage = pAnimInst->PlaySlotAnimationAsDynamicMontage(AnimSeq, InSlotName);
	if (pMontage == nullptr)
		return;

	FAnimMontageInstance* Inst = pAnimInst->GetActiveMontageInstance();
	if (Inst == nullptr)
		return;

	Inst->bEnableAutoBlendOut = false;
	//	Inst->OnMontageEnded.BindUObject(this, &AWRCharacter::OnInteractionMontageEnded);
	//	Inst->OnMontageBlendingOutStarted.BindUObject(this, &AWRCharacter::OnInteractionMontageEnded);
}

WRTableID AWRCharacter::GetMontageTID(EWRFSM InFSMType, FString InSectionName)
{
	if (IsPlayer())
	{
		//1. 케릭터 테이블에 해당 fsm으로 세팅된 몽타쥬가 잇는지 체크
		WRTableID MontageTID = GetMontageTIDBasedOnCharacterTable(InFSMType);
		if (INVALID_RECORD_TID == MontageTID)
		{
			//2. 플레이어테이블에 없으면 케릭터가 들고 있는 무기를 확인
			FWRWeaponTable* WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(GetTemporaryTID());
			if (nullptr == WeaponTable)
			{
				WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(GetRightWeaponTID());
				if (nullptr == WeaponTable)
				{
					WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(GetLeftWeaponTID());
				}
			}

			if (WeaponTable)
			{
				WRTableID WeaponMontageTID = INVALID_RECORD_TID;
				if (IsMe() && IsShowing3rdCharacter() == false)
				{
					WeaponMontageTID = WeaponTable->BaseMontageTID;
				}
				else
				{
					WeaponMontageTID = WeaponTable->BaseSubMontageTID;
				}

				//해당 웨폰 몽타주에 InFSMType 타입이 있는지 체크
				UAnimMontage* AnimMontage = WRAnimManager::Get()->FindMontage(WeaponMontageTID);
				if (AnimMontage)
				{
					if (InSectionName.IsEmpty() == false && AnimMontage->IsValidSectionName(FName(*InSectionName)))
					{
						return WeaponMontageTID;
					}
					else
					{
						return GetMontageTIDBasedOnCharacterTable(EWRFSM::None);
					}
				}
				else
				{
					return GetMontageTIDBasedOnCharacterTable(EWRFSM::None);
				}
			}
			else
			{
				//3. 무기마저 없으면 플레이어테이블의 기본 none을 확인
				return GetMontageTIDBasedOnCharacterTable(EWRFSM::None);
			}
		}
		else
		{
			return MontageTID;
		}
	}
	else
	{
		return GetMontageTIDBasedOnCharacterTable(InFSMType);
	}
}

WRTableID AWRCharacter::GetMontageTIDBasedOnCharacterTable(const EWRFSM& InFSMType)
{
	if (IsPlayer())
	{
		FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(GetCharacterTableID());
		if (nullptr == PlayerTable)
			return INVALID_RECORD_TID;

		int32 Index = 0;
		bool bResult = PlayerTable->FSMType.Find(InFSMType, Index);
		if (false == bResult)
			return INVALID_RECORD_TID;

		if (IsMe())
		{
			return PlayerTable->BaseMontageTID[Index];
		}
		else
		{
			return PlayerTable->BaseSubMontageTID[Index];
		}
	}
	else if (CharacterType == EWRCharacterType::NPC || CharacterType == EWRCharacterType::ENPC)
	{
		AWRCharacterNPC* NpcCharacter = Cast<AWRCharacterNPC>(this);
		if (NpcCharacter == nullptr)
			return INVALID_RECORD_TID;

		FWRNPCBaseTable* NpcBaseTable = NpcCharacter->GetNpcBaseTable();
		if (NpcBaseTable == nullptr)
			return INVALID_RECORD_TID;

		return NpcBaseTable->BaseMontageTID;
	}

	return INVALID_RECORD_TID;
}

void AWRCharacter::SetAnimMontageRate(float InRate)
{
	if (nullptr == GetAnimInstance())
		return;

	FAnimMontageInstance* MontageInstance = GetAnimInstance()->GetActiveMontageInstance();
	if (nullptr == MontageInstance)
		return;

	MontageInstance->SetPlayRate(InRate);


	// 	UAnimMontage* CurrMontage = GetAnimInstance()->GetCurrentActiveMontage();
	// 	if (nullptr == CurrMontage)
	// 		return;
	// 
	// 	GetAnimInstance()->Montage_SetPlayRate(CurrMontage, InRate);
}

void AWRCharacter::PlayAnimJumpToSection(EWRFSM InFSMType, const FString& InSectionName)
{
	if (nullptr == GetAnimInstance())
		return;

	if (GetCurrentFSMType() != InFSMType)
		return;

	GetAnimInstance()->Montage_JumpToSection(FName(*InSectionName));
}

FString AWRCharacter::GetCurrentMontageSectionName()
{
	if (nullptr == GetAnimInstance())
		return "";

	return GetAnimInstance()->GetMontageSectionName();
}

WRRecordTID AWRCharacter::GetMontageBandwidth()
{
	WRRecordTID Bandwidth = INVALID_RECORD_TID;
	if (IsPlayer())
	{
		if (IsMe())
		{
			AWRCharacterMine* pMe = Cast<AWRCharacterMine>(this);
			if (pMe && pMe->IsShowing3rdCharacter())
			{
				Bandwidth = DEF_BANDWIDTH_PLAYER_TID * GetCharacterTableID();
			}
		}
		else
		{
			Bandwidth = DEF_BANDWIDTH_PLAYER_TID * GetCharacterTableID();
		}
	}

	return Bandwidth;
}

float AWRCharacter::GetCurrentMontageSectionLength()
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (nullptr == AnimInstance)
		return 0.0f;

	FAnimMontageInstance* AnimMontageInstance = AnimInstance->GetActiveMontageInstance();
	if (nullptr == AnimMontageInstance || nullptr == AnimMontageInstance->Montage)
		return 0.0f;

	FString SecionName = AnimMontageInstance->GetCurrentSection().ToString();
	const int32 SectionID = AnimMontageInstance->Montage->GetSectionIndex(FName(*SecionName));
	float CurrSectionLength = AnimMontageInstance->Montage->GetSectionLength(SectionID); //현재 섹션의 

	return CurrSectionLength;
}

float AWRCharacter::GetSectionTimeLeftFromPos()
{
	UAnimInstance* AnimInstance = GetAnimInstance();
	if (nullptr == AnimInstance)
		return 0.0f;

	FAnimMontageInstance* AnimMontageInstance = AnimInstance->GetActiveMontageInstance();
	if (nullptr == AnimMontageInstance || nullptr == AnimMontageInstance->Montage)
		return 0.0f;

	float CurrPosition = AnimMontageInstance->GetPosition();
	float LeftLength = AnimMontageInstance->Montage->GetSectionTimeLeftFromPos(CurrPosition);

	return LeftLength;
}

WRTableID AWRCharacter::ConvertMontageTIDFromBaseTID(const WRTableID IN InBaseTID)
{
	WRTableID ResultTID = InBaseTID;

	if (IsMe())
	{
		// [ 2020-8-19 : kyu ] 1인칭 : 1000*CharTID + BaseTID
		ResultTID += DEF_BANDWIDTH_NORMAL_PLAYER_TID * GetCharacterTableID();

		if (IsShowing3rdCharacter())
		{
			// [ 2020-8-19 : kyu ] 3인칭 : 10000*CharTID + 1000*CharTID + BaseTID
			ResultTID += DEF_BANDWIDTH_3RD_PLAYER_TID * GetCharacterTableID();
		}
	}
	else if (IsPlayer())
	{
		// [ 2020-8-19 : kyu ] 3인칭 : 10000*CharTID + 1000*CharTID + BaseTID
		ResultTID += DEF_BANDWIDTH_NORMAL_PLAYER_TID * GetCharacterTableID();
		ResultTID += DEF_BANDWIDTH_3RD_PLAYER_TID * GetCharacterTableID();
	}

	return ResultTID;
}

void AWRCharacter::OnAnimUpdateRateParameterCreated(FAnimUpdateRateParameters* InParamter)
{
	if (InParamter != nullptr)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		// [ 2020-8-20 : magedoga ] 기본 Enable
		SkeletalMeshComponent->bEnableUpdateRateOptimizations = true;
#if WITH_EDITOR
		if (WRConsoleManager::IsValid() == true)
		{
			bool DisplayURODebug = WRConsoleManager::Get()->GetInt(EWRConsole::Anim_DisplayURODebug) == 1;
			SkeletalMeshComponent->bDisplayDebugUpdateRateOptimizations = DisplayURODebug;
		}
#endif // WITH_EDITOR

		InParamter->BaseNonRenderedUpdateRate = BaseNonRenderedUpdateRate;
		InParamter->BaseVisibleDistanceFactorThesholds = BaseVisibleDistanceFactorThesholds;

		// [ 2020-8-20 : magedoga ] 추후 Character LOD 작업되면 추가
// 		InParamter->bShouldUseLodMap = false;
// 		InParamter->LODToFrameSkipMap = LODToFrameSkipMap;
	}
}

void AWRCharacter::SetEnableURO(const bool bInEnable)
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (SkeletalMeshComponent->IsValidLowLevel() == false)
		return;

	SkeletalMeshComponent->bEnableUpdateRateOptimizations = bInEnable;
}

void AWRCharacter::SetEnableDisplayURODebug(const bool bInEnable)
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (SkeletalMeshComponent->IsValidLowLevel() == false)
		return;

	SkeletalMeshComponent->bDisplayDebugUpdateRateOptimizations = bInEnable;
}

void AWRCharacter::SetCollisionProfileNameAndEnabledTemporary(FName InProfileName, ECollisionEnabled::Type InType, EWRSetCollisionTo InSetTo)
{
	CollisionBackupType = InSetTo;

	switch (InSetTo)
	{
	case EWRSetCollisionTo::CapsuleAndMesh:
	{
		GetCapsuleComponent()->SetCollisionProfileName(InProfileName);
		GetCapsuleComponent()->SetCollisionEnabled(InType);

		GetMesh()->SetCollisionProfileName(InProfileName);
		GetMesh()->SetCollisionEnabled(InType);
	}
	break;
	case EWRSetCollisionTo::CapsuleOnly:
	{
		GetCapsuleComponent()->SetCollisionProfileName(InProfileName);
		GetCapsuleComponent()->SetCollisionEnabled(InType);
	}
	break;
	case EWRSetCollisionTo::MeshOnly:
	{
		GetMesh()->SetCollisionProfileName(InProfileName);
		GetMesh()->SetCollisionEnabled(InType);
	}
	break;
	default:
		break;
	}
}

void AWRCharacter::RevertCollision()
{
	if (EWRSetCollisionTo::None == CollisionBackupType)
		return;

	switch (CollisionBackupType)
	{
	case EWRSetCollisionTo::CapsuleAndMesh:
	{
		GetCapsuleComponent()->SetCollisionProfileName(CapsuleOriginalCollisionProfileName);
		GetCapsuleComponent()->SetCollisionEnabled(CapsuleOriginalCollisionEnabled);

		GetMesh()->SetCollisionProfileName(MeshOriginalCollisionProfileName);
		GetMesh()->SetCollisionEnabled(MeshOriginalCollisionEnabled);
	}
	break;
	case EWRSetCollisionTo::CapsuleOnly:
	{
		GetCapsuleComponent()->SetCollisionProfileName(CapsuleOriginalCollisionProfileName);
		GetCapsuleComponent()->SetCollisionEnabled(CapsuleOriginalCollisionEnabled);
	}
	break;
	case EWRSetCollisionTo::MeshOnly:
	{

		GetMesh()->SetCollisionProfileName(MeshOriginalCollisionProfileName);
		GetMesh()->SetCollisionEnabled(MeshOriginalCollisionEnabled);
	}
	break;
	default:
		break;
	}

	CollisionBackupType = EWRSetCollisionTo::None;

	GetMesh()->SetCollisionProfileName(MeshOriginalCollisionProfileName);
	GetMesh()->SetCollisionEnabled(MeshOriginalCollisionEnabled);

}

bool AWRCharacter::IsMe()
{
	return WRCharacterManager::Get()->IsMyCharacter(this);
}

bool AWRCharacter::IsPlayer()
{
	if (EWRCharacterType::Player == CharacterType)
	{
		return true;
	}

	return false;
}

EWRCharacterType AWRCharacter::GetCharacterType() const
{
	return CharacterType;
}

void AWRCharacter::SetCharacterType(EWRCharacterType InType)
{
	CharacterType = InType;
}

void AWRCharacter::SetWalkSpeedByRateValue(float InRate)
{
	UCharacterMovementComponent* MyMoveComponent = this->GetCharacterMovement();
	if (MyMoveComponent)
	{
		// [ 2019-11-28 : magedoga ] 이거 작업한사람 로직적인 오류 확인해주세요.
		// [ 2019-11-28 : magedoga ] 모수를 바꾸면 어쩝니까~
		MyMoveComponent->MaxWalkSpeed *= InRate;


		// 		MyMoveComponent->ServerMove()
		// 		MyMoveComponent->SetIsReplicated();		
		// 		MyMoveComponent->ReplicateMoveToServer
	}
}

void AWRCharacter::SetWalkMaxSpeed(float InValue)
{
	UCharacterMovementComponent* MyMoveComponent = this->GetCharacterMovement();
	if (MyMoveComponent)
	{
		MyMoveComponent->MaxWalkSpeed = InValue;
	}
}

void AWRCharacter::ReceiveMoveCompleted(FAIRequestID InRequestID, EPathFollowingResult::Type InResult)
{
	switch (InResult)
	{
	case EPathFollowingResult::Success:
	{
		DetermineFSMSignal(EWRFSM::Idle);
		SetRotationForWhenArrived();
	}
	break;
	case EPathFollowingResult::Blocked:
	{
		int32 aa = 0;
	}
	break;
	case EPathFollowingResult::OffPath:
	{
		int32 aa = 0;

	}
	break;
	case EPathFollowingResult::Aborted:
	{
		int32 aa = 0;

	}
	break;
	case EPathFollowingResult::Invalid:
	{
		int32 aa = 0;

	}
	break;
	default:
		break;
	}
}

void AWRCharacter::OnAnimInitializedDele()
{
	UWRAnimInstance* AnimInstance = GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(AnimInstance, &UWRAnimInstance::OnAnimMontageEnded);
		AnimInstance->OnMontageBlendingOut.AddDynamic(AnimInstance, &UWRAnimInstance::OnAnimMontageBlendingOut);
	}

	if (GetBoardingVehicleAID())
	{
		DetermineFSMSignal(EWRFSM::DrivingIdle);
	}

	// Head Area를 Head Socket에 Attach
	TArray<UActorComponent*> FoundComponents = this->GetComponentsByTag(USphereComponent::StaticClass(), "HeadArea");
	if (FoundComponents.Num() != 0)
	{
		USphereComponent* CastedSphereComponent = Cast<USphereComponent>(FoundComponents[0]);
		if (CastedSphereComponent)
		{
			bool bReturn = CastedSphereComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false), "HealthBar");
		}
	}
}

void AWRCharacter::OnAnimInitializedDele4Me()
{
	AWRCharacterMine* pMe = Cast<AWRCharacterMine>(this);
	if (pMe)
	{
		UWRAnimInstance* MyAnimInstance = Cast<UWRAnimInstance>(pMe->Get3rdSkeletalMeshComponent()->GetAnimInstance());
		if (MyAnimInstance)
		{
			MyAnimInstance->OnMontageEnded.AddDynamic(MyAnimInstance, &UWRAnimInstance::OnAnimMontageEnded);
			MyAnimInstance->OnMontageBlendingOut.AddDynamic(MyAnimInstance, &UWRAnimInstance::OnAnimMontageBlendingOut);
		}
	}
}

bool AWRCharacter::AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType)
{
	AAIController* _aiController = GetParent()->GetAIController();
	if (false == _aiController->IsValidLowLevel())
		return false;

	EPathFollowingRequestResult::Type ResultType = EPathFollowingRequestResult::Failed;
	if (IsAIMoveTo())	//이동중이면 일단 성공으로 체크
	{
		ResultType = EPathFollowingRequestResult::RequestSuccessful;
		if (OriginalMoveRequestGoalLocation.Equals(InDestLocation) == false) //이전 이동 목적지와 같으면 체크 패스
		{
			ResultType = _aiController->MoveToLocation(InDestLocation, -1.0f, false, true, true);
			OriginalMoveRequestGoalLocation = InDestLocation;
		}
	}
	else //이동중인 상태가 아니면 이동시도
	{
		ResultType = _aiController->MoveToLocation(InDestLocation, -1.0f, false, true, true);
		OriginalMoveRequestGoalLocation = InDestLocation;
	}

	if (ResultType == EPathFollowingRequestResult::RequestSuccessful)
	{
		UWRAnimInstance* AnimInstance = GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->EnableAIMoveFlag();
		}

		if (GetParent()->GetCurrentFSMType() == EWRFSM::Move)
		{
			return true;
		}
		else
		{
			WRFSMInfo FSMInfo(EWRFSM::Move, INVALID_TABLE_TID, INVALID_AID, INVALID_SKILL_SEQ, true);
			bool bClearAIMoveTo = false;
			FSMInfo.VoidPtr = &bClearAIMoveTo;
			if (EWRActionType::WALK == InMoveType)
			{
				FSMInfo.JumpToSectionName = "Walk";
			}

			//이동이 실패하면 자동이동도 취소
			bool bResult = GetParent()->DetermineFSMSignal(FSMInfo);
			if (!bResult)
			{
				ClearAIMoveTo();
			}

			return bResult;
		}
	}
	else
	{
		OriginalMoveRequestGoalLocation = FVector::ZeroVector;
	}

	return false;
}

void AWRCharacter::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult)
{
	UWRAnimInstance* AnimInstance = GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->DisableAIMoveFlag();
	}
}

bool AWRCharacter::IsAIMoveTo()
{
	AAIController* _aiController = GetAIController();
	if (_aiController == nullptr)
		return false;

	if (_aiController->GetCurrentMoveRequestID().IsValid())
		return true;

	return false;
}

bool AWRCharacter::ClearAIMoveTo()
{
	AAIController* _aiController = GetAIController();
	if (_aiController == nullptr)
		return false;

	if (_aiController->GetCurrentMoveRequestID().IsValid() == false)
		return false;

	_aiController->StopMovement();

	return true;
}

void AWRCharacter::SetDestPositionWhenCharacterSpawned(const FVector& InDestPosition)
{
	if (IsSamePostion2StopWithReceivedDestPos2AIMove(InDestPosition) == false)
	{
		ReceivedRotateWhenStop = FRotator(0.0f, 0.0f, 0.0f);
	}

	ReceivedDestPos2AIMove = InDestPosition;
}

bool AWRCharacter::IsSamePostion2StopWithReceivedDestPos2AIMove(const FVector& InStopPosition)
{
	if (IsAIMoveTo() == false)
		return true;

	float ModifiedStopZ = InStopPosition.Z;
	WRActorUtility::ModifyZHeightForServer(this, InStopPosition, ModifiedStopZ);
	FVector StopPosition(InStopPosition.X, InStopPosition.Y, ModifiedStopZ);

	FVector VecDistance = ReceivedDestPos2AIMove - StopPosition;
	float fDistance = VecDistance.Size2D();
	if (fDistance <= 5.0f)
		return true;

	return false;
}

void AWRCharacter::ProcessInterpolateDestinationLocationWhenSpawned(FVector DestPosition, EWRActionType::en InMoveType)
{
	AIMoveTo(DestPosition, InMoveType);

	//나중에 Stop패킷이 왔을때 저장된 값을 비교해줘야 하는데 Stop패킷에서는 케릭터 캡슐에 따른 높이값 보정을 이용하므로 여기서도 보정한 값으로 계산하여 저장한다.
	FVector ModifiedPosition = DestPosition;
	//WRActorUtility::ModifyZHeightForClient(this, DestPosition, ModifiedPosition.Z);
	SetDestPositionWhenCharacterSpawned(ModifiedPosition);
}

void AWRCharacter::SetEquippedWeaponItemTID(WRTableID InRightWeaponItemTID, WRTableID InLeftWeaponItemTID, WRTableID InTemporaryItemTID)
{
	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent == nullptr)
		return;

	UWRActorComponentAttack* AttackComponent = GetComponent<UWRActorComponentAttack>();
	if (AttackComponent == nullptr)
		return;

	// Weapon Table
	FWRWeaponTable* RightWeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(InRightWeaponItemTID));
	FWRWeaponTable* LeftWeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(InLeftWeaponItemTID));
	FWRWeaponTable* TemporaryTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(InTemporaryItemTID));

	// Weapon Item Table
	FWRWeaponItemTable* RightWeaponItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, InRightWeaponItemTID);
	FWRWeaponItemTable* LeftWeaponItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRWeaponItemTable, InLeftWeaponItemTID);
	FWRTemporaryItemTable* TemporaryItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, InTemporaryItemTID);


	// 1. Right Weapon
	WRTableID& RightWeaponItemTID = EquippedItemList.FindOrAdd(EWRItemEquippableSlot::RightWeapon);
	RightWeaponItemTID = InRightWeaponItemTID;

	// 2. Left Weapon
	WRTableID& LeftWeaponItemTID = EquippedItemList.FindOrAdd(EWRItemEquippableSlot::LeftWeapon);
	LeftWeaponItemTID = InLeftWeaponItemTID;

	// 	bool bTowHandMainWeapon = false;
	// 	if (MainWeaponTable && MainWeaponTable->WeaponType == EWRWeaponType::TwoHand)
	// 		EquippedWeaponsItemTID.Value = INVALID_TABLE_TID;
	// 	else
	// 		EquippedWeaponsItemTID.Value = InSubTID;	// [ 2020-3-12 : kyu ] 애초에 TowHand면 Sub를 빼야하지 않나?

		// 3. Temporary
	WRTableID& TemporaryItemTID = EquippedItemList.FindOrAdd(EWRItemEquippableSlot::LeftWeapon);
	WRTableID AbandonTemporaryItemTID = INVALID_TABLE_TID;
	if (TemporaryItemTID != INVALID_TABLE_TID)
	{
		AbandonTemporaryItemTID = TemporaryItemTID;
	}
	TemporaryItemTID = InTemporaryItemTID;


	// [ 2020-3-12 : kyu ] Temporary Item이 우선
	if (TemporaryItemTable != nullptr)
	{
		// 1.외형
		FWREquipmentLookTable* pEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(TemporaryItemTable->EquipmentLookTID);
		if (pEquipLookTable != nullptr)
		{
			// 임시장비의 반대손은 비워준다
			if (pEquipLookTable->EquipPart == EWREquipPart::LeftHand)
			{
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
				EquipmentLookComponent->Unequip(EWREquipPart::RightHand);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
				EquipmentLookComponent->UnEquip(EWREquipPart::RightHand);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
			}
			else if (pEquipLookTable->EquipPart == EWREquipPart::RightHand)
			{
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
				EquipmentLookComponent->Unequip(EWREquipPart::LeftHand);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
				EquipmentLookComponent->UnEquip(EWREquipPart::LeftHand);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
			}
		}
		EquipmentLookComponent->Equip(pEquipLookTable);
		SetEquipPartVisibility(EWREquipPart::RightHand, true);

		// 2.스킬
		AttackComponent->GenerateSkillInfo(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(TemporaryItemTID));

		// [ 2020-7-27 : kyu ] 임시장비 장착 후 기본Idle이 작동 안하길래 강제로 실행시킴
		if (IsMe() == false)
		{
			DetermineFSMSignal(EWRFSM::Idle);
		}
	}
	else
	{
		// 1.외형
		if (RightWeaponItemTable)
		{
			EquipmentLookComponent->Equip(RightWeaponItemTable->EquipmentLookTID);
			SetEquipPartVisibility(EWREquipPart::RightHand, true);
		}
		else
		{
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
			EquipmentLookComponent->Unequip(EWREquipPart::RightHand);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
			EquipmentLookComponent->UnEquip(EWREquipPart::RightHand);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

			SetEquipPartVisibility(EWREquipPart::RightHand, false);
		}

		if (LeftWeaponItemTable)
		{
			EquipmentLookComponent->Equip(LeftWeaponItemTable->EquipmentLookTID);
			SetEquipPartVisibility(EWREquipPart::LeftHand, true);
		}
		else
		{
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
			EquipmentLookComponent->Unequip(EWREquipPart::LeftHand);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
			EquipmentLookComponent->UnEquip(EWREquipPart::LeftHand);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_

			SetEquipPartVisibility(EWREquipPart::LeftHand, false);
		}

		// 2.스킬
		AttackComponent->GenerateSkillInfo(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(InRightWeaponItemTID));
		AttackComponent->GenerateSkillInfo(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(InLeftWeaponItemTID));

		// 3.Abandon
		if (AbandonTemporaryItemTID != INVALID_TABLE_TID)
		{
			FWRTemporaryItemTable* AbandonedTemporaryItemTable = FIND_AND_CAST_FROM_ITEM_TABLE(FWRTemporaryItemTable, AbandonTemporaryItemTID);
			if (AbandonedTemporaryItemTable != nullptr)
			{
				FWREquipmentLookTable* pAbandonedEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(AbandonedTemporaryItemTable->EquipmentLookTID);
				if (pAbandonedEquipLookTable != nullptr)
				{
					WRCharacterManager::Get()->SpawnAbandonedThing(pAbandonedEquipLookTable->StaticMesh, this, AbandonTemporaryItemTID);
				}
			}
		}
	}
}

void AWRCharacter::SetEquippedWeaponItemTID(WRTableID InWeaponItemTID)
{
	FWRWeaponTable* WeaponTable = WRTableManager::Get()->FindRow<FWRWeaponTable>(WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(InWeaponItemTID));
	if (WeaponTable == nullptr)
	{
		ensureMsgf(false, TEXT("Could not find weapon in table."));
		return;
	}

	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent)
	{
		if (WeaponTable)
		{
			EquipmentLookComponent->Equip(WeaponTable->WeaponLookTid);
		}
	}

	UWRActorComponentAttack* AttackComponent = GetComponent<UWRActorComponentAttack>();
	if (AttackComponent)
	{
		AttackComponent->GenerateSkillInfo(WeaponTable->GetTableID());
	}
}

WRTableID AWRCharacter::GetRightWeaponTID()
{
	WRTableID* RightWeaponItemTID = EquippedItemList.Find(EWRItemEquippableSlot::RightWeapon);
	if (RightWeaponItemTID == nullptr)
		return INVALID_TABLE_TID;

	return WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(*RightWeaponItemTID);
}

WRTableID AWRCharacter::GetLeftWeaponTID()
{
	WRTableID* LeftWeaponItemTID = EquippedItemList.Find(EWRItemEquippableSlot::LeftWeapon);
	if (LeftWeaponItemTID == nullptr)
		return INVALID_TABLE_TID;

	return WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(*LeftWeaponItemTID);
}

WRTableID AWRCharacter::GetTemporaryTID()
{
	WRTableID* TemporaryItemTID = EquippedItemList.Find(EWRItemEquippableSlot::Temporary);
	if (TemporaryItemTID == nullptr)
		return INVALID_TABLE_TID;

	return WRItemManager::Get()->ConvertItemTIDToCharacterWeaponTID(*TemporaryItemTID);
}

WRTableID AWRCharacter::GetRightWeaponItemTID()
{
	WRTableID* RightWeaponItemTID = EquippedItemList.Find(EWRItemEquippableSlot::RightWeapon);
	if (RightWeaponItemTID == nullptr)
		return INVALID_TABLE_TID;

	return *RightWeaponItemTID;
}

WRTableID AWRCharacter::GetLeftWeaponItemTID()
{
	WRTableID* LeftWeaponItemTID = EquippedItemList.Find(EWRItemEquippableSlot::LeftWeapon);
	if (LeftWeaponItemTID == nullptr)
		return INVALID_TABLE_TID;

	return *LeftWeaponItemTID;
}

WRTableID AWRCharacter::GetTemporaryItemTID()
{
	WRTableID* TemporaryItemTID = EquippedItemList.Find(EWRItemEquippableSlot::Temporary);
	if (TemporaryItemTID == nullptr)
		return INVALID_TABLE_TID;

	return *TemporaryItemTID;
}

void AWRCharacter::ChangeWeaponWithAnimation(EWRChangeEquipment IN InType)
{
	UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
	if (FSMComponent)
	{
		// [ 2020-3-10 : kyu ] 공격중 장비 획득시 FSM 우선순위에 의해 장착FSM변환 안될 때 있음. 예외추가
		if (FSMComponent->IsPossibleEndCurrentFSM(EWRFSM::WeaponChange))
		{
			WRFSMInfo fsmInfo(EWRFSM::WeaponChange, 0, 0, 0, false, &InType);
			DetermineFSMSignal(fsmInfo);
		}
		else
		{
			this->ChangeWeaponWithoutAnimation(InType);
		}
	}
}

void AWRCharacter::SetEquipPartVisibility(const EWREquipPart IN InPart, const bool IN InShow)
{
	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent == nullptr)
		return;

	EquipmentLookComponent->SetVisibility(InPart, InShow);
}

bool AWRCharacter::IsEquipedVisible(const EWREquipPart IN InPart)
{
	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent == nullptr)
		return false;

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
	return EquipmentLookComponent->IsVisible(InPart);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	return EquipmentLookComponent->IsEquipedVisible(InPart);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
}

void AWRCharacter::ClearAllEquippedHands()
{
	UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipmentLookComponent)
	{
#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
		EquipmentLookComponent->Unequip(EWREquipPart::LeftHand);
		EquipmentLookComponent->Unequip(EWREquipPart::RightHand);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
		EquipmentLookComponent->UnEquip(EWREquipPart::LeftHand);
		EquipmentLookComponent->UnEquip(EWREquipPart::RightHand);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
	}
}

void AWRCharacter::RevertToSrcEquippedWeapons(const bool IN InWithAnimaiton)
{
	ClearAllEquippedHands();

	if (GetRightWeaponTID() != INVALID_TABLE_TID)
	{
		if (InWithAnimaiton)
		{
			ChangeWeaponWithAnimation(EWRChangeEquipment::MainWeapon);
		}
		else
		{
			ChangeWeaponWithoutAnimation(EWRChangeEquipment::MainWeapon);
		}
		ChangeWeaponWithoutAnimation(EWRChangeEquipment::SubWeapon);
	}
	else if (GetLeftWeaponTID() != INVALID_TABLE_TID)
	{
		if (InWithAnimaiton)
		{
			ChangeWeaponWithAnimation(EWRChangeEquipment::SubWeapon);
		}
		else
		{
			ChangeWeaponWithoutAnimation(EWRChangeEquipment::SubWeapon);
		}
	}
}

void AWRCharacter::SetEquippedCostume(const EWRItemEquippableSlot IN InEquipSlot, const WRTableID IN InCostumeItemTID, FWREquipmentComplete InDelegate /* = nullptr */)
{
	if (InCostumeItemTID == INVALID_TABLE_TID)
	{
		WRTableID* SrcItemTID = EquippedItemList.Find(InEquipSlot);
		if (SrcItemTID != nullptr)
		{
			FWRCostumeItemTable* SrcCostumeItemTable = WRTableManager::Get()->FindRow<FWRCostumeItemTable>(*SrcItemTID);
			if (SrcCostumeItemTable == nullptr)
			{
				ensureMsgf(false, TEXT("Could not find costume in table."));
				return;
			}

			FWREquipmentLookTable* SrcEquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(SrcCostumeItemTable->EquipmentLookTID);
			if (SrcEquipLookTable != nullptr)
			{
				UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
				if (EquipmentLookComponent)
				{
					EquipmentLookComponent->Unequip(SrcEquipLookTable->EquipPart);
				}
			}
			EquippedItemList.Remove(InEquipSlot);
		}
	}
	else
	{
		FWRCostumeItemTable* CostumeItemTable = WRTableManager::Get()->FindRow<FWRCostumeItemTable>(InCostumeItemTID);
		if (CostumeItemTable == nullptr)
		{
			ensureMsgf(false, TEXT("Could not find costume in table."));
			return;
		}

		UWRActorComponentEquipmentLook* EquipmentLookComponent = GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipmentLookComponent)
		{
			EquipmentLookComponent->Equip(CostumeItemTable->EquipmentLookTID, NAME_None, InDelegate);
			EquippedItemList.Emplace(InEquipSlot, InCostumeItemTID);
		}
	}
}

void AWRCharacter::SetEquippedCostume(const TArray<WRTableID> CostumeList, FWREquipmentComplete InDelegate /* = nullptr */)
{
	for (auto& CostumeTID : CostumeList)
	{
		FWRCostumeItemTable* CostumeItemTable = WRTableManager::Get()->FindRow<FWRCostumeItemTable>(CostumeTID);
		if (CostumeItemTable == nullptr)
		{
			ensureMsgf(false, TEXT("Could not find costume in table."));
			continue;
		}
		SetEquippedCostume(CostumeItemTable->EquippableSlot, CostumeTID, InDelegate);
	}
}

WRTableID AWRCharacter::GetFactionTID()
{
	if (IsPlayer())
	{
		FWRPlayerTable* Table = WRTableManager::Get()->FindRow<FWRPlayerTable>(GetCharacterTableID());
		if (Table)
		{
			return (int32)(Table->FactionType);
		}
	}
	else
	{
		FWREnpcTable* Table = WRTableManager::Get()->FindRow<FWREnpcTable>(GetCharacterTableID());
		if (Table)
		{
			return (int32)(Table->FactionType);
		}
	}

	return INVALID_TID;
}

FWRFactionTable* AWRCharacter::GetFactionTable()
{
	return WRTableManager::Get()->FindRow<FWRFactionTable>(GetFactionTID());
}

EWRCharacterFactionType AWRCharacter::GetFactionType()
{
	if (IsPlayer())
	{
		FWRPlayerTable* Table = WRTableManager::Get()->FindRow<FWRPlayerTable>(GetCharacterTableID());
		if (Table)
		{
			return Table->FactionType;
		}
	}
	else
	{
		FWREnpcTable* Table = WRTableManager::Get()->FindRow<FWREnpcTable>(GetCharacterTableID());
		if (Table)
		{
			return Table->FactionType;
		}
	}

	return EWRCharacterFactionType::None;
}

EWRFactionRelation AWRCharacter::GetRelationToTarget(AWRCharacter* InTarget)
{
	if (WRConsoleManager::Get()->GetInt(EWRConsole::Test_NoFaction) == 1)
	{
		return EWRFactionRelation::Hostile;
	}

	if (nullptr == InTarget)
		return EWRFactionRelation::MAX;

	if (IsPlayer() && InTarget->IsPlayer())
	{
		FWRMapTableBase* MapRecord = WRTableManager::Get()->FindRowInMapTable(WRMapManager::Get()->GetCurrentMapTID());
		if (MapRecord && MapRecord->GetMapType() == EWRMapType::BattleField)
		{
			return EWRFactionRelation::Hostile;
		}
	}

	FWRFactionTable* OwnerFactionTable = WRTableManager::Get()->FindRow<FWRFactionTable>(GetFactionTID());
	if (OwnerFactionTable)
	{
		return OwnerFactionTable->GetFactionRelation(InTarget->GetFactionType());
	}

	return EWRFactionRelation::MAX;
}

EWRFactionRelation AWRCharacter::GetRelationToTarget(WRTableID InFactionTID)
{
	if (WRConsoleManager::Get()->GetInt(EWRConsole::Test_NoFaction) == 1)
	{
		return EWRFactionRelation::Hostile;
	}

	FWRFactionTable* OwnerFactionTable = WRTableManager::Get()->FindRow<FWRFactionTable>(GetFactionTID());
	if (OwnerFactionTable)
	{
		FWRFactionTable* TargetFactionTable = WRTableManager::Get()->FindRow<FWRFactionTable>(GetFactionTID());
		if (TargetFactionTable)
		{
			return OwnerFactionTable->GetFactionRelation(TargetFactionTable->FactionType);
		}
	}

	return EWRFactionRelation::MAX;
}

EWRFactionRelation AWRCharacter::GetRelationToTarget(EWRCharacterFactionType InFactionType)
{
	if (WRConsoleManager::Get()->GetInt(EWRConsole::Test_NoFaction) == 1)
	{
		return EWRFactionRelation::Hostile;
	}

	FWRFactionTable* OwnerFactionTable = WRTableManager::Get()->FindRow<FWRFactionTable>(GetFactionTID());
	if (OwnerFactionTable)
	{
		return OwnerFactionTable->GetFactionRelation(InFactionType);
	}

	return EWRFactionRelation::MAX;
}

void AWRCharacter::OnInformAIMoveTo(const pt_Game_User::Inf_ActorMoveToPos_G2U& InInform)
{
	if (IsAlive() == false)
		return;

	uint8 actionType = InInform.actionType;
	FVector DestLocation = FVector(InInform.target_posX, InInform.target_posY, InInform.target_posZ);
	AIMoveTo(DestLocation, (EWRActionType::en)actionType);

	if (IsSamePostion2StopWithReceivedDestPos2AIMove(DestLocation) == false)
	{
		ReceivedRotateWhenStop = FRotator(0.0f, 0.0f, 0.0f);
	}

	if (IsPlayer() == false)
	{
		ReceivedDestPos2AIMove = DestLocation;
		if (InInform.actionType == EWRActionType::SIDE_STEP || InInform.actionType == EWRActionType::AWAY_STEP)
		{
			IsAwayStep = true;
		}
		else
		{
			if (IsAwayStep == true)
			{
				FRotator LookAtRotation = FRotator(0.f, GetLookAtRotator(DestLocation).Yaw, 0.f);

				this->SetRotation(LookAtRotation);
				IsAwayStep = false;
			}
		}
	}
	else
	{
		FRotator LookAtRotation = FRotator(0.0f, GetLookAtRotator(DestLocation).Yaw, 0.0f);
		if (this->IsMe() == false)
		{
			SetRotation(LookAtRotation);
		}
		else
		{
			AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(this);
			if (MyCharacter->IsValidLowLevel() == true)
			{
				MyCharacter->GetRotator()->RotateCharacterWithHMD(LookAtRotation);
			}
		}
	}
}

void AWRCharacter::OnInformAIMoveStop(const pt_Game_User::Inf_ActorMoveStop_G2U& InInform)
{
	bool bStop = true;

	FVector LastPosition(InInform.stop_posX, InInform.stop_posY, InInform.stop_posZ);

	FRotator LastRotation;

	if (this->IsInBattle() == true && this->IsPlayer() == false)
	{
		const AWRCharacter* InTargetCharacter = this->GetTargetCharacter();
		if (InTargetCharacter != nullptr)
		{
			FVector TargetLocation = this->GetTargetCharacter()->GetActorLocation();
			LastRotation = GetLookAtRotator(TargetLocation);
		}
	}
	else
	{
		LastRotation = FRotator(0.0f, InInform.rotate, 0.0f);
	}

	if (IsSamePostion2StopWithReceivedDestPos2AIMove(this->GetActorLocation()) == false)	//이전에 받은 Move위치랑 다르면
	{
		bStop = false;

		ReceivedDestPos2AIMove = LastPosition;
		ReceivedRotateWhenStop = LastRotation;
		AIMoveTo(ReceivedDestPos2AIMove);
	}
	else
	{
		ReceivedDestPos2AIMove = FVector::ZeroVector;
		ReceivedRotateWhenStop = FRotator::ZeroRotator;
	}

	if (bStop)
	{
		IsAwayStep = false;

		if (GetCurrentFSMType() != EWRFSM::Dance)
			DetermineFSMSignal(EWRFSM::Idle);

		ClearAIMoveTo();
	}

	WRActorUtility::DrawDebug(this, EWRDrawDebugType::StopMove, LastPosition);
}

void AWRCharacter::OnInformAIWarp(const pt_Game_User::Inf_ActorWarp_G2U & InInform)
{
	FVector LastPosition(InInform.stop_posX, InInform.stop_posY, InInform.stop_posZ);

	FRotator LastRotation(0.0f, InInform.rotate, 0.0f);

	DetermineFSMSignal(EWRFSM::Idle);

	SetLocationAndRotation(LastPosition, LastRotation, true, false, nullptr, ETeleportType::TeleportPhysics);

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	ClearAIMoveTo();
#endif // DEF_FIX_CLEARAIMOVETO

	/*	if (GetCharacterTableID() == 20000007)
		{
			int32 aa = 0;
		}
	*/
	WRActorUtility::DrawDebug(this, EWRDrawDebugType::WarpTo, LastPosition);
}

void AWRCharacter::OnInformChangeHP(const pt_Game_Actor::Inf_ChangeHP_G2U& InInform)
{
	/*if (CharacterStats.HP < InInform.currentHP)
		WRVFXManager::Get()->Spawn(EFFECT_PATH_4_HEAL, this, nullptr);*/

	CharacterStats.MaxHP = InInform.maxHP;
	CharacterStats.HP = InInform.currentHP;
}

void AWRCharacter::OnInformBeginFall(const pt_Game_User::Inf_BeginFall_G2U& InInform)
{
	//떨어지는 애니가 있으면 여기서 재생
}

void AWRCharacter::OnInformEndFall(const pt_Game_User::Inf_EndFallen_G2U& InInform)
{
	if (InInform.curHP <= 0)
	{
		DetermineFSMSignal(EWRFSM::Die);
	}
}

void AWRCharacter::OnInformChangeCostume(const pt_Game_User_Inventory::Inf_ChangeCostume_G2U& InInform)
{
	SetEquippedCostume(EWRItemEquippableSlot::Top, InInform.topTID);
	SetEquippedCostume(EWRItemEquippableSlot::Bottom, InInform.bottomTID);
	SetEquippedCostume(EWRItemEquippableSlot::Shoes, InInform.shoesTID);
	SetEquippedCostume(EWRItemEquippableSlot::Hat, InInform.hatTID);
	SetEquippedCostume(EWRItemEquippableSlot::Gloves, InInform.glovesTID);
	SetEquippedCostume(EWRItemEquippableSlot::Glasses, InInform.glassesTID);
	SetEquippedCostume(EWRItemEquippableSlot::Neck, InInform.neckTID);
	SetEquippedCostume(EWRItemEquippableSlot::Mask, InInform.maskTID);
	SetEquippedCostume(EWRItemEquippableSlot::Back, InInform.backTID);
}

void AWRCharacter::OnInformPlayerStop(const pt_Game_User::Inf_PlayerStop_G2U& InInform)
{
	FVector Location(InInform.posX, InInform.posY, InInform.posZ);
	FRotator Rotator(0.0f, InInform.rotate, 0.0f);

	SetLocationAndRotation(Location, Rotator);
}

void AWRCharacter::OnInformActorCCState(const pt_Game_User::Inf_ActorCCState_G2U& InInform)
{
	switch ((EWRCCState::en)InInform.ccState)
	{
	case EWRCCState::None:
	{

	}
	break;
	case EWRCCState::Weak:
	{
		// 		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
		// 		if (CombatInfo)
		// 		{	
		// 			CombatInfo->ExecuteParrying(GetActorID());
		// 		}

		if (InInform.enabled)
		{
			DetermineFSMSignal(EWRFSM::BeParried);
		}
		else
		{
			DetermineFSMSignal(EWRFSM::Idle, true);
		}
	}
	break;
	default:
		break;
	}
}

void AWRCharacter::OnInformGuardBreak(const pt_Game_User::Inf_GuardBreak_G2U& InInform)
{
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
	if (CombatInfo)
	{
		CombatInfo->ExecuteGuardBreak(InInform.targetAID);
	}
}

void AWRCharacter::OnInformSuccessToShield(const pt_Game_User::Inf_SuccessToShield_G2U& InInform)
{
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
	if (CombatInfo)
	{
		CombatInfo->ExecuteBlockSuccessed(InInform.casterAID, InInform.targetAID);
	}
}

void AWRCharacter::OnInformSyncDynamicMoveLocation(const pt_Game_User::Inf_SyncDynamicMoveLocation_G2U& InInform)
{
	FVector Location(InInform.posX, InInform.posY, InInform.posZ);
	FRotator Rotator(0.0f, InInform.rotate, 0.0f);

	SetLocationAndRotation(Location, Rotator);
}

void AWRCharacter::OnInformDynamicMove(const pt_Game_User::Inf_DynamicMove_G2U& InInform)
{
	if (IsMe())
		return;

	UWRActorComponentDynamicMovement* DynamicMoveComponent = GetComponent<UWRActorComponentDynamicMovement>();
	if (nullptr == DynamicMoveComponent)
		return;

	FVector DestLocation(InInform.DestX, InInform.DestY, InInform.DestZ);
	DynamicMoveComponent->InvokeDynamicMovement(InInform.dynamicMoveTableTID, DestLocation);
}

void AWRCharacter::OnInformSuccessAvoid(const pt_Game_User::Inf_SuccessToAvoid_G2U& InInform)
{
	DetermineFSMSignal(EWRFSM::Avoiding);
}

void AWRCharacter::OnInformHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform)
{
	switch ((EWRCCState::en)InInform.ccState)
	{
	case EWRCCState::None:
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
		if (CombatInfo)
		{
			SetHP(InInform.currentHP);
			CombatInfo->ExecuteHit(GetActorID(), InInform.damage, InInform.reaction);
		}
	}
	break;
	case EWRCCState::Weak:
	{

	}
	break;
	default:
		break;
	}

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
	if (CombatInfo && CombatInfo->GetSkillTable())
	{
		switch (CombatInfo->GetSkillTable()->SkillType)
		{
		case EWRSkillType::Hook:
		{
			if (CombatInfo->GetAttacker()->IsPlayer())
			{
				Cast<AWRCharacterPlayer>(CombatInfo->GetAttacker())->HitHook();
			}
		}
		break;
		default:
			break;
		}
	}

	// [ 2019-12-3 : animal14 ] 임시 코드
	//WRSFXManager::Get()->SpawnAtLocation(SOUND_PATH_4_DAMAGE, this->GetActorLocation());
}

void AWRCharacter::OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InInform.skillTID, GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return;

	FVector Position(InInform.casterPosX, InInform.casterPosY, InInform.casterPosZ);
	FRotator Rotation(0.0f, InInform.rotate, 0.0f);
	SetRotation(Rotation);

	bool bDoFSMImmediately = false;
	switch (GetCurrentFSMType())
	{
	case EWRFSM::Attack:
	case EWRFSM::Skill:
	case EWRFSM::MaintainingSkill:
	case EWRFSM::Damage:
	case EWRFSM::CC:
	{
		bDoFSMImmediately = true;
	}
	default:
		break;
	}

	//현재 타겟이 항상 내 공격에 맞는다는 보장이 없다. 거리가 멀수도 있기에.
	AID_t TargetUID = INVALID_AID;
	if (InInform.lstTarget.size() != 0)
	{
		auto Iter = InInform.lstTarget.begin();
		TargetUID = *Iter;
		SetTargetAID(*Iter);
	}

	WRFSMInfo FSMInfo(SkillRecord->FSMType, InInform.skillTID, TargetUID, InInform.skillSeq, bDoFSMImmediately);
	EWRAttackHeightType HeightType = (EWRAttackHeightType)InInform.attackHeight;
	FSMInfo.VoidPtr = &HeightType;
	bool bSuccess = DetermineFSMSignal(FSMInfo);
	if (bSuccess && SkillRecord->TargetingType == EWRTargetingType::Multi)
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
		if (CombatInfo)
		{
			TArray<AID_t> Temp;
			for (auto iter = InInform.lstTarget.begin(); iter != InInform.lstTarget.end(); ++iter)
			{
				Temp.Add(*iter);
			}

			CombatInfo->SetChainTargetInfo(Temp);
		}
	}
}

void AWRCharacter::OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InInform.skillTID, GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return;

	FVector LastPosition(InInform.casterPosX, InInform.casterPosY, InInform.casterPosZ);	//이동공격이 가능하기에 위치세팅은 안함 (상황볼것)
	FRotator Rotation(0.0f, InInform.casterRotate, 0.0f);
	SetRotation(Rotation);


	bool bDoFSMImmediately = false;
	switch (GetCurrentFSMType())
	{
	case EWRFSM::Attack:
	case EWRFSM::Damage:
	case EWRFSM::CC:
	{
		bDoFSMImmediately = true;
	}
	default:
		break;
	}

	EWRFSM FSMType = WRSkillManager::Get()->GetFSMTypeDependOnSkillTable(this, InInform.skillTID);
	WRFSMInfo FSMInfo(FSMType, InInform.skillTID, GetTargetAID(), InInform.skillSeq, bDoFSMImmediately);
	EWRAttackHeightType HeightType = (EWRAttackHeightType)InInform.attackHeight;
	FSMInfo.VoidPtr = &HeightType;
	bool bSuccess = DetermineFSMSignal(FSMInfo);
	if (bSuccess)
	{
		if (SkillRecord->TargetingType == EWRTargetingType::Multi)
		{
			WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
			if (CombatInfo)
			{
				TArray<AID_t> Temp;
				for (auto iter = InInform.lstTarget.begin(); iter != InInform.lstTarget.end(); ++iter)
				{
					Temp.Add(*iter);
				}

				CombatInfo->SetChainTargetInfo(Temp);
			}
		}
		else
		{
			if (InInform.lstTarget.size() != 0)
			{
				AID_t _TargetAID = (*InInform.lstTarget.begin());
				SetTargetAID(_TargetAID);
			}
		}
	}
}

void AWRCharacter::OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform)
{
	//DetermineFSMSignal(EWRFSM::Idle, true);
	DetermineFSMSignal(EWRFSM::Idle);
}

void AWRCharacter::OnInformRevive(const pt_Game_Actor::Inf_Revive_G2U& InInform)
{
	SetLocationAndRotation(FVector(InInform.posX, InInform.posY, InInform.posZ), FRotator(0.0f, InInform.angle, 0.0f), true);
	SetMaxHP(InInform.maxhp);
	SetHP(InInform.hp);

	DetermineFSMSignal(EWRFSM::Revival);

	//죽어잇던 케릭터 목록에서 삭제
	WRCharacterManager::Get()->RemoveDeadCharacter(this);
	//캡슐 컬리전 활성화
	WRActorUtility::SetComponentCollisionEnabled(GetRootComponent(), ECollisionEnabled::QueryAndPhysics);
	//피직스 끄기
	SetSimulatePhysics(false);


	FWRFXPathTable* FxPathTable = WRTableManager::Get()->FindRow<FWRFXPathTable>(REVIVE_FX_PATH_TABLEID);
	if (FxPathTable)
	{
		WRVFXManager::Get()->Spawn(FxPathTable->FXPath, this, this);
	}

	if (this->IsMe())
	{
		WRMaterialParameterCollectionManager::Get()->SetHP(0);
	}
	else
	{
		this->SetMaterialParameterValue("HitBloodMain", 0.0F);
		if (this->GetCharacterType() == EWRCharacterType::Player)
		{
			USkeletalMeshComponent*InSkelMeshComp = GetMesh();

			InSkelMeshComp->SetRelativeTransform(SkeletalMeshLocalTransform);
			InSkelMeshComp->AttachToComponent(MeshParent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void AWRCharacter::SetCharacterTableID(WRTableID InTID)
{
	CharacterTableID = InTID;
}

WRTableID AWRCharacter::GetCharacterTableID()
{
	return CharacterTableID;
}

void AWRCharacter::SetSimulatePhysics(const bool InIsEnable)
{
	USkeletalMeshComponent* SkeletalMeshComponent = this->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}
	SkeletalMeshComponent->SetSimulatePhysics(InIsEnable);
}

void AWRCharacter::AddImpulseToAllBodiesBelow(const float InImpulse /*= 1.0F*/, const FName& InBoneName /*= NAME_None*/)
{
	float Impulse = 0.0F;

	AWRCharacter* Murderer = WRCharacterManager::Get()->FindbyAID(MurdererAID);
	if (Murderer == nullptr)
	{
		Impulse = WRConsoleManager::Get()->GetFloat(EWRConsole::Add_Impulse);
	}
	else
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Murderer->GetMesh();
		if (SkeletalMeshComponent == nullptr)
		{
			return;
		}

		UWRActorComponentFSM* ActorComponentFSM = Murderer->GetComponent<UWRActorComponentFSM>();
		if (ActorComponentFSM == nullptr)
		{
			return;
		}

		FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(ActorComponentFSM->GetCurrentFSMInfo()->SkillTableID, Murderer->GetSkillTableFileName());
		if (SkillTable == nullptr)
		{
			return;
		}

		Impulse = SkillTable->Impulse * SkeletalMeshComponent->BodyInstance.MassScale;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = this->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}
	SkeletalMeshComponent->AddImpulseToAllBodiesBelow(PhysicsDirection * Impulse / SkeletalMeshComponent->BodyInstance.MassScale, InBoneName, true);
}

void AWRCharacter::SpawnTOD()
{
}

void AWRCharacter::RemoveTOD()
{
}

void AWRCharacter::SelectDeadDisplay()
{
	switch (CharacterType)
	{
	case EWRCharacterType::Player:
		IsDeadPlay = true;
		break;
	case EWRCharacterType::NPC:
		IsDeadPlay = true;
		break;
	case EWRCharacterType::ENPC:
		CheckDeadEffect();
		break;
	case EWRCharacterType::IO:
		this->MakeRemoveDisplay();
		break;
	}
}

void AWRCharacter::SetWhiteOut(const FVector InLocation, const FRotator InRotation)
{

}

void AWRCharacter::StartRestraint(int32 InDuration)
{
	if (InDuration > 0)
	{
		WRActorUtility::ClearTimer(RestraintHandle);

		// [ 2019-12-11 : animal14 ] 움직임 제한
		WRInputSystemManager::Get()->SetLocationLock(true);

		// [ 2019-12-11 : animal14 ] UI
		WRTutorialManager::Get()->OpenTutorialAlert();

		float Duration = InDuration * 0.001F;
		WRActorUtility::SetTimer(RestraintHandle, FTimerDelegate::CreateUObject(this, &AWRCharacter::FinishRestraint), Duration, false);
	}
	else
	{
	}
}

void AWRCharacter::StartDrunk(float InDuration /*= 1.0F*/)
{
	// [ 2020-3-16 : animal14 ] 화면 연출
	if (this->IsMe() == false)
	{
		return;
	}
	WRMaterialParameterCollectionManager::Get()->SetDrunk(1.0F);

	// [ 2020-3-16 : animal14 ] 음향 연출
	UReverbEffect* ReverbEffect = LoadObject<UReverbEffect>(nullptr, *REVERB_PATH_4_DRUNK);
	if (ReverbEffect == nullptr)
	{
		return;
	}
	UGameplayStatics::ActivateReverbEffect(this, ReverbEffect, TEXT("BeDrunk"));

	WRActorUtility::SetTimer(DrunkHandle, FTimerDelegate::CreateUObject(this, &AWRCharacter::FinishDrunk), InDuration, false);
}

FString AWRCharacter::GetSkillTableFileName()
{
	if (IsPlayer())
	{
		return "SkillPlayer_Atk";
	}
	else
	{
		return "SkillEnpc_Atk";
	}
}

void AWRCharacter::MakeHitDisplay(const FVector& InImpactPoint)
{
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	if (this->IsMe())
	{
		float HitDuration = WRConsoleManager::Get()->GetFloat(EWRConsole::HitRed_Duration);
		ActorComponentManageMaterial->Spawn(EWRManageMaterial::Hit, HitDuration);
	}
	else
	{
		WRMaterialParameterCollectionManager::Get()->SetImpactPoint(InImpactPoint);

		float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "HitPartTime");
		ActorComponentManageMaterial->Spawn(EWRManageMaterial::Hit, Duration);

		ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Hit).AddUObject(this, &AWRCharacter::CompleteHit);
	}
}

void AWRCharacter::MakeUnderWaterDisplay()
{
	UWRActorComponentVFX* ActorComponentVFX = this->GetComponent<UWRActorComponentVFX>();
	if (ActorComponentVFX == nullptr)
		return;

	if (ActorComponentVFX->Contains(EWRVFXActor::UnderWater) == false)
		return;

	WRVFXManager::Get()->Spawn(EFFECT_PATH_4_WATERVOLUME, this, nullptr);
}

void AWRCharacter::MakeWarpDisplay()
{
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "Death01_DissolveTime");
	ActorComponentManageMaterial->Spawn(EWRManageMaterial::Spawn, Duration);
}

void AWRCharacter::PlayClothChangeDirection(const float IN InDuration /*= 3.f*/, const bool IN bInInverse /*= false*/)
{
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	if (bInInverse)
		ActorComponentManageMaterial->Remove(EWRManageMaterial::ChangeCloth, InDuration);
	else
		ActorComponentManageMaterial->Spawn(EWRManageMaterial::ChangeCloth, InDuration);
}



TArray<FString> AWRCharacter::DebugActorInfomation()
{
	TArray<FString> strText;

	/*
		// AID
		strText.Add(FString::Printf(TEXT("AID : %d"), AID));

		// State
		strText.Add(FString::Printf(TEXT("State : %s"), *EWRVehicleState::GetStr(eState)));

		// Location
		strText.Add(FString::Printf(TEXT("Location : %.0f, %.0f, %.0f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));

		// Overlap Event State
		bool bOverlapEvent = false;
		UPrimitiveComponent* SceneToPrimitive = Cast<UPrimitiveComponent>(GetRootComponent());
		if (SceneToPrimitive != nullptr)
			bOverlapEvent = SceneToPrimitive->GetGenerateOverlapEvents();
		strText.Add(FString::Printf(TEXT("Overlap State : %d"), bOverlapEvent));

		// Overlap Component Count
		strText.Add(FString::Printf(TEXT("Recognition Overlaped Components : %d"), FOR_DEBUG_RecognitionOverlapedComponentNum));

		// Overlap OutterDoor Count
		strText.Add(FString::Printf(TEXT("Recognition Overlaped OutterDoors : %d"), FOR_DEBUG_RecognitionOverlapedOuterDoorNum));

		// OutterDoor Count
		strText.Add(FString::Printf(TEXT("OutterDoors : %d"), OutterDoorList.Num()));*/

	return strText;
}

bool AWRCharacter::IsSimulatingPhysics()
{
	USkeletalMeshComponent* SkeletalMeshComponent = this->GetMesh();
	UWRDestructibleComponent* InDestructibleComp = Cast<UWRDestructibleComponent>(this->GetComponentByClass(UWRDestructibleComponent::StaticClass()));

	bool bResultPhysics = false;

	if (SkeletalMeshComponent != nullptr)
	{
		bResultPhysics = SkeletalMeshComponent->IsSimulatingPhysics();
	}
	else if (InDestructibleComp != nullptr)
	{
		bResultPhysics = InDestructibleComp->IsAnySimulatingPhysics();
	}

	return bResultPhysics;
}

void AWRCharacter::ProcessRemoveDisplay(float InDeltaTime)
{
	if (IsDeadPlay == false)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = this->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	FVector CurrentLocation = SkeletalMeshComponent->GetComponentLocation();

	// [ 2019-11-28 : animal14 ] 이동량 확인
	float Difference = FMath::Abs(PreviousLocation.Size() - CurrentLocation.Size());
	float MovementAmount = WRConsoleManager::Get()->GetFloat(EWRConsole::Movement_Amount);
	if (Difference > MovementAmount)
	{
		PreviousLocation = CurrentLocation;
	}
	else
	{
		AccDeltaTime += InDeltaTime;

		float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::Dead_Duration);
		if (Duration > AccDeltaTime)
		{
			return;
		}

		this->MakeRemoveDisplay();
	}
}

void AWRCharacter::MakeSpawnDisplay()
{
	if (IsSpawnDisplay)
	{
		// [ 2019-10-22 : animal14 ] 이펙트 생성
		//AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_SPAWN, this, nullptr);
		//if (VFX == nullptr)
		//{
		//	return;
		//}

		// [ 2019-11-8 : animal14 ] 스폰 연출
		UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
		if (ActorComponentManageMaterial == nullptr)
		{
			return;
		}

		float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "Death01_DissolveTime");
		ActorComponentManageMaterial->Spawn(EWRManageMaterial::Spawn, Duration);

		// [ 2019-11-22 : animal14 ] 이벤트 바인딩
		ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Spawn).AddUObject(this, &AWRCharacter::CompleteSpawn);
	}
	else
	{
		// [ 2019-11-22 : animal14 ] 
		//CreateComponent<UWRActorComponentFSM>(EWRCharacterComponent::FSM, true);
	}
}

//#define WR_EFFECT_NIAGARA

void AWRCharacter::MakeRemoveDisplay()
{
	// [ 2019-10-22 : animal14 ] 이펙트 생성
#ifdef WR_EFFECT_NIAGARA
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_NIAGARA_PATH_4_DEAD, this, nullptr);
	if (VFX == nullptr)
	{
		return;
	}
	VFX->SetNiagaraDataInterfaceSkeletalMesh();
#else // WR_EFFECT_NIAGARA
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_CASCADE_PATH_4_DEAD, this, nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = this->GetMesh();
	if (SkeletalMeshComponent)
	{
		FVector ComponentScale = SkeletalMeshComponent->GetComponentScale();
		float AverageScale = ComponentScale.SizeSquared() / 3;;

		VFX->SetFloatParameter(TEXT("ENPC_Size"), FMath::Sqrt(AverageScale));
	}
#endif // WR_EFFECT_NIAGARA

	// [ 2019-11-13 : animal14 ] 이벤트 바인드
	VFX->OnComplete().AddUObject(this, &AWRCharacter::CompleteDissolve);

	// [ 2019-11-8 : animal14 ] 디졸브 연출
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "Death01_DissolveTime");
	ActorComponentManageMaterial->Spawn(EWRManageMaterial::Dissolve, Duration);

	IsDeadPlay = false;
}

void AWRCharacter::CompleteSpawn()
{
	// [ 2019-11-22 : animal14 ] 이벤트 언바인딩
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}
	ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Spawn).RemoveAll(this);

	// [ 2019-11-22 : animal14 ] 
	//CreateComponent<UWRActorComponentFSM>(EWRCharacterComponent::FSM, true);
}

void AWRCharacter::CompleteDissolve()
{
	// [ 2019-10-22 : animal14 ] 액터 반환
	WRCharacterManager::Get()->Remove(this);
}

void AWRCharacter::CompleteHit()
{
	this->SetMaterialParameterValue("HitPart", 0.0F);

	// [ 2019-11-22 : animal14 ] 이벤트 언바인딩
	UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}
	ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Hit).RemoveAll(this);
}

void AWRCharacter::SetWhiteIn()
{

}

void AWRCharacter::FinishRestraint()
{
	if (RestraintHandle.IsValid() == false)
	{
		return;
	}

	// [ 2019-12-11 : animal14 ] 움직임 제한
	WRInputSystemManager::Get()->SetLocationLock(false);

	// [ 2019-12-11 : animal14 ] UI
	WRTutorialManager::Get()->CloseTutorialAlert();

	WRActorUtility::ClearTimer(RestraintHandle);
}

void AWRCharacter::FinishDrunk()
{
	if (DrunkHandle.IsValid() == false)
	{
		return;
	}

	WRActorUtility::ClearTimer(DrunkHandle);

	UGameplayStatics::DeactivateReverbEffect(this, TEXT("BeDrunk"));

	if (this->IsMe() == false)
	{
		return;
	}

	WRMaterialParameterCollectionManager::Get()->SetDrunk(0.0F);
}

void AWRCharacter::SetMoveAnimType(const EWRMoveAnimType IN InMoveAnimType)
{
	MoveAnimType = InMoveAnimType;

	if (MoveAnimType == EWRMoveAnimType::None)
	{
		MoveAnimType = EWRMoveAnimType::Montage;
	}
}

FTransform AWRCharacter::GetInteractionTransform()
{
	FTransform WCTransform;

	return WCTransform;
}

void AWRCharacter::SetRotationForWhenArrived()
{
	if (this->GetTargetCharacter() != nullptr)
	{
		FVector TargetLocation = this->GetTargetCharacter()->GetActorLocation();
		FRotator LookAtRotation = GetLookAtRotator(TargetLocation);
		this->SetRotation(LookAtRotation);
	}
}

void AWRCharacter::RemoveCombinedUI()
{
	WRUIManager::Get()->CloseAllUIFromCharacter(this);
}


//====================================================================================
// [ 2020-3-16 : kyu ] Hook 관련 공용화...나중에 Component로 뺴야겠다....
//====================================================================================

bool AWRCharacter::IsHookableState()
{
	// 갈고리 걸 수 없는 상태 체크 필요
	if (GetHookType() == EWRHookType::None)
		return false;

	if (WRCharacterManager::IsValid() == false)
		return false;

	if (WRCharacterManager::Get()->IsEquippedHook() == false)
		return false;

	if (WRCharacterManager::Get()->IsHookableRange(this) == false)
		return false;

	return true;
}

void AWRCharacter::OnHookEventToTarget(AWRCharacter* IN InHookCaster, EWRCollisionType IN InConnectedCollisionType)
{
	// 시전자의 갈고리 Pull, Push 발동
	UWRCapsuleCollisionComponent* pConnectedCollisionComp = FindCapsuleCollisionComponent(InConnectedCollisionType);
	if (pConnectedCollisionComp == nullptr)
		return;

	pConnectedCollisionComp->UpdateCollisionWidget();

	// 이벤트 발생시 Hit 보냄
	if (InHookCaster != nullptr && InHookCaster->IsMe())
	{
		WRCombatInfo* LastCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InHookCaster->GetActorID());
		if (LastCombatInfo != nullptr)
		{
			IMMessageSender::ReqestHitResult(InHookCaster, this, LastCombatInfo->GetCombatSeqID(), EWRCollisionType::None);
		}
	}
}

void AWRCharacter::OnHookArrival(const EWRConnectCableEvent IN InEvent, const FVector IN InHitLocation, AWRCharacter* IN InHookCaster)
{
	// 시전자의 갈고리가 박힘
	if (TargetedCollision != nullptr)
	{
		TargetedCollision->UpdateCollisionWidget();
	}
}

void AWRCharacter::OnHookCutted()
{
	// 시전자의 갈고리가 끊김
	if (TargetedCollision != nullptr)
	{
		TargetedCollision->UpdateCollisionWidget();
	}
}

void AWRCharacter::UpdateRecognitionArea()
{
	if (WRCharacterManager::IsValid() == false)
		return;

	if (WRCharacterManager::Get()->IsEquippedHook() == false)
	{
		if (HighLightType != EWRObjectHighLight::None)
		{
			SetHighLight(EWRObjectHighLight::None);
		}
		return;
	}

	AWRCharacterMine* pMyPlayer = WRCharacterManager::Get()->GetMyCharacter();
	if (pMyPlayer == nullptr)
		return;

	if (IsHookableState() == false)
	{
		SetHighLight(EWRObjectHighLight::None);
		return;
	}

	if (TargetedCollision == nullptr)
	{
		if (pMyPlayer->IsRecognizableRange(this))
		{
			SetHighLight(EWRObjectHighLight::Recognition);
		}
		else
		{
			SetHighLight(EWRObjectHighLight::None);
		}
	}
}

//====================================================================================

void AWRCharacter::SetTargetAID(const AID_t IN InTargetAID)
{
	if (TargetAID == InTargetAID)
		return;

	if (InTargetAID == INVALID_AID)
	{
		TargetAID = InTargetAID;
		return;
	}

	AWRCharacter* TargetCharacter = WRCharacterManager::Get()->FindbyAID(InTargetAID);
	if (IsPossibleToTarget(TargetCharacter) == false)
		return;

	TargetAID = InTargetAID;
}

void AWRCharacter::OnTargeted(const bool IN InMulti /*= false*/, const bool IN InByMe /*= true*/)
{
	if (InMulti)
	{
		WRSFXManager::Get()->Spawn2D("Multi_Target.Multi_Target");

		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				float TargetSize = 1.f;
				switch (GetCharacterType())
				{
				case EWRCharacterType::ENPC:
				case EWRCharacterType::NPC:
				{
					AWRCharacterNPC* CharacterNPC = Cast<AWRCharacterNPC>(this);
					if (CharacterNPC)
					{
						FWRNPCBaseTable* NPCBaseTable = CharacterNPC->GetNpcBaseTable();
						if (NPCBaseTable)
							TargetSize = NPCBaseTable->TargetSize;
					}
				}
				break;
				default:
					break;
				}

				InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::TargetUI_FixedSize) * 0.0001f * TargetSize); // Manual Set
				InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("View_Target")));

				UWRUIPanel_TargetUI* Panel_TargetUI = Cast<UWRUIPanel_TargetUI>(InCombinedPanelData.UIPanel);
				if (Panel_TargetUI)
				{
					Panel_TargetUI->SetMulti(InMulti);
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::TargetUI, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, this);
		WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	}
}

void AWRCharacter::ReleasedTarget(const bool IN InMulti /*= false*/, const bool IN InByMe /*= true*/)
{
	WRUIManager::Get()->CloseUI(this, EWRUIPanelKind::TargetUI);

}

FTransform AWRCharacter::GetWidgetTransform()
{
	FTransform WCTransform;

	return WCTransform;
}

EWRUIWidgetCreationBitFlag AWRCharacter::GetWidgetComponentShowType()
{
	return EWRUIWidgetCreationBitFlag::None;
}

const AWRCharacter* AWRCharacter::GetTargetCharacter()
{
	if (TargetAID == INVALID_AID)
		return nullptr;

	return WRCharacterManager::Get()->FindbyAID(TargetAID);
}

void AWRCharacter::OnTargetLockChanged(const bool IN bInLcok) const
{
	FWRCombinedPanelData* CombinedPanelData = WRUIManager::Get()->FindUIData(this, EWRUIPanelKind::TargetUI);
	if (CombinedPanelData && CombinedPanelData->Is3DWidget())
	{
		UWRUIPanel_TargetUI* Panel_TargetUI = Cast<UWRUIPanel_TargetUI>(CombinedPanelData->UIPanel);
		if (Panel_TargetUI)
		{
			Panel_TargetUI->OnTargetLockChanged(bInLcok);

			// [ 2019-11-18 : kyu ] 튜토리얼
			WRTutorialManager::Get()->OnEvent(EWRTutorialConditionType::Targeting);
		}
	}
}

bool AWRCharacter::IsPossibleToTarget(const AWRCharacter* IN InTarget)
{
	if (InTarget == nullptr)
		return false;

	// 타겟 가능한가?

	return true;
}

void AWRCharacter::CheckDeadEffect()
{
	FWREnpcTable* InEpncTable = WRTableManager::Get()->FindRow<FWREnpcTable>(this->GetCharacterTableID());
	if (InEpncTable == nullptr)
		return;

	switch (InEpncTable->DeadEffectType)
	{
	case EWRDeadEffectType::slice:
	{
		WRKismetMeshSliceLibrary::BeginSlice(this);

		this->SetVisibility(false);
		this->CompleteDissolve();
	}break;
	case EWRDeadEffectType::Destructible:
	{
		UWRDestructibleComponent* InDestructibleComp = Cast<UWRDestructibleComponent>(this->GetComponentByClass(UWRDestructibleComponent::StaticClass()));

		if (InDestructibleComp != nullptr)
		{
			//InDestructibleComp->BeginDestruction();
		}
		else
		{
			this->CompleteDissolve();
		}
	}
	break;
	default:
	{
		IsDeadPlay = true;
	}break;
	}
}

void AWRCharacter::SetLookIKTarget(AWRCharacter* InTarget, FName InSocketName)
{
	LookTarget = InTarget;
	SocketName = InSocketName;

	if (InTarget != nullptr)
	{
		if (WRCharacterManager::Get()->IsMyCharacter(InTarget))
		{
			MyCharacterSpawnedDelegatehandle = WRCharacterManager::Get()->GetMyCharacterSpawnedDelegate().AddUObject(this, &AWRCharacter::OnMyCharacterSpawned);
		}
	}
	else
	{
		if (MyCharacterSpawnedDelegatehandle.IsValid())
		{
			WRCharacterManager::Get()->GetMyCharacterSpawnedDelegate().Remove(MyCharacterSpawnedDelegatehandle);
		}
	}
}

void AWRCharacter::GetLookIKTarget(AWRCharacter*& OutTarget, FName& OutSocketName)
{
	OutTarget = LookTarget;
	OutSocketName = SocketName;
}

void AWRCharacter::SetSkeletalMeshComponentToFABRIK(USkeletalMeshComponent* InMeshComp /*= nullptr*/)
{
	GetAnimInstance()->SetSkelMeshToFABRIK(InMeshComp);
}

void AWRCharacter::OnMyCharacterSpawned(class AWRCharacterMine* IN InMyNewCharacter)
{
	if (WRCharacterManager::Get()->IsMyCharacter(InMyNewCharacter))
	{
		LookTarget = InMyNewCharacter;
	}
}

//====================================================================================

UWRAnimInstance* AWRCharacter::GetAnimInstance()
{
	if (GetMesh() == nullptr)
		return nullptr;

	return Cast<UWRAnimInstance>(GetMesh()->GetAnimInstance());
}

EWRFSM AWRCharacter::GetCurrentFSMType()
{
	UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return EWRFSM::None;

	if (nullptr == FSMComponent->GetCurrentFSM())
		return EWRFSM::None;

	return FSMComponent->GetFSMType();

}


WRFSMBase* AWRCharacter::GetCurrentFSM()
{
	UWRActorComponentFSM* FSMComponent = GetComponent<UWRActorComponentFSM>();
	if (nullptr == FSMComponent)
		return nullptr;

	return FSMComponent->GetCurrentFSM();
}

void AWRCharacter::SetActorID(AID_t InAID)
{
	ActorID = InAID;
}

AID_t AWRCharacter::GetActorID() const
{
	return ActorID;
}

void AWRCharacter::SetBattleState(bool bFlag)
{
	bBattleState = bFlag;

	// [ 2020-4-10 : kyu ] 임시로 리턴
	if (GetCurrentFSMType() == EWRFSM::Interact)
		return;

	if (GetCurrentFSMType() == EWRFSM::Dance)
		return;

	//술잔같은건 항상 들고 있기때문에 값이 0이 아니면리턴
	if (GetTemporaryTID() != INVALID_TABLE_TID)
		return;

	//무기를 아무것도 장착하지 않은 상태면 리턴
	if (GetRightWeaponTID() == INVALID_TABLE_TID && GetLeftWeaponTID() == INVALID_TABLE_TID)
		return;

	if (bFlag)
	{
		DetermineFSMSignal(EWRFSM::Arm);
	}
	else
	{
		DetermineFSMSignal(EWRFSM::Disarm);
	}
}

bool AWRCharacter::IsInBattle()
{
	return bBattleState;
}

void AWRCharacter::SetRotationLerpToTarget(AWRCharacter* InTarget, FWRRotationEndDelegate InRotationEndDelegate /*= nullptr*/, FName InSocketName /*= NAME_None*/)
{
	FName InHeadName = TEXT("head");

	AWRCharacterNPC* InNPC = Cast<AWRCharacterNPC>(InTarget);

	if (InNPC != nullptr)
	{
		FWRNpcTable* NPCTable = WRTableManager::Get()->FindRow<FWRNpcTable>(InTarget->GetCharacterTableID());
		if (NPCTable != nullptr)
			InHeadName = NPCTable->HeadBoneName;
	}

	SetLookIKTarget(InTarget, InHeadName);
	IsOnRotationLerp = true;

	RotationEndDelegate = InRotationEndDelegate;
}

//====================================================================================
// [ 2020-04-09 : dlwlgus9125 ] Rotation Lerp
bool AWRCharacter::CheckShouldTurnAround(FRotator& InLookRotation)
{
	FRotator Gap = InLookRotation - this->GetActorRotation();
	if (FMath::Abs(Gap.Yaw) > WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKLimitYawForDialog))
	{
		return true;
	}
	return false;
}

void AWRCharacter::ProcessRotationLerp()
{
	if (this->IsAIMoveTo() == true || this->IsAwayStep == true || IsOnRotationLerp == false)
		return;

	AWRCharacter* InLookTarget = nullptr;
	FName InSocketName = NAME_None;
	FRotator LookRotation = FRotator::ZeroRotator;
	this->GetLookIKTarget(InLookTarget, InSocketName);

	if (InLookTarget != nullptr)
	{
		LookRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), InLookTarget->GetActorLocation());
		ForTurnAroundRotator = LookRotation;
	}
	else
	{
		LookRotation = ForTurnAroundRotator;
	}

	if (this->CheckShouldTurnAround(LookRotation) == true)
	{
		if (IsShouldTurnAround == false)
		{
			IsShouldTurnAround = true;
		}
	}
	else
	{
		if (this->IsLookingTarget() == false)
		{
			if (IsShouldTurnAround == false)
			{
				IsShouldTurnAround = true;
			}
		}
	}

	if (LookTargetAlways == true)
		IsShouldTurnAround = LookTargetAlways;

	if (IsShouldTurnAround == false)
	{
		if (RotationEndDelegate.IsBound())
		{
			RotationEndDelegate.Execute();
			RotationEndDelegate.Unbind();
		}
		return;
	}

	FRotator LookAngle = FRotator(0.f, FMath::RInterpTo(this->GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), WRConsoleManager::Get()->GetFloat(EWRConsole::IK_LookIKBlendSpeed)).Yaw, 0.f);

	this->SetActorRotation(LookAngle);
	bool StopRotation = UKismetMathLibrary::EqualEqual_RotatorRotator(LookRotation, this->GetActorRotation(), 5.f);
	if (StopRotation == true)
	{
		IsShouldTurnAround = false;

		if (RotationEndDelegate.IsBound())
		{
			RotationEndDelegate.Execute();
			RotationEndDelegate.Unbind();
		}

		if (this->IsLookingTarget() == false)
		{
			IsOnRotationLerp = false;
		}
	}
}

//====================================================================================

void AWRCharacter::MakeDebugString(TArray<FString>& OUT OutString)
{
	// AID 
	OutString.Add(FString::Printf(TEXT("AID : %d"), GetActorID()));

	// Location
	OutString.Add(FString::Printf(TEXT("Location : %.0f, %.0f, %.0f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));
}

void AWRCharacter::SetOwnedVehicleAID(AID_t InAID)
{
	OwnedVehicleAID = InAID;
}

void AWRCharacter::SetBoardingVehicleAID(AID_t InAID)
{
	BoardingVehicleAID = InAID;
}

AID_t AWRCharacter::GetOwnedVehicleAID()
{
	return OwnedVehicleAID;
}

AID_t AWRCharacter::GetBoardingVehicleAID()
{
	return BoardingVehicleAID;
}

AWRVehicle* AWRCharacter::GetOwnedVehicle()
{
	return WRVehicleManager::Get()->FindVehicleByAID(OwnedVehicleAID);
}

AWRVehicle* AWRCharacter::GetBoardingVehicle()
{
	return WRVehicleManager::Get()->FindVehicleByAID(BoardingVehicleAID);
}

bool AWRCharacter::IsVehicleDriver()
{
	return WRVehicleManager::Get()->IsDriver(GetActorID(), GetBoardingVehicleAID());
}

void AWRCharacter::SetCollisionTargetting(const ECollisionEnabled::Type InCollisionState)
{
	if (WRTargettingCapsule != nullptr)
		WRTargettingCapsule->SetCollisionEnabled(InCollisionState);
}

void AWRCharacter::SetCharacterInvisible(bool IsCharacterVisible, bool IsUseFade /*= true*/)
{
	if (this->IsMe() == true)
		return;

	if (this->GetMesh() == nullptr)
		return;

	if (IsUseFade == true)
	{
		UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
		if (ActorComponentManageMaterial == nullptr)
		{
			return;
		}

		float Duration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "Death01_DissolveTime");

		if (IsCharacterVisible == true)
		{
			//WRCharacterManager::Get()->SetDistanceCulling(false, this);
			
			if (this->IsPlayer() == true)
			{				
				ActorComponentManageMaterial->Remove(EWRManageMaterial::Dissolve, Duration);				
			}
			else
			{
				ActorComponentManageMaterial->Spawn(EWRManageMaterial::Spawn, Duration);
			}			
		}
		else
		{
			if (this->IsPlayer() == true)
			{				
				ActorComponentManageMaterial->Spawn(EWRManageMaterial::Dissolve, Duration);
				ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Dissolve).AddWeakLambda(this, [this]()->void
				{
					//WRCharacterManager::Get()->SetDistanceCulling(true, this);

					this->SetMaterialParameterValue("Dissolve", 0.f); // dissolve Material parameter 초기화

					UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
					if (ActorComponentManageMaterial == nullptr)
					{
						return;
					}
					ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Dissolve).RemoveAll(this);
				});
			}
			else
			{
				ActorComponentManageMaterial->Remove(EWRManageMaterial::Spawn, Duration);
				ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Spawn).AddWeakLambda(this, [this]()->void
				{
					//WRCharacterManager::Get()->SetDistanceCulling(true, this);
					UWRActorComponentManageMaterial* ActorComponentManageMaterial = this->GetComponent<UWRActorComponentManageMaterial>();
					if (ActorComponentManageMaterial == nullptr)
					{
						return;
					}
					ActorComponentManageMaterial->OnComplete(EWRManageMaterial::Spawn).RemoveAll(this);
				});
			}
		}
	}
	else
	{
		//WRCharacterManager::Get()->SetDistanceCulling(!IsCharacterVisible, this);
		this->SetMaterialParameterValue("Dissolve", IsCharacterVisible);
	}

	// 투명화 완료되면 콜리전 비활성화
	if (IsCharacterVisible == true)
	{
		this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		this->SetCollisionTargetting(ECollisionEnabled::QueryOnly);
	}
	else
	{
		this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->SetCollisionTargetting(ECollisionEnabled::NoCollision);
	}

	IsCharacterActive = IsCharacterVisible;
}

void AWRCharacter::AddVisibilityBit(const EWRVisibility InBit)
{
	VisibilityBits = (EWRVisibility)WREnumUtility::AddBit((uint64)VisibilityBits, (uint64)InBit);
}

void AWRCharacter::RemoveVisibilityBit(const EWRVisibility InBit)
{
	VisibilityBits = (EWRVisibility)WREnumUtility::RemoveBit((uint64)VisibilityBits, (uint64)InBit);
}

bool AWRCharacter::IsVisibilityBits(const EWRVisibility InBit)
{
	return WREnumUtility::IsBit((uint64)VisibilityBits, (uint64)InBit);
}

