// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "Enum/EWRInputSystem.h"
#include "TextLayout.h"
#include "WRUISlot_VoiceCommandText.generated.h"


class UWRTextBlock;
UCLASS()
class WR_API UWRUISlot_VoiceCommandText : public UWRUISlot
{
	GENERATED_BODY()
public:
	enum EWRVoiceCommandState {Wait, Recording, VoiceSending, Impossible }; // ���, �Է���, ������, ���̽� �Ұ�
	enum EWRVoiceWaitUIType { Collapsed, Visible };
public:
	virtual void NativeConstruct() override;

	void SetVoiceCommandState(const EWRVoiceCommandState IN InVoiceCommandState);
	void SetVoiceContentType(const EWRVoiceContentType IN InVoiceContentType);
	void SetVoiceWaitUIType(const EWRVoiceWaitUIType IN InVoiceWaitUIType);
	void UpdateUI();
	void SetJustification(const ETextJustify::Type IN InJustify);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* LoopAnimation;

	UPROPERTY(EditAnywhere)
		FText WaitText;
	UPROPERTY(EditAnywhere)
		FText RecordingText;
	UPROPERTY(EditAnywhere)
		FText VoiceSendingText;
	UPROPERTY(EditAnywhere)
		FText ImpossibleText;

private:
	UWRTextBlock* WRTextBlock_Content;
	EWRVoiceContentType VoiceContentType = EWRVoiceContentType::NONE;
	EWRVoiceWaitUIType CurVoiceWaitUIType;
};