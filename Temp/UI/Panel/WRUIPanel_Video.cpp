// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Video.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Image.h"
#include "Define/WRUIDefine.h"

void UWRUIPanel_Video::NativeConstruct()
{
	Super::NativeConstruct();
	BIND_WIDGET(UImage, Image_Video);
	TotalTIme = 0.f;

	VideoDynamicMaterial = GetDynamicMaterial();
}

void UWRUIPanel_Video::UpdateMaterial(const float IN InDeltaTime)
{
	TotalTIme += InDeltaTime;
	if (TotalTIme > 2)
		TotalTIme = 0;

	if(VideoDynamicMaterial)
		VideoDynamicMaterial->SetScalarParameterValue("time", TotalTIme);

	SetRenderScale(FVector2D(TotalTIme + 1, TotalTIme + 1));
}

UMaterialInstanceDynamic* UWRUIPanel_Video::GetDynamicMaterial()
{
	if (Image_Video == nullptr || Image_Video->IsValidLowLevel() == false)
	{
		return nullptr;
	}
	UObject* Resource = Image_Video->Brush.GetResourceObject();
	if (Resource == nullptr || Resource->IsValidLowLevel() == false)
	{
		return nullptr;
	}

	// If we already have a dynamic material, return it.
	if (UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Resource))
	{
		return DynamicMaterial;
	}
	// If the resource has a material interface we'll just update the brush to have a dynamic material.
	else if (UMaterialInterface* Material = Cast<UMaterialInterface>(Resource))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
		Image_Video->Brush.SetResourceObject(DynamicMaterial);

		return DynamicMaterial;
	}

	return nullptr;
}
