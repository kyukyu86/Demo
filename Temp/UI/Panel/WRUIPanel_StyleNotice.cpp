// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_StyleNotice.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCustomTextTable.h"


#include "TextBlock.h"


void UWRUIPanel_StyleNotice::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	BIND_WIDGET(UTextBlock, TextBlock_Notice);
	//BIND_WIDGET(UTextBlock, TextBlock_Context);

}

void UWRUIPanel_StyleNotice::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	bool bUpdateText = false;
	TDoubleLinkedList<WRStyleNoticeData>::TDoubleLinkedListNode* Node = SavedStyleNoticeDataList.GetHead();
	for (; Node != nullptr; )
	{
		WRStyleNoticeData& StyleNoticeData = Node->GetValue();
		if (StyleNoticeData.Tick_DetermineDestroy(InDeltaTime))
		{
			TDoubleLinkedList<WRStyleNoticeData>::TDoubleLinkedListNode* TempNode = Node;
			Node = Node->GetNextNode();
			SavedStyleNoticeDataList.RemoveNode(TempNode);
			bUpdateText = true;
			continue;
		}

		Node = Node->GetNextNode();
	}

	if (bUpdateText)
		UpdateNoticeText();

	/*if (bShowOff)
		return;

	fShowDuringTime += InDeltaTime;
	if (fShowDuringTime >= ShowOnTime)
	{
		FString strNextNotice;
		if (GetNextNotice(strNextNotice))
		{
			ShowOnNotice(strNextNotice);
		}
		else
		{
			ShowOffNotice();
		}
	}*/
}

void UWRUIPanel_StyleNotice::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	if (Animation == Ani_ShowOn)
	{

	}
	else if (Animation == Ani_ShowOff)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWRUIPanel_StyleNotice::AddNotice(const FString& IN InNotice)
{
	SavedStyleNoticeDataList.AddTail(WRStyleNoticeData(InNotice, WRConsoleManager::Get()->GetFloat(EWRConsole::UI_DefaultStyleNoticeTime)));

	if (SavedStyleNoticeDataList.Num() > WRConsoleManager::Get()->GetInt(EWRConsole::UI_ChatBubbleLimitCount))
	{
		SavedStyleNoticeDataList.RemoveNode(SavedStyleNoticeDataList.GetHead());
	}
	UpdateNoticeText();
	//Notices.Enqueue(InNotice);

	//// + 추가시 닫혀있거나 종료중이면 다시 실행

	//if (GetVisibility() == ESlateVisibility::Hidden
	//	|| IsAnimationPlaying(Ani_ShowOff))
	//{
	//	FString strNextNotice;
	//	if (GetNextNotice(strNextNotice))
	//	{
	//		ShowOnNotice(strNextNotice);
	//	}
	//}	
}

bool UWRUIPanel_StyleNotice::GetNextNotice(FString& OUT OutNotice)
{
	/*if (Notices.IsEmpty())
		return false;

	Notices.Dequeue(OutNotice);*/
	return true;
}

void UWRUIPanel_StyleNotice::ShowOnNotice(FString IN InNotice)
{
//	bShowOff = false;
//	fShowDuringTime = 0.0f;
//
//	WRUIUtility::SetTextSafe(TextBlock_Notice, InNotice);
//
//#ifdef USING_STYLENOTICE_CONTEXT
//	// [ 2020-4-24 : vuvle ] : 기획자 피드백으로 Context 를 추가 하였지만 다른 Notice 에는 들어가지 않아도 될 것 같음. 일단 임시. 추후에 공지 룰이 생기면 Context 를 따로 받을지 체크하자.
//	FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(19);
//	
//	if (CustomTextTable)
//	{
//		WRUIUtility::SetTextSafe(TextBlock_Context, CustomTextTable->Localkey);
//	}
//#endif // USING_STYLENOTICE_CONTEXT
//
//	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
//
//	PlayAnimation(Ani_ShowOn);
}

void UWRUIPanel_StyleNotice::ShowOffNotice()
{
	/*bShowOff = true;
	PlayAnimation(Ani_ShowOff);*/
}

void UWRUIPanel_StyleNotice::UpdateNoticeText()
{
	if (SavedStyleNoticeDataList.Num() == 0)
	{
		FinishStyleNotice();
		return;
	}

	FString Notice(NULL_STRING);
	TDoubleLinkedList<WRStyleNoticeData>::TDoubleLinkedListNode* Node = SavedStyleNoticeDataList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		WRStyleNoticeData& StyleNoticeData = Node->GetValue();
		if (Notice.IsEmpty() == false)
			Notice += TEXT("\n");
		Notice += StyleNoticeData.GetContents();
	}
	
	TextBlock_Notice->SetText(FText::FromString(Notice));
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UWRUIPanel_StyleNotice::FinishStyleNotice()
{
	SetVisibility(ESlateVisibility::Hidden);
	//WRUIManager::Get()->CloseUI(UIHandle);
}

bool WRStyleNoticeData::Tick_DetermineDestroy(const float IN InDeltaTime)
{
	FlowTime += InDeltaTime;
	if (FlowTime > HoldingTime)
	{
		return true;
	}

	return false;
}
