// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacterPlayer.h"
//#include "ActorComponent/Character/WRActorComponentVR.h"
#include "GameFramework/CharacterMovementComponent.h"

#include <Components/InputComponent.h>
#include <Components/WidgetInteractionComponent.h>
#include "Components/StaticMeshComponent.h"
#include "Component/Character/WRActorComponentVR.h"
#include "Component/Character/WRActorComponentAttack.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponentFSM.h"
#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentConnectbyCable.h"
#include "Component/Character/WRActorComponentDanceModeForMine.h"
#include "Component/Character/WRActorComponentPlayerSocial.h"
#include "Component/Character/WRActorComponentLadder.h"
#include "Component/Character/WRActorComponentInteraction.h"
#include "Component/Character/WRActorComponentVoiceChatAudio.h"

#include "Camera/CameraComponent.h"

#include "Manager/WRPopupManager.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRSkillManager.h"
#include "Manager/WREnvInteractionManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRPopupManager.h"
#include "Manager/WRInventoryManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Table/WRPlayerTable.h"

#include "HeadMountedDisplayFunctionLibrary.h"

#include "WidgetComponent/WRWidgetComponentBase.h"
#include "WRCharacter.h"
#include "WRCharacterIO.h"
#include "../Animation/WRAnimInstance.h"
#include "../FSM/WRFSMBase.h"

#include "Engine/Engine.h"
#include <Animation/AnimMontage.h>
#include "Define/WRPathDefine.h"
#include <Kismet/KismetMathLibrary.h>

#include "CombatSystem/Combat/WRCombatInfo.h"
#include "AIController.h"
#include "Network/IMMessageSender.h"
#include "../EnvActor/Ladder/WRLadder.h"
#include "../EnvActor/WREnvVehicleBase.h"
#include "Enum/EWRItem.h"
#include "Component/Character/WRActorComponentBuff.h"
#include "../EnvActor/MovingWalkway/WREnvMovingWalkway.h"
#include "../EnvActor/SecretDialog/WRSecretDialogArea.h"
#include "../EnvActor/SecretDialog/WRSecretDialogPortal.h"
#include "Manager/WRVoiceContentsManager.h"
#include "Component/Character/WRActorComponentMic.h"
#include "Utility/WRActorUtility.h"


// For Cpp
DEFINE_LOG_CATEGORY(WRCharacterPlayer)



AWRCharacterPlayer::AWRCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: AWRCharacter(ObjectInitializer)
{
	CharacterType = EWRCharacterType::Player;

	bSyncHMDHead = true;
}


void AWRCharacterPlayer::OnPostSpawn()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;
	Super::OnPostSpawn();

	HMDRotationToUpdate = FRotator::ZeroRotator;
}

void AWRCharacterPlayer::OnPreDestroy()
{
	HMDRotationToUpdate = FRotator::ZeroRotator;

	Super::OnPreDestroy();
}

void AWRCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
//====================================================================================

void AWRCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickAddMovementInput(DeltaTime);

	TickSyncHMD(DeltaTime);

	// for Debug
	FWRDebugUIData data;
	data.Flag = EWRDebugUIBitFlag::Player;
	data.Actor = this;
	MakeDebugString(data.ArrayContents);
	data.Offset = FVector(0, 0, 50);
	WRUIManager::Get()->UpdateDebugUI(data);
}

bool AWRCharacterPlayer::AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType /*= EWRActionType::en::RUN*/)
{
	if (Super::AIMoveTo(InDestLocation, InMoveType))
	{
		return true;
	}

	return false;
}

void AWRCharacterPlayer::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type MovementResult)
{
	Super::OnMoveCompleted(RequestID, MovementResult);

	switch (MovementResult)
	{
	case EPathFollowingResult::Success:
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::Aborted:
	{

	}
	break;
	case EPathFollowingResult::OffPath:
	default:
	{

	}
	break;
	}
}

void AWRCharacterPlayer::BeginFSMAttack(const WRFSMInfo& InFSM)
{
	Super::BeginFSMAttack(InFSM);
}

void AWRCharacterPlayer::TickFSMAttack(float InDeltaTime)
{
	Super::TickFSMAttack(InDeltaTime);
}

void AWRCharacterPlayer::EndFSMAttack(const WRFSMInfo& InFSM)
{
	Super::EndFSMAttack(InFSM);
}

