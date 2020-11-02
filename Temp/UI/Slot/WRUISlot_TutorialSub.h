// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUISlot.h"
#include "WRUISlot_TutorialSub.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUISlot_TutorialSub : public UWRUISlot
{
	GENERATED_BODY()
	
public:
	class UCanvasPanel* CanvasPanel_Skip;

	class UCanvasPanel* CanvasPanel_ProgressBar;
	class UProgressBar* ProgressBar;

private:
	bool bClosing = true;

public:
	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_TutorialSub")
		UWidgetAnimation* Ani_ShowOn;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUISlot_TutorialSub")
		UWidgetAnimation* Ani_ShowOff;

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void SetProgressBar(float IN InRate);
	void OpenTutorialSub(const bool IN InShowSkip);
	void CloseTutorialSub();

};
