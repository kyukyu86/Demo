// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentConnectbyCable.h"
#include "Engine/World.h"
#include "Ticker.h"
#include "Manager/WRConsoleManager.h"
#include "WRActorComponentEquipmentLook.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRCharacterManager.h"
#include "Utility/WRActorUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMesh.h"
#include "../../CableComponent/Source/CableComponent/Classes/CableComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "Sound/SoundCue.h"
#include "Actor/StaticMeshActor/WRStaticMeshActor.h"
#include "Actor/Character/WRCharacter.h"
#include "LogMacros.h"

TMap<uint32, FWRReservedCut*> UWRActorComponentConnectbyCable::GCutDirection;
uint32 UWRActorComponentConnectbyCable::GCutDirectionID = 0;

#define IGNORE_BONE_NAME_ARRAY TArray<FString>() = { "root", "cloth", "ghost", "weapon", "pinky", "index", "ik", "feather" }

// For Cpp
DEFINE_LOG_CATEGORY(ConnectByCable)
DEFINE_LOG_CATEGORY(CableCollision)

//====================================================================================
// [ 2019-10-7 : magedoga ] Cut direction

FWRReservedCut::FWRReservedCut(uint32 InID, class UWRActorComponentConnectbyCable* InConnectCableComponent, UCableComponent* InCableComponent, class AWRStaticMeshActor* InMeshActor, AActor* InImpactLocationActor, class UMaterialInstanceDynamic* InMaterial)
	: ID(InID)
	, ConnectCableComponent(InConnectCableComponent)
	, CableComponent(InCableComponent)
	, MeshActor(InMeshActor)
	, ImpactLocationActor(InImpactLocationActor)
	, DynamicMaterial(InMaterial)
{
	checkf(InCableComponent->IsValidLowLevel() == true, TEXT("FWRReservedCut - null CableComponent"));
	InCableComponent->bAttachStart = false;

	UWorld* World = InCableComponent->GetWorld();
	World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateRaw(this, &FWRReservedCut::OnEndCutDirect), WRConsoleManager::Get()->GetFloat(EWRConsole::Cable_CutDirectionTime), false);
}

void FWRReservedCut::OnEndCutDirect()
{
	UWRActorComponentConnectbyCable::EndCutDirection(ID);
}

uint32 UWRActorComponentConnectbyCable::CreateCutDirection(UWRActorComponentConnectbyCable* InConnectCableComponent, UCableComponent* InCableComponent, AWRStaticMeshActor* InMeshActor, AActor* InImpcatLocationActor, class UMaterialInstanceDynamic* InMaterial)
{
	if (InConnectCableComponent->IsValidLowLevel() == false || InCableComponent->IsValidLowLevel() == false || InMeshActor->IsValidLowLevel() == false)
		return -1;

	uint32 _ID = GCutDirectionID;
	GCutDirection.Add(_ID, new FWRReservedCut(_ID, InConnectCableComponent, InCableComponent, InMeshActor, InImpcatLocationActor, InMaterial));
	++GCutDirectionID;

	return _ID;
}

void UWRActorComponentConnectbyCable::EndCutDirection(uint32 InID)
{
	if (GCutDirection.Contains(InID) == false)
		return;

	FWRReservedCut* ReservedCut = GCutDirection[InID];
	
	// [ 2019-10-7 : magedoga ] Remove from TimerManager
	if (ReservedCut->TimerHandle.IsValid() == true)
	{
		UWorld* World = ReservedCut->ConnectCableComponent->GetWorld();
		World->GetTimerManager().ClearTimer(ReservedCut->TimerHandle);
	}

	// [ 2019-10-7 : magedoga ] Destroy Connect Objects
	UWRActorComponentConnectbyCable::DestroyObject(ReservedCut->ConnectCableComponent->GetOwnerCharacter(), ReservedCut->CableComponent, ReservedCut->MeshActor, ReservedCut->ImpactLocationActor, ReservedCut->DynamicMaterial);

	// [ 2019-10-7 : magedoga ] Reset Direction ID
	ReservedCut->ConnectCableComponent->RemoveCutDirectionID(InID);

	delete ReservedCut;
	GCutDirection.Remove(InID);
}