void AWRCharacterPlayer::SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart EquipPart, ECollisionEnabled::Type InType)
{
	UWRActorComponentEquipmentLook* EquipComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipComponent == nullptr)
		return;

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	if (nullptr == CombatInfo)
		return;

	FWRSkillTable* SkillTable = CombatInfo->GetSkillTable();
	if (nullptr == SkillTable)
		return;

	EquipComponent->SetCollisionEnable(EquipPart, InType);
	//EquipComponent->SetCollisionEnable(SkillTable->EquipType, InType);
}

void AWRCharacterPlayer::SetWeaponCollisionBasedOnSide(EWREquipPart InPart, ECollisionEnabled::Type InType)
{
	UWRActorComponentEquipmentLook* EquipComponent = GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipComponent == nullptr)
		return;

	EquipComponent->SetCollisionEnable(InPart, InType);
}

void AWRCharacterPlayer::OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlapCapsuleAttacking(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (this == Other)
		return;
}

void AWRCharacterPlayer::OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlapCapsuleHitting(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (this == Other)
		return;
}

void AWRCharacterPlayer::OnEndOverlapHittingCapsule(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnEndOverlapHittingCapsule(OverlappedComp, Other, OtherComp, OtherBodyIndex);

	if (this == Other)
		return;
}

void AWRCharacterPlayer::TickAddMovementInput(float DeltaTime)
{
	if (IsPlayer() == false || IsMe())
	{
		return;
	}

	if (ReceivedCharLocation != FVector::ZeroVector)
	{
		static float InterpPlayerSpeed = 1.5f;
		FVector Test = GetActorLocation();
		FVector InterpVec = UKismetMathLibrary::VInterpTo(Test, ReceivedCharLocation, DeltaTime, InterpPlayerSpeed);
		SetLocation(InterpVec);

		int32 aa = 0;
	}

	if (0.0f != MoveForwardValue)
	{
		AddMovementInput(GetActorForwardVector(), MoveForwardValue);

		UpdateHookMoving();
	}

	if (0.0f != MoveRightValue)
	{
		AddMovementInput(GetActorRightVector(), MoveRightValue);

		UpdateHookMoving();
	}
}

//====================================================================================
void AWRCharacterPlayer::OnHookThrow()
{
	UWRActorComponentConnectbyCable* CableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
	{
		return;
	}

	CableComponent->Cut();

	FStringAssetReference ChainWeaponStrAsset = FStringAssetReference("/Game/Asset/Data/Asset/ChainWeapon/TestChainWeapon.TestChainWeapon");
	bool bSuccess = CableComponent->ConnectByAsset((AWRCharacter*)GetTargetCharacter(), Cast<UWRChainWeaponAsset>(ChainWeaponStrAsset.TryLoad())
		, FWRConnectCableEventDelegate::CreateUObject(this, &AWRCharacter::OnConnectCableEvent), eHookPart);

	CableComponent->EnableCableCollision(this);

	if (bSuccess == false)
	{
		DetermineFSMSignal(EWRFSM::Idle, true);
		return;
	}

	// + 보조무기 숨기기
	SetEquipPartVisibility(EWREquipPart::LeftHand, false);
}

void AWRCharacterPlayer::OnHookReturn()
{
	UWRActorComponentConnectbyCable* CableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
	{
		return;
	}

	CableComponent->ReturnToBase();

	// + 보조무기 나타내기
	SetEquipPartVisibility(EWREquipPart::LeftHand, true);
}

