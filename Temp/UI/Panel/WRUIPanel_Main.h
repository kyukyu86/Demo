// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// [ 2019-8-16 : kyu ] Created

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "UI/Slot/WRUISlot_Compass.h"
#include "UI/Slot/WRUISlot_DropPopup.h"
#include "UI/Slot/WRUISlot_TutorialSub.h"
#include "UI/Slot/WRUISlot_TutorialAlert.h"
#include "UI/Slot/WRUISlot_GyroInteraction.h"
#include "UI/Slot/WRUISlot_VoiceCommand.h"
#include "UI/Slot/WRUISlot_ChatRecording.h"
#include "Define/WRTypeDefine.h"
#include "WRUIPanel_Tutorial.h"
#include "WRUIPanel_Main.generated.h"



struct FWRQuestTaskTable;
struct FWRCompassMarkInfoBase;
UCLASS()
class WR_API UWRUIPanel_Main : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


public:

	// 나침판 관련 함수들 ----------------------------------------------------------------------
	FORCEINLINE void AddMark(const WRCompassMarkHandle IN InCompassMarkHandle, const FWRCompassMarkInfoBase* IN InCompassMarkInfo) { Slot_Compass->AddMark(InCompassMarkHandle, InCompassMarkInfo); }
	FORCEINLINE void RemoveMark(const WRCompassMarkHandle IN InCompassMarkHandle) { Slot_Compass->RemoveMark(InCompassMarkHandle); }
	FORCEINLINE void InitCompassUI() { Slot_Compass->InitCompassUI(); }
	FORCEINLINE void ClearCompassUI() { Slot_Compass->ClearCompassUI(); }
	//------------------------------------------------------------------------------------------

	// 드랍
	FORCEINLINE void AddItem(const WRTableID IN InTableID, const int32 IN InCount) { Slot_DropPopup->AddItem(InTableID, InCount); }
	
	// 자이로
	FORCEINLINE void OpenGyroInteraction(const FString& IN InIconPath, const FString& IN InDescription) { Slot_GyroInteraction->SetVisibility(ESlateVisibility::SelfHitTestInvisible); Slot_GyroInteraction->SetInformation(InIconPath, InDescription); }
	FORCEINLINE void CloseGyroInteraction() { Slot_GyroInteraction->SetVisibility(ESlateVisibility::Collapsed); }

	// 튜토리얼
	FORCEINLINE void OpenTutorialSub(const bool IN InSkip) { Slot_TutorialSub->OpenTutorialSub(InSkip); }
	FORCEINLINE void SetTutorialSubProgressBar(float IN InRate) { Slot_TutorialSub->SetProgressBar(InRate); }
	FORCEINLINE void CloseTutorialSub() { Slot_TutorialSub->CloseTutorialSub(); }
	FORCEINLINE void OpenTutorialAlert(const float IN InTime = 0.f) { Slot_TutorialAlert->OpenTutorialAlert(InTime); }
	FORCEINLINE void CloseTutorialAlert() { Slot_TutorialAlert->CloseTutorialAlert(); }

	// 음성 인식 
	FORCEINLINE void SetVoiceCommandState(const UWRUISlot_VoiceCommand::EWRVoiceCommandState IN InVoiceCommandState) { Slot_VoiceCommand->SetVoiceCommandState(InVoiceCommandState); }
	FORCEINLINE void SetRecordingTime(const float IN InTime) { Slot_VoiceCommand->SetRecordingTime(InTime); }
	FORCEINLINE void StartChatRecording(const float IN InTime) { Slot_ChatRecording->StartChatRecording(InTime); }
	FORCEINLINE void StopChatRecording() { Slot_ChatRecording->StopChatRecording(); }
		

public:
	class UTextBlock* TextBlock_GyroTransform; // [ 2019-11-15 : kyu ] for Test

public:
	UWRUISlot_Compass* Slot_Compass;
	UWRUISlot_DropPopup* Slot_DropPopup;
	UWRUISlot_GyroInteraction* Slot_GyroInteraction;
	UWRUISlot_TutorialSub* Slot_TutorialSub;
	UWRUISlot_TutorialAlert* Slot_TutorialAlert;
	UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	UWRUISlot_ChatRecording* Slot_ChatRecording;
};