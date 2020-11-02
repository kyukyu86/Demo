// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_VideoChatBubble.h"
#include "Define/WRUIDefine.h"
#include "Actor/Character/WRCharacter.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRUIManager.h"
#include "Utility/WRUIUtility.h"


void UWRUISlot_VideoChatBubble::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRTextBlock, WRTextBlock_Content, WRTextBlock_Content->SetText(NAME_None););
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUISlot_VideoChatBubble::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	bool bUpdateContents = false;
	TDoubleLinkedList<WRVideoChatBubbleData>::TDoubleLinkedListNode* Node = SavedVideoChatBubbleList.GetHead();
	for (; Node != nullptr; )
	{
		WRVideoChatBubbleData& VideoChatBubbleData = Node->GetValue();
		if (VideoChatBubbleData.Tick_DetermineDestroy(InDeltaTime))
		{
			TDoubleLinkedList<WRVideoChatBubbleData>::TDoubleLinkedListNode* TempNode = Node;
			Node = Node->GetNextNode();
			SavedVideoChatBubbleList.RemoveNode(TempNode);
			bUpdateContents = true;
			continue;
		}

		Node = Node->GetNextNode();
	}

	if (bUpdateContents)
		UpdateContents();
}

void UWRUISlot_VideoChatBubble::AddVideoChatBubble(const FText& IN InText, float IN InTime /*= 0.f*/)
{
	if (InTime <= 0.f)
		InTime = WRConsoleManager::Get()->GetFloat(EWRConsole::UI_DefaultChatBubbleTime);;

	SavedVideoChatBubbleList.AddTail(WRVideoChatBubbleData(InText.ToString(), InTime));

	if (SavedVideoChatBubbleList.Num() > WRConsoleManager::Get()->GetInt(EWRConsole::UI_ChatBubbleLimitCount))
	{
		SavedVideoChatBubbleList.RemoveNode(SavedVideoChatBubbleList.GetHead());
	}

	UpdateContents();
}

void UWRUISlot_VideoChatBubble::SetVideoChatBubble(const FText& IN InText, const float IN InTime /*= 0.f*/)
{
	WRTextBlock_Content->SetText(InText);
}

void UWRUISlot_VideoChatBubble::FinishVideoChatBubble()
{
	WRTextBlock_Content->SetText(NAME_None);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUISlot_VideoChatBubble::UpdateContents()
{
	if (SavedVideoChatBubbleList.Num() == 0)
	{
		FinishVideoChatBubble();
		return;
	}
	else if (IsVisible() == false)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	FString Contents(NULL_STRING);
	TDoubleLinkedList<WRVideoChatBubbleData>::TDoubleLinkedListNode* Node = SavedVideoChatBubbleList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRVideoChatBubbleData& VideoChatBubbleData = Node->GetValue();
		if (Contents.IsEmpty() == false)
			Contents += TEXT("\n");
		Contents += VideoChatBubbleData.GetContents();
	}

	WRTextBlock_Content->SetText(FText::FromString(Contents));
}

bool WRVideoChatBubbleData::Tick_DetermineDestroy(const float IN InDeltaTime)
{
	FlowTime += InDeltaTime;
	if (FlowTime > HoldingTime)
	{
		return true;
	}

	return false;
}