void AWRCharacterPlayer::OnHookEventToCaster()
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

	switch (pTarget->GetHookType())
	{
	case EWRHookType::Activate:
	{

	}
	break;
	case EWRHookType::Pull:
	{
		// 		if (SkillTable->TargetDynamicMovementTID != INVALID_TABLE_TID)
		// 		{
		// 			UWRActorComponentDynamicMovement* DynamicMoveComponent = pTarget->GetComponent<UWRActorComponentDynamicMovement>();
		// 			if (DynamicMoveComponent)
		// 			{
		// 				DynamicMoveComponent->DetermineDynamicMovement(SkillTable->TargetDynamicMovementTID, this, false);
		// 			}
		// 		}		
	}
	break;
	case EWRHookType::Dash:
	{
		if (SkillTable->OwnerDynamicMovementTID != INVALID_TABLE_TID)
		{
			UWRActorComponentDynamicMovement* DynamicMoveComponent = GetComponent<UWRActorComponentDynamicMovement>();
			if (DynamicMoveComponent)
			{
				if (DynamicMoveComponent->DetermineDynamicMovement(SkillTable->OwnerDynamicMovementTID, pTarget, false))
				{
					WRFSMInfo FSMInfo;
					FSMInfo.FSMType = EWRFSM::MaintainingSkill;
					FSMInfo.SkillTableID = CombatInfo->GetCombatSeqParam().SkillTID;
					FSMInfo.bImmediateUpdate = true;
					FSMInfo.JumpToSectionName = FString::FromInt(CombatInfo->GetCombatSeqParam().SkillTID) + "_loop";
					DetermineFSMSignal(FSMInfo);
				}
			}
		}

		// [ 2019-12-19 : kyu ] 나 일때만 이펙트
		if (IsMe())
		{
			if (WRConsoleManager::Get()->GetBool(EWRConsole::BlackOutIn_Enable))
			{
				float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_HookDashEffectDuration);
				WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::DashOut, Duration);

				// [ 2020-2-21 : animal14 ] 이벤트 바인딩
				WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::DashOut).AddUObject(this, &AWRCharacterPlayer::DashIn);
			}
			else
			{
				float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_HookDashEffectDuration);
				WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::Dash, Duration);
			}
		}
	}
	break;
	}

	CutOffHookChain(true, nullptr, IsMe()); // 이벤트 후 끊기
}

void AWRCharacterPlayer::OnConnectCableEvent(EWRConnectCableEvent InEventType)
{
	UWRActorComponentConnectbyCable* ConnectCableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (nullptr == ConnectCableComponent ||
		nullptr == ConnectCableComponent->GetCableTarget())
	{
		return;
	}

	if (ConnectCableComponent->GetCableTarget()->GetCharacterType() != EWRCharacterType::IO)
		return;

	if (InEventType == EWRConnectCableEvent::ArrivalToTarget)
	{
		ConnectCableComponent->AttachAtoB(EWRConnectByCableActor::Chain, EWRConnectByCableActor::Target, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false));
		vStartHookLocation = GetActorLocation();

		// [ 2019-11-4 : kyu ] 박혔을 때 대상에게 설정된 연출 추가
		if (ConnectCableComponent->GetCableTarget() != nullptr)
		{
			ConnectCableComponent->GetCableTarget()->OnHookArrival(InEventType, ConnectCableComponent->GetImpactLocation(), this);
		}
	}
	else if (InEventType == EWRConnectCableEvent::ArrivalToCaster)
	{
		vStartHookLocation = FVector::ZeroVector;
		ConnectCableComponent->Cut(true);
	}
}

void AWRCharacterPlayer::CutOffHookChain(const bool bInImmediately /* = false */, AWRCharacter* IN InTarget /* = nullptr */, const bool IN InByMe /* = true */, const bool IN InIdle /* = false */)
{
	UWRActorComponentConnectbyCable* CableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
		return;

	if (CableComponent->IsWorking() == false)
		return;

	AWRCharacter* CableTarget = CableComponent->GetCableTarget();

	// 대상이 지정되어있으면 대상과 동일할 때만 끊음
	if (InTarget != nullptr)
	{
		if (InTarget != CableTarget)
			return;
	}

	// 끊음
	CableComponent->Cut(bInImmediately);

	// [ 2019-11-25 : kyu ] 대상에게 끊김 알림
	if (CableTarget != nullptr)
		CableTarget->OnHookCutted();

	// + 보조무기 나타내기
	SetEquipPartVisibility(EWREquipPart::LeftHand, true);

	// [ 2020-1-13 : kyu ] 갈고리 파츠정보 삭제
	SetHookPart(EWRCollisionType::None);

	if (InIdle)
	{
		DetermineFSMSignal(EWRFSM::Idle, true);
	}

	// [ 2020-1-21 : kyu ] 타의로 갈고리 끊어질 때 통보
	if (IsMe() && InByMe == false)
	{
		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
		if (CombatInfo)
		{
			IMMessageSender::RequestUseSkillEnd(this, CombatInfo->GetCombatSeqID());
		}
	}
}

