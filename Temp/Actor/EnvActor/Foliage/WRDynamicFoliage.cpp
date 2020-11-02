// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRDynamicFoliage.h"
#include "Actor/Character/WRCharacter.h"
#include "Engine/Engine.h"
#include "Manager/WRCharacterManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRFoliageTable.h"
#include "Engine/DataTable.h"

#define MAIN_MATERIAL_INDEX 0

void AWRDynamicFoliage::BeginPlay()
{
	Super::BeginPlay();

	//this->InitializeFoliage();
}

void AWRDynamicFoliage::Tick(float DeltaTime)
{
	AID_t InAID = this->GetMyOwner();
	AWRCharacter* InOwner = WRCharacterManager::Get()->FindbyAID(InAID);

	if (InOwner == nullptr)
		return;

	FVector OwnerPos = InOwner->GetActorLocation();
	FVector MyPos = this->GetActorLocation();

	FVector SumVec = MyPos - OwnerPos;
	FVector ToTarget = (MyPos - OwnerPos).GetSafeNormal();

	float DotResult = FVector::DotProduct(InOwner->GetActorForwardVector().GetSafeNormal(), SumVec.GetSafeNormal());

	float InOwnerDistance = this->GetDistanceTo(InOwner);

	FVector TempVec;

	SumVec.ToDirectionAndLength(TempVec, InOwnerDistance);

	USkeletalMeshComponent* InSkelMeshComp = Cast<USkeletalMeshComponent>(this->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (InSkelMeshComp == nullptr)
		return;

	if ((DotResult < 0.0f&& InOwnerDistance > MinLength) || InOwnerDistance > MaxLength)
	{
		UWRFoliageInstancedStaticMeshComponent* InInstanceFoliage = this->GetInstanceFoliage();

		if (InInstanceFoliage == nullptr)
			return;

		InInstanceFoliage->AddInstance(this->GetTransform());
		this->Destroy();
	}

	if (IsFoliageInitialized == true)
	{
		AttenuationGlow();
	}

}

void AWRDynamicFoliage::OnComponentHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (IsFoliageInitialized == true)
	{
		IncreaseGlow();
	}
	else
	{
		InitializeFoliage();
	}
}

void AWRDynamicFoliage::InitializeFoliage()
{
	USkeletalMeshComponent* InSkelMeshComp = Cast<USkeletalMeshComponent>(this->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

	if (InSkelMeshComp == nullptr)
		return;

	InSkelMeshComp->GetMaterial(MAIN_MATERIAL_INDEX)->GetScalarParameterValue(FName("Glow Overlay Intensity"), CurrentGlow);

	DynamicMaterial = UMaterialInstanceDynamic::Create(InSkelMeshComp->GetMaterial(MAIN_MATERIAL_INDEX), InSkelMeshComp);
	InSkelMeshComp->SetMaterial(MAIN_MATERIAL_INDEX, DynamicMaterial);

	InSkelMeshComp->OnComponentHit.AddDynamic(this, &AWRDynamicFoliage::OnComponentHit);

	StandardGlow = CurrentGlow;

	FWRFoliageTable* FoliageTable = WRTableManager::Get()->FindRow<FWRFoliageTable>(this->GetTID());
	if (FoliageTable == nullptr)
		return;

	if (FoliageTable->FoliageTID == this->GetTID())
	{
		if (FoliageTable->MaxGlow > 0.f && FoliageTable->IncreaseGlowSize > 0.f&& FoliageTable->AttenuationRatio > 0.f)
		{
			this->MaxGlow = FoliageTable->MaxGlow;
			this->AttenuationRatio = FoliageTable->AttenuationRatio;
			this->IncreaseGlowSize = FoliageTable->IncreaseGlowSize;
		}
	}

	IsFoliageInitialized = true;
}

void AWRDynamicFoliage::AttenuationGlow()
{
	if (StandardGlow < CurrentGlow)
	{
		CurrentGlow -= IncreaseGlowSize * AttenuationRatio;
		DynamicMaterial->SetScalarParameterValue(FName("Glow Overlay Intensity"), CurrentGlow);
	}
}

void AWRDynamicFoliage::IncreaseGlow()
{
	CurrentGlow += IncreaseGlowSize;
	if (MaxGlow <= CurrentGlow)
		CurrentGlow = MaxGlow;
	DynamicMaterial->SetScalarParameterValue(FName("Glow Overlay Intensity"), CurrentGlow);
}
