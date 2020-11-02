// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_HouseSelectPopup.h"
#include "UI/Slot/WRUISlot_HouseSelectSlideElement.h"
#include "UI/CustomWidget/SlideList/WRSlideList.h"
#include "Utility/WRUIUtility.h"
#include "Manager/WRUIManager.h"

void UWRUIPanel_HouseSelectPopup::NativePreConstruct()
{
	Super::NativeConstruct();

}

void UWRUIPanel_HouseSelectPopup::NativeConstruct()
{
	Super::NativeConstruct();
		
	// ** �ܺο��� ȣ���������
	SetDataList(TestDataList);
}

void UWRUIPanel_HouseSelectPopup::NativeDestruct()
{

	Super::NativeDestruct();
}

void UWRUIPanel_HouseSelectPopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

FReply UWRUIPanel_HouseSelectPopup::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
		return FReply::Handled();

	EWRInput ConvertedInputType = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (ConvertedInputType != EWRInput::None)
	{
		switch (ConvertedInputType)
		{
		case EWRInput::Triangle:
		{

		}
		break;
		case EWRInput::Square:
		{

		}
		break;
		case EWRInput::Circle:
		{
			int32 MainDataIndex = SlideList->GetMainDataIndex();
			if (DataList.IsValidIndex(MainDataIndex))
			{

			}
		}
		break;
		case EWRInput::Cross:
		{
			WRUIManager::Get()->CloseUI(EWRUIPanelKind::HouseSelectPopup);
			return FReply::Handled();
		}
		break;
		}
	}
	return FReply::Unhandled();
}

void UWRUIPanel_HouseSelectPopup::SetDataList(TArray<FWRHouseSelectPopupData>& IN InDataList)
{
	// + �ܺο��� �����Ͱ����ͼ� �����س���
	DataList.Empty();
	DataList.Append(InDataList);

	// ** �ܺο��� ���� ������ ������ �����ؾ���
	SlideList->SetDataSize(DataList.Num());
	SlideList->UpdateElementList(true);
}
