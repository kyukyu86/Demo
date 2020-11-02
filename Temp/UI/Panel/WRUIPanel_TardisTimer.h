// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_TardisTimer.generated.h"


enum class EWRTimerDirection : uint8
{
	None,
	Increase,
	Decrease,
};

class UCanvasPanel;
class UTextBlock;
class UWRTextBlock;

UCLASS()
class WR_API UWRUIPanel_TardisTimer : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	void SetStandby();
	void SetTimerInfo(const EWRTimerDirection IN InDirection, const float IN InStartTime = 0.f);
	void SetTimerStop();
	void SetContents(const FString IN InContents);

	void SetBelongToBP();
private:
	void ConstructDetail();
	void SetTime(int32 IN InDisplayTime);
private:
	UCanvasPanel* CanvasPanel_Timer;
	UTextBlock* TextBlock_Hour;
	UTextBlock* TextBlock_Minute;
	UTextBlock* TextBlock_Second;
	UCanvasPanel* CanvasPanel_Contents;
	UWRTextBlock* WRTextBlock_Contents;

	bool TickActivate;
	float FlowTime;
	int32 DisplayTime;
	EWRTimerDirection TimeDirection = EWRTimerDirection::Increase;

	// Belong To BP
	bool IsBelongToBP = false;
};
