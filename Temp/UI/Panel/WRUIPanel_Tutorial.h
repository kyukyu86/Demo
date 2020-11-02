// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRTutorial.h"
#include "Manager/WRTutorialManager.h"
#include "WRUIPanel_Tutorial.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_Tutorial : public UWRUIPanel
{
	GENERATED_BODY()

public:
	class UCanvasPanel* CanvasPanel_Guide;
	class UImage* Image_Guide;
	class USizeBox* SizeBox_Description;
	class UWRTextBlock* WRTextBlock_Description;
	class UTextBlock* TextBlock_RepeatCount;
	class UProgressBar* ProgressBar;

	class UOverlay* Overlay_Dynamic;
	UWRUserWidget* Slot_Dynamic = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "UWRUIPanel_Tutorial")
		class UWidgetAnimation* Ani_ShowOn;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUIPanel_Tutorial")
		class UWidgetAnimation* Ani_ShowOff;

	bool bReOpen = false;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void SetInformation(struct FWRTutorialTable* IN InTable);
	void SetProgressBar(float IN InRate);
	void SetRepeatCount(FString& IN InText);
	void CloseTutorialPanel();

private:
	EWRTutorialPanel PanelType = EWRTutorialPanel::Main;
#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
	FString strInnerWidgetAsyncKey = "";
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
	
};
