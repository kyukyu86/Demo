// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/DMWidgetActorBase.h"
#include "DMWidgetActorInventory.generated.h"


UENUM(BlueprintType)
enum class EDMWidgetActorInventory : uint8
{
	Preview,
	SlotList,
	SimpleInfo,
	DetailInfo,
	Category,
	Bottom,
};

UENUM(BlueprintType)
enum class EDMWidgetActorInventorySequence : uint8
{
	None,
	Appear,
	Disappear,
	ShowOnDetail,
	ShowOffDetail,
};

class UDMWidgetComponentBase;
class UActorSequenceComponent;
class UActorSequencePlayer;

UCLASS()
class THIRDPERSON_API ADMWidgetActorInventory : public ADMWidgetActorBase
{
	GENERATED_BODY()
	
public:	
	ADMWidgetActorInventory();

protected:
	virtual void BeginPlay() override;
	virtual void OnActorSequenceFinished_Implementation() override;
	virtual FString GetActorSequenceTypeName(int32 InType) override;
	FString GetCurrentPlayingActorSequenceTypeName();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void OnAppear();
	virtual void OnDisappear();

};