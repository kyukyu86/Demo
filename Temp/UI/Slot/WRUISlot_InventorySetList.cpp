// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUISlot_InventorySetList.h"
#include "Define/WRUIDefine.h"
#include "UI/CustomWidget/WRFocusButton.h"
#include "Button.h"
#include "Manager/WRInventoryManager.h"

void UWRUISlot_InventorySetList::NativeConstruct()
{
	Super::NativeConstruct();
	
	int32 nSlotIndex = 0;
	bool bCreating = true;
	while (bCreating)
	{
		FString WidgetName = FString::Printf(TEXT("FocusButton_%d"), nSlotIndex);
		UWRFocusButton* pFocusButton = Cast<UWRFocusButton>(GetWidgetFromName(FName(*WidgetName)));
		if (pFocusButton != nullptr)
		{
			pFocusButton->SetIndex(nSlotIndex);
			pFocusButton->OnClicked.BindUObject(this, &UWRUISlot_InventorySetList::OnClicked_List);
			FocusButton_List.Add(pFocusButton);
			++nSlotIndex;
		}
		else
		{
			bCreating = false;
		}
	}
}

void UWRUISlot_InventorySetList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UWRUISlot_InventorySetList::OnClicked_List(int32 InIndex)
{
	if (FocusButton_List.IsValidIndex(InIndex))
	{
		WRInventoryManager::Get()->OnClickedSet(InIndex + 1);
	}
}