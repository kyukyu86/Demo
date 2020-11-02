// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_ChatRecording.h"
#include "Define/WRUIDefine.h"
#include "Manager/WRInputSystemManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Utility/WRUIUtility.h"

void UWRUISlot_ChatRecording::NativeConstruct()
{
	Super::NativeConstruct();

	BIND_WIDGET(UImage, Image_Icon);
	BIND_WIDGET(UImage, Image_RecordingTime);
	BIND_WIDGET(UTextBlock, TextBlock_Content);
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
}

void UWRUISlot_ChatRecording::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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


void UWRUISlot_ChatRecording::StartChatRecording(const float IN InTime)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	PlayAnimation(LoopAnimation, 0, 0);
	SetRecordingTime(InTime);
}

void UWRUISlot_ChatRecording::StopChatRecording()
{
	StopAnimation(LoopAnimation);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWRUISlot_ChatRecording::SetRecordingTime(const float IN InTime)
{
	FlowRecordingTime = 0.f;
	MaxRecordingTime = InTime;
}


void UWRUISlot_ChatRecording::CompleteRecording()
{
	//WRInputSystemManager::Get()->StopRecording();

	MaxRecordingTime = 0.f;
	FlowRecordingTime = 0.f;
	Mat_RecordingTime->SetScalarParameterValue("RotateValue", 0);
}
