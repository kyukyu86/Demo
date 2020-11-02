// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_ChatRecording.generated.h"

class UMaterialInstanceDynamic;
class UImage;
class UTextBlock;
class UWRUISlot_VoiceCommandText;
UCLASS()
class WR_API UWRUISlot_ChatRecording : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void StartChatRecording(const float IN InTime);
	void StopChatRecording();
private:
	void SetRecordingTime(const float IN InTime);
	void CompleteRecording();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture* Texture_RecordingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* LoopAnimation;
private:
	UImage* Image_Icon;
	UImage* Image_RecordingTime;
	UMaterialInstanceDynamic* Mat_RecordingTime;
	UTextBlock* TextBlock_Content;
	
	float FlowRecordingTime;
	float MaxRecordingTime;
};