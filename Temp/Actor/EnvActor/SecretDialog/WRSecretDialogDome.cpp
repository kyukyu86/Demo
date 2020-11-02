// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSecretDialogDome.h"
#include "Network/IMMessageSender.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Manager/WREnvInteractionManager.h"
#include "Table/WREnvObjectTable.h"
#include "Table/Base/WRTableManager.h"
#include "Manager/WRSFXManager.h"

// 현재는 종류가 한개 뿐이라 임시용으로 대응 추후 랜덤으로 공간이 늘어나게되면 패킷에서 제공하는 TID로 대응
#define SecretDialogAreaTID 90009

void AWRSecretDialogDome::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;

	this->InStaticMeshComp = this->FindComponentByClass<UStaticMeshComponent>();

	if (this->InStaticMeshComp != nullptr)
	{
		this->InMaterial = this->InStaticMeshComp->CreateDynamicMaterialInstance(0);
		if (this->InMaterial != nullptr)
		{
			this->InStaticMeshComp->SetMaterial(0, this->InMaterial);
			this->InMaterial->SetScalarParameterValue("SpawnMain", 0.f);

			this->InStaticMeshComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWRSecretDialogDome::OnBeginOverlap);
			this->InStaticMeshComp->OnComponentEndOverlap.AddUniqueDynamic(this, &AWRSecretDialogDome::OnEndOverlap);
		}
	}

	FWRSecretDialogAreaTable * InPortalTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (InPortalTable == nullptr)
		return;

	this->DomeRadius = InPortalTable->LeaveDistance;
	this->MaxDeltaTime = InPortalTable->DomeOpenTimer;

	OverlapedActorList.Empty();

	
}

void AWRSecretDialogDome::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	
	if (IsReserveEndDialog == true)
	{
		if (CurrentDeltaTime >= 0.f)
		{
			CurrentDeltaTime -= DeltaTime;

			if (this->InMaterial != nullptr)
			{
				if (this->EffectTimeCloseCurve == nullptr)
					return;
				float InCurrentSize = this->EffectTimeCloseCurve->GetFloatValue(this->CurrentDeltaTime);
				//float InDomeSpawnProgress = CurrentDeltaTime / MaxDeltaTime;
				//InDomeSpawnProgress = FMath::Clamp(InDomeSpawnProgress, 0.00001f, MaxDeltaTime);
				this->InMaterial->SetScalarParameterValue("SpawnMain", InCurrentSize);
			}
		}
		else
		{
			WREnvInteractionManager::Get()->UnSpawnSecretDialogDome();
		}
		
		return;
	}
	else
	{
		if (CurrentDeltaTime <= MaxDeltaTime)
		{
			CurrentDeltaTime += DeltaTime;

			if (this->InMaterial != nullptr)
			{
				if (IsPlayedSpawnsound == false)
				{
					FWRSecretDialogAreaTable * InPortalTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
					if (InPortalTable == nullptr)
						return;
					IsPlayedSpawnsound = true;
					WRSFXManager::Get()->SpawnAtLocation(InPortalTable->SecretDialogSpawnAudioPath, GetActorLocation());
				}
				if (this->EffectTimeOpenCurve == nullptr)
					return;
				float InCurrentSize = this->EffectTimeOpenCurve->GetFloatValue(this->CurrentDeltaTime);
				this->InMaterial->SetScalarParameterValue("SpawnMain", InCurrentSize);
			}
		}
	}

	FVector InVecDistance = this->GetActorLocation() - WRCharacterManager::Get()->GetMyCharacter()->GetActorLocation();

	if (InVecDistance.Size() >= this->DomeRadius)
	{

		IMMessageSender::RequestEndOfSecretDialog(this->GroupID);
		IsReserveEndDialog = true;
		PrimaryActorTick.bCanEverTick = false;
	}
}

void AWRSecretDialogDome::BeginDestroy()
{
	Super::BeginDestroy();

	if (this->InStaticMeshComp != nullptr)
	{
		this->InStaticMeshComp->OnComponentBeginOverlap.RemoveAll(this);
		this->InStaticMeshComp->OnComponentEndOverlap.RemoveAll(this);
	}
}

void AWRSecretDialogDome::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWRCharacter* InChar = Cast<AWRCharacter>(Other);
	if (InChar != nullptr)
	{
		if (InChar->IsMe() == true)
			return;

		if (InChar->GetDialogGroupID() == this->GroupID)
			return;

		if (OverlapedActorList.Contains(Other) == true)
			return;

		InChar->AddVisibilityBit(EWRVisibility::SpecialConversation);
		InChar->SetCharacterInvisible(false);
		OverlapedActorList.Add(Other);
	}
}

void AWRSecretDialogDome::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRCharacter* InChar = Cast<AWRCharacter>(Other);
	if (InChar != nullptr)
	{
		if (InChar->IsMe() == true)
			return;

		if (OverlapedActorList.Contains(Other) == false)
			return;

		if (this->IsOverlappingActor(Other) == true)
			return;

		InChar->RemoveVisibilityBit(EWRVisibility::SpecialConversation);
		InChar->SetCharacterInvisible(true);
		OverlapedActorList.Remove(Other);
	}
}

void AWRSecretDialogDome::ReserveDestroy(bool InReserve)
{
	this->IsReserveEndDialog = InReserve;
		
	FWRSecretDialogAreaTable * InPortalTable = WRTableManager::Get()->FindRow<FWRSecretDialogAreaTable>(SecretDialogAreaTID);
	if (InPortalTable == nullptr)
		return;
	WRSFXManager::Get()->SpawnAtLocation(InPortalTable->SecretDialogUnspawnAudioPath, GetActorLocation());
}