void AWRCharacterPlayer::HitHook()
{
	UWRActorComponentConnectbyCable* ConnectCableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (nullptr == ConnectCableComponent ||
		nullptr == ConnectCableComponent->GetCableTarget())
	{
		return;
	}

	bool bAttached = ConnectCableComponent->AttachAtoB(EWRConnectByCableActor::Chain, EWRConnectByCableActor::Target, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false));
	vStartHookLocation = GetActorLocation();

	// [ 2019-11-4 : kyu ] 박혔을 때 대상에게 설정된 연출 추가
	if (ConnectCableComponent->GetCableTarget() != nullptr)
	{
		ConnectCableComponent->GetCableTarget()->OnHookArrival(EWRConnectCableEvent::ArrivalToTarget, ConnectCableComponent->GetImpactLocation(), this);
	}

	if (bAttached)
	{
		ConnectCableComponent->EnableCableCollision(nullptr);
	}
}

const EWRHookType AWRCharacterPlayer::GetHookType()
{
	FWRPlayerTable* PlayerTable = WRTableManager::Get()->FindRow<FWRPlayerTable>(CharacterTableID);
	if (PlayerTable)
	{
		return PlayerTable->HookType;
	}

	return EWRHookType::None;
}

bool AWRCharacterPlayer::IsHookableState()
{
	if (AWRCharacter::IsHookableState() == false)
		return false;

	// [ 2020-4-29 : kyu ] 사다리 이용중이면 갈고리를 걸 수 없다
	UWRActorComponentLadder* LadderComponent = GetComponent<UWRActorComponentLadder>();
	if (LadderComponent != nullptr)
	{
		if (LadderComponent->IsUsingLadder())
			return false;
	}

	// [ 2020-4-29 : kyu ] 점유중이면 갈고리를 걸 수 없다
	UWRActorComponentInteraction* InteractionComponent = GetComponent<UWRActorComponentInteraction>();
	if (InteractionComponent != nullptr)
	{
		if (InteractionComponent->GetOccupiedIO() != nullptr)
			return false;
	}

	return true;
}

void AWRCharacterPlayer::UpdateHookMoving()
{
	UWRActorComponentConnectbyCable* CableComponent = GetComponent<UWRActorComponentConnectbyCable>();
	if (CableComponent == nullptr)
		return;

	if (CableComponent->IsConnecting() == false)
		return;

	if (IsMe() == false)
		return;

	if (FVector::Dist(vStartHookLocation, GetActorLocation()) >= HookMovingLimitDist)
	{
		CutOffHookChain(false, nullptr, false, true); // 일부러 끊은게 아님
// 		WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
// 		if (CombatInfo)
// 		{
// 			IMMessageSender::RequestUseSkillEnd(this, CombatInfo->GetCombatSeqID());
// 		}
	}
}

void AWRCharacterPlayer::OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InInform.skillTID, GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return;

	switch (SkillRecord->SkillType)
	{
	case EWRSkillType::Hook:
	{
		SetHookPart((EWRCollisionType)InInform.collisionType);
	}
	break;
	default:
	{
	}
	break;
	}

	Super::OnInformUseSkill(InInform);
}

void AWRCharacterPlayer::OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InInform.skillTID, GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return;

	switch (SkillRecord->SkillType)
	{
	case EWRSkillType::Hook:
	{
		SetHookPart((EWRCollisionType)InInform.collisionType);
	}
	break;
	// 	case EWRSkillType::ChainProjectile:
	// 	{
	// 		UWRActorComponentEquipmentLook* EquipComponent = GetComponent<UWRActorComponentEquipmentLook>();
	// 		if (EquipComponent)
	// 		{
	// 			EquipComponent->SetWeaponVisibility(EWREquipPart::LeftHand, false);
	// 		}
	// 	}
	// 	break;
	default:
	{
	}
	break;
	}

	Super::OnInformBeginSkill(InInform);
}

void AWRCharacterPlayer::OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform)
{
	Super::OnInformEndSkill(InInform);

	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindBySeqID(InInform.casterAID, InInform.skillSeq);
	if (CombatInfo && CombatInfo->GetSkillTable())
	{
		if (CombatInfo->GetSkillTable()->SkillType == EWRSkillType::ChainProjectile)
		{
			WRCombatManager::Get()->Unregister(CombatInfo, true);
		}
	}
}

