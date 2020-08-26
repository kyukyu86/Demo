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

	static UMaterialInstanceDynamic* GetDynamicMaterial(UImage* const IN InImage);
};
