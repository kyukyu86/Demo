// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRUIEnum.h"
#include "Define/WRTypeDefine.h"
#include "Network/Share/Define/Define_BluePrint.h"
#include "WRUIPanel_Dialogue.generated.h"


struct FWRDialogueTable;
class UWRTextBlock;
class UTextBlock;
class AWRCharacter;
class USoundBase;
class UCurveFloat;

UCLASS()
class WR_API UWRUIPanel_Dialogue : public UWRUIPanel
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	void OpenDialgoue(const FWRDialogueTable* IN InDialogueTable);
	void OnDialogueSetSelected(const EWRDialogueType IN InNextDialogueType, const WRTableID IN InNextDialogueTID);
	void ForcedFinishDialogue();
	FORCEINLINE void StopDialogue() { ClearDialogue(); }
private:
	void SetDialogueData(const FWRDialogueTable* IN InDialogueTable, const bool IN bInCheckAsynLoadData = true);
	void SetDialogueDataRelatedToCharacter(const FWRDialogueTable* IN InDialogueTable);
	void NextDialogue();
	void FinishDialogue();
	void ClearDialogue();
	void ClearPreviousCharacterData(const WRTableID IN InCurrentCharacterTID = INVALID_TABLE_TID);

	void PlayVoice(const FWRDialogueTable* IN InDialogueTable);
	void PlayTalkAni(AWRCharacter* IN InCharacter, const FName IN InAniID);
	void StopVoice();

	void ProgressDialogueAction(const FWRDialogueTable* IN InDialogueTable);
	void DetermineToCreateIcon(const FWRDialogueTable* IN InDialogueTable, AWRCharacter* IN InSpeakCharacter);
	void OpenSelectDialogueUI(const FWRDialogueTable* IN InDialogueTable, AWRCharacter* IN InSpeakCharacter);

	void StartAsyncLoadData(const FWRDialogueTable* IN InDialogueTable);	// 어싱크 로드 시작
	void OnAsyncLoadCompleted(UObject* IN InLoadedAsset, FString IN InHash);	// 어싱크 로드 완료
private:
	UWRTextBlock* WRTextBlock_Content;

	EWRDialogueType NextDialogueType;
	WRTableID NextDialogueTID;
	const FWRDialogueTable* PreviousDialogueData;
	float RemainPlayTime;
	bool bAutoSkip;
	int32 VoiceHandle;

	FString AsyncLoadHashForVoiceSound;
	FString AsyncLoadHashForLipsyncCurve;

	const FWRDialogueTable* SaveDialogueTable;
	USoundBase* VoiceSound;
	UCurveFloat* LipsyncCurve;
};	