void AWRCharacterPlayer::OnInformHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform)
{
	Super::OnInformHitResult(InInform);
}

void AWRCharacterPlayer::OnInformPlayerMove(const pt_Game_User::Inf_PlayerMove_G2U& InInform)
{
#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	ClearAIMoveTo();
#endif // DEF_FIX_CLEARAIMOVETO

	DetermineFSMSignal(EWRFSM::Move);

	//방향
	FRotator CharRotator(0.0f, InInform.rotate, 0.0f);
	SetRotation(CharRotator);

	MoveForwardValue = InInform.forwardScale;
	MoveRightValue = InInform.rightScale;
	ReceivedCharLocation = FVector(InInform.posX, InInform.posY, InInform.posZ);
	WRActorUtility::ModifyZHeightForClient(this, ReceivedCharLocation, ReceivedCharLocation.Z);

	if (MoveForwardValue != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), MoveForwardValue);
	}

	if (MoveRightValue != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), MoveRightValue);
	}
}

void AWRCharacterPlayer::OnInformPlayerMoveStop(const pt_Game_User::Inf_PlayerStop_G2U& InInform)
{
	if (GetCurrentFSMType() == EWRFSM::Move)
	{
		DetermineFSMSignal(EWRFSM::Idle);
	}

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	if (IsAIMoveTo())
	{
		ClearAIMoveTo();
	}
#endif // DEF_FIX_CLEARAIMOVETO

	ResetMovementValue2Sync();

	//좌표 및 방향
	FRotator LastRotation(0.0f, InInform.rotate, 0.0f);
	FVector LastPosition(InInform.posX, InInform.posY, InInform.posZ);	
	float NormalizedRotate = FRotator::NormalizeAxis(LastRotation.Yaw);
	FRotator InterpRotation(0.0f, NormalizedRotate, 0.0f);

	AWRCharacter* ParentCharacter = GetParent();
	ParentCharacter->SetLocationAndRotation(LastPosition, InterpRotation);
}

void AWRCharacterPlayer::OnInformPlayerMoveInVehicle(const pt_Game_Vehicle::Inf_PlayerMoveInTransport_G2U& InInform)
{
	if (InInform.vehicleAID == INVALID_AID)
		return;

	AWREnvVehicleBase* FoundEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(InInform.vehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	// Offset
	SetEnvVehicleTransformOffset(true, FVector(InInform.localPosX, InInform.localPosY, InInform.localPosZ), InInform.localAngle);

	MoveForwardValue = InInform.forwardScale;
	MoveRightValue = InInform.rightScale;
	DetermineFSMSignal(EWRFSM::Move);

	if (InInform.forwardScale != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), InInform.forwardScale);
	}

	if (InInform.forwardScale != InInform.rightScale)
	{
		AddMovementInput(GetActorRightVector(), InInform.rightScale);
	}
}

void AWRCharacterPlayer::OnInformPlayerStopInVehicle(const pt_Game_Vehicle::Inf_PlayerStopInTransport_G2U& InInform)
{
	if (GetCurrentFSMType() == EWRFSM::Move)
	{
		DetermineFSMSignal(EWRFSM::Idle);
	}

	ResetMovementValue2Sync();

	if (InInform.vehicleAID == INVALID_AID)
		return;

	AWREnvVehicleBase* FoundEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(InInform.vehicleAID);
	if (FoundEnvVehicle == nullptr)
		return;

	// Offset
	SetEnvVehicleTransformOffset(true, FVector(InInform.localPosX, InInform.localPosY, InInform.localPosZ), InInform.localAngle);
}

void AWRCharacterPlayer::OnInformPlayerMoveInMW(const pt_Game_Vehicle::Inf_PlayerMoveInMW_G2U& InInform)
{
	SetEnvVehicleTransformOffset(true, FVector(InInform.localPosX, InInform.localPosY, InInform.localPosZ), InInform.localAngle);

	MoveForwardValue = InInform.forwardScale;
	MoveRightValue = InInform.rightScale;
	DetermineFSMSignal(EWRFSM::Move);

	if (InInform.forwardScale != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), InInform.forwardScale);
	}

	if (InInform.forwardScale != InInform.rightScale)
	{
		AddMovementInput(GetActorRightVector(), InInform.rightScale);
	}
}

