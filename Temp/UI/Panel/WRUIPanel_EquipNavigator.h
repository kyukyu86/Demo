// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_EquipNavigator.generated.h"


/**
 * 
 */


class UWRTextBlock;
class UWRProgressBar;

UCLASS()
class WR_API UWRUIPanel_EquipNavigator : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
	void OnTypingEffectCompletedForTitle();
	void OnTypingEffectCompletedForOK();
	void OnTypingEffectCompletedForComplete();
	void OnPrgressCompleted();
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
	UWRTextBlock* WRTextBlock_Title;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
	UWRTextBlock* WRTextBlock_OK;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
	UWRTextBlock* WRTextBlock_Complete;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
	UWRProgressBar* WRProgressBar_Gauge;

	UPROPERTY(EditAnywhere)
	float OpenAnimationSpeed = 1.f;
	UPROPERTY(EditAnywhere)
	float CloseAnimationSpeed = 1.f;
	UPROPERTY(EditAnywhere)
	float TextBottomAnimationSpeed = 1.f;
	UPROPERTY(EditAnywhere)
	float TypingEffectSpeed = 0.1f;
	UPROPERTY(EditAnywhere)
	float GaugeTime = 1.f;
	UPROPERTY(EditAnywhere)
	int32 GaugeStepNum = 20;
};
