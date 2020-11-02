#include "WRCombatInfo.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRSkillTable.h"
#include "Table/WREquipmentLookTable.h"
#include "Table/WREnpcTable.h"

#include "Component/Character/WRActorComponentDynamicMovement.h"
#include "Component/Character/WRActorComponentManageMaterial.h"
#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRCapsuleCollisionComponent.h"

#include "Enum/EWRCharacter.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/StaticMeshActor/WRStaticMeshProjectile.h"

#include "Manager/WRCharacterManager.h"
#include "Manager/WRCombatManager.h"
#include "Manager/WRSkillManager.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRVFXManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRPathManager.h"
#include "Manager/WRInputSystemManager.h"

#include <Kismet/KismetMathLibrary.h>
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Define/WRPathDefine.h"

#include "DrawDebugHelpers.h"

#include "Camera/CameraComponent.h"

#include "Components/DecalComponent.h"

#include "Network/IMMessageSender.h"

#include "Utility/WRActorUtility.h"
#include "Utility/WRSkeletalMeshModifier.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "Component/Character/WRActorComponentFSM.h"
#include "Engine/StaticMesh.h"
#include "Component/Destructible/WRDestructibleComponent.h"



WRCombatInfo::WRCombatInfo()
{
}


WRCombatInfo::~WRCombatInfo()
{
}

void WRCombatInfo::Init()
{

}

void WRCombatInfo::Reset()
{
	if (GetAttacker() && GetAttacker()->IsMe())
	{
		UWRActorComponentAttack* AttackComponent = GetAttacker()->GetComponent<UWRActorComponentAttack>();
		if (AttackComponent)
		{
			AttackComponent->SetThrowingShieldFlag(false);
		}
	}

	ProcessCombatEnded();

	CombatSeqParam.Reset();
	HitCharacters.Reset();

	PreviousLocations.Empty();
	bReserveDestroy = false;

	DelayedPacket.Reset();
	arrChainTarget.Reset();

	WRActorUtility::DestroySplineComponent(ThrowingkMeshActor, SplineComponent);
	SplineComponent = nullptr;
	ElapsedTime4ThrowingActorMove = 0.0f;

	if (ThrowingkMeshActor->IsValidLowLevel())
	{
		// [ 2019-12-12 : animal14 ] ����Ʈ ����
		ThrowingkMeshActor->Release();

		ThrowingkMeshActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		ThrowingkMeshActor->ConditionalBeginDestroy();

		ThrowingkMeshActor = nullptr;
	}

	WRActorUtility::ClearTimer(InProcessDelayedPacketTiemr);
}

void WRCombatInfo::Tick(float DeltaTime)
{
	TickProcessDelayedPacket();

	this->Tick4Hit();

	TickThrow(DeltaTime);
}

bool WRCombatInfo::IsValidNextTarget(AID_t InAID)
{
	AWRCharacter* pOwner = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
	if (nullptr == pOwner)
		return false;

	if (pOwner->GetActorID() == InAID)
		return true;

	return false;
}

bool WRCombatInfo::IsGettingBackProjectile()
{
	if (arrChainTarget.Num() == 0)
		return false;

	if (ChainTargetIndex != 0)	//�������� �׻� ������ �ڽ��̾�� ��
		return false;

	AWRCharacter* pOwner = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
	if (nullptr == pOwner)
		return false;

	if (GetAttacker() == pOwner)
	{
		return true;
	}

	return false;
}

void WRCombatInfo::OnProcessProjectileBeginOverlap()
{

}

void WRCombatInfo::SetChainTargetInfo(const TArray<AID_t>& InChainTarget)
{
	arrChainTarget.Add(GetAttacker()->GetActorID());	//���а� ������ ���� �ɸ� (������ �ڽ�)

	arrChainTarget += InChainTarget;
}

void WRCombatInfo::SetNextChainTaretIndex(AID_t InActorID)
{
	const AWRCharacter* pHitChar = WRCharacterManager::Get()->FindbyAID(InActorID);
	const AWRCharacter* pIndexChar = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
	if (pHitChar != pIndexChar)
		return;

	if (nullptr == SplineComponent)
	{
		Destroy();
		return;
	}

	FVector DestLocation = FVector::ZeroVector;
	TArray<FVector> SplineMoveLocation;
	while (0 < ChainTargetIndex)
	{
		--ChainTargetIndex;
		AWRCharacter* pNextChar = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
		if (pNextChar)
		{
			TSet<AActor*> OverlappingActors;
			ThrowingkMeshActor->GetOverlappingActors(OverlappingActors);
			for (auto iter = OverlappingActors.CreateConstIterator(); iter; ++iter)
			{
				if (*iter == pNextChar)	//���� Ÿ������ ���� �ϴµ� �̹� ���� ������ ���¸�
				{
					if (0 < ChainTargetIndex) //Ÿ���� ���� ���¸�
					{
						//��Ʈ ó�� �ϰ� �ε����ϳ� �� �����ش�
						DetermineHit(pNextChar->GetActorID(), nullptr);
						continue;
					}
					else if (0 == ChainTargetIndex) //���� �����ڷ� ���ư��� �ϴ� ��Ȳ�̸� ����
					{
						Destroy();	//���� ���� ����
						return;
					}
				}
			}
		}
		else
		{
			if (0 == ChainTargetIndex)	//�����ڰ� ����� ����� ��Ȳ
			{
				Destroy();	//���� ���� ����
				return;
			}

			continue;
		}

		//DestLocation = pNextChar->GetActorLocation();
		DestLocation = GetProjectileHitLocation(pNextChar);

		break;
	}


	//�ð� ����
	float Distance = (DestLocation - ThrowingkMeshActor->GetActorLocation()).Size() / 100.0f;	//���ͷ� ����
	ElapsedTime4ThrowingActorMove = Distance / GetSkillTable()->ProjectileVelocity;
	//ElapsedTime4ThrowingActorMove = Distance / 3.0f;
	SplineComponent->Duration = ElapsedTime4ThrowingActorMove;


	SplineMoveLocation.Add(DestLocation);
	SplineMoveLocation.Add(ThrowingkMeshActor->GetActorLocation());
	SplineComponent->SetSplinePoints(SplineMoveLocation, ESplineCoordinateSpace::World);

}

