// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUISlot_MaterialEffect.h"
#include "../../Util/DMUIUtil.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"

void UDMUISlot_MaterialEffect::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (MaterialInstance == nullptr)
	{
		MaterialInstance = DMUIUtil::GetDynamicMaterial(Image_Effect);
		if (MaterialInstance != nullptr)
		{
			MaterialInstance->SetScalarParameterValue("UIRandom", FMath::FRandRange(0.f, 1.f));
		}
	}
}

void UDMUISlot_MaterialEffect::NativeDestruct()
{

	Super::NativeDestruct();
}

void UDMUISlot_MaterialEffect::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MaterialInstance)
	{
		fRate += (InDeltaTime * PlaySpeed);
		if (fRate > 1.f)
			fRate = 0.f;

		MaterialInstance->SetScalarParameterValue("Rate", fRate);
	}
}
