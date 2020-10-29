// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIInventorySlotList.h"
#include <Components/TileView.h>
#include "DMUIInventoryItemSlot.h"


void UDMUIInventorySlotList::NativePreConstruct()
{
	Super::NativePreConstruct();

	int32 CreateTileNum = MinTileNum != 0 ? FMath::Max(MinTileNum, TestDataList.Num()) : TestDataList.Num();
	for (int32 i = 0; i < CreateTileNum; ++i)
	{
		UDMInventoryItemSlotDataObject* NewData = NewObject<UDMInventoryItemSlotDataObject>();
		if (TestDataList.IsValidIndex(i))
		{
			NewData->Data = TestDataList[i];
			NewData->bIsEmpty = false;
			DataObjectList.Add(TestDataList[i].ID, NewData);
		}
		TileView_List->AddItem(NewData);
	}
}

void UDMUIInventorySlotList::NativeConstruct()
{
	Super::NativeConstruct();

	TileView_List->OnItemScrolledIntoView().AddUObject(this, &UDMUIInventorySlotList::OnItemScrolledIntoView);
	TileView_List->OnItemSelectionChanged().AddUObject(this, &UDMUIInventorySlotList::OnItemSelectionChanged);
	TileView_List->OnItemClicked().AddUObject(this, &UDMUIInventorySlotList::OnItemClicked);
	TileView_List->OnItemIsHoveredChanged().AddUObject(this, &UDMUIInventorySlotList::OnItemHoveredChanged);
}

void UDMUIInventorySlotList::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDMUIInventorySlotList::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

UDMInventoryItemSlotDataObject* UDMUIInventorySlotList::GetDataObject(const int32 IN InFindID)
{
	UDMInventoryItemSlotDataObject* FoundDataObject = DataObjectList.FindRef(InFindID);
	return FoundDataObject;
}

void UDMUIInventorySlotList::SetFocusThis()
{
	const TArray<UUserWidget*> EntryWidgets = TileView_List->GetDisplayedEntryWidgets();
	if (EntryWidgets.IsValidIndex(0))
	{
		SetFocusableInputMode(true, EntryWidgets[0]);

		UDMUIInventoryItemSlot* CastedWidget = Cast<UDMUIInventoryItemSlot>(EntryWidgets[0]);
		if (CastedWidget)
			CastedWidget->SetFocusState(true);
	}
}

void UDMUIInventorySlotList::OnItemSelectionChanged(UObject* InObject)
{
	if (InObject == nullptr)
		return;

	UDMInventoryItemSlotDataObject* CastedDataObject = Cast<UDMInventoryItemSlotDataObject>(InObject);
	if (CastedDataObject == nullptr)
		return;

	int32 PrevSelectionSlotID = SelectionSlotID;
	SelectionSlotID = CastedDataObject->Data.ID;

	UDMInventoryItemSlotDataObject* PrevDataObject = GetDataObject(PrevSelectionSlotID);
	if (PrevDataObject)
	{
		UDMUIInventoryItemSlot* ItemSlot = TileView_List->GetEntryWidgetFromItem<UDMUIInventoryItemSlot>(PrevDataObject);
		if (ItemSlot)
		{
			ItemSlot->SetFocusState(false);
		}
	}
	UDMInventoryItemSlotDataObject* NewDataObject = GetDataObject(SelectionSlotID);
	if (NewDataObject)
	{
		UDMUIInventoryItemSlot* ItemSlot = TileView_List->GetEntryWidgetFromItem<UDMUIInventoryItemSlot>(NewDataObject);
		if (ItemSlot)
		{
			ItemSlot->SetFocusState(true);
		}
	}
}

void UDMUIInventorySlotList::OnItemScrolledIntoView(UObject* InObject, UUserWidget& InUserWidget)
{

}

void UDMUIInventorySlotList::OnItemClicked(UObject* InObject)
{
	if (InObject == nullptr)
		return;

}

void UDMUIInventorySlotList::OnItemHoveredChanged(UObject* InObject, bool InState)
{
	if (InObject == nullptr)
		return;

}