void WRCombatInfo::OnNotifyThrow(WRTableID WeaponLookTID)
{
	if (nullptr == GetAttacker())
		return;

	FWREquipmentLookTable* LookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(WeaponLookTID);
	if (nullptr == LookTable)
		return;

	// 	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	// 	if (nullptr == World)
	// 		return;

	if (arrChainTarget.Num() == 0)
	{
		return;
	}

	// ------ Refresh Target List ------
	TArray<AID_t> NotExistAID;
	for (int32 TargetIdx = arrChainTarget.Num() - 1; 0 <= TargetIdx; --TargetIdx)
	{
		AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(arrChainTarget[TargetIdx]);
		if (nullptr == pChar)
		{
			NotExistAID.Add(arrChainTarget[TargetIdx]);
			continue;
		}

		break;
	}

	//�̹� ���� �ɸ��ʹ� ���� �صд�
	for (int32 Idx = 0; Idx < NotExistAID.Num(); ++Idx)
	{
		arrChainTarget.Remove(NotExistAID[Idx]);
	}

	//���� �ε��� ����
	ChainTargetIndex = arrChainTarget.Num() - 1;
	// ------------------------------
	if (ChainTargetIndex <= 0)
	{
		Destroy();
		return;
	}

	// ------ create mesh ------
// 	ThrowingkMeshActor = World->SpawnActor<AWRStaticMeshProjectile>(AWRStaticMeshProjectile::StaticClass());
	if (ThrowingkMeshActor == nullptr)
		return;

	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *LookTable->StaticMesh);
	ThrowingkMeshActor->SetStaticMesh(StaticMesh);

	// [ 2019-12-12 : animal14 ] ����Ʈ ����
	ThrowingkMeshActor->Initialize();

	UStaticMeshComponent* MeshComponent = ThrowingkMeshActor->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetCollisionProfileName(FName("WRAttackObject"));
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		MeshComponent->SetGenerateOverlapEvents(true);
		MeshComponent->OnComponentBeginOverlap.AddDynamic(ThrowingkMeshActor, &AWRStaticMeshProjectile::OnBeginOverlap);
	}

	ThrowingkMeshActor->SetIdendifier(GetAttacker()->GetActorID(), CombatSeqParam.CombatSeqID);

	if (GetAttacker()->GetMesh())
	{
		FTransform ShieldSocketWorldLocation = GetAttacker()->GetMesh()->GetSocketTransform("Shield_socket");
		ThrowingkMeshActor->SetActorLocation(ShieldSocketWorldLocation.GetTranslation());
	}
	// ------------------------------



	// ------ process spline ------
//	SplineComponent = WRActorUtility::CreateSplineComponent(ThrowingkMeshActor, true);
	if (SplineComponent)
	{
		TArray<FVector> SplineMoveLocation;
		AWRCharacter* FirstTarget = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
		if (FirstTarget)
		{
			//1. ù��° ��� ��ġ ����
			//SplineMoveLocation.Add(FirstTarget->GetActorLocation());
			FVector FirstTargetLocation = GetProjectileHitLocation(FirstTarget);
			SplineMoveLocation.Add(FirstTargetLocation);

			//2. �߰���ġ ����			
			FVector DirToDest = FirstTargetLocation - ThrowingkMeshActor->GetActorLocation();
			float HalfDistanceToDest = DirToDest.Size() / 2.0f;
			float QuarterDistance = DirToDest.Size() / 4.0f;
			if (200.0f < QuarterDistance)
			{
				QuarterDistance = 200.0f;
			}

			FVector normalDirToDest = DirToDest.GetSafeNormal();
			FVector MiddleLocation = normalDirToDest * HalfDistanceToDest + ThrowingkMeshActor->GetActorLocation();
			if (2 < arrChainTarget.Num())	//����� �� �̻��̸� arrChainTarget�� 3�� �̻��� ���� ����
			{
				AWRCharacter* LastTarget = WRCharacterManager::Get()->FindbyAID(arrChainTarget[1]);

				FVector dir2First = FirstTargetLocation - GetAttacker()->GetActorLocation();
				FVector dir2Last = LastTarget->GetActorLocation() - GetAttacker()->GetActorLocation();

				FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(GetAttacker()->GetActorLocation(), FirstTargetLocation);
				Rotation2RotateDir.Yaw += 90.0f;

				FVector RightVecOfDir2FirstTarget = Rotation2RotateDir.Vector().GetSafeNormal();
				float DotValue = UKismetMathLibrary::Dot_VectorVector(dir2Last, RightVecOfDir2FirstTarget);
				if (DotValue <= 0.0f)
				{
					MiddleLocation += RightVecOfDir2FirstTarget * QuarterDistance;
				}
				else if (0.0f < DotValue)
				{
					RightVecOfDir2FirstTarget *= -1;
					MiddleLocation += RightVecOfDir2FirstTarget * QuarterDistance;
				}
			}
			else
			{
				FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(ThrowingkMeshActor->GetActorLocation(), FirstTargetLocation);
				Rotation2RotateDir.Yaw += 90.0f;


				FVector RightVecOfDir2FirstTarget = Rotation2RotateDir.Vector();

				RightVecOfDir2FirstTarget *= -1;
				MiddleLocation += RightVecOfDir2FirstTarget * QuarterDistance;

				//���� �ڵ�
				//FVector ModifiedDirection = -1 * GetAttacker()->GetActorRightVector();
				//MiddleLocation += ModifiedDirection * QuarterDistance;
			}

			//2.
			SplineMoveLocation.Add(MiddleLocation);


			//3. ���� ��ġ ����
			SplineMoveLocation.Add(ThrowingkMeshActor->GetActorLocation());


			//�ð� �� ���� ����
			float Distance = (FirstTargetLocation - ThrowingkMeshActor->GetActorLocation()).Size() / 100.0f;	//���ͷ� ����
			ElapsedTime4ThrowingActorMove = Distance / GetSkillTable()->ProjectileVelocity;
			//ElapsedTime4ThrowingActorMove = Distance / 3.0f;
			SplineComponent->Duration = ElapsedTime4ThrowingActorMove;
			SplineComponent->SetSplinePoints(SplineMoveLocation, ESplineCoordinateSpace::World);


			// 			FRotator Rotation2RotateDir = UKismetMathLibrary::FindLookAtRotation(ThrowingkMeshActor->GetActorLocation(), FirstTargetLocation);
			// 			ThrowingkMeshActor->SetActorRotation(FRotator(Rotation2RotateDir.Pitch, Rotation2RotateDir.Yaw, MAX_THROWING_SHILED_ROLL));

		}

		GetAttacker()->SetEquipPartVisibility(EWREquipPart::LeftHand, false);
	}
	// ------------------------------

}

FVector WRCombatInfo::GetProjectileHitLocation(AWRCharacter* InActor)
{
	if (nullptr == InActor)
		return FVector::ZeroVector;

	if (nullptr == InActor->GetMesh())
		return InActor->GetActorLocation();

	if (InActor->GetMesh()->DoesSocketExist("View_Target"))
	{
		FTransform ShieldSocketWorldLocation = InActor->GetMesh()->GetSocketTransform("View_Target");
		return ShieldSocketWorldLocation.GetTranslation();
	}
	else
	{
		return InActor->GetActorLocation();
	}
}

void WRCombatInfo::ProcessCombatEnded()
{
	AWRCharacter* pChar = GetAttacker();
	if (nullptr == pChar)
		return;

	FWRSkillTable* SkillTable = GetSkillTable();
	if (nullptr == SkillTable)
		return;

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::ChainProjectile:
	{
		pChar->SetEquipPartVisibility(EWREquipPart::LeftHand, true);

		pChar->DetermineFSMSignal(EWRFSM::Idle);
	}
	break;
	default:
		break;
	}
}

bool WRCombatInfo::IsValidProjectileCombat()
{
	FWRSkillTable* SkillTable = GetSkillTable();
	if (nullptr == SkillTable)
		return false;

	switch (SkillTable->SkillType)
	{
	case EWRSkillType::ChainProjectile:
	{
		if (arrChainTarget.Num() == 0)
			return false;

		if (ThrowingkMeshActor == nullptr)
			return false;
	}
	break;
	case EWRSkillType::Projectile:
	{
	}
	break;
	default:
		break;
	}

	return true;
}