void UWRActorComponentConnectbyCable::EmptyCutDirection()
{
	for (auto It = GCutDirection.CreateIterator(); It; ++It)
	{
		delete It.Value();
	}

	GCutDirection.Empty();
}

//====================================================================================




void UWRActorComponentConnectbyCable::OnDestroyComponent()
{
	//if (CutDirectionID >= 0)
	if(CutDirectionIDs.Num() > 0)
	{
		for (auto ID : CutDirectionIDs)
		{
			UWRActorComponentConnectbyCable::EndCutDirection(ID);
		}
	}

	this->Cut(true);
}


//====================================================================================



//====================================================================================

bool UWRActorComponentConnectbyCable::ConnectByAsset(class AWRCharacter* InTarget, class UWRChainWeaponAsset* InAsset, FWRConnectCableEventDelegate InEventDelegate /* = nullptr */, const EWRCollisionType IN InPart /* = EWRCollisionType::None */)
{
	if (InTarget == nullptr || InAsset == nullptr)
		return false;

	FWRConnectByCableParams TempParam = InAsset;
	TempParam.Target = InTarget;
	TempParam.ConnectCableEventDelegate = InEventDelegate;
	TempParam.HookPart = InPart;

	return this->Connect_Internal(TempParam);
}

bool UWRActorComponentConnectbyCable::ConnectByAsset(const FVector& InWorldLocation, class UWRChainWeaponAsset* InAsset, FWRConnectCableEventDelegate InEventDelegate /*= nullptr*/)
{
	if (InAsset == nullptr)
		return false;

	FWRConnectByCableParams TempParam = InAsset;
	TempParam.Target = nullptr;
	TempParam.WorldPosition = InWorldLocation;
	TempParam.ConnectCableEventDelegate = InEventDelegate;

	return this->Connect_Internal(TempParam);
}

bool UWRActorComponentConnectbyCable::ConnectByParam(const FWRConnectByCableParams& InParams)
{
	return this->Connect_Internal(InParams);
}

bool UWRActorComponentConnectbyCable::ReturnToBase()
{
	if (this->IsConnecting() == false)
		return false;

	MovementState = EWRCableMovementState::ToCaster;
	return true;
}

void UWRActorComponentConnectbyCable::Cut(const bool bInImmediately /*= false*/, const bool bInForce /*= false*/)
{
	if (bInForce == false && this->IsWorking() == false)
		return;

	this->SetEnableTick(false);

	CableComponent->SolverIterations = Param.CableTightValue;

	// [ 2019-10-7 : magedoga ] 즉시 삭제
	if (bInImmediately == true)
	{
		UWRActorComponentConnectbyCable::DestroyObject(GetOwnerCharacter(), CableComponent, MeshActor, ImpactLocationActor, DynamicMaterial);
	}
	// [ 2019-10-7 : magedoga ] 연출 후 삭제
	else
	{
		uint32 _CutDirectionID = UWRActorComponentConnectbyCable::CreateCutDirection(this, CableComponent, MeshActor, ImpactLocationActor, DynamicMaterial);
		CutDirectionIDs.AddTail(_CutDirectionID);
		
		// [ 2019-10-10 : magedoga ] Spawn Sound
		WRSFXManager::Get()->SpawnAtLocation(Cast<USoundBase>(Param.CutChainSound.LoadSynchronous()), WRActorUtility::GetSocketTransform(GetOwner(), Param.CasterSocket).GetTranslation());
	}

	CableComponent = nullptr;
	MeshActor = nullptr;

	if (Param.Target->IsValidLowLevel() == true && Param.Target->IsAttachedTo(MeshActor))
	{
		Param.Target->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	}

	MovementState = EWRCableMovementState::Ready;

	// [ 2020-5-7 : kyu ] Release Cable Component To Target
	Param.Target->SetConnectedHookCableComponent(nullptr);

}