void AWRCharacterPlayer::OnInformPlayerStopInMW(const pt_Game_Vehicle::Inf_PlayerStopInMW_G2U& InInform)
{
	if (GetCurrentFSMType() == EWRFSM::Move)
	{
		DetermineFSMSignal(EWRFSM::Idle);
	}

	ResetMovementValue2Sync();

	// Offset
	SetEnvVehicleTransformOffset(true, FVector(InInform.localPosX, InInform.localPosY, InInform.localPosZ), InInform.localAngle);
}

void AWRCharacterPlayer::OnInformSyncHMD(const pt_Game_User::Inf_SyncHMD_G2U& InInform)
{
	if (InInform.ActorID != this->ActorID)
	{
		UE_LOG(WRCharacterPlayer, Display, TEXT("AWRCharacterPlayer::OnInformSyncHMD - Mismatch ActorID [Inf : %d] / [Char : %d]"), InInform.ActorID, this->ActorID);
		return;
	}

	GoalHMDRotation = FRotator(InInform.RotationY, InInform.RotationZ, InInform.RotationX);
	if (LastHMDRotation == FRotator::ZeroRotator)
	{
		LastHMDRotation = GoalHMDRotation;
	}
}

bool AWRCharacterPlayer::TickSyncHMD(float InDeltaTime)
{
	static float TestInterpSpeed = 200.0f;
	//이전틱에 세팅된 goalRot이 현재 rot이 되므로
	HMDRotationToUpdate = UKismetMathLibrary::RInterpTo_Constant(LastHMDRotation, GoalHMDRotation, InDeltaTime, TestInterpSpeed);

	LastHMDRotation = HMDRotationToUpdate;

	return true;
}

//====================================================================================

//====================================================================================
// [ 2019-7-19 : magedoga ] Component
void AWRCharacterPlayer::BuildCustomComponents()
{
	Super::BuildCustomComponents();

	CreateComponent<UWRActorComponentAttack>(EWRCharacterComponent::Attack);
	CreateComponent<UWRActorComponentEquipmentLook>(EWRCharacterComponent::EquipmentLook);
	CreateComponent<UWRActorComponentConnectbyCable>(EWRCharacterComponent::ConnectbyCable);
	CreateComponent<UWRActorComponentPlayerSocial>(EWRCharacterComponent::PlayerSocial, true);
	CreateComponent<UWRActorComponentLadder>(EWRCharacterComponent::Ladder);
	CreateComponent<UWRActorComponentInteraction>(EWRCharacterComponent::Interaction, true);
	CreateComponent<UWRActorComponentBuff>(EWRCharacterComponent::Buff, true);
	CreateComponent<UWRActorComponentMic>(EWRCharacterComponent::Mic);

	if (GetComponent<UWRActorComponentDanceModeForMine>() == nullptr)
	{
		CreateComponent<UWRActorComponentDanceMode>(EWRCharacterComponent::DanceMode);
	}

//#if !PLATFORM_PS4
	CreateComponent<UWRActorComponentVoiceChatAudio>(EWRCharacterComponent::VoiceChatAudio, true);
//#endif
}

//====================================================================================

void AWRCharacterPlayer::SetForceStop()
{
	MoveForwardValue = 0.f;
	MoveRightValue = 0.f;
	ConsumeMovementInputVector();
}

