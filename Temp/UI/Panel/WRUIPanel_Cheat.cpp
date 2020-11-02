// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Cheat.h"

#include "UI/CustomWidget/WRFocusButton.h"
#include "UI/Slot/WRUISlot_Preview.h"

#include "TextBlock.h"
#include "Overlay.h"
#include "SWindow.h"
#include "SharedPointer.h"
#include "WidgetPath.h"
#include "SWidget.h"
#include "Button.h"
#include "ScrollBox.h"
#include "CanvasPanel.h"
#include "VerticalBox.h"
#include "WidgetSwitcher.h"
#include "WidgetBlueprintLibrary.h"

#include "Core/WRGameInstance.h"

#include "Define/WRUIDefine.h"

#include "Manager/WRUIManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRObjectTable.h"
#include "Table/WREnpcTable.h"
#include "Table/WRItemTable.h"
#include "Manager/WRPreviewManager.h"
#include "Manager/WRCharacterManager.h"
#include "Actor/Vehicle/WRVehicle.h"
#include "Utility/WRActorSpawnUtility.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Manager/WRVehicleManager.h"
#include "Utility/WREnumUtility.h"
#include "Utility/WRUIUtility.h"




void UWRUIPanel_Cheat::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본 기능입니다.
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Main, FocusButton_Main->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Main););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Item, FocusButton_Item->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Item););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Monster, FocusButton_Monster->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Monster););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_IO, FocusButton_IO->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_IO););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Character, FocusButton_Character->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Character););

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_First, FocusButton_First->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_First););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Prev, FocusButton_Prev->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Prev););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Next, FocusButton_Next->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Next););
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Last, FocusButton_Last->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Last););
	BIND_WIDGET(UTextBlock, TextBlock_Page);
	//BIND_WIDGET(UOverlay, Overlay_Preview);
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRUISlot_Preview, Slot_Preview, Slot_Preview->SetVisibility(ESlateVisibility::Collapsed););
	BIND_WIDGET(UWidgetSwitcher, WidgetSwitcher_RightContents);
	BindMain();	
	BindSubCategory();
	BindList();
	// Focus
	SetFocusableInputMode(true, Cast<UWidget>(FocusButton_Main->GetButton()));	
	LastFocusedButton = FocusButton_Main;
	SwitchCategory(EWRCheatCategory::Main);

	//====================================================================================
	// [ 2019-11-19 : kyu ] ▼▼▼▼ 추가 코드는 이곳에 추가하세요.
	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Custom_01, FocusButton_Custom_01->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Custom_01););

	BIND_WIDGET_WITH_IMPLEMENTATION(UWRFocusButton, FocusButton_Custom_02, FocusButton_Custom_02->GetButton()->OnClicked.AddDynamic(this, &UWRUIPanel_Cheat::OnClicked_Custom_02););
}

void UWRUIPanel_Cheat::BindMain()
{
	BIND_WIDGET(UCanvasPanel, CanvasPanel_RightMain);
}

void UWRUIPanel_Cheat::BindSubCategory()
{
	int32 nIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("FocusButton_SubCategory_%d"), nIndex);
		UWRFocusButton* pCaseButton = Cast<UWRFocusButton>(GetWidgetFromName(FName(*WidgetName)));
		if (pCaseButton != nullptr)
		{
			pCaseButton->SetIndex(nIndex + 1);
			pCaseButton->OnClicked.BindUObject(this, &UWRUIPanel_Cheat::OnClicked_SubCategory);

			SubCategoryList.Add(pCaseButton);
			++nIndex;
		}
		else
		{
			bCreating = false;
		}
	}
}

void UWRUIPanel_Cheat::BindList()
{
	BIND_WIDGET(UVerticalBox, VerticalBox_RightList);

	int32 nIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("Slot_CheatElement_%d"), nIndex);
		UWRUISlot_CheatRightListElement* pCaseButton = Cast<UWRUISlot_CheatRightListElement>(GetWidgetFromName(FName(*WidgetName)));
		if (pCaseButton != nullptr)
		{
			Slot_RightListElement.Add(pCaseButton);
			++nIndex;
		}
		else
		{
			bCreating = false;
		}
	}
}

void UWRUIPanel_Cheat::NativeDestruct()
{
	SetFocusableInputMode(false);

// 	if (Panel_Preview != nullptr)
// 	{
// 		Overlay_Preview->RemoveChild(Panel_Preview);
// 		Panel_Preview = nullptr;
// 	}

	Super::NativeDestruct();
}

