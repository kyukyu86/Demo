// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "WRUIPanel_ChatBubble.generated.h"


class WR_API WRChatBubbleData
{
public:
	WRChatBubbleData() {}
	WRChatBubbleData(const FString& IN InContents, const float IN InHoldingTime) : Contents(InContents), HoldingTime(InHoldingTime), FlowTime(0.f) {}
	virtual ~WRChatBubbleData() {}

	bool Tick_DetermineDestroy(const float IN InDeltaTime);	// 삭제 되야하는지를 체크하여 return 해준다.
	FORCEINLINE const FString& GetContents() const { return Contents; }
private:
	FString Contents;
	float HoldingTime;
	float FlowTime;
};

UCLASS()
class WR_API UWRUIPanel_ChatBubble : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void AddChatBubble(const FText& IN InText, float IN InTime = 0.f);
private:
	void SetChatBubble(const FText& IN InText, const float IN InTime = 0.f);
	void FinishChatBubble();
	void UpdateContents();
private:
	class UTextBlock* TextBlock_Content;

	//TQueue<TPair<FText, float>> SavedChatBubbles;
	TDoubleLinkedList<WRChatBubbleData> SavedChatBubbleList;
	//TQueue<WRChatBubbleData> SavedChatBubbles;

	/*float MaxTime = 0.f;
	float FlowTime = 0.f;*/
};