void UWRActorComponentConnectbyCable::DestroyObject(AWRCharacter* InOwnerCharacter, UCableComponent* InCableComponent, class AWRStaticMeshActor* InMeshActor, AActor* InImpactLocationActor, class UMaterialInstanceDynamic* InMaterial)
{
	if (InCableComponent->IsValidLowLevel() == true)
	{
		if (InOwnerCharacter->IsValidLowLevel() == true)
		{
			InOwnerCharacter->RemoveInstanceComponent(InCableComponent);
		}

		InCableComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		InCableComponent->UnregisterComponent();
		InCableComponent->ConditionalBeginDestroy();
	}

	if (InMeshActor->IsValidLowLevel() == true)
	{
		InMeshActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		InMeshActor->Destroy();
	}

	if (InImpactLocationActor->IsValidLowLevel() == true)
	{
		InImpactLocationActor->RemoveFromRoot();
		InImpactLocationActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		InImpactLocationActor->Destroy();
	}

	if (InMaterial->IsValidLowLevel() == true)
	{
		InMaterial->RemoveFromRoot();
	}
}

FVector UWRActorComponentConnectbyCable::GetChainLocation() const
{
	if(this->IsWorking() == false)
		return FVector::ZeroVector;

	return MeshActor->GetActorLocation();
}

bool UWRActorComponentConnectbyCable::AttachAtoB(EWRConnectByCableActor InA, EWRConnectByCableActor InB, FAttachmentTransformRules InAttachRule)
{
	USceneComponent* ASceneComponent = this->GetAttachComponent(InA);
	if (ASceneComponent->IsValidLowLevel() == false)
	{
		bAttachWhenSpawnMeshActorCompleted = true;
	}

	USceneComponent* BSceneComponent = this->GetAttachComponent(InB);
	if (BSceneComponent->IsValidLowLevel() == false)
	{
		bAttachWhenSpawnMeshActorCompleted = true;
	}

	if (bAttachWhenSpawnMeshActorCompleted)
	{
		if (EWRConnectByCableActor::Chain != InA || EWRConnectByCableActor::Target != InB)
		{
			bAttachWhenSpawnMeshActorCompleted = false;
		}

		return false;
	}


	bool bResult = false;
	if (this->ShouldUseImpactLocation() == true && InA == EWRConnectByCableActor::Chain && InB == EWRConnectByCableActor::Target)
	{
		bResult = ASceneComponent->AttachToComponent(ImpactLocationActor->GetRootComponent(), InAttachRule);
	}
	else
	{
		FName AttachSocketName = this->GetAttachSocketName(InB);
		bResult = ASceneComponent->AttachToComponent(BSceneComponent, InAttachRule, AttachSocketName);
	}

	return bResult;
}

//====================================================================================

