// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_VoiceCommand.h"
#include "Image.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRInputSystemManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRConsoleManager.h"

void UWRUISlot_VoiceCommand::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Icon);
	BIND_WIDGET(UImage, Image_Loop);
	BIND_WIDGET(UImage, Image_RecordingTime);
	if (Image_RecordingTime)
	{
		Mat_RecordingTime = WRUIUtility::GetDynamicMaterial(Image_RecordingTime);
		if (Mat_RecordingTime)
		{
			Mat_RecordingTime->SetScalarParameterValue("RotateValue", 0);
			Mat_RecordingTime->SetTextureParameterValue("RotateTexture", Texture_RecordingTime);
		}
	}

	FlowRecordingTime = 0.f;
	MaxRecordingTime = 0.f;

	CurVoiceWaitUIType = EWRVoiceWaitUIType::Collapsed;
	SetVoiceCommandState(EWRVoiceCommandState::Wait);
}

void UWRUISlot_VoiceCommand::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	if (MaxRecordingTime > 0)
	{
		FlowRecordingTime += InDeltaTime;

		if (Mat_RecordingTime)
			Mat_RecordingTime->SetScalarParameterValue("RotateValue", FlowRecordingTime / MaxRecordingTime);

		if (FlowRecordingTime > MaxRecordingTime)
			CompleteRecording();
	}
}

void UWRUISlot_VoiceCommand::SetVoiceCommandState(const EWRVoiceCommandState IN InVoiceCommandState)
{
	switch (InVoiceCommandState)
	{
	case EWRVoiceCommandState::Wait:
		if (CurVoiceWaitUIType == EWRVoiceWaitUIType::Visible)
		{
			Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
		}
		Image_Icon->SetBrushFromTexture(Texture_BaseIcon);
		Image_Loop->SetVisibility(ESlateVisibility::Collapsed);
		Image_RecordingTime->SetVisibility(ESlateVisibility::Collapsed);
		StopAnimation(ActiveStateLoopAnimation);
		break;
	case EWRVoiceCommandState::Recording:
		Image_Icon->SetBrushFromTexture(Texture_BaseIcon);
		Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_Loop->SetVisibility(ESlateVisibility::Collapsed);
		Image_RecordingTime->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		StopAnimation(ActiveStateLoopAnimation);
		break;
	case EWRVoiceCommandState::VoiceSending:
		Image_Icon->SetBrushFromTexture(Texture_BaseIcon);
		Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_Loop->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Image_RecordingTime->SetVisibility(ESlateVisibility::Collapsed);
		PlayAnimation(ActiveStateLoopAnimation, 0, 0);
		break;
	case EWRVoiceCommandState::Impossible:
		Image_Icon->SetBrushFromTexture(Texture_ImpossibleIcon);
		Image_Loop->SetVisibility(ESlateVisibility::Collapsed);
		Image_RecordingTime->SetVisibility(ESlateVisibility::Collapsed);
		StopAnimation(ActiveStateLoopAnimation);
		break;
	default:
		break;
	}

	if (InVoiceCommandState != EWRVoiceCommandState::Recording)
	{
		MaxRecordingTime = 0.f;
		FlowRecordingTime = 0.f;
	}
}

void UWRUISlot_VoiceCommand::SetRecordingTime(const float IN InTime)
{
	FlowRecordingTime = 0.f;
	MaxRecordingTime = InTime;
}

void UWRUISlot_VoiceCommand::SetVoiceWaitUIType(const EWRVoiceWaitUIType IN InVoiceWaitUIType)
{
	CurVoiceWaitUIType = InVoiceWaitUIType;
	if (CurVoiceWaitUIType == EWRVoiceWaitUIType::Visible)
		Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UpdateUI();
}

void UWRUISlot_VoiceCommand::SetVoiceContentType(const EWRVoiceContentType IN InVoiceContentType)
{
	VoiceContentType = InVoiceContentType;
	UpdateUI();
}

void UWRUISlot_VoiceCommand::UpdateUI()
{
	bool bImpossibleVoiceRecording = WRInputSystemManager::Get()->IsImpossibleVoiceRecording();
	bool bChatRecording = WRInputSystemManager::Get()->IsChatRecording();
	if (bImpossibleVoiceRecording || bChatRecording)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Impossible);
		return;
	}

	EWRVoiceContentType RecordingVoiceContentType = WRInputSystemManager::Get()->GetVoiceContentType();	// 보이스 레코딩 중인 VoiceContentType
	EWRVoiceRecordingStateType VoiceRecordingState = WRInputSystemManager::Get()->GetVoiceRecordingState();
	if (RecordingVoiceContentType != VoiceContentType)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
	}
	else if (VoiceRecordingState == EWRVoiceRecordingStateType::RECORDING)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Recording);
		SetRecordingTime(WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentTime));
	}
	else if (VoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::VoiceSending);
	}
	else
	{
		SetVoiceCommandState(UWRUISlot_VoiceCommand::EWRVoiceCommandState::Wait);
	}
}

void UWRUISlot_VoiceCommand::CompleteRecording()
{
	WRInputSystemManager::Get()->StopRecording();
	MaxRecordingTime = 0.f;
	FlowRecordingTime = 0.f;
}
