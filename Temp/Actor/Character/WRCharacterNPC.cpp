// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacterNPC.h"
#include "WRCharacter.h"
#include "WRCharacterMine.h"

#include "../Animation/WRAnimInstance.h"
#include "../AI/WRAIControllerNPC.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Table/WREnpcTable.h"

#include "Component/Character/WRCapsuleCollisionComponent.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentLipsync.h"
#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Character/WRActorComponentIdleSpeechDialogue.h"
#include "Component/Character/WRActorComponentAcceptQuest.h"
#include "Component/Character/WRActorComponentCommonNpcTalk.h"
#include "Component/Character/WRActorComponentStateMark.h"
#include "Component/Character/WRActorComponentDanceMode.h"
#include "Component/Character/WRActorComponentNpcSocial.h"
#include "Component/Character/WRActorComponentBuff.h"
#include "Component/Character/WRActorComponentEventAction.h"

#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRCharacterManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRAnimManager.h"
#include "Manager/WRSkillManager.h"

#include "CombatSystem/Combat/WRCombatInfo.h"
#include "Define/WRPathDefine.h"
#include "Network/IMMessageSender.h"

#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>



AWRCharacterNPC::AWRCharacterNPC()
{
	AIControllerClass = AWRAIControllerNPC::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CharacterType = EWRCharacterType::NPC;

}

void AWRCharacterNPC::BeginPlay()
{
	Super::BeginPlay();
	IsOnRotationLerp = LookTargetAlways;
	IsShouldTurnAround = true;
}

void AWRCharacterNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRecognitionArea();
}

void AWRCharacterNPC::OnPostSpawn()
{
	AWRCharacter::OnPostSpawn();

	AAIController* _aiController = GetAIController();
	if (_aiController)
	{
		_aiController->ReceiveMoveCompleted.AddUniqueDynamic(this, &ThisClass::ReceiveMoveCompleted);
	}

	if (CharacterType == EWRCharacterType::NPC)
		CreateComponent<UWRActorComponentAcceptQuest>(EWRCharacterComponent::AcceptQuest);
}

void AWRCharacterNPC::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AWRCharacterNPC::OnPreDestroy()
{
	AWRCharacter::OnPreDestroy();
}

void AWRCharacterNPC::BuildCustomComponents()
{
	AWRCharacter::BuildCustomComponents();

	CreateComponent<UWRActorComponentEquipmentLook>(EWRCharacterComponent::EquipmentLook);
	CreateComponent<UWRActorComponentLipsync>(EWRCharacterComponent::Lipsync);
	CreateComponent<UWRActorComponentStateMark>(EWRCharacterComponent::StateMark);
	CreateComponent<UWRActorComponentBuff>(EWRCharacterComponent::Buff, true);
}

void AWRCharacterNPC::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//GetCharacterMovement()->MaxWalkSpeed = 100.0f;
}

void AWRCharacterNPC::BeginFSMAttack(const WRFSMInfo& InFSM)
{
	Super::BeginFSMAttack(InFSM);

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	ClearAIMoveTo();
#endif // DEF_FIX_CLEARAIMOVETO
}

void AWRCharacterNPC::TickFSMAttack(float InDeltaTime)
{
	Super::TickFSMAttack(InDeltaTime);
}

void AWRCharacterNPC::EndFSMAttack(const WRFSMInfo& InFSM)
{
	Super::EndFSMAttack(InFSM);

	// ¿¡ÀÌ´Ù Èú ½ºÅ³ÀÏ¶§ ¿¹¿Ü Ã³¸®
	if (InFSM.PreviousSkillTableID == DEF_AIDA_HEAL_SKILL_TABLE_ID && this->GetCharacterTableID() == DEF_AIDA_TABLIE_ID)
	{
		AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
		if (MyCharacter)
		{
			UWRActorComponentAttack* ActorComponentAttack = MyCharacter->GetComponent<UWRActorComponentAttack>();
			if (ActorComponentAttack)
			{
				ActorComponentAttack->SetCoolTime(this, InFSM.PreviousSkillTableID);
			}
		}
	}
}