void UWRUIPanel_Cheat::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UWRUIPanel_Cheat::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// 닫음 키입력 추가
	int32 nIndex = 0;
	if (PagePrevKey.Find(InKeyEvent.GetKey(), nIndex))
	{
		if (WidgetSwitcher_RightContents->GetActiveWidgetIndex() == (int32)EWRCheatRightForm::List)
		{
			OnClicked_Prev();
		}		
	}
	else if (PageNextKey.Find(InKeyEvent.GetKey(), nIndex))
	{
		if (WidgetSwitcher_RightContents->GetActiveWidgetIndex() == (int32)EWRCheatRightForm::List)
		{
			OnClicked_Next();
		}
	}
	else
	{
		EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
		switch (ConvertedInputType)
		{
		case EWRInput::Circle:
		{
			LastFocusedButton->OnClickedButton();
			return FReply::Handled();
		}
		break;

		case EWRInput::Cross:
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::Cheat);
			return FReply::Handled();
		}
		break;
		}
	}

	return FReply::Unhandled();
}

void UWRUIPanel_Cheat::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	// 포커스 잃으면 복구
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);

	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	// + Preview 중에는 포커스 복구하지말것
// 	if (Panel_Preview != nullptr)
// 		return;

	if (this->HasUserFocusedDescendants(PlayerController) && LastFocusedButton != nullptr)
	{
		LastFocusedButton->SetUserFocus(PlayerController);
	}
}

void UWRUIPanel_Cheat::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	UWRFocusButton* PrevFocusedButton = nullptr;

	for (auto EachWidget : PreviousFocusPath.Widgets)
	{
		if (EachWidget.IsValid() == false)
			continue;

		UWidget* FoundWidget = GetWidgetHandle(EachWidget.Pin().ToSharedRef());
		if (FoundWidget != nullptr)
		{
			if (FoundWidget->IsA<UWRFocusButton>())
			{
				PrevFocusedButton = Cast<UWRFocusButton>(FoundWidget);
			}
		}
	}

// 	TSharedRef<SWindow> RefWindow = NewWidgetPath.GetWindow();
// 	TSharedRef<const SWidget> RefconstWidget = RefWindow->GetContent();
// 	TSharedRef<SWidget> _A = ConstCastSharedRef<SWidget>(RefconstWidget);
// 	UWidget* GettedWidget = GetWidgetHandle(_A);
// 	UUserWidget* TopWidget = Cast<UUserWidget>(GettedWidget);
//	if (TopWidget != nullptr)
	{
		for (auto EachWidget : NewWidgetPath.Widgets.GetInternalArray())
		{
			//UWidget* FoundWidget = TopWidget->GetWidgetHandle(EachWidget.Widget);
			UWidget* FoundWidget = GetWidgetHandle(EachWidget.Widget);
			if (FoundWidget != nullptr)
			{
				if (FoundWidget->IsA<UWRFocusButton>())
				{
					LastFocusedButton = Cast<UWRFocusButton>(FoundWidget);
				}
			}
		}
	}
	

	//TSharedRef<SWindow> RefWindow = NewWidgetPath.GetWindow();	
	//UWidget* FoundWidget = GetWidgetHandle(RefWindow);			// null
	//FString RefName = RefWindow->GetReferencerName();				// L"FSlateInvalidationRoot"
	//TSharedPtr<SWidget> RefWidget = RefWindow->GetParentWidget();	// null
	//SWindow& _sWindow = RefWindow.Get();
	//const SWidget& constWidget = RefconstWidget.Get();
	//TWeakPtr<SWidget> _sWidget = _sWindow.GetParentWidget();		// null

// 	TSharedRef<SWindow> RefWindow = NewWidgetPath.GetDeepestWindow();	// 무조건 최상단 Panel이 뜸
// 	TSharedRef<const SWidget> RefconstWidget = RefWindow->GetContent();
// 	TSharedRef<SWidget> _A = ConstCastSharedRef<SWidget>(RefconstWidget);
// 	UWidget* FoundWidget = GetWidgetHandle(_A);
	
}

void UWRUIPanel_Cheat::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation->MovieScene->GetFName() == FName("OpenAnimation"))
	{
		SetFocusableInputMode(true, Cast<UWidget>(FocusButton_Main->GetButton()));
		LastFocusedButton = FocusButton_Main;
	}
}