void UWRActorComponentConnectbyCable::DetachComponent(EWRConnectByCableActor InActorType)
{
	USceneComponent* SceneComponent = this->GetAttachComponent(InActorType);
	if (SceneComponent->IsValidLowLevel() == false)
		return;

	SceneComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

//====================================================================================

USceneComponent* UWRActorComponentConnectbyCable::GetAttachComponent(EWRConnectByCableActor InKindofActor)
{
	USceneComponent* AttachComponent = nullptr;
	switch (InKindofActor)
	{
	case EWRConnectByCableActor::Chain: { if (MeshActor->IsValidLowLevel() == true) { AttachComponent = MeshActor->GetRootComponent(); } } break;
	case EWRConnectByCableActor::Caster: { if (GetOwner()->IsValidLowLevel() == true) { AttachComponent = GetOwnerCharacter()->GetMesh(); } } break;
	case EWRConnectByCableActor::Target: { if (Param.Target->IsValidLowLevel() == true) { AttachComponent = Param.Target->GetMesh(); } } break;
	}

	return AttachComponent;
}

FName UWRActorComponentConnectbyCable::GetAttachSocketName(EWRConnectByCableActor InKindofActor)
{
	FName AttachSocketName = NAME_None;
	switch (InKindofActor)
	{
	case EWRConnectByCableActor::Caster: { AttachSocketName = Param.CasterSocket; } break;
	case EWRConnectByCableActor::Target: { AttachSocketName = Param.TargetSocket; } break;
	}

	return AttachSocketName;
}

//====================================================================================

bool UWRActorComponentConnectbyCable::Connect_Internal(const FWRConnectByCableParams& InParam)
{
	if (this->IsValidParam(InParam) == false)
		return false;

	if (this->IsReady() == false)
		return false;

	// [ 2019-9-18 : magedoga ] Spawn Mesh Actor
	UWorld* World = this->GetWorld();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Owner = GetOwnerCharacter();
	MeshActor = World->SpawnActor<AWRStaticMeshActor>(AWRStaticMeshActor::StaticClass(), spawnParams);

	if (MeshActor == nullptr)
	{
		UE_LOG(ConnectByCable, Display, TEXT("UWRActorComponentConnectbyCable::Connect - Can't Spawn Mesh Actor"));
		return false;
	}

	Param = InParam;

	AWRCharacter* OwnerCharacter = GetOwnerCharacter();
	// [ 2019-9-18 : magedoga ] Cable Component
	CableComponent = NewObject<UCableComponent>(OwnerCharacter);
	CableComponent->EndLocation = FVector::ZeroVector;
	CableComponent->CableLength = Param.CableLength;
	CableComponent->CableWidth = Param.CableWidth;
	CableComponent->SetMaterial(0, Param.ChainMaterial.LoadSynchronous());
	CableComponent->bEnableCollision = false;

	this->UpdateMaterialTileByLength();

	// [ 2019-9-18 : magedoga ] Attach Cable Component to Owner Character
	CableComponent->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Param.CasterSocket);

	// [ 2019-9-24 : magedoga ] Register to World
	CableComponent->RegisterComponentWithWorld(World);

	OwnerCharacter->AddInstanceComponent(CableComponent);

	MeshActor->SetActorEnableCollision(false);
	MeshActor->GetStaticMeshComponent()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	ImpactLocationActor = nullptr;

	// [ 2019-10-8 : magedoga ] Use Impact Location
	if (this->ShouldUseImpactLocation() == true)
	{
		FHitResult HR;
		FVector HMDLocation = WRInputSystemManager::Get()->GetHMDLocation();
		FVector HMDForward = WRInputSystemManager::Get()->GetHMDRotation().Quaternion().GetForwardVector();
		float TargettingRayLength = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingRayLength);
		FVector EndLocation = HMDLocation + HMDForward * TargettingRayLength;

		FName NearestBoneName = NAME_None;
		// [ 2019-10-10 : magedoga ] 1. Physic body와 충돌검사
		// [ 2019-10-10 : magedoga ] 1-1. Mesh
		if (this->GetHitResultFromTarget(HR, HMDLocation, EndLocation, true) == false)
		{
			// [ 2019-10-10 : magedoga ] 1-2. Root(Capsule)
			// [ 2019-10-10 : magedoga ] Get Nearest SocketName
			this->GetHitResultFromTarget(HR, HMDLocation, EndLocation);

			// [ 2020-1-13 : kyu ] SocketTitle_PartName_01 과 같은 형식의 임의 소켓이 존재하는지 체크하여 가장 가까운 위치의 본네임 획득
			bool bSocket = false;
			NearestBoneName = WRActorUtility::GetNearestSocketNameWithTitleName(Param.Target, HR.ImpactPoint, Param.HookSocketTitleName.ToString(), Param.HookPart);
			if (NearestBoneName != NAME_None)
			{
				bSocket = true;
			}
			else
			{
				// [ 2020-1-13 : kyu ] Socket형식이 없으면 가장 가까운 Bone으로 검사
				NearestBoneName = WRActorUtility::GetNearestBoneName(Param.Target, HR.ImpactPoint, IGNORE_BONE_NAME_ARRAY);
			}

			// [ 2019-10-10 : magedoga ] Get Impact Location by nearest socket
			EndLocation = WRActorUtility::GetBoneTransform(Param.Target, NearestBoneName).GetTranslation();

			// 재검사
			if (this->GetHitResultFromTarget(HR, HMDLocation, EndLocation, true))
			{
				// 유효하면 HR 그대로 사용
			}
			else if (NearestBoneName != NAME_None)
			{
				// 검출실패시 Bone위치를 그대로 사용
				if (bSocket)
				{
					HR.ImpactPoint = WRActorUtility::GetSocketTransform(Param.Target, NearestBoneName).GetTranslation();
				}
				else
				{
					HR.ImpactPoint = WRActorUtility::GetBoneTransform(Param.Target, NearestBoneName).GetTranslation();
				}
			}
			else
			{
				UE_LOG(ConnectByCable, Error, TEXT("UWRActorComponentConnectbyCable::Connect_Internal - Couldn't get hit result."));
				this->Cut(true, true);
				return false;
			}
			// 			else
			// 			{
			// 				UKismetSystemLibrary::DrawDebugSphere(this, HR.ImpactPoint, 10.0f, 20, FLinearColor::White, 15.0f);
			// 			}
		}
		else
		{
			NearestBoneName = WRActorUtility::GetNearestSocketNameWithTitleName(Param.Target, HR.ImpactPoint, Param.HookSocketTitleName.ToString(), Param.HookPart);
			if (NearestBoneName != NAME_None)
			{
				HR.ImpactPoint = WRActorUtility::GetSocketTransform(Param.Target, NearestBoneName).GetTranslation();
			}
			else
			{
				NearestBoneName = WRActorUtility::GetNearestBoneName(Param.Target, HR.ImpactPoint, IGNORE_BONE_NAME_ARRAY);
				HR.ImpactPoint = WRActorUtility::GetBoneTransform(Param.Target, NearestBoneName).GetTranslation();
			}
		}

		// [ 2019-10-10 : magedoga ] 2. Spawn Actor at ImpactLocation for Attach
		ImpactLocationActor = World->SpawnActor<AActor>(AActor::StaticClass()/*, HR.ImpactPoint, HR.ImpactNormal.Rotation()*/);
		if (ImpactLocationActor->IsValidLowLevel() == false)
		{
			UE_LOG(ConnectByCable, Display, TEXT("UWRActorComponentConnectbyCable::Connect_Internal - Null ImpactLocationActor"));
			this->Cut(true, true);
			return false;
		}

		ImpactLocationActor->AddToRoot();
		USceneComponent* ActorSceneComponent = NewObject<USceneComponent>(ImpactLocationActor);
		ActorSceneComponent->RegisterComponentWithWorld(World);
		ImpactLocationActor->SetRootComponent(ActorSceneComponent);
		ImpactLocationActor->SetActorTransform(FTransform(HR.ImpactNormal.Rotation(), HR.ImpactPoint, FVector::OneVector));

		// [ 2019-10-10 : magedoga ] 3. Attach ImpactLocationActor to Socket
		ImpactLocationActor->GetRootComponent()->AttachToComponent(Param.Target->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, NearestBoneName);
	}

	// [ 2019-9-23 : magedoga ] Non Movement
	if (Param.bProjectile == false)
	{
		if (Param.Target != nullptr)
		{
			this->AttachAtoB(EWRConnectByCableActor::Chain, EWRConnectByCableActor::Target, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			MeshActor->SetActorLocation(Param.WorldPosition);
			MeshActor->SetActorRotation((Param.WorldPosition - OwnerCharacter->GetActorLocation()).Rotation());
		}

		MovementState = EWRCableMovementState::Arrival;
	}
	else
	{
		FTransform TRSocket = OwnerCharacter->GetMesh()->GetSocketTransform(Param.CasterSocket);
		MeshActor->SetActorLocation(TRSocket.GetTranslation());

		this->SetEnableTick(true);

		MovementState = EWRCableMovementState::ToTarget;
	}

	// [ 2019-9-18 : magedoga ] Set EndPoint Actor
	CableComponent->SetAttachEndTo(MeshActor, NAME_None);

	if (Param.EndpointShape != nullptr)
	{
		MeshActor->GetStaticMeshComponent()->SetStaticMesh(Param.EndpointShape);
	}
	else
	{
		UWRActorComponentEquipmentLook* EquipmentLookComponent = OwnerCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipmentLookComponent != nullptr)
		{
			UStaticMesh* StaticMesh = EquipmentLookComponent->GetStaticMesh(EWREquipPart::LeftHand);
			MeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
		}
	}

	MeshActor->SetActorScale3D(FVector(Param.MeshScale));

	// [ 2020-1-16 : kyu ] for Cable Material
	DynamicMaterial = UMaterialInstanceDynamic::Create(CableComponent->GetMaterial(0), nullptr);
	CableComponent->SetMaterial(0, DynamicMaterial);
	if (Param.Target->GetCharacterType() == EWRCharacterType::ENPC
		|| Param.Target->GetCharacterType() == EWRCharacterType::Player
		|| Param.Target->GetCharacterType() == EWRCharacterType::IO)
	{
		if (GetOwnerCharacter()->IsMe())
		{
			AWRCharacterMine* CastedMine = Cast<AWRCharacterMine>(GetOwnerCharacter());
			if (CastedMine)
			{
				FOnCutTime DG;
				DG.BindUObject(CastedMine, &AWRCharacterMine::OnChainCutTime);
				SetOnCurTime(true, DG);
			}
		}
		else
		{
			SetOnCurTime(true, nullptr);
		}
	}

	// [ 2020-5-7 : kyu ] Save Cable Component To Target
	Param.Target->SetConnectedHookCableComponent(this);


	//여기가 호출되기 전에 HitResult가 호출되면 HitHook 호출시점에 component가 없기 땜시롱 attach가 제대로 되지 않는다.
	//해당 상황에 플래그를 하나두고 생성이 완료 되었을때 어태치를 시도한다.
	if (bAttachWhenSpawnMeshActorCompleted)
	{
		//hithook이전에 먼저 초기화 해야 한다. 
		bAttachWhenSpawnMeshActorCompleted = false;
		AWRCharacterPlayer* Player = Cast<AWRCharacterPlayer>(GetOwnerCharacter());
		if (Player)
		{
			Player->HitHook();
		}
	}

	return true;
}

