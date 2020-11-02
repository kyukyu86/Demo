// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_VoiceCommandText.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRInputSystemManager.h"

void UWRUISlot_VoiceCommandText::NativeConstruct()
{
	Super::NativeConstruct();
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Content);

	SetVoiceCommandState(EWRVoiceCommandState::Wait);
	CurVoiceWaitUIType = EWRVoiceWaitUIType::Collapsed;
	PlayAnimation(LoopAnimation, 0, 0);
}

void UWRUISlot_VoiceCommandText::SetVoiceCommandState(const EWRVoiceCommandState IN InVoiceCommandState)
{
	switch (InVoiceCommandState)
	{
	case EWRVoiceCommandState::Wait:
		WRTextBlock_Content->SetText(WaitText);
		if (CurVoiceWaitUIType == EWRVoiceWaitUIType::Visible)
		{
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
		break;
	case EWRVoiceCommandState::Recording:
		WRTextBlock_Content->SetText(RecordingText);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EWRVoiceCommandState::VoiceSending:
		WRTextBlock_Content->SetText(VoiceSendingText);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		break;
	case EWRVoiceCommandState::Impossible:
		WRTextBlock_Content->SetText(ImpossibleText);
		break;
	default:
		break;
	}
}

void UWRUISlot_VoiceCommandText::SetVoiceContentType(const EWRVoiceContentType IN InVoiceContentType)
{
	VoiceContentType = InVoiceContentType;
	UpdateUI();
}

void UWRUISlot_VoiceCommandText::SetVoiceWaitUIType(const EWRVoiceWaitUIType IN InVoiceWaitUIType)
{
	CurVoiceWaitUIType = InVoiceWaitUIType;
	if (CurVoiceWaitUIType == EWRVoiceWaitUIType::Visible)
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdateUI();
}

void UWRUISlot_VoiceCommandText::UpdateUI()
{
	bool bImpossibleVoiceRecording = WRInputSystemManager::Get()->IsImpossibleVoiceRecording();
	bool bChatRecording = WRInputSystemManager::Get()->IsChatRecording();
	if (bImpossibleVoiceRecording || bChatRecording)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Impossible);
		return;
	}

	EWRVoiceContentType RecordingVoiceContentType = WRInputSystemManager::Get()->GetVoiceContentType();	// 보이스 레코딩 중인 VoiceContentType
	EWRVoiceRecordingStateType VoiceRecordingState = WRInputSystemManager::Get()->GetVoiceRecordingState();
	if (RecordingVoiceContentType != VoiceContentType)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
	else if (VoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Recording);
	}
	else if (VoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommandText::EWRVoiceCommandState::Wait);
	}
}

void UWRUISlot_VoiceCommandText::SetJustification(const ETextJustify::Type IN InJustify)
{
	WRTextBlock_Content->SetJustification(InJustify);
}