void UWRUIPanel_Cheat::ShowPreview(FWRPreviewInfo& IN InPreviewInfo)
{	
// 	auto AsyncCreateComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
// 	{
// 		strAsyncKey = "";
// 
// 		if (Overlay_Preview == nullptr)
// 			return;
// 
// 		Panel_Preview = Cast<UWRUIPanel_Preview>(InObject);
// 		if (Panel_Preview == nullptr)
// 			return;
// 
// 		Overlay_Preview->AddChildToOverlay(Panel_Preview);
// 
// 		FWRPreviewInfo PreviewInfo;
// 		PreviewInfo.BPPath = InPath;
// 		Panel_Preview->SetPreviewTarget(PreviewInfo);
// 	});
// 	strAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(WRUIPathManager::Get()->GetUIPath(EWRUIPanelKind::Preview), AsyncCreateComplete);

// 	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
// 	{
// 		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
// 		{
// 			UWRUIPanel_Preview* CastedWidget = Cast<UWRUIPanel_Preview>(InCombinedPanelData.UIPanel);
// 			if (CastedWidget == nullptr)
// 				return;
// 
// 			CastedWidget->SetPreviewTarget(InPreviewInfo);
// 		}
// 	});
// 
// 	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Preview, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
// 	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

// 	if (Slot_Preview->IsValidLowLevel() == false)
// 		return;

	InPreviewInfo.SetPreviewWidget(Slot_Preview);
	InPreviewInfo.PreviewExitDelegate = FWRPreviewExitDelegate::CreateUObject(this, &UWRUIPanel_Cheat::OnFocus);
	WRPreviewManager::Get()->SetPreview(InPreviewInfo);

//	Slot_Preview->SetPreviewTarget(InPreviewInfo);
}

void UWRUIPanel_Cheat::OnFocus()
{
	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	//if (this->HasUserFocusedDescendants(PlayerController) && LastFocusedButton != nullptr)
	if (LastFocusedButton != nullptr)
	{
		LastFocusedButton->SetUserFocus(PlayerController);
	}
}

//====================================================================================
//
//====================================================================================
void UWRUIPanel_Cheat::OnClicked_Main()
{
	SwitchCategory(EWRCheatCategory::Main);
}

void UWRUIPanel_Cheat::OnClicked_Item()
{
	SwitchCategory(EWRCheatCategory::Item);
}

void UWRUIPanel_Cheat::OnClicked_Monster()
{
	SwitchCategory(EWRCheatCategory::Monster);
}

void UWRUIPanel_Cheat::OnClicked_IO()
{
	SwitchCategory(EWRCheatCategory::IO);
}

void UWRUIPanel_Cheat::OnClicked_Character()
{
	SwitchCategory(EWRCheatCategory::Character);
}