//====================================================================================



//====================================================================================

void UWRActorComponentConnectbyCable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// [ 2019-9-27 : magedoga ] Moving
	this->TickForMove(DeltaTime);

	if (this->IsWorking() == true)
	{
		// [ 2019-9-27 : magedoga ] Update Cable Force
		this->UpdateForceByDistance();
	}

	// [ 2020-1-16 : kyu ] Chain Cut
	this->TickForCut(DeltaTime);
}


bool UWRActorComponentConnectbyCable::IsValidParam(const FWRConnectByCableParams& InParams)
{
	if (InParams.Target == nullptr && InParams.WorldPosition == FVector::ZeroVector)
		return false;

	if (GetOwnerCharacter() == nullptr)
		return false;

	return true;
}

bool UWRActorComponentConnectbyCable::IsReady() const
{
	return MovementState == EWRCableMovementState::Ready;
}

bool UWRActorComponentConnectbyCable::IsConnecting() const
{
	return MovementState == EWRCableMovementState::Arrival;
}

bool UWRActorComponentConnectbyCable::IsWorking() const
{
	return this->IsConnecting() == true || this->IsMoving() == true;
}

bool UWRActorComponentConnectbyCable::HasTarget() const
{
	return Param.Target->IsValidLowLevel();
}

bool UWRActorComponentConnectbyCable::IsConnectingPart(const EWRCollisionType IN InPart)
{
	return Param.HookPart == InPart;
}

