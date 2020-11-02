// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_ChatBubble.h"
#include "Define/WRUIDefine.h"
#include "Actor/Character/WRCharacter.h"
#include "Manager/WRConsoleManager.h"
#include "Manager/WRUIManager.h"
#include "Utility/WRUIUtility.h"


void UWRUIPanel_ChatBubble::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UTextBlock, TextBlock_Content);
}

void UWRUIPanel_ChatBubble::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
	bool bUpdateContents = false;
	TDoubleLinkedList<WRChatBubbleData>::TDoubleLinkedListNode* Node = SavedChatBubbleList.GetHead();
	for (; Node != nullptr; )
	{
		WRChatBubbleData& ChatBubbleData = Node->GetValue();
		if (ChatBubbleData.Tick_DetermineDestroy(InDeltaTime))
		{
			TDoubleLinkedList<WRChatBubbleData>::TDoubleLinkedListNode* TempNode = Node;
			Node = Node->GetNextNode();
			SavedChatBubbleList.RemoveNode(TempNode);
			bUpdateContents = true;
			continue;
		}

		Node = Node->GetNextNode();
	}

	if (bUpdateContents)
		UpdateContents();

	//if (MaxTime > 0.f)
	//{
	//	FlowTime += InDeltaTime;
	//	if (FlowTime > MaxTime)
	//	{
	//		FinishChatBubble();
	//	}
	//}
}

void UWRUIPanel_ChatBubble::AddChatBubble(const FText& IN InText, float IN InTime /*= 0.f*/)
{
	if (InTime <= 0.f)
		InTime = WRConsoleManager::Get()->GetFloat(EWRConsole::UI_DefaultChatBubbleTime);;

	SavedChatBubbleList.AddTail(WRChatBubbleData(InText.ToString(), InTime));

	if (SavedChatBubbleList.Num() > WRConsoleManager::Get()->GetInt(EWRConsole::UI_ChatBubbleLimitCount))
	{
		SavedChatBubbleList.RemoveNode(SavedChatBubbleList.GetHead());
	}

	UpdateContents();

	//if (ReservedChatBubbles.IsEmpty())
	//{
	//	SetChatBubble(InText, InTime);
	//}
	//else
	//{
	//	ReservedChatBubbles.Enqueue(TPair<FText, float>(InText, InTime));
	//}
}

void UWRUIPanel_ChatBubble::SetChatBubble(const FText& IN InText, const float IN InTime /*= 0.f*/)
{
	TextBlock_Content->SetText(InText);
	/*MaxTime = InTime;
	FlowTime = 0.f;*/
	/*if (MaxTime == 0.f)
		MaxTime = WRConsoleManager::Get()->GetFloat(EWRConsole::UI_DefaultChatBubbleTime);*/
}

void UWRUIPanel_ChatBubble::FinishChatBubble()
{	
	WRUIManager::Get()->CloseUI(UIHandle);
}

void UWRUIPanel_ChatBubble::UpdateContents()
{
	if (SavedChatBubbleList.Num() == 0)
	{
		FinishChatBubble();
		return;
	}

	FString Contents(NULL_STRING);
	TDoubleLinkedList<WRChatBubbleData>::TDoubleLinkedListNode* Node = SavedChatBubbleList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRChatBubbleData& ChatBubbleData = Node->GetValue();
		if (Contents.IsEmpty() == false)
			Contents += TEXT("\n");
		Contents += ChatBubbleData.GetContents();
	}

	TextBlock_Content->SetText(FText::FromString(Contents));
}

bool WRChatBubbleData::Tick_DetermineDestroy(const float IN InDeltaTime)
{
	FlowTime += InDeltaTime;
	if (FlowTime > HoldingTime)
	{
		return true;
	}

	return false;
}