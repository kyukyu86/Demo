// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVehicleComponentManageMaterial.h"

#include "Actor/Vehicle/WRVehicle.h"

#include "Components/SkeletalMeshComponent.h"

void UWRVehicleComponentManageMaterial::OnCreateComponent()
{
	Super::OnCreateComponent();

	// [ 2019-10-15 : animal14 ] ¹ÙÀÎµù
	Sets.SetNum((int32)EWRManageMaterial::Max);
	Sets[(int32)EWRManageMaterial::Spawn].BindUObject(this, &UWRVehicleComponentManageMaterial::SetSpawn);

	SetMaterialnterface();
}

void UWRVehicleComponentManageMaterial::OnDestroyComponent()
{
	ClearMaterialInterface();

	Super::OnDestroyComponent();
}

void UWRVehicleComponentManageMaterial::RestoreMaterialInterface()
{
	AWRVehicle* Vehicle = GetOwnerVehicle();
	if (Vehicle == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Vehicle->GetMesh();
	if (SkeletalMeshComponent == nullptr)
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

		SkeletalMeshComponent->SetMaterial(i, MaterialInterface);
	}
}

void UWRVehicleComponentManageMaterial::SetMaterial(int32 ElementIndex, class UMaterialInterface* Material)
{
	AWRVehicle* Vehicle = GetOwnerVehicle();
	if (Vehicle == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Vehicle->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	SkeletalMeshComponent->SetMaterial(ElementIndex, Material);
}

void UWRVehicleComponentManageMaterial::SetSpawn(float InRatio)
{
	AWRVehicle* Vehicle = GetOwnerVehicle();
	if (Vehicle == nullptr)
	{
		return;
	}
	Vehicle->SetMaterialParameterValue("00.SpawnMain", 1.0F - InRatio);
}

void UWRVehicleComponentManageMaterial::SetMaterialnterface()
{
	AWRVehicle* Vehicle = GetOwnerVehicle();
	if (Vehicle == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Vehicle->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	for (auto& MaterialInterface : SkeletalMeshComponent->GetMaterials())
	{
		MaterialInterfaces.Add(MaterialInterface);
	}
}

void UWRVehicleComponentManageMaterial::ClearMaterialInterface()
{
	MaterialInterfaces.Empty();
}