void UWRActorComponentConnectbyCable::EnableCableCollision(AWRCharacter* InOwner)
{
	if (nullptr == MeshActor || nullptr == MeshActor->GetStaticMeshComponent())
		return;

	if (nullptr == InOwner)
	{
		MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}

	if (MeshActor->GetStaticMeshComponent()->GetCollisionProfileName() != FName("WRAttackObject"))
	{
		//-------------------------------------------------------------------
		MeshActor->SetActorEnableCollision(true);
		WRActorUtility::SetComponentCollisionEnabled(MeshActor->GetRootComponent(), ECollisionEnabled::NoCollision);

		//-------------------------------------------------------------------
		MeshActor->GetStaticMeshComponent()->SetCollisionProfileName(FName("WRAttackObject"));
		MeshActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		MeshActor->GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
		MeshActor->GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(GetOwnerCharacter(), &AWRCharacter::OnBeginOverlapCapsuleAttacking);
	}
}

void UWRActorComponentConnectbyCable::UpdateCableLocationOnVehicle(FVector InLocationToModify)
{
	if (IsMoving() == false) // && IsWorking() == false)
		return;

	FVector CurrentLocation = MeshActor->GetActorLocation();
	FVector ModifiedLocation = CurrentLocation + InLocationToModify;

	MeshActor->SetActorLocation(ModifiedLocation);
}