void WRCombatInfo::TickThrow(float DeltaTime)
{
	if (ElapsedTime4ThrowingActorMove <= 0.0f)
		return;

	if (nullptr == ThrowingkMeshActor)
		return;

	TickRotateShield(DeltaTime);

	TickUpdateTargetLocation();

	SplineComponent->UpdateSpline();
	ElapsedTime4ThrowingActorMove -= DeltaTime;
	if (ElapsedTime4ThrowingActorMove < 0.0f)
	{
		ElapsedTime4ThrowingActorMove = 0.0f;
	}

	FVector CurrLocation = SplineComponent->GetWorldLocationAtTime(ElapsedTime4ThrowingActorMove);
	//bSweep�� true�� �浹�� �Ͼ�� �ش� ��ġ���� ���߰� �ȴ�
	bool bResult = ThrowingkMeshActor->SetActorLocation(CurrLocation, false);
}

void WRCombatInfo::TickRotateShield(float DeltaTime)
{
	if (nullptr == ThrowingkMeshActor)
		return;

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
	if (nullptr == pChar)
	{
		//������ �׳� �����ϰ� ����� ������ ���߿�..
		return;
	}

	static float yawRatio = 15.0f;
	static float rollRatio = 3.0f;



	//Yaw
	FRotator MeshRotation = ThrowingkMeshActor->GetStaticMeshComponent()->GetComponentRotation();
	int32 DestYawValue = MeshRotation.Yaw + 90.0f;
	DestYawValue %= 360;
	FRotator CurrentYaw = FRotator(0.0f, MeshRotation.Yaw, 0.0f);
	FRotator DestYaw = FRotator(0.0f, DestYawValue, 0.0f);
	FRotator InterpYaw = UKismetMathLibrary::RInterpTo(CurrentYaw, DestYaw, DeltaTime, yawRatio);
	ThrowingkMeshActor->GetStaticMeshComponent()->SetRelativeRotation(InterpYaw);

	// 	//Roll
	// 	FRotator CurrentRotation = ThrowingkMeshActor->GetActorRotation();
	// 	FRotator RollDestRotation = FRotator(0.0f, 0.0f, MAX_THROWING_SHILED_ROLL);
	// 	FRotator InterpRoll = UKismetMathLibrary::RInterpTo(CurrentRotation, RollDestRotation, DeltaTime, rollRatio);
	// 
	// 	//���� ���п��� Ÿ�������� �����̼�
	// 	FRotator DirectionRotation = UKismetMathLibrary::FindLookAtRotation(ThrowingkMeshActor->GetActorLocation(), pChar->GetActorLocation());
	// 	FRotator ResultInterpRoll = FRotator(0.0f, DirectionRotation.Yaw, InterpRoll.Roll);
	// 	ThrowingkMeshActor->SetActorRotation(ResultInterpRoll);
}

void WRCombatInfo::TickUpdateTargetLocation()
{
	if (nullptr == SplineComponent)
		return;

	if (arrChainTarget.Num() == 0)
		return;

	if (arrChainTarget.IsValidIndex(ChainTargetIndex) == false)
	{
		Destroy();
		return;
	}

	AWRCharacter* pChar = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
	if (pChar)
	{
		//SplineComponent->SetWorldLocationAtSplinePoint(0, pChar->GetActorLocation());
		SplineComponent->SetWorldLocationAtSplinePoint(0, GetProjectileHitLocation(pChar));
	}
	else
	{
		FVector DestLocation = FVector::ZeroVector;
		TArray<FVector> SplineMoveLocation;
		while (0 < ChainTargetIndex)
		{
			--ChainTargetIndex;
			AWRCharacter* pNextChar = WRCharacterManager::Get()->FindbyAID(arrChainTarget[ChainTargetIndex]);
			if (nullptr == pNextChar)
			{
				continue;
			}

			//DestLocation = pNextChar->GetActorLocation();
			DestLocation = GetProjectileHitLocation(pNextChar);

			break;
		}

		//1.
		SplineMoveLocation.Add(DestLocation);

		//3.
		SplineMoveLocation.Add(ThrowingkMeshActor->GetActorLocation());

		//set
		SplineComponent->SetSplinePoints(SplineMoveLocation, ESplineCoordinateSpace::World);
	}
}

void WRCombatInfo::TickProcessDelayedPacket()
{
	//�������� ���̵� �ޱ� ���̸� ���� ��Ŵ
	if (IsValidServerSeqID() == false)
		return;

	for (int32 Index = 0; Index < DelayedPacket.Num(); ++Index)
	{
		AWRCharacter* Caster = WRCharacterManager::Get()->FindbyAID(DelayedPacket[Index].CasterAID);
		AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(DelayedPacket[Index].TargetAID);

		switch (DelayedPacket[Index].Type)
		{
		case EWRDelayedPacketType::GuardBreak:
		{
			RequestToServer_GuardBreak(Caster, Target);
		}
		break;
		case EWRDelayedPacketType::Block:
		{
			RequestToServer_BlockSuccessed(Caster, Target);
		}
		break;
		case EWRDelayedPacketType::Hit:
		{
			RequestToServer_Hit(Caster, Target, (EWRCollisionType)(DelayedPacket[Index].CommonValue));
		}
		break;
		case EWRDelayedPacketType::Avoid:
		{
			RequestToServer_Avoid(Caster, DelayedPacket[Index].AvoidDir);
		}
		break;
		}
	}

	//�� ���±� ������ ����
	DelayedPacket.Empty();
}

bool WRCombatInfo::IsValidServerSeqID()
{
	return CombatSeqParam.bReceivedServerSeqID;
}

void WRCombatInfo::SetReceivedServerSeqIDFlag(bool bFlag)
{
	CombatSeqParam.bReceivedServerSeqID = bFlag;
}

void WRCombatInfo::ModifyCombatSeqID(SKILL_SEQ_t InSeqID)
{
	CombatSeqParam.CombatSeqID = InSeqID;
	CombatSeqParam.bReceivedServerSeqID = true;

	if (ThrowingkMeshActor->IsValidLowLevel())
	{
		ThrowingkMeshActor->SetIdendifier(GetAttacker()->GetActorID(), InSeqID);
	}
}

void WRCombatInfo::InitializeCombatInfo(const FWRCombatParam& InParam)
{
	SetCombatSeqParam(InParam);

	FWRSkillTable* SkillTable = GetSkillTable();
	if (SkillTable)
	{
		switch (SkillTable->SkillType)
		{
		case EWRSkillType::ChainProjectile:
		{
			UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
			if (World)
			{
				// ------ create mesh ------
				FActorSpawnParameters spawnParams;
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				spawnParams.Owner = GetAttacker();
				ThrowingkMeshActor = World->SpawnActor<AWRStaticMeshProjectile>(AWRStaticMeshProjectile::StaticClass(), spawnParams);
				if (ThrowingkMeshActor)
				{
					WRActorUtility::SetComponentCollisionEnabled(ThrowingkMeshActor->GetRootComponent(), ECollisionEnabled::NoCollision);
					SplineComponent = WRActorUtility::CreateSplineComponent(ThrowingkMeshActor, true);
				}
			}
		}
		break;
		default:
			break;
		}
	}
}

