// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSpiderWeb.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#define MAIN_MATERIAL_INDEX 0

void AWRSpiderWeb::BeginPlay()
{
	Super::BeginPlay();

	InitializeWeb();
}

void AWRSpiderWeb::Tick(float DeltaTime)
{
	if (IsKickOffStart == true)
		this->RemoveWeb();
	
}

void AWRSpiderWeb::RemoveWeb()
{
	if (CurrentDissolve <= 1.f)
	{
		CurrentDissolve += IncreaseDissolveSize;

		if (DynamicMaterial == nullptr)
			return;

		DynamicMaterial->SetScalarParameterValue(FName("Dissolve"), CurrentDissolve);	
	}
	else
	{
		this->Destroy();
	}
}

void AWRSpiderWeb::InitializeWeb()
{
	StaticMeshComp = Cast<UStaticMeshComponent>(this->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (StaticMeshComp == nullptr)
		return;

	DynamicMaterial = UMaterialInstanceDynamic::Create(StaticMeshComp->GetMaterial(MAIN_MATERIAL_INDEX), StaticMeshComp);
	if (DynamicMaterial == nullptr)
		return;

	StaticMeshComp->SetMaterial(MAIN_MATERIAL_INDEX, DynamicMaterial);

	if (DynamicMaterial == nullptr)
		return;
}
