// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRCharacter.h"
#include "Engine/EngineTypes.h"
#include "WRUIPanel_HMDGuide.generated.h"

/**
 * 
 */
enum class EWRInput : uint8;

USTRUCT(BlueprintType)
struct FWRGuideStep
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HmdGuideStep)
		FName WidgetAnimName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HmdGuideStep)
		TArray<EWRInput> Keys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HmdGuideStep)
		bool IsAutoProceed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HmdGuideStep)
		bool IsAnimLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HmdGuideStep)
		bool IsAttachToCamera = false;

	bool IsPlayed = false;
};

UCLASS()
class WR_API UWRUIPanel_HMDGuide : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	void StartGuideProcess();
private:
	bool IsCanProceed(const TArray<EWRInput>& IN InNeedInputKeys);
	
	void ProceedStep();
	void EndStep();

	void PlayUIAnim();

	void SetAttachToCamera();
	void SetLocationOnTable();

	UFUNCTION()
	void AutoProceed();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HMDGuide)
		TArray<FWRGuideStep> InSteps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HMDGuide)
		TArray<EWRInput> ReturnFirstStepKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HMDGuide)
		TArray<EWRInput> FinishKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HMDGuide)
		FTransform PanelTransform = FTransform::Identity;
private:
	TDoubleLinkedList<EWRInput> DownKeys;
	
	int32 CurrentIndex = -1;	

	FTimerHandle GuideProcessTimer;

	UWidgetAnimation* CurrentUIAnim = nullptr;
	UWidgetAnimation* PrevUIAnim = nullptr;

	TArray<EWRInput> ResetOrientationKeys;

public:
	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;

	bool PlayAnimationByName(FName AnimationName,
		float StartAtTime,
		int32 NumLoopsToPlay,
		EUMGSequencePlayMode::Type PlayMode,
		float PlaybackSpeed);

protected:
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	void FillAnimationsMap();
};
