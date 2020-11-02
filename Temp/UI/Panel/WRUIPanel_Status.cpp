// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Status.h"
#include "UI/Slot/WRUISlot_Test.h"

void UWRUIPanel_Status::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBoxUtil.Create(1, 10, this, EWRUISlotKind::Test, "WRUIScrollBox_Test", "");
	ScrollBoxUtil.SetRefreshAllContentsCallbackFunc(this, &RefreshCallback);
}

void UWRUIPanel_Status::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ScrollBoxUtil.Tick(InDeltaTime, 100);
}

void UWRUIPanel_Status::RefreshCallback(void* _Ptr)
{
	UWRUIPanel_Status* CastedPanel = static_cast<UWRUIPanel_Status*>(_Ptr);
	if (CastedPanel)
		CastedPanel->UpdateScrollBox();
}

void UWRUIPanel_Status::UpdateScrollBox()
{
	for (int32 Count = 0; Count < 100; ++Count)
	{
		UUserWidget* SlotWidget = ScrollBoxUtil.GetScrollWidget(Count);
		if(SlotWidget == nullptr)
			continue;

		UWRUISlot_Test* Slot_Test = Cast<UWRUISlot_Test>(SlotWidget);
		if(Slot_Test == nullptr)
			continue;

		Slot_Test->SetNum(Count);
	}
	
}
