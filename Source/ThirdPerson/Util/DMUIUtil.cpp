// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIUtil.h"
#include "Engine/Texture2D.h"

DMUIUtil::DMUIUtil()
{
}

DMUIUtil::~DMUIUtil()
{
}

UObject* DMUIUtil::LoadImageObject(FString InImageFullPath)
{
	if (InImageFullPath.IsEmpty() == true)
		return nullptr;

	FStringAssetReference AssetReference(InImageFullPath);
	UObject* ImageObject = AssetReference.TryLoad();
	if (ImageObject == nullptr)
	{
//		UE_LOG(IMError, Error, TEXT("Load Failed at path = %s"), *TextureFullPath);
	}
	return ImageObject;
}

bool DMUIUtil::SetResourceObject(UImage* const IN InImage, const FString& IN InTextureFileFullPath, const bool IN bInMatchSize /*= false*/)
{
	if (InImage == nullptr)
		return false;

	UObject* Texture = DMUIUtil::LoadImageObject(InTextureFileFullPath);
	return DMUIUtil::SetResourceObject(InImage, Texture, bInMatchSize);
}

bool DMUIUtil::SetResourceObject(UImage* const IN InImage, UObject* const IN InResourceObject, const bool IN bInMatchSize /*= false*/)
{
	if (InImage == nullptr)
		return false;

	if (InResourceObject == nullptr)
		return false;

	InImage->Brush.SetResourceObject(InResourceObject);

	if (bInMatchSize)
	{
		UTexture2D* CastedTexture = Cast<UTexture2D>(InResourceObject);
		if (CastedTexture != nullptr)
		{
			FVector2D vSize(CastedTexture->GetSizeX(), CastedTexture->GetSizeY());
			InImage->SetBrushSize(vSize);
		}
	}
	return true;
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