void WRCombatInfo::SetCombatSeqParam(const FWRCombatParam& InParam)
{
	CombatSeqParam = InParam;

	if (INVALID_SKILL_SEQ == CombatSeqParam.CombatSeqID)
	{
		CombatSeqParam.CombatSeqID = WRCombatManager::Get()->GenerateCombatSeqIDForStandalone();
	}
}

FWRCombatParam WRCombatInfo::GetCombatSeqParam()
{
	return CombatSeqParam;
}

SKILL_SEQ_t WRCombatInfo::GetCombatSeqID()
{
	return CombatSeqParam.CombatSeqID;
}

AWRCharacter* WRCombatInfo::GetAttacker()
{
	return WRCharacterManager::Get()->FindbyAID(CombatSeqParam.AttackerUID);
}

AWRCharacter* WRCombatInfo::GetTarget()
{
	return WRCharacterManager::Get()->FindbyAID(CombatSeqParam.TargetUID);
}

FWRSkillTable* WRCombatInfo::GetSkillTable()
{
	if (GetAttacker() == nullptr)
		return nullptr;

	return WRTableManager::Get()->FindRow<FWRSkillTable>(CombatSeqParam.SkillTID, GetAttacker()->GetSkillTableFileName());
}

bool WRCombatInfo::DetermineHit(AID_t InAID, UWRCapsuleCollisionComponent* InCapsule)
{
	if (HitCharacters.Contains(InAID))
		return false;

	AWRCharacter* HitChar = WRCharacterManager::Get()->FindbyAID(InAID);
	if (nullptr == HitChar)
		return false;

	AWRCharacter* Attacker = GetAttacker();
	if (nullptr == Attacker)
		return false;

	bool bHit = false;
	bool bGuardBreark = false;
	switch (HitChar->GetCurrentFSMType())
	{
	case EWRFSM::Defense:
	{
		if (WRSkillManager::Get()->IsPlayingMontageGuardBreakSkill(Attacker))
		{
			bHit = true;
			bGuardBreark = true;
		}
		else
		{
			//���� �� ���̳� �ڳĸ� ���� �Ǵ�
			FVector ForwardVec = HitChar->GetActorForwardVector();
			FVector DirToTarget = Attacker->GetActorLocation() - HitChar->GetActorLocation();
			DirToTarget.Normalize();

			float Dot = UKismetMathLibrary::Dot_VectorVector(ForwardVec, DirToTarget);
			if (0 < Dot)	//���� ����
			{
				if (DetermineBlocking(Attacker, HitChar))
				{
					//���� ��Ŷ�޾� ó�� �ϰ� ���� (ExecuteBlockSuccessed)
					RequestToServer_BlockSuccessed(GetAttacker(), HitChar);
				}
				else
				{
					bHit = true;	//�ٸ��� ��Ʈ ����
				}
			}
			else //���� ����
			{
				bHit = true;
			}
		}
	}
	break;
	case EWRFSM::Avoiding:
	{
		bHit = false;
	}
	break;
	default:
	{
		bHit = true;
	}
	break;
	}

	if (bHit)
	{
		if (bGuardBreark)
		{
			// ��Ŷ �߰� : ���� �극��ũ 
			RequestToServer_GuardBreak(GetAttacker(), HitChar);

			ExecuteGuardBreak(HitChar->GetActorID());
		}

		HitCharacters.Add(InAID);

		// ��Ŷ �߰� : ��Ʈ����
		EWRCollisionType CollisionType = EWRCollisionType::None;
		if (InCapsule)
			CollisionType = InCapsule->CollisionType;

		RequestToServer_Hit(GetAttacker(), HitChar, CollisionType);

		if (GetAttacker()->IsMe())
		{
			FWRSkillTable* SkillTable = GetSkillTable();

			if (SkillTable)
			{
				if (0.0f < SkillTable->AttackPauseAnimTime)
				{
					UWRActorComponentFSM* FSMComponent = GetAttacker()->GetComponent<UWRActorComponentFSM>();
					if (FSMComponent)
					{
						FSMComponent->PauseAnimation(SkillTable->AttackPauseAnimTime, false);
					}
				}

				if (0.0f < SkillTable->HitPauseAnimTime)
				{
					UWRActorComponentFSM* FSMComponent = HitChar->GetComponent<UWRActorComponentFSM>();
					if (FSMComponent)
					{
						FSMComponent->PauseAnimation(SkillTable->HitPauseAnimTime, true);
					}
				}
			}
		}
	}

	return true;
}

void WRCombatInfo::Destroy(bool bForce)
{
	if (bForce)
	{
		bReserveDestroy = true;
	}
	else
	{
		if (DelayedPacket.Num() != 0)
		{
			DeleteTimer();
		}
		else
		{
			bReserveDestroy = true;
		}
	}
}

void WRCombatInfo::DeleteTimer()
{
	FTimerDelegate DeleTimer = FTimerDelegate::CreateRaw(this, &WRCombatInfo::CallbackOverDelayTimer);
	WRActorUtility::SetTimer(InProcessDelayedPacketTiemr, DeleTimer, 2.0f, false);
}

bool WRCombatInfo::IsReservedDestroy()
{
	return bReserveDestroy;
}

bool WRCombatInfo::IsDelayedPacketTimerActive()
{
	if (WRActorUtility::IsTimerActive(InProcessDelayedPacketTiemr))
	{
		return true;
	}

	return false;
}

void WRCombatInfo::CallbackOverDelayTimer()
{
	bReserveDestroy = true;
}

