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
	BIND_WIDGET_ACTOR_SEQUENCE(EDMWidgetActorInventorySequence::ShowOnDetail, FName("SequenceOnDetail"));
	BIND_WIDGET_ACTOR_SEQUENCE(EDMWidgetActorInventorySequence::ShowOffDetail, FName("SequenceOffDetail"));

	OnAppear();
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