void UWRUIPanel_Cheat::OnClicked_SubCategory(int32 InIndex)
{
	switch (CurCategory)
	{
	case EWRCheatCategory::Item:
	{
		EWRCheatItemSubCategory _ItemSubCategory = (EWRCheatItemSubCategory)InIndex;
		switch (_ItemSubCategory)
		{
		case EWRCheatItemSubCategory::Weapon:
		{
			UpdateData<FWRWeaponItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Armor:
		{
			UpdateData<FWRArmorItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Costume:
		{
			UpdateData<FWRCostumeItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Temporary:
		{
			UpdateData<FWRTemporaryItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Vehicle:
		{
			UpdateData<FWRVehicleItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Material:
		{
			UpdateData<FWRMaterialItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Interior:
		{
			UpdateData<FWRInteriorItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Emote:
		{
			UpdateData<FWREmoteItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Placement:
		{
			UpdateData<FWRPlacementItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		case EWRCheatItemSubCategory::Housing:
		{
			UpdateData<FWRHousingItemTable>(CurCategory, 1, _ItemSubCategory);
		}
		break;
		}
	}
	break;
	}
}

void UWRUIPanel_Cheat::OnClicked_First()
{
	nPage = 1;
	UpdatePage();
}

void UWRUIPanel_Cheat::OnClicked_Prev()
{
	if (nPage > 1)
	{
		--nPage;
		UpdatePage();
	}
}

void UWRUIPanel_Cheat::OnClicked_Next()
{
	if (nPage < nMaxPage)
	{
		++nPage;
		UpdatePage();
	}
}

void UWRUIPanel_Cheat::OnClicked_Last()
{
	nPage = nMaxPage;
	UpdatePage();
}

void UWRUIPanel_Cheat::OnClicked_Custom_01()
{

}

void UWRUIPanel_Cheat::OnClicked_Custom_02()
{
	WRVehicleManager::Get()->SpawnVehicleFromCheat();
}

//====================================================================================
//
//====================================================================================
void UWRUIPanel_Cheat::HideAllList()
{
	for (UWRUISlot_CheatRightListElement* List : Slot_RightListElement)
	{
		List->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWRUIPanel_Cheat::SwitchCategory(const EWRCheatCategory IN InCategory)
{
	if (CurCategory == InCategory)
		return;

	// Old
	SetSelectCategory(CurCategory, false);

	EWRCheatCategory PrevCategory = CurCategory;
	switch (PrevCategory)
	{
	case EWRCheatCategory::Item:
	{
		PlayAnimationByName("OpenSubCategory", 0, 1, EUMGSequencePlayMode::Reverse, 1);
	}
	break;
	}

	// New
	CurCategory = InCategory;
	SetSelectCategory(CurCategory, true);
	switch (InCategory)
	{
	case EWRCheatCategory::Main:
	{
		SwitchRightForm(EWRCheatRightForm::Main);
	}
	break;
	case EWRCheatCategory::Item:
	{
		SwitchRightForm(EWRCheatRightForm::List);		
		UpdateItemSubCategory(EWRCheatItemSubCategory::Weapon);		
		PlayAnimationByName("OpenSubCategory");
	}
	break;
	case EWRCheatCategory::Monster:
	{
		SwitchRightForm(EWRCheatRightForm::List);
		UpdateData<FWREnpcTable>(InCategory, 1);
	}
	break;
	case EWRCheatCategory::IO:
	{
		SwitchRightForm(EWRCheatRightForm::List);
		UpdateData<FWRInteractionObjectTable>(InCategory, 1);
	}
	break;
	case EWRCheatCategory::Character:
	{
		SwitchRightForm(EWRCheatRightForm::List);
		UpdateData<FWRNpcTable>(InCategory, 1);
	}
	break;
	}	
}

void UWRUIPanel_Cheat::SwitchRightForm(const EWRCheatRightForm IN InList)
{
	if (WidgetSwitcher_RightContents == nullptr)
		return;
	
	WidgetSwitcher_RightContents->SetActiveWidgetIndex((int32)InList);
}

void UWRUIPanel_Cheat::SetSelectCategory(const EWRCheatCategory IN InCategory, const bool IN InSelect)
{
	switch (InCategory)
	{
	case EWRCheatCategory::Main:
	{
		FocusButton_Main->SetSelect(InSelect);
	}
	break;
	case EWRCheatCategory::Item:
	{
		FocusButton_Item->SetSelect(InSelect);
	}
	break;
	case EWRCheatCategory::Monster:
	{
		FocusButton_Monster->SetSelect(InSelect);
	}
	break;
	case EWRCheatCategory::IO:
	{
		FocusButton_IO->SetSelect(InSelect);
	}
	break;
	case EWRCheatCategory::Character:
	{
		FocusButton_Character->SetSelect(InSelect);
	}
	break;
	}
}

void UWRUIPanel_Cheat::UpdateItemSubCategory(const EWRCheatItemSubCategory IN InItemCategory, const int32 IN InPage /* = 1 */)
{
	switch (InItemCategory)
	{
	case EWRCheatItemSubCategory::Weapon:
	{
		UpdateData<FWRWeaponItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Armor:
	{
		UpdateData<FWRArmorItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Costume:
	{
		UpdateData<FWRCostumeItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Temporary:
	{
		UpdateData<FWRTemporaryItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Vehicle:
	{
		UpdateData<FWRVehicleItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Material:
	{
		UpdateData<FWRMaterialItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Interior:
	{
		UpdateData<FWRInteriorItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Emote:
	{
		UpdateData<FWREmoteItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Placement:
	{
		UpdateData<FWRPlacementItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	case EWRCheatItemSubCategory::Housing:
	{
		UpdateData<FWRHousingItemTable>(EWRCheatCategory::Item, InPage, InItemCategory);
	}
	break;
	}

	// for Naming
	for (auto& EachSubCategory : SubCategoryList)
	{
		EWRCheatItemSubCategory SubCategoryType = (EWRCheatItemSubCategory)EachSubCategory->GetIndex();
		if (SubCategoryType < EWRCheatItemSubCategory::Max)
		{
			EachSubCategory->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			EachSubCategory->SetContents(WREnumUtility::EnumToString("EWRCheatItemSubCategory", SubCategoryType));
		}
		else
		{
			EachSubCategory->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UWRUIPanel_Cheat::UpdatePage()
{
	switch (CurCategory)
	{	
	case EWRCheatCategory::Item:
	{
		UpdateItemSubCategory(ItemSubCategory, nPage);
	}
	break;
	case EWRCheatCategory::Monster:
	{
		UpdateData<FWREnpcTable>(CurCategory, nPage);
	}
	break;
	case EWRCheatCategory::IO:
	{
		UpdateData<FWRInteractionObjectTable>(CurCategory, nPage);
	}
	break;
	case EWRCheatCategory::Character:
	{
		UpdateData<FWRNpcTable>(CurCategory, nPage);
	}
	break;
	}
}

EWRCheatRightListType UWRUIPanel_Cheat::GetListTypeFromCategory(const EWRCheatCategory IN InCategory)
{
	switch (InCategory)
	{
	case EWRCheatCategory::Item:
	{
		return EWRCheatRightListType::Item;
	}
	break;
	case EWRCheatCategory::Monster:
	{
		return EWRCheatRightListType::Monster;
	}
	break;
	case EWRCheatCategory::IO:
	{
		return EWRCheatRightListType::IO;
	}
	break;
	case EWRCheatCategory::Character:
	{
		return EWRCheatRightListType::Character;
	}
	break;
	}
	return EWRCheatRightListType::None;
}
