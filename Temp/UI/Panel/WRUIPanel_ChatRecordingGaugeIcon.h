// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_ChatRecordingGaugeIcon.generated.h"

/**
 * 
 */

class UImage;

DECLARE_DELEGATE(FWRGaugingCompletedDelegate)

UCLASS()
class WR_API UWRUIPanel_ChatRecordingGaugeIcon : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:

	void StartGauging(const float IN InGaugingMaxTime, const FWRGaugingCompletedDelegate& IN InDelegate);
	void StopGauging();

	void ShowDisabledIcon();
	void ShowEnabledIcon();
private:
	void CompleteGauging();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* Texture_Gauging;
private:
	UImage* Image_Gauging;
	//UImage* Image_DisabledIcon;
	UImage* Image_Icon;
	UMaterialInstanceDynamic* Mat_Gauging;

	float GaugingMaxTime;
	float GaugingTime;

	FWRGaugingCompletedDelegate  GaugingCompletedDelegate;
};
