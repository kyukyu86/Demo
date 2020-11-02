// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Controller/WRVideoChatController.h"
#include "WRUIPanel_VideoChatScreen.generated.h"


/**
 * 
 */

class UWRUISlot_Preview;
class UWRTextBlock;
class UWRUISlot_VoiceCommand;
class UWRUISlot_VoiceCommandText;
class UWRUISlot_VideoChatBubble;
class UCanvasPanel;
class AWRStaticMeshActor;
UCLASS()
class WR_API UWRUIPanel_VideoChatScreen : public UWRUIPanel
{
	GENERATED_BODY()
public:
	enum class EWRUIType {None, Calling, Receiving, Connected, Disconnected};
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation);
public:
	void ShowCallingUI(const FWRVideoChatUserData IN InVideoChatUserData);
	void ShowReceivingUI(const FWRVideoChatUserData IN InVideoChatUserData);
	void ShowConnectedUI();
	void ShowDisConnectedUI();

	void PlayCallingAnimation();
	void PlayRejectAnimation();
	void PlaySpeakGesture();
	void PlayPreviewFloorProdution();
	void StopPreviewFloorProdution();

	void PlaySoundWave();
	void StopSoundWave();

	UWRUISlot_VoiceCommand* GetSlotVoiceCommand() { return Slot_VoiceCommand; }
	UWRUISlot_VoiceCommandText* GetSlotVoiceCommandText() { return Slot_VoiceCommandText; }

	void StartVoiceRecognition();
	void OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID);	// 음성인식 결과 받음
	void OnChatRecordingPressed();

	void AddChatBubble(const FText& IN InContent, const float IN InTime = 0.f);
	void DetermineTranslationUIState();
private:
	void SetVideoChatUserData(const FWRVideoChatUserData& IN InVideoChatUserData);
	void OnPreviewCharacterSpawCompleted(AActor* IN InCharacter);
	void SpawnPreviewCharacter(const FWRVideoChatUserData& IN InVideoChatUserData);
	void ShowPreviewCharacter();
private:
	UCanvasPanel* CanvasPanel_Cyber;
	UCanvasPanel* CanvasPanel_NoCyber;

	UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText;
	UWRUISlot_VideoChatBubble* Slot_VideoChatBubble;

	UWRUISlot_Preview* Slot_Preview;

	UWRTextBlock* WRTextBlock_Msg;
	UWRTextBlock* WRTextBlock_PlayerName;
	UCanvasPanel* CanvasPanel_Calling;
	UCanvasPanel* CanvasPanel_Reject;
	UCanvasPanel* CanvasPanel_CallingIcon;
	UCanvasPanel* CanvasPanel_RejectIcon;
	UCanvasPanel* CanvasPanel_RejectArrow;
	UCanvasPanel* CanvasPanel_Translation;

	UImage* Image_TranslatingLoop;
	UImage* Image_SoundWave;

	UMaterialInstanceDynamic* Mat_SoundWave;

	UPROPERTY(EditAnywhere)
		float PreviewFloorZOffset = 0;
	UPROPERTY(EditAnywhere)
		float PreviewFloorScale = 1;

	EWRUIType CurrentUIType = EWRUIType::None;
	AID_t PlayerAID = INVALID_AID;  // 화상채팅 관련 PlayerAID
	uint32 ReceivingLoopSoundIndex = INDEX_NONE;


	FString AsyncLoadKeyForPreviewFloor;
	AWRStaticMeshActor* PreviewFloorActor;

	float PreviewFloorTIme = 0.f;
	float SoundWaveTime = 0.f;
};
