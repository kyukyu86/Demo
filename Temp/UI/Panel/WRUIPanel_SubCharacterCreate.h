// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Manager/WRCharacterCustomizingManager.h"
#include "WRUIPanel_SubCharacterCreate.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_SubCharacterCreate : public UWRUIPanel
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWidgetSwitcher* WidgetSwitcher_Scene;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Title;
	/*UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_SubTitle;*/
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_SelectedName;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_GenderChoice_ON;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_Preset_ON;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_SkinColor_ON;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_DetailSetting_ON;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_Naming_ON;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanel_Decision;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UCanvasPanel* CanvasPanel_Back;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Select;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UWRTextBlock* WRTextBlock_Back;

private:
	enum
	{
		Title,
		Contents,
	};

private:
	virtual void	NativeConstruct() override;
	//virtual void	NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void	NativeDestruct() override;

	void UpdateMark();
	void TrunOffAllMarks();
	void OnAnimationFinished_Implementation(const UWidgetAnimation* IN InAnimation);
	void OnPhaseCompleted(const EWRCharacterCustomizingPhase& IN InPhase);
	void OnPrePhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase);
	void OnPostPhaseChanged(const EWRCharacterCustomizingPhase& IN InPhase);

public:
	bool UpdateSubSelectedNameText(const EWRCharacterCustomizingPhase& IN InPhase = EWRCharacterCustomizingPhase::None);
	void SetSelectButtonText(const FString& IN InString);
	void SetBackButtonText(const FString& IN InString);
};
