// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Utility/WRUIUtility.h"
#include "WRUIPanel_DanceMode.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
class UImage;
class UWidgetSwitcher;
struct FWRSocialAnimationTable;
UCLASS()
class WR_API UWRUIPanel_DanceMode : public UWRUIPanel
{
	GENERATED_BODY()
public:
	enum EWRFunctionType { None, StartDanceMode, NextStep };
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UWRUIPanel_DanceMode::EWRFunctionType GetCurFunctionType() { return CurFunctionType; }
	EWRInput GetNextStepInput() { return NextStepInput; }
	void SetFunctionType(const UWRUIPanel_DanceMode::EWRFunctionType IN InFunctionType);
	void SetNextStepInput(const EWRInput IN InInput, UTexture2D* IN InIcon);
	void SetNextStepInput(const EWRInput IN InInput, const FString& IN InIconPath);

	void OnStartDanceModeClicked();
	void OnNextStepClicked();

	void NextDanceKind();
	void PrevDanceKind();
private:
	void Init();
	void SetDanceKind(const FWRSocialAnimationTable* IN InSocialAnimationTable);
	void OnLoadCompletedForIconTexture(UObject* IN InIconTexture, FString IN InHash);
private:
	//UButton* Button_Function;
	UWidgetSwitcher* WidgetSwitcher_Type;
	UTextBlock* TextBlock_Content;
	UImage* Image_NextStep;
	UWRUIPanel_DanceMode::EWRFunctionType CurFunctionType;

	EWRInput NextStepInput;

	TArray<FWRSocialAnimationTable*> SocialAnimationTables;
	int32 CurDanceKindIndex;

	FString AsyncLoadHashForIconTexture;
};
