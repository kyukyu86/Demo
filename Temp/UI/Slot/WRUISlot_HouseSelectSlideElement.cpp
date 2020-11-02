// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_HouseSelectSlideElement.h"
#include "UI/CustomWidget/SlideList/WRSlideList.h"
#include "UI/Panel/WRUIPanel_HouseSelectPopup.h"
#include "Manager/WRUIManager.h"
#include "Utility/WRUIUtility.h"

void UWRUISlot_HouseSelectSlideElement::NativeConstruct()
{
	Super::NativeConstruct();

}

void UWRUISlot_HouseSelectSlideElement::NativeDestruct()
{

	Super::NativeDestruct();
}

void UWRUISlot_HouseSelectSlideElement::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_HouseSelectSlideElement::SetMain(const bool IN InSet)
{
	UWRSlideElement::SetMain(InSet);

	Image_Main->SetVisibility(InSet ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UWRUISlot_HouseSelectSlideElement::SetData(const int32 IN InDataIndex)
{	
	if (ParentSlideListWidget == nullptr)
		return;

	// 각 컨텐츠 매니저에서 Get해도 되고...Panel에 데이터가 있으면 거기서 가져와도 되고..

	UWRUIPanel_HouseSelectPopup* HouseSelectPopup = WRUIManager::Get()->FindUI<UWRUIPanel_HouseSelectPopup>(EWRUIPanelKind::HouseSelectPopup);
	if (HouseSelectPopup == nullptr)
		return;

	TArray<FWRHouseSelectPopupData>& DataList = HouseSelectPopup->GetDataList();
	if (DataList.IsValidIndex(InDataIndex) == false)
		return;

	Image_BG->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Image_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WRUIUtility::SetResourceObject(Image_Icon, DataList[InDataIndex].ImagePath, true);
}

void UWRUISlot_HouseSelectSlideElement::SetEmpty()
{
	Image_BG->SetVisibility(ESlateVisibility::Collapsed);
	Image_Icon->SetVisibility(ESlateVisibility::Collapsed);
}