bool UWRActorComponentConnectbyCable::IsMoving() const
{
	return MovementState == ToTarget || MovementState == ToCaster;
}

AWRCharacter* UWRActorComponentConnectbyCable::GetCableTarget()
{
	return Param.Target;
}

FVector UWRActorComponentConnectbyCable::GetImpactLocation()
{
	if (this->IsWorking() == false)
		return FVector::ZeroVector;

	if (ImpactLocationActor == nullptr)
		return FVector::ZeroVector;

	return ImpactLocationActor->GetActorLocation();
}

//====================================================================================




//====================================================================================

FVector UWRActorComponentConnectbyCable::GetDestinationPosition()
{
	FVector DestinationPosition = FVector::ZeroVector;
	switch (MovementState)
	{
	case EWRCableMovementState::ToTarget:
	{
		if (this->ShouldUseImpactLocation() == true)
		{
			DestinationPosition = ImpactLocationActor->GetActorLocation();
		}
		else if (Param.Target->IsValidLowLevel() == true)
		{
			DestinationPosition = Param.Target->GetMesh()->GetSocketLocation(Param.TargetSocket);
		}
		else
		{
			DestinationPosition = Param.WorldPosition;
		}

	} break;
	case EWRCableMovementState::ToCaster:
	{
		AWRCharacter* OwnerCharacter = this->GetOwnerCharacter();
		checkf(OwnerCharacter->IsValidLowLevel(), TEXT("UWRActorComponentConnectbyCable::GetDestinationPosition() - Null  Caster"));

		DestinationPosition = OwnerCharacter->GetMesh()->GetSocketLocation(Param.CasterSocket);
	} break;
	}

	return DestinationPosition;
}

float UWRActorComponentConnectbyCable::GetDistanceCasterToMeshActor()
{
	if (MeshActor == nullptr)
		return 0.0f;

	float Distance = FVector::Dist(MeshActor->GetActorLocation(), GetOwner()->GetActorLocation());
	return Distance;
}

bool UWRActorComponentConnectbyCable::GetHitResultFromTarget(FHitResult& OutHR, const FVector& InStart, const FVector& InEnd, bool bUseMeshComponent /*= false*/)
{
	if (Param.Target->IsValidLowLevel() == false)
		return false;

// 	FHitResult HR;
// 	UPrimitiveComponent* PrimitiveComponent = bUseMeshComponent == true ? Param.Target->GetMesh() : Cast<UPrimitiveComponent>(Param.Target->GetRootComponent());
// 	float SphereRadius = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingSphereRadius);
// 	if (PrimitiveComponent != nullptr && PrimitiveComponent->SweepComponent(HR, InStart, InEnd, FQuat::Identity, FCollisionShape::MakeSphere(SphereRadius)) == true)
// 	{
// 		OutHR = HR;
// 		return true;
// 	}

	//====================================================================================

	FHitResult HR;
	if (bUseMeshComponent == true)
	{
		FCollisionQueryParams QP = FCollisionQueryParams::DefaultQueryParam;
		QP.AddIgnoredActor(WRCharacterManager::Get()->GetMyCharacter());

		if (Param.Target->GetMesh()->LineTraceComponent(HR, InStart, InEnd, QP) == true)
		{
			OutHR = HR;
			return true;
		}
	}
	else
	{
		float SphereRadius = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_TargettingSphereRadius);
 		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Param.Target->GetRootComponent());
		if (PrimitiveComponent != nullptr && PrimitiveComponent->SweepComponent(HR, InStart, InEnd, FQuat::Identity, FCollisionShape::MakeSphere(SphereRadius)) == true)
		{
			OutHR = HR;
			return true;
		}
	}

	return false;
}

bool UWRActorComponentConnectbyCable::ShouldUseImpactLocation() const
{
	return Param.bUseImpactLocation == true && Param.Target->IsValidLowLevel() == true;
}

//====================================================================================

