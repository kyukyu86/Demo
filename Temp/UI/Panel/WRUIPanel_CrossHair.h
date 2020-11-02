// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_CrossHair.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FWRTargetLoadingCompletedDelegate)

class UMaterialInstanceDynamic;
class UImage;
class UTexure;
class UCanvasPanel;
UCLASS()
class WR_API UWRUIPanel_CrossHair : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void ShowCrossHairPanel();
	void HideCrossHairPanel();

	void ShowHookCrossHair();
	void HideHookCrossHair();
	
	void TopAttackUI();
	void BottomAttackUI();

	void StartLoading(const float IN InLoadingMaxTime, const FWRTargetLoadingCompletedDelegate& IN InDelegate);
	void StopLoading();

	void OpenPressCircleButton();
	void ClosePressCircleButton();
private:
	void CompleteLoading();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* Texture_Lading;
private:
	UImage* Image_HookCrossHair;
	UImage* Image_Arrow; 
	UImage* Image_Loading;
	UCanvasPanel* CanvasPanel_PressCircleButton;
	UMaterialInstanceDynamic* Mat_Loading;

	float TopAttackDegree;
	float BottomAttackDegree;

	float LoadingMaxTime;
	float LoadingTime;

	FWRTargetLoadingCompletedDelegate  TargetLoadingCompletedDelegate;
};
