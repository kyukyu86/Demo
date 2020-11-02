// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Queue.h"

#include "WRUIPanel_StyleNotice.generated.h"

class WR_API WRStyleNoticeData
{
public:
	WRStyleNoticeData() {}
	WRStyleNoticeData(const FString& IN InContents, const float IN InHoldingTime) : Contents(InContents), HoldingTime(InHoldingTime), FlowTime(0.f) {}
	virtual ~WRStyleNoticeData() {}

	bool Tick_DetermineDestroy(const float IN InDeltaTime);	// 삭제 되야하는지를 체크하여 return 해준다.
	FORCEINLINE const FString& GetContents() const { return Contents; }
private:
	FString Contents;
	float HoldingTime;
	float FlowTime;
};

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_StyleNotice : public UWRUIPanel
{
	GENERATED_BODY()

public:
	class UTextBlock* TextBlock_Notice;
	//class UTextBlock* TextBlock_Context;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUIPanel_StyleNotice")
		float ShowOnTime = 3.0f;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUIPanel_StyleNotice")
		UWidgetAnimation* Ani_ShowOn;
	UPROPERTY(BlueprintReadWrite, Category = "UWRUIPanel_StyleNotice")
		UWidgetAnimation* Ani_ShowOff;

private:
	TDoubleLinkedList<WRStyleNoticeData> SavedStyleNoticeDataList;
	/*TQueue<FString> Notices;
	float fShowDuringTime = 0.0f;
	bool bShowOff = false;*/

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	void AddNotice(const FString& IN InNotice);

private:
	bool GetNextNotice(FString& OUT OutNotice);
	void ShowOnNotice(FString IN InNotice);
	void ShowOffNotice();

	void UpdateNoticeText();
	void FinishStyleNotice();
};
