// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Manager/WRCharacterCustomizingManager.h"
#include "WRUIPanel_NewCharacterCreate.generated.h"

UCLASS()
class WR_API UWRUIPanel_NewCharacterCreate : public UWRUIPanel
{
	GENERATED_BODY()

	//UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))

public:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*	WRTextBlock_GenderSelect;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*	WRTextBlock_Preset;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*	WRTextBlock_SkinColor;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*	WRTextBlock_PartsDetailSetting;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock*	WRTextBlock_Naming;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_CharacterName;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanel_MessageBox;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanel_ButtonInfo;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage*		Image_BG_Bar_GenderSelect;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage*		Image_BG_Bar_Preset;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage*		Image_BG_Bar_SkinColor;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage*		Image_BG_Bar_PartsDetailSetting;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage*		Image_BG_Bar_Naming;

	class UWRUIPanel_SubCharacterCreate* Panel_SubCharacterCreate;
	class UWRUIPanel_SubColorSelect* Panel_SubColorSelect;
	bool bUpdatePartsNameRequired = false;

private:
	virtual void	NativeConstruct() override;
	virtual void	NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void	NativeDestruct() override;

	//virtual FReply	NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply	NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	void OnPrePhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase);
	void OnPostPhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase);
	void OnPhaseCompleted(const EWRCharacterCustomizingPhase& IN InPhase);
	void InitializeImageColors();
	void InitializeFontProperty();
	void OnDPadLeftPressed();
	void OnDPadRightPressed();
	void OnCirclePressed();
	void OnCrossPressed();
	void MoveToPresetTID();
	void OnTriaglePressed();
	void OnOptionPressed();
	bool UpdateChildUI(const EWRCharacterCustomizingPhase& IN InPhase);
	void UpdateSelectButtonText(const FString& IN InStringToChagne);
	void UpdateBackButtonText(const FString& IN InStringToChagne);
	void OnAnimationFinished_Implementation(const UWidgetAnimation* IN InAnimation);
	
public:
	void SetPlayerName(const FString& IN InName);
	void SetSubCharacterCreatePanel(class UWRUIPanel_SubCharacterCreate* IN InPanel) { Panel_SubCharacterCreate = InPanel; }
	void SetSubColorSelectPanel(class UWRUIPanel_SubColorSelect* IN InPanel) { Panel_SubColorSelect = InPanel; }

};
