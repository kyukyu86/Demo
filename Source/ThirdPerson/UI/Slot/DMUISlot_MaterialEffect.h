// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "DMUISlot_MaterialEffect.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UDMUISlot_MaterialEffect : public UDMUISlot
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_Effect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float PlaySpeed = 1.f;

	class UMaterialInstanceDynamic* MaterialInstance = nullptr;
	float fRate = 0.f;
};