//inattacker inHitChar ���� �ϳ��� �� �ڽ�
//InHitChar�� �÷��̾� �ϰ�� DetermineHit �̰� ��ü�� ȣ����� �ʴ´�. �´� ����ڿ��� ó�� �ϱ� ����
/*
���⼺�� ���� �����̸� ���и��� ��󿡰Դ� �׻� ����
���и��⿡ ���⼺�� ��� ������̵� ������ ��

�̰� ȣ��Ǵ� ������ �ǰ� ����ڰ� defense�����϶���.
return true�� ���, false�� hit
*/
bool WRCombatInfo::DetermineBlocking(AWRCharacter* InAttacker, AWRCharacter* InHitChar)
{
	if (nullptr == InAttacker || nullptr == InHitChar)
		return false;

	//���⼺�� ���� �����̸� ���и��� ��󿡰Դ� �׻� ����
	if (GetSkillTable()->IsNoneHeightFlag())
	{
		//���⼺�� ���� ������ ��� ����� middle�̰ų� noneŸ���� �� �������϶� ������ ��
		if (InAttacker->IsMe())
		{
			if (InHitChar->IsPlayer())//���� �������, ��Ʈ ����� �������� �ʰ� �����ڰ� ������ �ϰ� ��
			{
				FString HitCharLeftS;
				FString HitCharRightS;
				FString HitCharSectionName = InHitChar->GetCurrentMontageSectionName();
				HitCharSectionName.Split("_", &HitCharLeftS, &HitCharRightS);
				if (HitCharRightS.IsEmpty())	//����ڰ� ���⼺�� ���� ���и��� ���̸�
				{
					return true;
				}
				else
				{
					if (HitCharRightS.Compare("middle") == 0)	//�ߴܸ��� ���̸� ��� ����
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
			else
			{
				WRCombatInfo* NpcCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InHitChar->GetActorID());
				if (NpcCombatInfo && NpcCombatInfo->GetSkillTable())
				{
					//���и��⿡ ���⼺�� ��� ������̵� ������ ��
					if (NpcCombatInfo->GetSkillTable()->IsNoneHeightFlag())
					{
						return true;
					}
					else
					{
						if (NpcCombatInfo->GetSkillTable()->IsInHeightFlag(EWRAttackHeightType::Middle))
						{
							return true;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
		else //InHitChar�� �� �ڽ��� ��
		{
			//�� �ִ� ����
			FString MyLeftS;
			FString MyRightS;
			FString MyCharacterSectionName = InHitChar->GetCurrentMontageSectionName();
			MyCharacterSectionName.Split("_", &MyLeftS, &MyRightS);

			//���� ���⼺�� ���� ���и��⸦ �������̸� ��� ����
			if (MyRightS.IsEmpty())
			{
				return true;
			}
			else
			{
				//�����ڰ� ���⼺ ���� ������ �������϶� ���� middle������̸� ��� ����
				if (MyRightS.Compare("middle") == 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	else
	{
		if (InAttacker->IsMe())
		{
			//�� �ִ� ����
			FString MyLeftS;
			FString MyRightS;
			FString MyCharacterSectionName = InAttacker->GetCurrentMontageSectionName();
			MyCharacterSectionName.Split("_", &MyLeftS, &MyRightS);

			if (InHitChar->IsPlayer())	//���� �������, ��Ʈ ����� �������� �ʰ� �����ڰ� ������ �ϰ� ��
			{
				FString HitCharLeftS;
				FString HitCharRightS;
				FString HitCharSectionName = InHitChar->GetCurrentMontageSectionName();
				HitCharSectionName.Split("_", &HitCharLeftS, &HitCharRightS);
				if (MyRightS.Compare(HitCharRightS) == 0)	//���� ������ ������
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
				WRCombatInfo* NpcCombatInfo = WRCombatManager::Get()->FindLastCombatInfo(InHitChar->GetActorID());
				if (NpcCombatInfo && NpcCombatInfo->GetSkillTable())
				{
					//���и��⿡ ���⼺�� ��� ������̵� ������ ��
					if (NpcCombatInfo->GetSkillTable()->IsNoneHeightFlag())
					{
						return true;
					}
					else
					{
						EWRAttackHeightType HeightType;
						if (MyRightS.IsEmpty())
						{
							HeightType = GetSkillTable()->GetAttackHeightFlag();
						}
						else
						{
							HeightType = (EWRAttackHeightType)WREnumUtility::StringToEnum("EWRAttackHeightType", MyRightS);
						}

						if (NpcCombatInfo->GetSkillTable()->IsInHeightFlag(HeightType))
						{
							return true;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
		else //InHitChar�� �� �ڽ��� ��
		{
			//�� �ִ� ����
			FString MyLeftS;
			FString MyRightS;
			FString MyCharacterSectionName = InHitChar->GetCurrentMontageSectionName();
			MyCharacterSectionName.Split("_", &MyLeftS, &MyRightS);

			if (InAttacker->IsPlayer())
			{
				//������
				FString AttackLeftS;
				FString AttackRightS;
				FString AttackerSectionName = InAttacker->GetCurrentMontageSectionName();
				AttackerSectionName.Split("_", &AttackLeftS, &AttackRightS);
				if (MyRightS.Compare(AttackRightS) == 0)	//���� ������ ������
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
				EWRAttackHeightType HeightType = (EWRAttackHeightType)WREnumUtility::StringToEnum("EWRAttackHeightType", MyRightS);
				if (GetSkillTable()->IsInHeightFlag(HeightType))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return false;
}

void WRCombatInfo::RequestToServer_GuardBreak(AWRCharacter* InCaster, AWRCharacter* InTarget)
{
	if (IsValidServerSeqID())
	{
		IMMessageSender::RequestGuardBreak(InCaster, InTarget, GetCombatSeqID());
	}
	else
	{
		FWRDelayedPacketInfo DelayedInfo;
		DelayedInfo.Type = EWRDelayedPacketType::GuardBreak;
		DelayedInfo.CasterAID = InCaster->GetActorID();
		DelayedInfo.TargetAID = InTarget->GetActorID();

		DelayedPacket.Add(DelayedInfo);
	}
}

void WRCombatInfo::RequestToServer_BlockSuccessed(AWRCharacter* InAttacker, AWRCharacter* InBlocker)
{
	if (IsValidServerSeqID())
	{
		IMMessageSender::RequestSuccessToShield(InAttacker, InBlocker, GetCombatSeqID());
	}
	else
	{
		FWRDelayedPacketInfo DelayedInfo;
		DelayedInfo.Type = EWRDelayedPacketType::Block;
		DelayedInfo.CasterAID = InAttacker->GetActorID();
		DelayedInfo.TargetAID = InBlocker->GetActorID();

		DelayedPacket.Add(DelayedInfo);
	}
}

void WRCombatInfo::RequestToServer_Hit(AWRCharacter* InCaster, AWRCharacter* InTarget, EWRCollisionType InCollisionType)
{
	if (IsValidServerSeqID())
	{
		IMMessageSender::ReqestHitResult(InCaster, InTarget, GetCombatSeqID(), InCollisionType);
	}
	else
	{
		FWRDelayedPacketInfo DelayedInfo;
		DelayedInfo.Type = EWRDelayedPacketType::Hit;
		DelayedInfo.CasterAID = InCaster->GetActorID();
		DelayedInfo.TargetAID = InTarget->GetActorID();

		DelayedInfo.CommonValue = (int32)InCollisionType;

		DelayedPacket.Add(DelayedInfo);
	}
}

void WRCombatInfo::RequestToServer_Avoid(AWRCharacter* InCaster, uint8 AvoidDir)
{
	if (IsValidServerSeqID())
	{
		IMMessageSender::RequestAvoid(InCaster, GetCombatSeqID(), AvoidDir);
	}
	else
	{
		FWRDelayedPacketInfo DelayedInfo;
		DelayedInfo.Type = EWRDelayedPacketType::Avoid;
		DelayedInfo.CasterAID = InCaster->GetActorID();
		DelayedInfo.AvoidDir = AvoidDir;

		DelayedPacket.Add(DelayedInfo);
	}
}

void WRCombatInfo::ExecuteGuardBreak(AID_t InAID)
{
	AWRCharacter* Defender = WRCharacterManager::Get()->FindbyAID(InAID);
	if (nullptr == Defender)
		return;

	Defender->PlayAnimJumpToSection(EWRFSM::Defense, "GuardBreak");

	// [ 2019-11-13 : animal14 ] 
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_PATH_4_BLOCKING, Defender, nullptr);
	if (VFX == nullptr)
	{
		return;
	}
}

void WRCombatInfo::ExecuteBlockSuccessed(AID_t InAttackerAID, AID_t InBlockAID)
{
	AWRCharacter* BlockingChar = WRCharacterManager::Get()->FindbyAID(InBlockAID);
	if (BlockingChar)
	{
		BlockingChar->PlayAnimJumpToSection(EWRFSM::Defense, "Blocking");

		//���� ����Ʈ ����
		WRVFXManager::Get()->Spawn(EFFECT_PATH_4_BLOCKING, BlockingChar, nullptr);
	}

	AWRCharacter* Attacker = WRCharacterManager::Get()->FindbyAID(InAttackerAID);
	if (Attacker)
	{
		switch (GetSkillTable()->SkillType)
		{
		case EWRSkillType::Hook:
		{
			Attacker->DetermineFSMSignal(EWRFSM::Idle);

			if (Attacker->IsPlayer())
			{
				(Cast<AWRCharacterPlayer>(Attacker))->CutOffHookChain(false, nullptr, false);
			}
		}
		break;
		case EWRSkillType::ChainProjectile:
		case EWRSkillType::Projectile:
		{
		}
		break;
		default:
		{
			Attacker->DetermineFSMSignal(EWRFSM::BeBlocked);

			// [ 2020-3-18 : animal14 ] 
			if (Attacker->IsMe())
			{
				FWRSkillTable* Table = this->GetSkillTable();
				if (Table == nullptr)
					return;

				float Duration = Attacker->GetCurrentMontageSectionLength();
				WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::Block, Duration);
			}
		}
		break;
		}
	}
}

void WRCombatInfo::ExecuteHit(AID_t InHitChar, int32 InDamage, bool bExecuteDynamicMovement)
{
	AWRCharacter* Attacker = GetAttacker();
	if (nullptr == Attacker)
		return;

	FWRSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWRSkillTable>(CombatSeqParam.SkillTID, GetAttacker()->GetSkillTableFileName());
	if (nullptr == SkillTable)
		return;

	AWRCharacter* HitChar = WRCharacterManager::Get()->FindbyAID(InHitChar);
	if (nullptr == HitChar)
		return;

	if (bExecuteDynamicMovement)
	{
		UWRActorComponentDynamicMovement* DynamicMoveComponent = HitChar->GetComponent<UWRActorComponentDynamicMovement>();
		if (DynamicMoveComponent)
		{
			DynamicMoveComponent->DetermineDynamicMovement(SkillTable->TargetDynamicMovementTID, Attacker);
		}
	}
	

	if (0 < InDamage)
	{
		MakeHit(HitChar);
	}

	//hpó����
	if (HitChar->IsAlive())
	{
		HitChar->DetermineFSMSignal(EWRFSM::Damage);
	}
	else
	{
		// [ 2020-4-2 : dlwlgus9125 ] Disappear ��Ŷ ����ó���� ���� ��� �߰�
		HitChar->SetRemoveImmediately(false);

		if (HitChar->GetCharacterType() == EWRCharacterType::ENPC)
		{
			// [2020-03-31 | LJH] �ı��Ǵ� ���� ��� �ߵ� ���� ���� �ӽ��ڵ� ������ Ÿ�Ա����� �Ϸ�Ǹ� ��ġ �̵��� ���ɼ� ����
			FWREnpcTable* InEnpcTable = WRTableManager::Get()->FindRow<FWREnpcTable>(HitChar->GetCharacterTableID());
			if (InEnpcTable != nullptr)
			{
				if (InEnpcTable->DeadEffectType == EWRDeadEffectType::Destructible)
				{
					UWRDestructibleComponent* InDestructibleComp = Cast<UWRDestructibleComponent>(HitChar->GetComponentByClass(UWRDestructibleComponent::StaticClass()));

					if (InDestructibleComp != nullptr)
					{
						MakeHit(HitChar);
						InDestructibleComp->BeginDestruction();
						// [ 2020-05-11 : EZHyun ] ĸ�� �浹 ��Ȱ��ȭ
						WRActorUtility::SetComponentCollisionEnabled(HitChar->GetRootComponent(), ECollisionEnabled::NoCollision);

					}
				}
			}
		}

		HitChar->DetermineFSMSignal(EWRFSM::Die, true);

		// [2019-12-20 | LJH] ����� �Ÿ����������� Collision ��Ȱ��ȭ �� ���� �ӽ��ڵ� ���� ���������� �����Ǿ�� ��
		TSet<UActorComponent*> CompSet = HitChar->GetComponents();

		for (UActorComponent* InComp : CompSet)
		{
			if (InComp == nullptr)
				continue;

			FString CompName = InComp->GetName();
			if (CompName == TEXT("collision"))
			{
				UCapsuleComponent* InCapsuleComp = Cast<UCapsuleComponent>(InComp);
				if (InCapsuleComp != nullptr)
					InCapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

void WRCombatInfo::ExecuteParrying(AID_t InAID)
{
	AWRCharacter* Target = WRCharacterManager::Get()->FindbyAID(InAID);
	if (nullptr == Target)
		return;

	WRVFXManager::Get()->Spawn(EFFECT_PATH_4_PARRYING, GetAttacker(), Target);
	Target->DetermineFSMSignal(EWRFSM::BeParried);
}

void WRCombatInfo::Tick4Hit()
{
	FVector CurrentLocation = this->GetComponentLocation();
	if (CurrentLocation.SizeSquared() < FLT_EPSILON)
	{
		return;
	}

	PreviousLocations.Add(CurrentLocation);
}

void WRCombatInfo::MakeHit(AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
	{
		return;
	}

	if (InCharacter->IsMe())
	{
		this->MakeHit4Me(InCharacter);
	}
	else
	{
		this->MakeHit4Other(InCharacter);
	}

	PreviousLocations.Empty();
}

void WRCombatInfo::MakeHit(AWRCharacter* InCharacter, const FVector& InLocation, const FRotator& InRotator)
{
	FWRSkillTable* Table = GetSkillTable();
	if (Table == nullptr)
	{
		return;
	}

	// [ 2019-10-22 : animal14 ] Ÿ�� ����Ʈ ����
	//AWRVFX* VFX = WRVFXManager::Get()->Spawn(Table->HitFX.ToString(), InCharacter, nullptr);
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(Table->HitFX, InCharacter, nullptr);
	if (VFX == nullptr)
	{

	}
	else
	{
		VFX->SetActorLocationAndRotation(InLocation, InRotator);
	}
}

void WRCombatInfo::MakeHit4Me(AWRCharacter* InCharacter)
{
	FVector CurrentLocation = this->GetComponentLocation();
	FVector PreviousLocation = this->GetPreviousLocation(CurrentLocation);

	FVector AttackDirection = this->GetAttackDirection(InCharacter, PreviousLocation, CurrentLocation);

	this->MakeDisplay4Me(InCharacter, CurrentLocation, AttackDirection);
	this->MakeRestraint(InCharacter);
}

void WRCombatInfo::MakeHit4Other(AWRCharacter* InCharacter)
{
	AWRCharacter* Attacker = WRCharacterManager::Get()->FindbyAID(CombatSeqParam.AttackerUID);
	if (Attacker == nullptr)
	{
		return;
	}

	this->SetCharacter(InCharacter);

	FVector CurrentLocation = this->GetComponentLocation();
	FVector PreviousLocation = this->GetPreviousLocation(CurrentLocation);

	FVector AttackDirection = this->GetAttackDirection(InCharacter, PreviousLocation, CurrentLocation);
	FVector ForwardDirection = this->GetForwardDirection();
	FVector PhysicsDirection = this->GetPhysicsDirection(InCharacter, AttackDirection, ForwardDirection);

	float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CullingDistance);

	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return;
	}

#if UE_EDITOR
	EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;

	if (InCharacter->IsDebug)
	{
		DrawDebugLine(World, CurrentLocation, CurrentLocation + (AttackDirection * Distance), FColor::Blue, false, 5.0F);
		DrawDebugLine(World, CurrentLocation, CurrentLocation + (PhysicsDirection * Distance), FColor::Yellow, false, 5.0F);

		DrawDebugType = EDrawDebugTrace::ForDuration;
	}
#else // UE_EDITOR
	EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
#endif // UE_EDITOR

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery4);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Attacker);

	TArray<FHitResult> Hits;
	if (UKismetSystemLibrary::LineTraceMultiForObjects(World, CurrentLocation, CurrentLocation + (ForwardDirection * Distance), ObjectTypes, false, ActorsToIgnore, DrawDebugType, Hits, true))
	{
		for (auto& Hit : Hits)
		{
			if (InCharacter->IsAlive() == false)
			{
				continue;
			}
			if (Hit.Actor != InCharacter)
			{
				continue;
			}
			if (Hit.Component->IsA(UMeshComponent::StaticClass()) == false)
			{
				continue;
			}

			this->MakeDisplay4Other(InCharacter, Hit.ImpactPoint, AttackDirection);
			// [ 2019-12-4 : animal14 ] �ӽ� �ڵ�
			this->MakeDecal(InCharacter, AttackDirection);

			break;
		}

		/*if (InCharacter->IsAlive() == false)
		{
			WRSkeletalMeshModifier::CheckSkeletalMeshToSlice(InCharacter, Hits);
		}*/
	}
	else
	{
		this->MakeDisplay4Other(InCharacter, InCharacter->GetActorLocation(), AttackDirection);
		// [ 2019-12-4 : animal14 ] �ӽ� �ڵ�
		this->MakeDecal(InCharacter, AttackDirection);
	}
}

void WRCombatInfo::SetCharacter(AWRCharacter* InCharacter)
{
	InCharacter->SetMurdererID(CombatSeqParam.AttackerUID);
}

FVector WRCombatInfo::GetComponentLocation()
{
	AWRCharacter* Attacker = WRCharacterManager::Get()->FindbyAID(CombatSeqParam.AttackerUID);
	if (Attacker == nullptr)
	{
		return FVector::ZeroVector;
	}

	if (Attacker->GetCharacterType() == EWRCharacterType::Player)
	{
		FWRSkillTable* Table = GetSkillTable();
		if (Table == nullptr)
		{
			return FVector::ZeroVector;
		}

		UWRActorComponentEquipmentLook* ActorComponent = Attacker->GetComponent<UWRActorComponentEquipmentLook>();
		if (ActorComponent == nullptr)
		{
			return FVector::ZeroVector;
		}

#ifdef REFACTORING_EQUIPMENT_BY_ANIMAL14_
		UMeshComponent* MeshComponent = ActorComponent->GetMesh(Table->EquipType);
#else // REFACTORING_EQUIPMENT_BY_ANIMAL14_
		UMeshComponent* MeshComponent = ActorComponent->GetMeshComponent(Table->EquipType);
#endif // REFACTORING_EQUIPMENT_BY_ANIMAL14_
		if (MeshComponent == nullptr)
		{
			return FVector::ZeroVector;
		}

		return MeshComponent->GetComponentLocation();
	}
	else
	{
		UCapsuleComponent* CapsuleComponent = Attacker->FindCapsuleCollisionComponent(EWRCollisionType::LeftAttack);
		if (CapsuleComponent)
		{
			ECollisionEnabled::Type CollisionEnabled = CapsuleComponent->GetCollisionEnabled();
			if (CollisionEnabled)
			{
				return CapsuleComponent->GetComponentLocation();
			}
		}

		CapsuleComponent = Attacker->FindCapsuleCollisionComponent(EWRCollisionType::RightAttack);
		if (CapsuleComponent)
		{
			ECollisionEnabled::Type CollisionEnabled = CapsuleComponent->GetCollisionEnabled();
			if (CollisionEnabled)
			{
				return CapsuleComponent->GetComponentLocation();
			}
		}

		return FVector::ZeroVector;
	}
}

FVector WRCombatInfo::GetPreviousLocation(const FVector& InCurrentLocation)
{
	if (PreviousLocations.Num())
	{
		return PreviousLocations.Last();
	}
	else
	{
		return FVector::ZeroVector;
	}

	//int32 Max = PreviousLocations.Num();
	//if (Max)
	//{
	//	int32 i = 0;
	//	for (; i < Max; ++i)
	//	{
	//		FVector Location = InCurrentLocation - PreviousLocations[i];
	//		if (Location.Size2D() < FLT_EPSILON)
	//		{
	//			break;
	//		}
	//	}
	//	return (Max == i) ? PreviousLocations[i - 1] : PreviousLocations[i];
	//}
	//else
	//{
	//	return FVector::ZeroVector;
	//}
}

FVector WRCombatInfo::GetAttackDirection(AWRCharacter* InCharacter, const FVector& InPreviousLocation, const FVector& InCurrentLocation)
{
	FVector Direction = InCurrentLocation - InPreviousLocation;
	Direction.Normalize();

	InCharacter->SetAttackDirection(Direction);

	return Direction;
}

FVector WRCombatInfo::GetForwardDirection()
{
	AWRCharacter* Attacker = WRCharacterManager::Get()->FindbyAID(CombatSeqParam.AttackerUID);
	if (Attacker == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector Direction = FVector::ZeroVector;
	if (Attacker->IsMe())
	{
		UActorComponent* ActorComponent = Attacker->GetComponentByClass(UCameraComponent::StaticClass());
		if (ActorComponent == nullptr)
		{
			return FVector::ZeroVector;
		}

		UCameraComponent* CameraComponent = Cast<UCameraComponent>(ActorComponent);
		if (CameraComponent == nullptr)
		{
			return FVector::ZeroVector;
		}

		Direction = CameraComponent->GetForwardVector();
		Direction.Normalize();
	}
	else
	{
		Direction = Attacker->GetActorForwardVector();
		Direction.Normalize();
	}
	return Direction;
}

FVector WRCombatInfo::GetPhysicsDirection(AWRCharacter* InCharacter, const FVector& InAttackDirection, const FVector& InForwardDirection)
{
	FVector Direction = InAttackDirection + InForwardDirection;
	Direction.Normalize();

	InCharacter->SetPhysicsDirection(Direction);

	return Direction;
}

float WRCombatInfo::GetHPRatio(AWRCharacter* InCharacter)
{
	float RemainHP = (float)InCharacter->GetHP() / (float)InCharacter->GetMaxHP();
	if (RemainHP > 0.5F)
	{
		return 0.0F;
	}
	else if (RemainHP <= 0.5F && RemainHP > 0.3F)
	{
		return 1.0F;
	}
	else if (RemainHP <= 0.3F && RemainHP > 0.1F)
	{
		return 2.0F;
	}
	else
	{
		return 3.0F;
	}
}

FString WRCombatInfo::TryLoad(AWRCharacter* InCharacter)
{
	if (InCharacter->GetCharacterType() == EWRCharacterType::Player)
	{
		return EFFECT_PATH_4_HIT;
	}
	if (InCharacter->GetCharacterType() == EWRCharacterType::NPC)
	{
		FWRNpcTable* Table = WRTableManager::Get()->FindRow<FWRNpcTable>(InCharacter->GetCharacterTableID());
		if (Table == nullptr)
		{
			return "";
		}

		//return Table->DMGFX.ToString();
		return Table->DMGFX;
	}
	if (InCharacter->GetCharacterType() == EWRCharacterType::ENPC)
	{
		FWREnpcTable* Table = WRTableManager::Get()->FindRow<FWREnpcTable>(InCharacter->GetCharacterTableID());
		if (Table == nullptr)
		{
			return "";
		}

		//return Table->DMGFX.ToString();
		return Table->DMGFX;
	}
	return "";
}

void WRCombatInfo::MakeDisplay4Me(AWRCharacter* InCharacter, const FVector& InLocation, const FVector& InDirection)
{
	// [ 2019-12-5 : animal14 ] Ÿ�� ����Ʈ ����
	this->MakeHit(InCharacter, InLocation, InDirection.Rotation());
	// [ 2019-10-22 : animal14 ] �ǰ� ����Ʈ ����
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(this->TryLoad(InCharacter), InCharacter, nullptr);
	if (VFX == nullptr)
	{
	}
	else
	{
		VFX->SetActorLocationAndRotation(InLocation, InDirection.Rotation());
	}
	// [ 2019-11-13 : animal14 ] �ǰ� ����
	InCharacter->MakeHitDisplay(InLocation);
	// [ 2019-12-4 : animal14 ] HP ����
	this->MakeHPDisplay(InCharacter);
	// [ 2020-1-15 : animal14 ] ȭ�� ����
	this->MakeStrongDisplay();
	this->MakeScreenDisplay();
}

void WRCombatInfo::MakeDisplay4Other(AWRCharacter* InCharacter, const FVector& InLocation, const FVector& InDirection)
{
	// [ 2019-12-5 : animal14 ] Ÿ�� ����Ʈ ����
	this->MakeHit(InCharacter, InLocation, InDirection.Rotation());
	// [ 2019-10-22 : animal14 ] �ǰ� ����Ʈ ����
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(this->TryLoad(InCharacter), InCharacter, nullptr);
	if (VFX == nullptr)
	{
	}
	else
	{
		VFX->SetActorLocationAndRotation(InLocation, InDirection.Rotation());
	}
	// [ 2019-11-13 : animal14 ] �ǰ� ����
	InCharacter->MakeHitDisplay(InLocation);
	// [ 2019-12-4 : animal14 ] HP ����
	this->MakeHPDisplay(InCharacter);
	// [ 2020-1-15 : animal14 ] ȭ�� ����
	this->MakeScreenDisplay();
}

#define WR_DECAL_LENGTH 100.0F

void WRCombatInfo::MakeDecal(AWRCharacter* InCharacter, const FVector& InDirection)
{
	//FString Filename = "/Game/Asset/BG/Megascans/Decal/Blood/M_Blood_Decal.M_Blood_Decal";
	FString Filename;
	if (InCharacter->GetCharacterType() == EWRCharacterType::NPC)
	{
		FWRNpcTable* Table = WRTableManager::Get()->FindRow<FWRNpcTable>(InCharacter->GetCharacterTableID());
		if (Table == nullptr)
		{
			return;
		}

		//Filename = Table->bloodstainPath.ToString();
		Filename = Table->bloodstainPath;
	}
	if (InCharacter->GetCharacterType() == EWRCharacterType::ENPC)
	{
		FWREnpcTable* Table = WRTableManager::Get()->FindRow<FWREnpcTable>(InCharacter->GetCharacterTableID());
		if (Table == nullptr)
		{
			return;
		}

		//Filename = Table->bloodstainPath.ToString();
		Filename = Table->bloodstainPath;
	}

	if (Filename.IsEmpty())
	{
		return;
	}

	UMaterialInterface* MaterialInterface = LoadObject<UMaterialInterface>(nullptr, *Filename);
	if (MaterialInterface == nullptr)
	{
		return;
	}

	FVector DecalSize = FVector::ZeroVector;
	DecalSize.X = 256.0F;
	DecalSize.Y = 128.0F;
	DecalSize.Z = 128.0F;

	FVector Direction = FVector::ZeroVector;
	Direction.X = InDirection.X;
	Direction.Y = InDirection.Y;

	FVector Location = InCharacter->GetActorLocation() + (Direction * WR_DECAL_LENGTH);

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(InCharacter, MaterialInterface, DecalSize, Location);
	if (DecalComponent == nullptr)
	{
		return;
	}

	float Duration = WRConsoleManager::Get()->GetFloat(EWRConsole::BloodDecal_Duration);
	DecalComponent->SetFadeOut(0.0F, Duration);
}

void WRCombatInfo::MakeHPDisplay(AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
	{
		return;
	}

	if (InCharacter->IsMe())
	{
		float HPDuration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::UI, "PCHitBlood_FB_Time");
		WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::HP, HPDuration);
	}
	else
	{
		float Ratio = this->GetHPRatio(InCharacter);
		InCharacter->SetMaterialParameterValue("HitBloodMain", Ratio);
	}
}

void WRCombatInfo::MakeStrongDisplay()
{
	FWRSkillTable* Table = this->GetSkillTable();
	if (Table == nullptr)
	{
		return;
	}

	if (Table->ScreenEffectTM)
	{
		if (Table->SkillType == EWRSkillType::Strong)
		{
			float Duration = Table->ScreenEffectTM * 0.001F;
			WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::Break, Duration);
		}
	}
	else
	{
	}
}

void WRCombatInfo::MakeScreenDisplay()
{
	FWRSkillTable* Table = this->GetSkillTable();
	if (Table == nullptr)
	{
		return;
	}

	if (Table->ScreenEffectTM)
	{
		float Duration = Table->ScreenEffectTM * 0.001F;
		switch (Table->ScreenEffectType)
		{
		case EWRDisplayType::ENPCToPCBlur:
			WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::ENPCToPCBlur, Duration);
			break;
		case EWRDisplayType::PCToENPCBlur:
			WRMaterialParameterCollectionManager::Get()->Spawn(EWRMaterialParameter::PCToENPCBlur, Duration);
			break;
		}
	}
	else
	{
	}
}

void WRCombatInfo::MakeRestraint(AWRCharacter* InCharacter)
{
	if (this->GetAttacker() == nullptr)
	{
		return;
	}

	FWREnpcSkillTable* SkillTable = WRTableManager::Get()->FindRow<FWREnpcSkillTable>(CombatSeqParam.SkillTID, GetAttacker()->GetSkillTableFileName());
	if (SkillTable == nullptr)
	{
		return;
	}

	int32 Duration = SkillTable->HitMoveLimitTM;
	InCharacter->StartRestraint(Duration);
}