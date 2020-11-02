// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSlicedActor.h"

#include "ProceduralMeshComponent.h"

#include "Define/WRPathDefine.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRConsoleManager.h"

#include "Enum/EWRMaterialParameterCollection.h"

#include "Component/Character/WRActorComponentEquipmentLook.h"

#include "UnrealMathUtility.h"

void AWRSlicedActor::BeginDestroy()
{
	this->SetActorTickEnabled(false);
	TArray<UActorComponent*> Components = this->GetComponents().Array();
	for (UActorComponent* It : Components)
	{
		if (It->IsRegistered() == true)
			It->UnregisterComponent();

		It->RemoveFromRoot();
		It->DestroyComponent();
	}
	Super::BeginDestroy();
}

bool AWRSlicedActor::CheckStillInWorld()
{
	/*this->SetActorTickEnabled(false);
	TArray<UActorComponent*> Components = this->GetComponents().Array();
	for (UActorComponent* It : Components)
	{
		if (It->IsRegistered() == true)
			It->UnregisterComponent();

		It->RemoveFromRoot();
		It->DestroyComponent();
	}*/
	return Super::CheckStillInWorld();
}

// Called every frame
void AWRSlicedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->ProcessRemoveDisplay(DeltaTime);
	this->SetDynamicMaterialValue(DeltaTime);
}

void AWRSlicedActor::Initialize()
{
	InProcMeshComp = NewObject<UProceduralMeshComponent>(this);
	if (InProcMeshComp == nullptr)
		return;
	InProcMeshComp->AddToRoot();
	this->AddInstanceComponent(InProcMeshComp);
	this->AddOwnedComponent(InProcMeshComp);
	this->SetRootComponent(InProcMeshComp);
	InProcMeshComp->bUseComplexAsSimpleCollision = false;
	InProcMeshComp->RegisterComponent();

	//this->CreateDynamicMaterials();	
}

void AWRSlicedActor::SpawnVFXForProcMeshComp()
{
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_CASCADE_PATH_4_DEAD, this, nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	if (InProcMeshComp != nullptr)
	{
		FVector ComponentScale = InProcMeshComp->GetComponentScale();
		float AverageScale = ComponentScale.SizeSquared() / 3;;

		VFX->SetFloatParameter(TEXT("ENPC_Size"), FMath::Sqrt(AverageScale));
	}

	VFX->OnComplete().AddUObject(this, &AWRSlicedActor::CompleteDissolve);
}

void AWRSlicedActor::CompleteDissolve()
{
	this->SetActorTickEnabled(false);
	TArray<UActorComponent*> Components = this->GetComponents().Array();
	for (UActorComponent* It : Components)
	{
		if (It->IsRegistered() == true)
			It->UnregisterComponent();

		It->RemoveFromRoot();
		It->DestroyComponent();
	}
	this->Destroy();
}

void AWRSlicedActor::CreateDynamicMaterials()
{
	
	DissolveDuration = WRMaterialParameterCollectionManager::Get()->GetScalarParameterValue(EWRMPC::Character, "Death01_DissolveTime");

	auto ConvertToMID = [&](UMaterialInterface* InMaterialInterface)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(InMaterialInterface);
		if (MID == nullptr)
		{
			MID = UMaterialInstanceDynamic::Create(InMaterialInterface, InProcMeshComp);
		}

		return MID;
	};

	int32 NumOfMaterial = InProcMeshComp->GetNumMaterials();

	for (int32 i = 0; i < NumOfMaterial; ++i)
	{
		UMaterialInterface* MaterialInterface = InProcMeshComp->GetMaterial(i);
		UMaterialInstanceDynamic* MID = ConvertToMID(MaterialInterface);
		if (InProcMeshComp->GetMaterial(i) != MID)
			InProcMeshComp->SetMaterial(i, MID);

		MIDs.Add(MID);
	}
}

void AWRSlicedActor::ProcessRemoveDisplay(float InDeltaTime)
{
	if (IsDeadPlay == false)
	{
		return;
	}

	FVector CurrentLocation = InProcMeshComp->GetComponentLocation();

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

void AWRSlicedActor::MakeRemoveDisplay()
{
	// [ 2020-2-19 : animal14 ] 이팩트 생성
	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_CASCADE_PATH_4_DEAD, this, nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	if (InProcMeshComp)
	{
		FVector ComponentScale = InProcMeshComp->GetComponentScale();
		float AverageScale = ComponentScale.SizeSquared() / 3;;

		VFX->SetFloatParameter(TEXT("ENPC_Size"), FMath::Sqrt(AverageScale));
	}

	// [ 2019-11-13 : animal14 ] 이벤트 바인드
	VFX->OnComplete().AddUObject(this, &AWRSlicedActor::CompleteDissolve);

	// [ 2020-2-19 : animal14 ] 디졸브 연출
	IsDissolvePlay = true;

	IsDeadPlay = false;
}

void AWRSlicedActor::SetDynamicMaterialValue(float InDeltaTime)
{
	if (IsDissolvePlay == false)
	{
		return;
	}

	DissolveDeltaTime += InDeltaTime;

	float Ratio = FMath::Clamp(DissolveDeltaTime / DissolveDuration, 0.0F, 1.0F);
	for (UMaterialInstanceDynamic* MID : MIDs)
	{
		MID->SetScalarParameterValue("Dissolve", Ratio);
	}
}