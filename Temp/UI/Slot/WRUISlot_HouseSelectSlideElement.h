// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/CustomWidget/SlideList/WRSlideElement.h"
#include "WRUISlot_HouseSelectSlideElement.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_HouseSelectSlideElement : public UWRSlideElement
{
	GENERATED_BODY()
	
private:
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
	virtual void SetData(const int32 IN InDataIndex) override;
	virtual void SetEmpty() override;
};
