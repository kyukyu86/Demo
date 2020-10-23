// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DMWidgetActorInventory.generated.h"


enum class EDMWidgetActorInventory
{
	Preview,
	SlotList,
	SimpleInfo,
	DetailInfo,
	Category,
	Bottom,
};

class UDMWidgetComponentBase;

UCLASS()
class THIRDPERSON_API ADMWidgetActorInventory : public AActor
{
	GENERATED_BODY()
	
private:
	TMap<EDMWidgetActorInventory, UDMWidgetComponentBase*> WidgetComponentList;

public:	
	ADMWidgetActorInventory();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	template<class T>
	T* GetWidget(const EDMWidgetActorInventory IN InWidgetType);

};

template<class T>
T* ADMWidgetActorInventory::GetWidget(const EDMWidgetActorInventory IN InWidgetType)
{
	UDMWidgetComponentBase* FoundWidgetComponent = WidgetComponentList.FindRef(InWidgetType);
	if (FoundWidgetComponent == nullptr)
		return nullptr;

	UUserWidget* pWidet = FoundWidgetComponent->GetUserWidgetObject();
	if (pWidet == nullptr)
		return nullptr;

	return Cast<T>(pWidet);
}
