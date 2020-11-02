// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRActorComponentManageMaterial.h"

#include "Actor/Character/WRCharacter.h"
#include "Actor/Character/WRCharacterIO.h"

#include "Manager/WRConsoleManager.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRCharacterManager.h"

#include "Components/DecalComponent.h"

#include "Kismet/GameplayStatics.h"

void UWRActorComponentManageMaterial::OnCreateComponent()
{
	Super::OnCreateComponent();

	// [ 2019-10-15 : animal14 ] ¹ÙÀÎµù
	Sets.SetNum((int32)EWRManageMaterial::Max);
	Sets[(int32)EWRManageMaterial::HeavyAttack].BindUObject(this, &UWRActorComponentManageMaterial::SetHeavyAttack);
	//Sets[(int32)EWRManageMaterial::SkillAttack].BindUObject(this, &UWRActorComponentManageMaterial::SetSkillAttack);
	Sets[(int32)EWRManageMaterial::Weakness].BindUObject(this, &UWRActorComponentManageMaterial::SetWeakness);
	Sets[(int32)EWRManageMaterial::Hit].BindUObject(this, &UWRActorComponentManageMaterial::SetHit);
	Sets[(int32)EWRManageMaterial::Spawn].BindUObject(this, &UWRActorComponentManageMaterial::SetSpawn);
	Sets[(int32)EWRManageMaterial::Dissolve].BindUObject(this, &UWRActorComponentManageMaterial::SetDissolve);
	Sets[(int32)EWRManageMaterial::Contruct].BindUObject(this, &UWRActorComponentManageMaterial::SetConstruct);
	Sets[(int32)EWRManageMaterial::ChangeCloth].BindUObject(this, &UWRActorComponentManageMaterial::SetChangeCloth);
	

	SetMaterialnterface();
}

void UWRActorComponentManageMaterial::OnDestroyComponent()
{
	ClearMaterialInterface();

	Super::OnDestroyComponent();
}

void UWRActorComponentManageMaterial::RestoreMaterialInterface()
{
	AWRCharacterIO* SelectedComponentOwner = Cast<AWRCharacterIO>(GetOwnerCharacter());
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	UMeshComponent* MeshComponent = SelectedComponentOwner->GetCurrentMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < MaterialInterfaces.Num(); ++i)
	{
		UMaterialInterface* MaterialInterface = MaterialInterfaces[i];
		if (MaterialInterface == nullptr)
		{
			continue;
		}

		MeshComponent->SetMaterial(i, MaterialInterface);
	}
}

void UWRActorComponentManageMaterial::SetMaterial(int32 ElementIndex, class UMaterialInterface* Material)
{
	AWRCharacterIO* SelectedComponentOwner = Cast<AWRCharacterIO>(GetOwnerCharacter());
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	UMeshComponent* MeshComponent = SelectedComponentOwner->GetCurrentMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	MeshComponent->SetMaterial(ElementIndex, Material);
}

void UWRActorComponentManageMaterial::SetHeavyAttack(float InRatio)
{
	AWRCharacter* SelectedComponentOwner = GetOwnerCharacter();
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}
	SelectedComponentOwner->SetMaterialParameterValue("HeavyAttack", InRatio);
}

//void UWRActorComponentManageMaterial::SetSkillAttack(float InRatio)
//{
//	AWRCharacter* SelectedComponentOwner = Cast<AWRCharacter>(this->GetOwner());
//	if (SelectedComponentOwner == nullptr)
//	{
//		return;
//	}
//
//	SelectedComponentOwner->SetMaterialParameterValue("SkillAttack", InRatio);
//}

void UWRActorComponentManageMaterial::SetWeakness(float InRatio)
{
	AWRCharacter* SelectedComponentOwner = GetOwnerCharacter();
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	if (SelectedComponentOwner->IsMe())
	{
	}
	else
	{
		SelectedComponentOwner->SetMaterialParameterValue("HitRim", InRatio);
	}
}

void UWRActorComponentManageMaterial::SetHit(float InRatio)
{
	AWRCharacter* SelectedComponentOwner = GetOwnerCharacter();
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	if (SelectedComponentOwner->IsMe())
	{
		WRMaterialParameterCollectionManager::Get()->SetHit(InRatio);
	}
	else
	{
		SelectedComponentOwner->SetMaterialParameterValue("HitPart", 1.0F - InRatio);
	}
}

void UWRActorComponentManageMaterial::SetSpawn(float InRatio)
{
	AWRCharacter* SelectedComponentOwner = GetOwnerCharacter();
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	if (SelectedComponentOwner->IsMe())
	{
	}
	else
	{
		SelectedComponentOwner->SetMaterialParameterValue("00.SpawnMain", 1.0F - InRatio);
	}
}

void UWRActorComponentManageMaterial::SetDissolve(float InRatio)
{
	AWRCharacter* SelectedComponentOwner = GetOwnerCharacter();
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	if (SelectedComponentOwner->IsMe())
	{
	}
	else
	{
		SelectedComponentOwner->SetMaterialParameterValue("Dissolve", 1.0F - InRatio);
	}
}

void UWRActorComponentManageMaterial::SetConstruct(float InRatio)
{
	AWRCharacterIO* SelectedComponentOwner = Cast<AWRCharacterIO>(GetOwnerCharacter());
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	UMeshComponent* MeshComponent = SelectedComponentOwner->GetCurrentMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	SelectedComponentOwner->SetMaterialParameterValue("00.SpawnMain", 1.0F - InRatio, MeshComponent);
}

void UWRActorComponentManageMaterial::SetChangeCloth(float InRatio)
{
	AWRCharacter* SelectedComponentOwner = Cast<AWRCharacter>(GetOwnerCharacter());
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	UMeshComponent* MeshComponent = SelectedComponentOwner->GetMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	SelectedComponentOwner->SetMaterialParameterValue("00.ClothChangingMain", 1.0F - InRatio, MeshComponent);
}

void UWRActorComponentManageMaterial::SetMaterialnterface()
{
	AWRCharacterIO* SelectedComponentOwner = Cast<AWRCharacterIO>(GetOwnerCharacter());
	if (SelectedComponentOwner == nullptr)
	{
		return;
	}

	UMeshComponent* MeshComponent = SelectedComponentOwner->GetCurrentMesh();
	if (MeshComponent == nullptr)
	{
		return;
	}

	for (auto& MaterialInterface : MeshComponent->GetMaterials())
	{
		MaterialInterfaces.Add(MaterialInterface);
	}
}

void UWRActorComponentManageMaterial::ClearMaterialInterface()
{
	MaterialInterfaces.Empty();
}