void AWRCharacterNPC::SetWeaponCollisionBasedOnPlayingMontage(EWREquipPart EquipPart, ECollisionEnabled::Type InType)
{
	WRCombatInfo* CombatInfo = WRCombatManager::Get()->FindLastCombatInfo(GetActorID());
	if (nullptr == CombatInfo)
		return;

	FWRSkillTable* SkillTable = CombatInfo->GetSkillTable();
	if (nullptr == SkillTable)
		return;

	//	switch (SkillTable->EquipType)
	switch (EquipPart)
	{
	case EWREquipPart::RightHand:
	{
		UWRCapsuleCollisionComponent* AttackingCapsule = FindCapsuleCollisionComponent(EWRCollisionType::RightAttack);
		if (AttackingCapsule)
		{
			AttackingCapsule->SetCollisionEnabled(InType);
		}
	}
	break;
	case EWREquipPart::LeftHand:
	{
		UWRCapsuleCollisionComponent* AttackingCapsule = FindCapsuleCollisionComponent(EWRCollisionType::LeftAttack);
		if (AttackingCapsule)
		{
			AttackingCapsule->SetCollisionEnabled(InType);
		}
	}
	break;
	default:
		break;
	}
}

void AWRCharacterNPC::SetWeaponCollisionBasedOnSide(EWREquipPart InPart, ECollisionEnabled::Type InType)
{
	switch (InPart)
	{
	case EWREquipPart::RightHand:
	{
		UWRCapsuleCollisionComponent* RightCapsule = FindCapsuleCollisionComponent(EWRCollisionType::RightAttack);
		if (RightCapsule)
		{
			RightCapsule->SetCollisionEnabled(InType);
		}
	}
	break;
	case EWREquipPart::LeftHand:
	{
		UWRCapsuleCollisionComponent* LeftCapsule = FindCapsuleCollisionComponent(EWRCollisionType::LeftAttack);
		if (LeftCapsule)
		{
			LeftCapsule->SetCollisionEnabled(InType);
		}
	}
	break;
	}
}

bool AWRCharacterNPC::AIMoveTo(const FVector& InDestLocation, EWRActionType::en InMoveType /*= EWRActionType::en::RUN*/)
{
	Super::AIMoveTo(InDestLocation, InMoveType);

	if (this->GetCharacterMovement())
	{
		const FWRNPCBaseTable* NpcBaseTable = WRCharacterManager::Get()->FindNpcBaseTable(this->GetCharacterTableID());
		if (NpcBaseTable)
		{
			switch (InMoveType)
			{
			case EWRActionType::en::RUN:
			case EWRActionType::en::AWAY_STEP:
			{
				this->GetCharacterMovement()->MaxWalkSpeed = NpcBaseTable->RunSpeed;

				UWRAnimInstance * InAnimInst = this->GetAnimInstance();
				if (InAnimInst != nullptr)
					InAnimInst->SetMaxDelta(1.0f);
			}
			break;
			case EWRActionType::en::WALK:
			case EWRActionType::en::SIDE_STEP:
			{
				this->GetCharacterMovement()->MaxWalkSpeed = NpcBaseTable->WalkSpeed;

				UWRAnimInstance * InAnimInst = this->GetAnimInstance();
				if (InAnimInst != nullptr)
					InAnimInst->SetMaxDelta(0.5f);
			}
			break;
			}
		}
	}
	return true;
}

void AWRCharacterNPC::OnBeginOverlapCapsuleAttacking(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlapCapsuleAttacking(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (this == Other)
		return;

	AWRCharacter* OtherCharacter = Cast<AWRCharacter>(Other);
	if (nullptr == OtherCharacter)
		return;

	switch (OtherCharacter->GetCurrentFSMType())
	{
	case EWRFSM::Avoiding:
	{
		SetAnimMontageRate(0.5f);
		return;
	}
	break;
	default:
	{
	}
	break;
	}
}

void AWRCharacterNPC::OnBeginOverlapCapsuleHitting(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlapCapsuleHitting(OverlappedComp, Other, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (this == Other)
		return;
}

void AWRCharacterNPC::OnEndOverlapHittingCapsule(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnEndOverlapHittingCapsule(OverlappedComp, Other, OtherComp, OtherBodyIndex);
	if (this == Other)
		return;
}

void AWRCharacterNPC::OnInformHitResult(const pt_Game_User::Inf_HitResult_G2U& InInform)
{
	Super::OnInformHitResult(InInform);

	if (InInform.coll_gauge == 0)
	{
		DetermineFSMSignal(EWRFSM::Damage, true);

		EWRCollisionType CollisionType = (EWRCollisionType)InInform.collisionType;
		FString strCollision = WREnumUtility::EnumToString("EWRCollisionType", CollisionType);
		strCollision += "_destroy";

		PlayAnimJumpToSection(EWRFSM::Damage, strCollision);
	}
}

void AWRCharacterNPC::OnInformUseSkill(const pt_Game_User::Inf_UseSkill_G2U& InInform)
{
	FWRSkillTable* SkillRecord = WRTableManager::Get()->FindRow<FWRSkillTable>(InInform.skillTID, GetSkillTableFileName());
	if (nullptr == SkillRecord)
		return;

	switch (SkillRecord->SkillType)
	{
	case EWRSkillType::Heal:
	{
		std::list<AID_t>::const_iterator iter = InInform.lstTarget.begin();
		std::list<AID_t>::const_iterator iter_end = InInform.lstTarget.end();

		for (; iter != iter_end; ++iter)
		{
			AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(*iter);
			if (Target == nullptr)
				continue;

			WRVFXManager::Get()->Spawn(EFFECT_PATH_4_HEAL, Target, nullptr);

			if (Target->IsMe() == false)
				continue;

			// [ 2019-11-13 : animal14 ] HP ¿¬Ãâ
			float HPDuration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::UI, "PCHitBlood_FB_Time");
			WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::HP, HPDuration);
		}
	}
	break;
	default:
	{
	}
	break;
	}

	Super::OnInformUseSkill(InInform);

#ifdef DEF_FIX_CLEARAIMOVETO
#else // DEF_FIX_CLEARAIMOVETO
	ClearAIMoveTo();
#endif // DEF_FIX_CLEARAIMOVETO
}

