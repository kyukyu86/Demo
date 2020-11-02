// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRCharacter.h"
#include "Define/WRTypeDefine.h"
#include "WRUIPanel_SocialMenu.generated.h"


/**
 * 
 */
class AWRCharacter;
class UWRFocusButton;
class UTextBlock;
class UWRTextBlock;
class UWRUISlot_VoiceCommand;
class UWRUISlot_VoiceCommandText;
struct FWRSocialAnimationTable;
UCLASS()
class WR_API UWRUIPanel_SocialMenu : public UWRUIPanel
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
private:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
public:
	UWRUISlot_VoiceCommand* GetSlotVoiceCommand() { return Slot_VoiceCommand; }
	UWRUISlot_VoiceCommandText* GetSlotVoiceCommandText() { return Slot_VoiceCommandText; }

	void SetOwnerCharacter(AWRCharacter* IN InOwnerCharacter);
	void InitSocialMenuInfo(const EWRCharacterType IN InCharacterType);

	void StartVoiceRecognition();
	void OnResultVoiceRecognitionReceived(const WRTableID IN InVoiceSubCommandTID);	// 음성인식 결과 받음

	bool OnInputEventReceived(const EWRInput IN InInputType);
private:
	UFUNCTION(BlueprintCallable, Category = "UWRUIPanel_SocialMenu")
		void OnClickedMenuButton();

	void CheckAutoCloseSocialMenu();	// 자동으로 소셜메뉴가 꺼져야 하는지 체크
	void DetermineFocusButton(const EWRInput IN InInputType);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FKey> CloseKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetAnimation* DisappearAnimation;
private:
	TArray<UWRFocusButton*> FocusButtonMenus;
	TArray<WRTableID> SocialAnimationTIDs;
	UTextBlock* TextBlock_Name;
	UWRTextBlock* WRTextBlock_Title1;
	UWRTextBlock* WRTextBlock_Title2;
	UWRUISlot_VoiceCommand* Slot_VoiceCommand;
	UWRUISlot_VoiceCommandText* Slot_VoiceCommandText;

	AWRCharacter* OwnerCharacter;
	float CheckTime = 0.f;

	EWRCharacterType CurrentCharacterType;
	int32 CurrentFocusButtonIndex;	// 현재 포커스 되어있는 버튼의 인덱스
	int32 UsingFocusButtonNum;	// 사용중인 포커스 버튼 수 
};
