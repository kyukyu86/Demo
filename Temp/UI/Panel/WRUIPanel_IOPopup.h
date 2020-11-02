// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Table/WRObjectTable.h"
#include "Table/WREnpcTable.h"
#include "WRUIPanel_IOPopup.generated.h"

DECLARE_DELEGATE(FWROnIOProgressCompleted)

enum class EWRIOPopupType : uint8
{
	Popup,
	Mark,
};

struct FWRIOInteractionWidget
{
	FWRIOInteractionWidget() : bHook(false)
	{

	}
	class UCanvasPanel* CanvasPanel_Slot;
	class UImage* Image_Icon;
	class UTextBlock* TextBlock_Contents;

	bool bHook;
	void SetFontColor(FLinearColor IN InColor);
};


struct FWRHookInfo
{
	FWRHookInfo() : 
		strTargetingIconPath(TEXT("")), strTargetingDescription(TEXT(""))
		, strActionIconPath(TEXT("")), strActionDescription(TEXT(""))
	{

	}
	FString strTargetingIconPath;
	FString strTargetingDescription;
	FString strActionIconPath;
	FString strActionDescription;
};

UCLASS()
class WR_API UWRUIPanel_IOPopup : public UWRUIPanel
{
	GENERATED_BODY()
	
private:
	bool bTargeted = false;
	bool bHookConnected = false;
	float fProgressTime = 0.f;
	float fProgressGoalTime = 0.f;
	FWROnIOProgressCompleted OnProgressCompleted;

public:
	// Popup
	class UCanvasPanel* CanvasPanel_Popup;
	class UCanvasPanel* CanvasPanel_PopupTitle;
	class UTextBlock* TextBlock_PopupTitle;
	class UCanvasPanel* CanvasPanel_PopupContents;
	class UTextBlock* TextBlock_PopupContents;
	class UImage* Image_PopupBG;

	// Mark
	class UCanvasPanel* CanvasPanel_Mark;
	TArray<FWRIOInteractionWidget> InteractionWidgets;

	// Progress
	class UCanvasPanel* CanvasPanel_Progress;
	class UProgressBar* ProgressBar;

	// Notify
	class UCanvasPanel* CanvasPanel_Notify;
	class UTextBlock* TextBlock_Notify;

	UPROPERTY(BlueprintReadWrite, Category = "UWRUIPanel_IOPopup")
		class UWidgetAnimation* Ani_Cancel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUIPanel_IOPopup")
		FLinearColor EnableFontColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUIPanel_IOPopup")
		FLinearColor DisableFontColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUIPanel_IOPopup")
		FLinearColor ExceptionFontColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWRUIPanel_IOPopup")
		FText ChangeToHookSubWeaponNotify;

private:
	void SetType(const EWRIOPopupType IN InPopupType);	
	// Popup
	void SetPopupTitle(bool IN InVisible, FString IN InTitle = TEXT(""));
	void SetPopupContents(FString IN InContents);
	// Mark
	void SetMarkInfo(uint8 IN InSlotIndex, FString IN InIconPath, FString IN InContents, FLinearColor IN InFontColor);
	void SetMarkDefault(uint8 IN InSlotIndex, FLinearColor IN InFontColor);
	// Progress
	void UpdateProgressBar(float IN InDeltaTime);
	// Notify
	void SetNotify(bool IN InVisible, FString IN InNotify = TEXT(""));

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	void SetDataFromTable(FWRInteractionObjectTable* IN InTable, class UWRActorComponentInteractionObject* const IN InInteractionInfo, const ISN_t IN InRelativeISN = INVALID_ISN);
	void SetHookInfo(const FWRHookInfo& IN InHookInfo, const bool IN InConnected);

	void ShowOnProgress(const float IN InGoalTime);
	void ShowOffProgress(const bool IN InComplete);
	void OnCancel();
	bool IsCanceling();
};