bool AWRCharacterPlayer::IsMoving()
{
	if (MoveForwardValue != 0.0f || MoveRightValue != 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AWRCharacterPlayer::ResetMovementValue2Sync()
{
	MoveForwardValue = 0.0f;
	MoveRightValue = 0.0f;
	ReceivedCharLocation = FVector::ZeroVector;
}

//====================================================================================

void AWRCharacterPlayer::SetEnvVehicleTransformOffset(const bool IN InSet, const FVector IN InOffset /* = FVector::ZeroVector */, const float IN InAngle /* = 0.f */)
{
	EnvVehicleOffset = InOffset;
	EnvVehicleOffsetAngle = InAngle;

	if (InSet)
	{
		if (EnvVehicleAID != INVALID_AID)
		{
			AWREnvVehicleBase* pEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
			if (pEnvVehicle != nullptr)
			{
				FVector vEnvVehicleLocation = pEnvVehicle->GetActorLocation();
				FVector vNewLocation = vEnvVehicleLocation + InOffset;
				SetLocation(vNewLocation);

				// Rotation (-180 ~ 180)
				float fYaw = EnvVehicleOffsetAngle + pEnvVehicle->GetActorRotation().Yaw;
				FRotator NewRotation(0.0f, FRotator::NormalizeAxis(fYaw), 0.0f);
				SetRotation(NewRotation);

				// Start
// 				vEnvVehicleLocation += InOffset;
// 				vEnvVehicleLocation.Z += 100.f;
// 				SetActorLocation(vEnvVehicleLocation);

				// End
// 				vEnvVehicleLocation.Z = fEnvVehicleZ;
// 				SetActorLocation(vEnvVehicleLocation, true, pResult);
			}
		}
		else
		{
			AWREnvMovingWalkway* InMovingwalkway = WREnvInteractionManager::Get()->FindOverlappedMWByCharacterAID(this->GetActorID());
			if (InMovingwalkway != nullptr)
			{
				FVector InBaseOffset = InMovingwalkway->GetActorLocation();
				FVector InNewLocation = InBaseOffset + InOffset;
				SetLocation(InNewLocation);

				// Rotation (-180 ~ 180)
				float InYaw = InAngle + InMovingwalkway->GetActorRotation().Yaw;
				FRotator NewRotation(0.0f, FRotator::NormalizeAxis(InYaw), 0.0f);
				SetRotation(NewRotation);
			}
		}
	}
	else
	{
		SetActorLocation(GetActorLocation());
	}
}

void AWRCharacterPlayer::UpdateEnvVehicleLocationOffset()
{
	if (EnvVehicleAID == INVALID_AID)
		return;

	AWREnvVehicleBase* pEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
	if (pEnvVehicle != nullptr)
	{
		FVector vEnvVehicleLocation = pEnvVehicle->GetActorLocation();
		FVector vNewLocation = vEnvVehicleLocation + EnvVehicleOffset;
		SetLocation(vNewLocation);

		float fYaw = EnvVehicleOffsetAngle + pEnvVehicle->GetActorRotation().Yaw;
		FRotator NewRotation(0.0f, FRotator::NormalizeAxis(fYaw), 0.0f);
		SetRotation(NewRotation);
	}
}

float AWRCharacterPlayer::GetEnvVehicleMoveSpeed()
{
	if (EnvVehicleAID == INVALID_AID)
		return 0.f;

	AWREnvVehicleBase* pEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
	if (pEnvVehicle != nullptr)
	{
		return pEnvVehicle->GetMoveSpeed();
	}
	return 0.f;
}

FVector AWRCharacterPlayer::GetEnvVehicleMoveVector()
{
	if (EnvVehicleAID == INVALID_AID)
		return FVector::ZeroVector;

	AWREnvVehicleBase* pEnvVehicle = WREnvInteractionManager::Get()->FindEnvVehicle<AWREnvVehicleBase>(EnvVehicleAID);
	if (pEnvVehicle != nullptr)
	{
		return pEnvVehicle->GetMoveVector();
	}
	return FVector::ZeroVector;
}

//====================================================================================

void AWRCharacterPlayer::DashIn()
{
	if (this->IsMe() == false)
	{
		return;
	}

	// [ 2019-11-22 : animal14 ] 이벤트 언바인딩
	WRMaterialParameterCollectionManager::Get()->OnComplete(EWRMaterialParameter::DashOut).RemoveAll(this);

	if (this->MoveForwardValue == 0.0F && this->MoveRightValue == 0.0F)
	{
	}
	else
	{
		return;
	}

	// [ 2019-11-8 : animal14 ] 
	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_HookDashEffectDuration);
	WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::DashIn, Duration);
}

void AWRCharacterPlayer::OnSpeakingStartedForVoiceChatting()
{
	if (IsMe())
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::CommonUI_FixedSize) * 0.0001f);
			InCombinedPanelData.WidgetComponentBase->SetAttachSocketName(FName(TEXT("Speaker")));
		}
	});
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::VoiceChatSpeakIcon, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, this);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::VoiceChatSpeakIcon, this, FTransform::Identity, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
}

