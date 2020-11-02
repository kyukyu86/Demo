// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_MonitorInLab.generated.h"



enum class EWRInput : uint8;
class URichTextBlock;
UCLASS()
class WR_API UWRUIPanel_MonitorInLab : public UWRUIPanel
{
	GENERATED_BODY()
public:
	enum EWRMonitorLabType {Start, End};
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
private:
	class UWRTextBlock* WRTextBlock_Content;
public:
	void OnAnyButtonClicked();

	EWRMonitorLabType GetMonitorLabType() { return MonitorLabType; }
public:
	void SetStartGameUI();
	void SetEndGameUI();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* AppearAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
		FText StartMonitorText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
		FText EndMonitorText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<EWRInput> AnyKeys;
private:
	EWRMonitorLabType MonitorLabType;

	FPostProcessSettings* PostProcessSettings;
	float BlurTime;
	float AppearMonitorTime;
	float AppearMonitorMaxTime;
};