void AWRCharacterNPC::OnInformBeginSkill(const pt_Game_User::Inf_BeginSkill_G2U& InInform)
{
	Super::OnInformBeginSkill(InInform);
}

void AWRCharacterNPC::OnInformEndSkill(const pt_Game_User::Inf_EndSkill_G2U& InInform)
{
	Super::OnInformEndSkill(InInform);
}

void AWRCharacterNPC::SetCharacterTableID(WRTableID InTID)
{
	Super::SetCharacterTableID(InTID);

	CharacterType = FWRNPCBaseTable::GetNPCType(InTID);

	FWRNPCBaseTable* NpcBaseTable = GetNpcBaseTable();
	if (CharacterType == EWRCharacterType::NPC)
	{
		FWRNpcTable* NpcTable = static_cast<FWRNpcTable*>(NpcBaseTable);
		if (NpcTable)
		{
			if (NpcTable->LookIKDist > 0)
				SetLookIKTarget(WRCharacterManager::Get()->GetMyCharacter(), "head");

			if (NpcTable->IdleSpeechDialogue.Num() > 0)
			{
				UWRActorComponentIdleSpeechDialogue* CreatedComponent = CreateComponent<UWRActorComponentIdleSpeechDialogue>(EWRCharacterComponent::IdleSpeechDialogue);
				if (CreatedComponent)
					CreatedComponent->SetData(NpcTable->IdleSpeechRange, NpcTable->IdleSpeechDialogue);
			}

			if (NpcTable->NpcTalkDialogueTID != INVALID_TABLE_TID)
				CreateComponent<UWRActorComponentCommonNpcTalk>(EWRCharacterComponent::CommonNpcTalk);

			if(NpcTable->ShowSocialMenu)
				CreateComponent<UWRActorComponentNpcSocial>(EWRCharacterComponent::NpcSocial, true);

			if(NpcTable->EventActionRange > 0.f)
				CreateComponent<UWRActorComponentEventAction>(EWRCharacterComponent::EventAction);
		}

		UWRActorComponentAcceptQuest* AcceptQuestComponent = GetComponent<UWRActorComponentAcceptQuest>();
		if (AcceptQuestComponent)
			AcceptQuestComponent->Init();
	}
}

FWRNPCBaseTable* AWRCharacterNPC::GetNpcBaseTable() const
{
	if (CharacterTableID == INVALID_TABLE_TID)
		return nullptr;

	switch (CharacterType)
	{
	case EWRCharacterType::NPC:	return WRTableManager::Get()->FindRow<FWRNpcTable>(CharacterTableID);
	case EWRCharacterType::ENPC: return WRTableManager::Get()->FindRow<FWREnpcTable>(CharacterTableID);
	default:
		break;
	}

	return nullptr;
}

