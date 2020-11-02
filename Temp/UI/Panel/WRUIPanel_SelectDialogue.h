// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// [ 2019-8-16 : kyu ] Created

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "Table/WRDialogueTable.h"
#include "Enum/EWRUIEnum.h"
#include "WRUIPanel_SelectDialogue.generated.h"

class UCanvasPanel;
class UWRFocusButton;
class UImage;
class UWRUISlot_VoiceCommand;
class UWRUISlot_VoiceCommandText;

struct FWRDialogueSetInfo
{
	FWRDialogueSetInfo(UCanvasPanel* IN InCanvasPanel, UWRFocusButton* IN InFocusButton, UImage* InImage_Icon, UImage* InImage_Focus)
		: CanvasPanel(InCanvasPanel), FocusButton(InFocusButton), Image_Icon(InImage_Icon), Image_Focus(InImage_Focus), CurScaleForZoom(1.f) 
	{
		VoiceSubCommandTIDs.Empty();
	}
	UCanvasPanel* CanvasPanel;
	UWRFocusButton* FocusButton;
	UImage* Image_Icon;
	UImage* Image_Focus;
	TArray<WRTableID> VoiceSubCommandTIDs;
	float CurScaleForZoom;
};


UCLASS()
class WR_API UWRUIPanel_SelectDialogue : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
	virtual void NativeDestruct() override;
public:
	UWRUISlot_VoiceCommand* GetSlotVoiceCommand() { return Slot_VoiceCommand; }
	UWRUISlot_VoiceCommandText* GetSlotVoiceCommandText() { return Slot_VoiceCommandText; }
	void SetDialogueSetGroupData(const WRTableID IN InDialogueSetGroupTID);
	void StartVoiceRecognition();
	void OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID);	// 음성인식 결과 받음
	void ForcedStopSelectDialogue();	// 강제로 멈출때 사용

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_SelectDialogue")
		void OnClickedDialogueSetButton();

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_SelectDialogue")
		void OnHoveredDialogueSetButton();

	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_SelectDialogue")
		void OnUnHoveredDialogueSetButton();
private:
	void Tick_Zoom(const float IN InDeltaTime);

	void SetDialogueSetData(const int8 IN InIndex, const WRTableID IN InDialogueSetTID, const FName& IN InDescription);
	bool CheckDialogueSetCondition(const FWRDialogueSetCondition& IN InDialogueSetCondition);
private:
	UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText;

	TArray<FWRDialogueSetInfo> DialogueSetInfos;
	TArray<TPair<EWRDialogueType, WRTableID>> NextDialogueInfos;
	float MaxScaleForZoom;
	float ZoomSpeed;
	bool AlreadyRelaseMoveLock = false;
};