// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Controller/WRVideoChatController.h"
#include "WRUIPanel_VideoChatUserList.generated.h"


class UWRUISlot_VideoChatUserInfo;
class UWRUISlot_VoiceCommand;
class UWRUISlot_VoiceCommandText;
class UWRTextBlock;
/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_VideoChatUserList : public UWRUIPanel
{
	GENERATED_BODY()

private:
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void UpdateVideoChatUserList();

	UWRUISlot_VoiceCommand* GetSlotVoiceCommand() { return Slot_VoiceCommand; }
	UWRUISlot_VoiceCommandText* GetSlotVoiceCommandText() { return Slot_VoiceCommandText; }

	void StartVoiceRecognition();
	void OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID);	// 음성인식 결과 받음

	void CloseUIAfterAnimation();
private:
	void InitVideoChatUserList();
	void NextPage();
	void PrevPage();
	void SetPage(const int32 IN InIndex);
private:
	UWRUISlot_VideoChatUserInfo* Slot_VideoChatUserInfo;
	UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText;
	UWRTextBlock* WRTextBlock_UserCount;

	TArray<FWRVideoChatUserData> VideoChatUserDataArray;
	int32 VideoChatUserDataIndex = -1;

	int32 ShortcutNumber = INVALID_SHORTCUT_NUMBER;
	FString NickName;
};