void AWRCharacterPlayer::OnSpeakingStopedForVoiceChatting()
{
	if (IsMe())
		return;

	WRUIManager::Get()->CloseUI(this, EWRUIPanelKind::VoiceChatSpeakIcon);
}

void AWRCharacterPlayer::SetBattleState(bool bFlag)
{
	Super::SetBattleState(bFlag);

	// [ 2020-4-10 : kyu ] 임시로 리턴
	if (GetCurrentFSMType() == EWRFSM::Interact)
		return;

	if (GetCurrentFSMType() == EWRFSM::Dance)
		return;

	if (IsEquipedVisible(EWREquipPart::RightHand) == false)
	{
		SetEquipPartVisibility(EWREquipPart::RightHand, true);
	}

	if (IsEquipedVisible(EWREquipPart::LeftHand) == false)
	{
		SetEquipPartVisibility(EWREquipPart::LeftHand, true);
	}
}

bool AWRCharacterPlayer::IsInBattle()
{
	return bBattleState;
}

void AWRCharacterPlayer::MakeDebugString(TArray<FString>& OUT OutString)
{
	AWRCharacter::MakeDebugString(OutString);

	// FSM
	OutString.Add(FString::Printf(TEXT("FSM : %s"), *WREnumUtility::EnumToString("EWRFSM", GetCurrentFSMType())));

	// Move Value
	OutString.Add(FString::Printf(TEXT("Move Forward : %.1f"), MoveForwardValue));
	OutString.Add(FString::Printf(TEXT("Move Right : %.1f"), MoveRightValue));

	// Get On/Off
	OutString.Add(FString::Printf(TEXT("Vehicle AID : %d"), GetEnvVehicleAID()));

	if (GetEnvVehicleAID() != INVALID_AID)
	{
		// Offset
		OutString.Add(FString::Printf(TEXT("Vehicle Offset : %.0f, %.0f, %.0f"), EnvVehicleOffset.X, EnvVehicleOffset.Y, EnvVehicleOffset.Z));
	}

	// Occupy
	UWRActorComponentInteraction* InteractionComponent = GetComponent<UWRActorComponentInteraction>();
	if (InteractionComponent != nullptr)
	{
		if (InteractionComponent->GetOccupiedIO() != nullptr)
		{
			OutString.Add(FString::Printf(TEXT("Occupied AID : %d"), InteractionComponent->GetOccupiedIO()->GetActorID()));
		}
	}
}

//FVector AWRCharacterPlayer::CalcLocationForSecretDialogPortal(const FVector& InLastPosition)
//{
//	FVector LastPosition = InLastPosition;
//
//	AWRSecretDialogArea* InSecretArea = WREnvInteractionManager::Get()->GetCurrentSecretArea();
//	if (InSecretArea != nullptr)
//	{
//		TArray<AID_t> InAIDList = InSecretArea->GetActorIDs();
//		if (InAIDList.Contains(this->GetActorID()) == true)
//		{
//			AWRSecretDialogPortal* InPortal = WREnvInteractionManager::Get()->FindPortalByGroupID(InSecretArea->GetChatGroupID());
//			if (InPortal != nullptr)
//			{
//				FVector TempVec = LastPosition - InSecretArea->GetActorLocation();
//
//				return InPortal->GetActorLocation() + TempVec;
//			}
//		}
//	}
//	return FVector::ZeroVector;
//}

//FVector AWRCharacterPlayer::CalcLocationForSecretDialogArea(const FVector& InLastPosition)
//{
//	FVector LastPosition = InLastPosition;
//
//	AWRSecretDialogArea* InSecretArea = WREnvInteractionManager::Get()->GetCurrentSecretArea();
//	if (InSecretArea != nullptr)
//	{
//		TArray<AID_t> InAIDList = InSecretArea->GetActorIDs();
//		if (InAIDList.Contains(this->GetActorID()) == true)
//		{
//			AWRSecretDialogPortal* InPortal = WREnvInteractionManager::Get()->FindPortalByGroupID(InSecretArea->GetChatGroupID());
//			if (InPortal != nullptr)
//			{
//				FVector TempVec = LastPosition - InPortal->GetActorLocation();
//
//				return InSecretArea->GetActorLocation() + TempVec;
//			}
//		}
//	}
//	return FVector::ZeroVector;
//}
