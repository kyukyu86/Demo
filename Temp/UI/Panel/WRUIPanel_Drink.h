// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_Drink.generated.h"

enum class EWRDrinkType : uint8 
{
	None,
	Guide,
	Suggest,
	Suggested,
};
/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_Drink : public UWRUIPanel
{
	GENERATED_BODY()

private:
	class UWRTextBlock* WRTextBlock_Comment;
	class UCanvasPanel* CanvasPanel_Guide_R1;
	class UCanvasPanel* CanvasPanel_Guide_R2;
	class UCanvasPanel* CanvasPanel_Guide_Cross;
	class UCanvasPanel* CanvasPanel_AnswerYes;
	class UCanvasPanel* CanvasPanel_AnswerNo;
	class UCanvasPanel* CanvasPanel_Timer;
	class UWRProgressBar* ProgressBar_Timer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		float GuideUIMaintainTime = 5.f;
	
	FTimerHandle MaintainTimerHandle;
	EWRDrinkType Type = EWRDrinkType::None;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetToGuide();
	void SetToSuggest();
	void SetToSuggested(FString& IN InComment);

	EWRDrinkType GetUIType() { return Type; }

	void FinishMaintainTimer();
};
