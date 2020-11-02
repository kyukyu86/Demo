// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_VideoChatBubble.generated.h"

class WR_API WRVideoChatBubbleData
{
public:
	WRVideoChatBubbleData() {}
	WRVideoChatBubbleData(const FString& IN InContents, const float IN InHoldingTime) : Contents(InContents), HoldingTime(InHoldingTime), FlowTime(0.f) {}
	virtual ~WRVideoChatBubbleData() {}

	bool Tick_DetermineDestroy(const float IN InDeltaTime);	// 삭제 되야하는지를 체크하여 return 해준다.
	FORCEINLINE const FString& GetContents() const { return Contents; }
private:
	FString Contents;
	float HoldingTime;
	float FlowTime;
};

class UCanvasPanel;
class UWRTextBlock;
UCLASS()
class WR_API UWRUISlot_VideoChatBubble : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void AddVideoChatBubble(const FText& IN InText, float IN InTime = 0.f);
private:
	void SetVideoChatBubble(const FText& IN InText, const float IN InTime = 0.f);
	void FinishVideoChatBubble();
	void UpdateContents();
private:
	class UWRTextBlock* WRTextBlock_Content;

	//TQueue<TPair<FText, float>> SavedChatBubbles;
	TDoubleLinkedList<WRVideoChatBubbleData> SavedVideoChatBubbleList;
};