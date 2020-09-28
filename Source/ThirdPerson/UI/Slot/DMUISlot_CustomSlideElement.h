// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Custom/SlideList/DMUISlideElement.h"
#include "DMUISlot_CustomSlideElement.generated.h"


UCLASS()
class THIRDPERSON_API UDMUISlot_CustomSlideElement : public UDMUISlideElement
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UTextBlock* TextBlock_Data;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_Main;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_Icon;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_BG;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual void SetMain(const bool IN InSet) override;	
	virtual void SetData(const int32 IN InDataIndex);
	virtual void SetEmpty() override;
};