EWRNpcJobType AWRCharacterNPC::GetNpcJobType()
{
	FWRNPCBaseTable* NPCBaseTable = GetNpcBaseTable();
	if (NPCBaseTable == nullptr)
		return EWRNpcJobType::None;

	if (GetCharacterType() == EWRCharacterType::NPC)
	{
		FWRNpcTable* NpcTable = static_cast<FWRNpcTable*>(NPCBaseTable);
		if (NpcTable)
			return NpcTable->JobType;
	}
	else if (GetCharacterType() == EWRCharacterType::ENPC)
	{
		FWREnpcTable* ENpcTable = static_cast<FWREnpcTable*>(NPCBaseTable);
		if (ENpcTable)
			return ENpcTable->JobType;
	}

	return EWRNpcJobType::None;
}

EWRNpcCreateType AWRCharacterNPC::GetNpcCreateType()
{
	FWRNPCBaseTable* NPCBaseTable = GetNpcBaseTable();
	if (NPCBaseTable != nullptr)
	{
		return NPCBaseTable->createType;
	}
	return EWRNpcCreateType::UNIQUE;
}

//====================================================================================

const EWRHookType AWRCharacterNPC::GetHookType()
{
	FWRNPCBaseTable* NPCBaseTable = GetNpcBaseTable();
	if (NPCBaseTable != nullptr)
	{
		return NPCBaseTable->HookType;
	}
	return EWRHookType::None;
}

void AWRCharacterNPC::SetHighLight(EWRObjectHighLight IN InHighLightType)
{
	HighLightType = InHighLightType;
	switch (InHighLightType)
	{
	case EWRObjectHighLight::None:
	{
		SetMaterialParameterValue<float>("05InteractionEmissiveL", 0.0f);
		SetMaterialParameterValue<float>("05InteractionEmissiveR", 0.0f);
		SetMaterialParameterValue<float>("05InteractionLine", 0.0f);
	}
	break;
	case EWRObjectHighLight::Recognition:
	{
		// ÀÎÁö ÄÔ
		SetMaterialParameterValue<float>("05InteractionLine", 1.0f);
		// Å¸°Ù ²û
		SetMaterialParameterValue<float>("05InteractionEmissiveL", 0.0f);
		SetMaterialParameterValue<float>("05InteractionEmissiveR", 0.0f);
	}
	break;
	case EWRObjectHighLight::Targeted:
	{
		// ÀÎÁö ²û
		SetMaterialParameterValue<float>("05InteractionLine", 0.0f);
		// Å¸°Ù ÄÔ
		if (TargetedCollision != nullptr)
		{
			if (TargetedCollision->HitCollisionType == EWRCollisionType::LeftLeg)
			{
				SetMaterialParameterValue<float>("05InteractionEmissiveL", 1.0f);
				SetMaterialParameterValue<float>("05InteractionEmissiveR", 0.0f);
			}
			if (TargetedCollision->HitCollisionType == EWRCollisionType::RightLeg)
			{
				SetMaterialParameterValue<float>("05InteractionEmissiveL", 0.0f);
				SetMaterialParameterValue<float>("05InteractionEmissiveR", 1.0f);
			}
			float fTargetHighLight = WRConsoleManager::Get()->GetFloat(EWRConsole::IO_NoneTargetHighLightValue);
			SetMaterialParameterValue<float>("05InteractionEmissiveValue", fTargetHighLight);
		}
		else
		{
			SetMaterialParameterValue<float>("05InteractionEmissiveL", 0.0f);
			SetMaterialParameterValue<float>("05InteractionEmissiveR", 0.0f);
		}
	}
	break;
	}
}

//====================================================================================

void AWRCharacterNPC::SetLookTargetForDialog(AWRCharacter * InTarget, FWRRotationEndDelegate InRotationEndDelegate /*= nullptr*/, FName InSocketName)
{
	SetRotationLerpToTarget(InTarget, InRotationEndDelegate, InSocketName);
}

void AWRCharacterNPC::ReleaseLookTargetForEndDialog(const FRotator & InRotator)
{
	this->SetLookIKTarget(nullptr);

	ForTurnAroundRotator = InRotator;
}

void AWRCharacterNPC::OnEmotion(const EWRNPCEmotion IN InEmotion, AWRCharacter* IN InTarget /* = nullptr */)
{
	if (InTarget != nullptr)
	{
// 		FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), InTarget->GetActorLocation());
// 		this->SetRotation(Rotation2RotateDir);
	}

	WRFSMInfo FSMInfo;
	FSMInfo.FSMType = EWRFSM::Emotion;

	switch (InEmotion)
	{
	case EWRNPCEmotion::Cheers:
	{
		FSMInfo.JumpToSectionName = "Cheers";
	}
	break;
	}
	DetermineFSMSignal(FSMInfo);
}
