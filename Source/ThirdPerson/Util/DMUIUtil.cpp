// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIUtil.h"

DMUIUtil::DMUIUtil()
{
}

DMUIUtil::~DMUIUtil()
{
}

UMaterialInstanceDynamic* DMUIUtil::GetDynamicMaterial(UImage* const IN InImage)
{
	if (InImage == nullptr || InImage->IsValidLowLevel() == false)
	{
		ensureMsgf(false, TEXT("InImage is nullptr"));
		return nullptr;
	}
	UObject* Resource = InImage->Brush.GetResourceObject();
	if (Resource == nullptr || Resource->IsValidLowLevel() == false)
	{
		ensureMsgf(false, TEXT("Image's Brush ResourceObject is nullptr"));
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
		InImage->Brush.SetResourceObject(DynamicMaterial);

		return DynamicMaterial;
	}

	ensureMsgf(false, TEXT("Failed to get 'Dynamic Mateiral Instance' of 'Image'"));

	return nullptr;
}