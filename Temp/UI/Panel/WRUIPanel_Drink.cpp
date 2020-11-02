// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Drink.h"
#include "CanvasPanel.h"
#include "Define/WRUIDefine.h"
#include "Utility/WRUIUtility.h"
#include "Utility/WRActorUtility.h"
#include "Manager/WRUIManager.h"
#include "UI/CustomWidget/WRProgressBar.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCustomTextTable.h"
#include "Manager/WRPopupManager.h"

#include "Localization/WRLocalizationProcessor.h"

#define DEF_SUGGEST_TEXT_TID 9

void UWRUIPanel_Drink::NativeConstruct()
{
	Super::NativeConstruct();

	// Comment
	BIND_WIDGET(UWRTextBlock, WRTextBlock_Comment);
	// Progress
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Timer);	
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRProgressBar, ProgressBar_Timer, ProgressBar_Timer->OnConstruct(););
	// Suggested
	BIND_WIDGET(UCanvasPanel, CanvasPanel_AnswerYes);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_AnswerNo);
	// Guide
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Guide_R1);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Guide_R2);
	BIND_WIDGET(UCanvasPanel, CanvasPanel_Guide_Cross);	

	WRPopupManager::Get()->ClosePopup();
}

void UWRUIPanel_Drink::NativeDestruct()
{
	Super::NativeDestruct();

	WRActorUtility::ClearTimer(MaintainTimerHandle);
}

void UWRUIPanel_Drink::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ProgressBar_Timer)
		ProgressBar_Timer->OnTick(InDeltaTime);
}

void UWRUIPanel_Drink::SetToGuide()
{	
	Type = EWRDrinkType::Guide;

	// Comment
	WRUIUtility::SetVisibilitySafe(WRTextBlock_Comment, ESlateVisibility::Collapsed);
	// Progress
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Timer, ESlateVisibility::Collapsed);	
	// Suggested
	WRUIUtility::SetVisibilitySafe(CanvasPanel_AnswerYes, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_AnswerNo, ESlateVisibility::Collapsed);
	// Guide
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_R1, ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_R2, ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_Cross, ESlateVisibility::SelfHitTestInvisible);

	//WRActorUtility::SetTimer(MaintainTimerHandle, FTimerDelegate::CreateUObject(this, &UWRUIPanel_Drink::FinishMaintainTimer), GuideUIMaintainTime, false);
}

void UWRUIPanel_Drink::SetToSuggest()
{	
	Type = EWRDrinkType::Suggest;

	// Comment
	WRUIUtility::SetVisibilitySafe(WRTextBlock_Comment, ESlateVisibility::SelfHitTestInvisible);
	FString strComment = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, "Misc_SuggestKey").ToString();
	FWRCustomTextTable* pTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(DEF_SUGGEST_TEXT_TID);
	if (pTextTable != nullptr)
	{
		FString LocalKey = pTextTable->Localkey.ToString();
		strComment = WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString();
	}
	WRUIUtility::SetTextSafe(WRTextBlock_Comment, strComment);
	// Progress
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Timer, ESlateVisibility::SelfHitTestInvisible);
	if (ProgressBar_Timer)
		ProgressBar_Timer->SetTimer(DEF_CHEERS_SUGGEST_LIMIT_TIME);
	// Suggested
	WRUIUtility::SetVisibilitySafe(CanvasPanel_AnswerYes, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_AnswerNo, ESlateVisibility::Collapsed);
	// Guide
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_R1, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_R2, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_Cross, ESlateVisibility::Collapsed);
}

void UWRUIPanel_Drink::SetToSuggested(FString& IN InComment)
{
	Type = EWRDrinkType::Suggested;

	// Comment
	WRUIUtility::SetVisibilitySafe(WRTextBlock_Comment, ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetTextSafe(WRTextBlock_Comment, InComment);
	// Progress
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Timer, ESlateVisibility::Collapsed);
	// Suggested
	WRUIUtility::SetVisibilitySafe(CanvasPanel_AnswerYes, ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_AnswerNo, ESlateVisibility::SelfHitTestInvisible);
	// Guide
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_R1, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_R2, ESlateVisibility::Collapsed);
	WRUIUtility::SetVisibilitySafe(CanvasPanel_Guide_Cross, ESlateVisibility::Collapsed);
}

void UWRUIPanel_Drink::FinishMaintainTimer()
{
	WRActorUtility::ClearTimer(MaintainTimerHandle);
	WRUIManager::Get()->CloseUI(EWRUIPanelKind::Drink);
}
