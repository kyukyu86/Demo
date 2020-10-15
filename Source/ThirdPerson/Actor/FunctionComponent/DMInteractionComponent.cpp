// Fill out your copyright notice in the Description page of Project Settings.


#include "DMInteractionComponent.h"
#include "Components/CapsuleComponent.h"
#include <DrawDebugHelpers.h>
#include <Components/SphereComponent.h>
#include <Net/UnrealNetwork.h>
#include "../../Manager/DMUIManager.h"



void UDMInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UDMInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Release();

	Super::EndPlay(EndPlayReason);
}

void UDMInteractionComponent::Initialize(AActor* IN InParentActor)
{
	// Setup
	SetupAttachment(InParentActor->GetRootComponent());
	this->OnComponentBeginOverlap.AddDynamic(this, &UDMInteractionComponent::OnBeginOverlap);
	this->OnComponentEndOverlap.AddDynamic(this, &UDMInteractionComponent::OnEndOverlap);
}

void UDMInteractionComponent::Release()
{
	if (DMUIManager::Get())
	{
		DMUIManager::Get()->CloseWidget(InteractionWidget);
		InteractionWidget = nullptr;
	}
}

void UDMInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FDMOpenWidgetInfo OpenWidgetInfo(WidgetPath, GetOwner()->GetActorTransform(), WidgetOffsetTransform, true, EDMWidgetComponentFlag::Billboarrd);
	OpenWidgetInfo.CompleteDelegate.BindUObject(this, &UDMInteractionComponent::OnOpenCompletedWidget);
	strAsyncKey = DMUIManager::Get()->OpenPanel(OpenWidgetInfo);
}

void UDMInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	DMUIManager::Get()->CloseWidget(InteractionWidget);
}

void UDMInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDMInteractionComponent::OnOpenCompletedWidget(UObject* InObject, FString InAsyncKey)
{
	strAsyncKey.Empty();
	InteractionWidget = Cast<UUserWidget>(InObject);
}