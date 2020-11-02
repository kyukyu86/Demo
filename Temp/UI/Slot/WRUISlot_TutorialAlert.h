// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WidgetAnimation.h"
#include "WRUISlot_TutorialAlert.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_TutorialAlert : public UWRUISlot
{
	GENERATED_BODY()
	
public:
	class UTextBlock* TextBlock_Alert;

	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_TutorialAlert")
		UWidgetAnimation* Ani_ShowOn;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_TutorialAlert")
		UWidgetAnimation* Ani_Flicker;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_TutorialAlert")
		UWidgetAnimation* Ani_ShowOff;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUISlot_TutorialAlert")
		int32 FlickerCount = 1;

private:
	float fElapsedTime = 0.f;
	float fMaxTime = 0.f;
	bool bTimeMode = false;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void OpenTutorialAlert(const float IN InTime = 0.f);
	void CloseTutorialAlert();
	void SetAlert(FString& IN InAlert);

private:
	void OnClose();
};