void UWRActorComponentConnectbyCable::UpdateMaterialTileByLength()
{
	CableComponent->TileMaterial = FMath::Clamp(CableComponent->CableLength * 0.1f, 20.0f, 30.0f);
}

void UWRActorComponentConnectbyCable::UpdateForceByDistance()
{
	float Distance = GetDistanceCasterToMeshActor();
	CableComponent->CableForce = FVector(0.0f, 0.0f, FMath::Clamp(-Distance, -2000.0f, 0.0f));
}

//====================================================================================

bool UWRActorComponentConnectbyCable::TickForMove(float InDeletaTime)
{
	if (this->IsMoving() == false)
		return false;

	float FrameMoveSpeed = InDeletaTime * Param.MoveSpeed;
	
	FVector PrePosition = MeshActor->GetActorLocation();
	FVector DestinationPosition = GetDestinationPosition();
	FVector MoveDirection = (DestinationPosition - PrePosition).GetSafeNormal();
	FVector NextPosition = PrePosition + MoveDirection * FrameMoveSpeed;
	FVector NextToDestDirection = (DestinationPosition - NextPosition).GetSafeNormal();

	bool bArrival = false;
	// [ 2019-9-23 : magedoga ] CCD
	if (FVector::DotProduct(MoveDirection, NextToDestDirection) < 0.0f)
	{
		bArrival = true;
	}
	// [ 2019-9-23 : magedoga ] Check Collision (Sphere, Dot)
	else
	{
		float Radius = WRConsoleManager::Get()->GetFloat(EWRConsole::Cable_CollisionRadius);
		if (FVector::Dist(NextPosition, DestinationPosition) < Radius)
		{
			bArrival = true;
		}
	}

	MeshActor->SetActorLocation(NextPosition);

	// [ 2019-9-27 : magedoga ] Setting Chain Mesh Rotation
	FQuat QuatMovewDirection = MoveDirection.Rotation().Quaternion();
	MeshActor->SetActorRotation(MovementState == EWRCableMovementState::ToTarget ? QuatMovewDirection.Rotator() : (-QuatMovewDirection.GetForwardVector()).Rotation());

	// [ 2019-9-27 : magedoga ] Arrival Mesh Actor
	if (bArrival == true)
	{
		if (MovementState == EWRCableMovementState::ToTarget)
		{
			MovementState = EWRCableMovementState::Arrival;
			Param.ConnectCableEventDelegate.ExecuteIfBound(EWRConnectCableEvent::ArrivalToTarget);
		}
		else if (MovementState == EWRCableMovementState::ToCaster)
		{
			MovementState = EWRCableMovementState::Ready;
			Param.ConnectCableEventDelegate.ExecuteIfBound(EWRConnectCableEvent::ArrivalToCaster);
		}
		else
		{
			UE_LOG(ConnectByCable, Fatal, TEXT("Please check MovementState. Not ToTarget or ToCaster."));
			return false;
		}
	}

	return !bArrival;
}

//====================================================================================

void UWRActorComponentConnectbyCable::SetOnCurTime(const bool IN InOn, FOnCutTime IN InDG)
{
	bActiveCutTime = InOn;
	fDuringTime = 0.f;
	if (bActiveCutTime)
	{
		OnCutTime = InDG;
	}
	else
	{
		OnCutTime.Unbind();
	}
}

EWRCollisionType UWRActorComponentConnectbyCable::GetCablePart()
{
	return Param.HookPart;
}

void UWRActorComponentConnectbyCable::TickForCut(float InDeletaTime)
{
	if (bActiveCutTime)
	{
		fDuringTime += InDeletaTime;
		if (fDuringTime >= const_CutTime)
		{
			fDuringTime = const_CutTime;
			OnCutTime.ExecuteIfBound();
		}
		else
		{
			UpdateChainColor();
		}
	}
}

void UWRActorComponentConnectbyCable::UpdateChainColor()
{
	if (DynamicMaterial->IsValidLowLevel() == false)
		return;

	float fRate = fDuringTime / const_CutTime;
	if (fRate < 0.f)
		fRate = 0.f;
	else if (fRate > 1.f)
		fRate = 1.f;
	DynamicMaterial->SetScalarParameterValue("05ChainTimeLimit", fRate);
}
