// Fill out your copyright notice in the Description page of Project Settings.


#include "DMWidgetActorInventory.h"
#include <Blueprint/UserWidget.h>
#include "../../UI/WidgetComponent/DMWidgetComponentBase.h"

// Sets default values
ADMWidgetActorInventory::ADMWidgetActorInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADMWidgetActorInventory::BeginPlay()
{
	Super::BeginPlay();
	
	auto BIND_OBJECT = ([=](EDMWidgetActorInventory InType, FName InName)
	{
		UDMWidgetComponentBase* FoundWidgetComponent = Cast<UDMWidgetComponentBase>(GetDefaultSubobjectByName(InName));
		if (FoundWidgetComponent)
		{
			WidgetComponentList.Add(InType, FoundWidgetComponent);
		}
	});
	BIND_OBJECT(EDMWidgetActorInventory::Preview, FName("DMWidgetComponentPreview"));
	BIND_OBJECT(EDMWidgetActorInventory::SlotList, FName("DMWidgetComponentSlotList"));
	BIND_OBJECT(EDMWidgetActorInventory::SimpleInfo, FName("DMWidgetComponentSimpleInfo"));
	BIND_OBJECT(EDMWidgetActorInventory::DetailInfo, FName("DMWidgetComponentDetailInfo"));
	BIND_OBJECT(EDMWidgetActorInventory::Category, FName("DMWidgetComponentCategory"));
	BIND_OBJECT(EDMWidgetActorInventory::Bottom, FName("DMWidgetComponentBottom"));
}

// Called every frame
void ADMWidgetActorInventory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}