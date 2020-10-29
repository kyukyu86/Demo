// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"

/**
 * 
 */
class THIRDPERSON_API DMUIUtil
{
public:
	DMUIUtil();
	~DMUIUtil();

	static void UEDebug(FString IN InLog);

	static UObject* LoadImageObject(FString InImageFullPath);
	static bool SetResourceObject(UImage* const IN InImage, const FString& IN InTextureFileFullPath, const bool IN bInMatchSize = false);
	static bool SetResourceObject(UImage* const IN InImage, UObject* const IN InResourceObject, const bool IN bInMatchSize = false);

	static UMaterialInstanceDynamic* GetDynamicMaterial(UImage* const IN InImage);

	static bool GetMouseScreenPosition(FVector2D& OUT OutMouseScreenPosition);
	static void GetMouseWorldTransform(FVector& OUT OutMouseWorldLocation, FVector& OUT OutMouseWorldDirection);
};