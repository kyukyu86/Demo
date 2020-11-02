// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "Enum/EWRInputSystem.h"
#include "WRUISlot_VoiceCommand.generated.h"

class UMaterialInstanceDynamic;
class UImage;
UCLASS()
class WR_API UWRUISlot_VoiceCommand : public UWRUISlot
{
	GENERATED_BODY()
public:
	enum EWRVoiceCommandState {Wait, Recording, VoiceSending, Impossible }; // 대기, 입력중, 전송중, 보이스 불가
	enum EWRVoiceWaitUIType { Collapsed, Visible};
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetVoiceCommandState(const EWRVoiceCommandState IN InVoiceCommandState);
	void SetRecordingTime(const float IN InTime);
	void SetVoiceWaitUIType(const EWRVoiceWaitUIType IN InVoiceWaitUIType);
	void SetVoiceContentType(const EWRVoiceContentType IN InVoiceContentType);
	void UpdateUI();
private:
	void CompleteRecording();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* ActiveStateLoopAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Texture_BaseIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Texture_ImpossibleIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* Texture_RecordingTime;
private:
	UImage* Image_Icon;
	UImage* Image_Loop;
	UImage* Image_RecordingTime;
	UMaterialInstanceDynamic* Mat_RecordingTime;
	
	float FlowRecordingTime;
	float MaxRecordingTime;

	EWRVoiceWaitUIType CurVoiceWaitUIType;
	EWRVoiceContentType VoiceContentType = EWRVoiceContentType::NONE;
};