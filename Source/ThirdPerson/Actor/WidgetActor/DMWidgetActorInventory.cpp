// Fill out your copyright notice in the Description page of Project Settings.


#include "DMWidgetActorInventory.h"
#include <Blueprint/UserWidget.h>
#include "../../UI/WidgetComponent/DMWidgetComponentBase.h"
#include "../Plugins/MovieScene/ActorSequence/Source/ActorSequence/Public/ActorSequenceComponent.h"
#include "../Plugins/MovieScene/ActorSequence/Source/ActorSequence/Public/ActorSequencePlayer.h"
#include "../../Util/DMEnumUtil.h"
#include "../../Util/DMUIUtil.h"
#include "../../GameInstance/DMGameInstance.h"
#include <Blueprint/WidgetBlueprintLibrary.h>
#include "../../Manager/DMUIManager.h"
#include "../../UI/Slot/Inventory/DMUIInventoryDetailInfo.h"
#include "../../UI/Slot/Inventory/DMUIInventorySlotList.h"

ADMWidgetActorInventory::ADMWidgetActorInventory()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADMWidgetActorInventory::BeginPlay()
{
	Super::BeginPlay();
	
	// Collect Widget Component
	BIND_WIDGET_ACTOR_COMPONENT(EDMWidgetActorInventory::Preview, FName("DMWidgetComponentPreview"));
	BIND_WIDGET_ACTOR_COMPONENT(EDMWidgetActorInventory::SlotList, FName("DMWidgetComponentSlotList"));
	BIND_WIDGET_ACTOR_COMPONENT(EDMWidgetActorInventory::SimpleInfo, FName("DMWidgetComponentSimpleInfo"));
	BIND_WIDGET_ACTOR_COMPONENT(EDMWidgetActorInventory::DetailInfo, FName("DMWidgetComponentDetailInfo"));
	BIND_WIDGET_ACTOR_COMPONENT(EDMWidgetActorInventory::Category, FName("DMWidgetComponentCategory"));
	BIND_WIDGET_ACTOR_COMPONENT(EDMWidgetActorInventory::Bottom, FName("DMWidgetComponentBottom"));


	// Collect Actor Sequence
	BIND_WIDGET_ACTOR_SEQUENCE(EDMWidgetActorInventorySequence::Appear, FName("SequenceAppear"));
	BIND_WIDGET_ACTOR_SEQUENCE(EDMWidgetActorInventorySequence::Disappear, FName("SequenceDisappear"));
	BIND_WIDGET_ACTOR_SEQUENCE(EDMWidgetActorInventorySequence::ShowOnDetail, FName("SequenceShowOnDetail"));
	BIND_WIDGET_ACTOR_SEQUENCE(EDMWidgetActorInventorySequence::ShowOffDetail, FName("SequenceShowOffDetail"));

	OnAppear();
}

FReply ADMWidgetActorInventory::OnKeyEvent(const int32 IN InComponentID, FKey IN InKeyEvent)
{	
	if (IsPlayingActorSequence())
	{
		return FReply::Handled();
	}

	switch ((EDMWidgetActorInventory)InComponentID)
	{
	case EDMWidgetActorInventory::SlotList:
	{
		if (InKeyEvent == EKeys::Enter)
		{
			UDMWidgetComponentBase* DetailInfoWidgetComponent = GetWidgetComponent<UDMWidgetComponentBase>((int32)EDMWidgetActorInventory::DetailInfo);
			if (DetailInfoWidgetComponent)
			{
				PlayActorSequence((int32)EDMWidgetActorInventorySequence::ShowOnDetail);
				return FReply::Handled();
			}
		}
	}
	break;

	case EDMWidgetActorInventory::DetailInfo:
	{
		if (InKeyEvent == EKeys::Enter)
		{

		}
		else if (InKeyEvent == EKeys::One)
		{
			UDMWidgetComponentBase* DetailInfoWidgetComponent = GetWidgetComponent<UDMWidgetComponentBase>((int32)EDMWidgetActorInventory::DetailInfo);
			if (DetailInfoWidgetComponent)
			{
				PlayActorSequence((int32)EDMWidgetActorInventorySequence::ShowOffDetail);
				return FReply::Handled();
			}
		}
	}
	break;
	}
	return FReply::Unhandled();
}

void ADMWidgetActorInventory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADMWidgetActorInventory::OnAppear()
{
	PlayActorSequence((int32)EDMWidgetActorInventorySequence::Appear);
}

void ADMWidgetActorInventory::OnDisappear()
{
	ChangeActorSequence((int32)EDMWidgetActorInventorySequence::Disappear);

	// Super::OnDisappear는 연출 후(OnActorSequenceFinished_Implementation) 호출됨
}

void ADMWidgetActorInventory::OnActorSequenceFinished_Implementation()
{
	// Debug
	FString strLog = FString::Printf(TEXT("Finished Actor Sequence : %s"), *GetCurrentPlayingActorSequenceTypeName());
	DMUIUtil::UEDebug(strLog);

	switch ((EDMWidgetActorInventorySequence)GetCurrentPlayingActorSequenceID())
	{
	case EDMWidgetActorInventorySequence::Disappear:
	{
		DMUIManager::Get()->RemoveDisappearProgressList(EDMPanelKind::Inventory);
		ADMWidgetActorBase::OnDisappear();
	}
	break;
	case EDMWidgetActorInventorySequence::ShowOnDetail:
	{
		UDMUIInventoryDetailInfo* DetailInfoWidget = GetWidget<UDMUIInventoryDetailInfo>((int32)EDMWidgetActorInventory::DetailInfo);
		if (DetailInfoWidget)
		{
			DetailInfoWidget->SetFocusThis();
		}
	}
	break;
	case EDMWidgetActorInventorySequence::Appear:
	case EDMWidgetActorInventorySequence::ShowOffDetail:
	{
		UDMUIInventorySlotList* SlotListWidget = GetWidget<UDMUIInventorySlotList>((int32)EDMWidgetActorInventory::SlotList);
		if (SlotListWidget)
		{
			SlotListWidget->SetFocusThis();
		}
	}
	break;
	}

	SetCurrentPlayingActorSequenceID(ACTOR_SEQUENCE_NONE_ID);
}

FString ADMWidgetActorInventory::GetActorSequenceTypeName(int32 InType)
{
	return DMEnumUtil::EnumToString<EDMWidgetActorInventorySequence>("EDMWidgetActorInventorySequence", (EDMWidgetActorInventorySequence)InType);
}

FString ADMWidgetActorInventory::GetCurrentPlayingActorSequenceTypeName()
{
	return DMEnumUtil::EnumToString<EDMWidgetActorInventorySequence>("EDMWidgetActorInventorySequence", (EDMWidgetActorInventorySequence)GetCurrentPlayingActorSequenceID());
}
