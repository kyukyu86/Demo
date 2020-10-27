// Fill out your copyright notice in the Description page of Project Settings.


#include "DMInteractionComponent.h"
#include "Components/CapsuleComponent.h"
#include <DrawDebugHelpers.h>
#include <Components/SphereComponent.h>
#include <Net/UnrealNetwork.h>
#include "../../Manager/DMUIManager.h"
#include "../Character/Base/DMCharacterBase.h"
#include "../../Component/DMActorComponentInteraction.h"
#include "../../Manager/DMCharacterManager.h"
#include "../../Util/DMActorUtil.h"
#include "../Character/Player/DMCharacterMine.h"
#include "../../Util/DMEnumUtil.h"
#include "../IO/Skeletal/DMIOSkeletal.h"
#include "../../Util/DMUIUtil.h"



void UDMInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	this->RegisterComponent();
	DMActorUtil::SetComponentTickEnable(this, true);
	
	// Setup Input Event Index
	int32 nInputIndex = 0;
	for (auto& InputData : InputEventList)
	{
		InputData.InputEventIndex = nInputIndex;
		++nInputIndex;

		int32 nResultIndex = 0;
		for (auto& ResultData : InputData.ResultForm.ResultList)
		{
			ResultData.ResultIndex = nResultIndex;
			++nResultIndex;
		}
	}

	// Debug
	if (bDebugInteractionArea)
	{
		FVector ComponentLocation = this->GetComponentLocation();
		DrawDebugCircle(GetWorld(), ComponentLocation, InteractionDistance, 50, FColor::Green, true, 0.f, 0, 3, FVector(1, 0, 0), FVector(0, 1, 0));
	}
}

void UDMInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Release();

	Super::EndPlay(EndPlayReason);
}

void UDMInteractionComponent::Initialize(AActor* IN InParentActor)
{
	ParentActor = InParentActor;

	// Setup
//	SetupAttachment(InParentActor->GetRootComponent());
// 	this->OnComponentBeginOverlap.AddDynamic(this, &UDMInteractionComponent::OnBeginOverlap);
// 	this->OnComponentEndOverlap.AddDynamic(this, &UDMInteractionComponent::OnEndOverlap);
	this->SetCollisionProfileName(FName("Targeting"));
}

void UDMInteractionComponent::Release()
{
	if (DMUIManager::Get())
	{
		DMUIManager::Get()->CloseWidget(this);
	}
	this->UnregisterComponent();
	this->RemoveFromRoot();
}

void UDMInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ADMCharacterMine* pMine = DMCharacterManager::Get()->GetMyCharacter();
	if (DMActorUtil::IsInnerComponent(pMine, this, InteractionDistance))
	{
		if (bIsInteractionableDistance == false)
		{
			UDMActorComponentInteraction* ActorCompInteraction = pMine->GetComponent<UDMActorComponentInteraction>();
			if (ActorCompInteraction == nullptr)
				return;

			// Targeting Flag 없으면 바로 범위체크로 띄운다
			if (IsContainedFlag(EDMInteractionFunctionFlag::Targeting) == false)
			{
				OnTargeted();
			}
			ActorCompInteraction->AddInteractionableComponent(this);
			bIsInteractionableDistance = true;
		}
	}
	else
	{
		if (bIsInteractionableDistance)
		{
			UDMActorComponentInteraction* ActorCompInteraction = pMine->GetComponent<UDMActorComponentInteraction>();
			if (ActorCompInteraction == nullptr)
				return;

			// Targeting 별도로 없는경우 범위체크로 끈다
			if (IsContainedFlag(EDMInteractionFunctionFlag::Targeting) == false)
			{
				OnTargetReleased();
			}
			ActorCompInteraction->RemoveInteractionableComponent(this);
			bIsInteractionableDistance = false;
		}
	}
}

#if WITH_EDITOR
void UDMInteractionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == "InteractionDistance")
	{
		FlushPersistentDebugLines(GetWorld());
		DrawDebugCircle(GetWorld(), this->GetComponentLocation(), InteractionDistance, 50, FColor::Green, true, 0.f, 0, 3, FVector(1,0,0), FVector(0,1,0));
	}
	else if (PropertyChangedEvent.GetPropertyName() == "bDebugInteractionArea")
	{
		const FBoolProperty* BoolProperty = CastField<FBoolProperty>(PropertyChangedEvent.Property);
		if (BoolProperty)
		{
			bool ChangedValue = BoolProperty->GetPropertyValue_InContainer(this);
			if (ChangedValue)
			{
				FlushPersistentDebugLines(GetWorld());
				DrawDebugCircle(GetWorld(), this->GetComponentLocation(), InteractionDistance, 50, FColor::Green, true, 0.f, 0, 3, FVector(1, 0, 0), FVector(0, 1, 0));
			}
			else
			{
				FlushPersistentDebugLines(GetWorld());
			}
		}
	}
}
#endif//WITH_EDITOR

// void UDMInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	ADMCharacterBase* CastedCharacterBase = Cast<ADMCharacterBase>(Other);
// 	if (CastedCharacterBase == nullptr)
// 		return;
// 
// 	UDMActorComponentInteraction* ActorCompInteraction = CastedCharacterBase->GetComponent<UDMActorComponentInteraction>();
// 	if (ActorCompInteraction == nullptr)
// 		return;
// 
// 	ActorCompInteraction->AddInteractionableComponent(this);
// 
// 	// + Flag
// 	if (IsContainedFlag(EDMInteractionFunctionFlag::Widget))
// 	{
// 		if (WidgetClass != nullptr)
// 		{
// 			FDMOpenWidgetInfo OpenWidgetInfo(WidgetClass.Get()->GetPathName(), GetOwner()->GetActorTransform(), WidgetLocation, WidgetRotator, WidgetScale, true, EDMWidgetComponentFlag::Billboarrd, this);
// 			DMUIManager::Get()->OpenPanel(OpenWidgetInfo);
// 		}		
// 	}
// }
// 
// void UDMInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	ADMCharacterBase* CastedCharacterBase = Cast<ADMCharacterBase>(Other);
// 	if (CastedCharacterBase == nullptr)
// 		return;
// 
// 	UDMActorComponentInteraction* ActorCompInteraction = CastedCharacterBase->GetComponent<UDMActorComponentInteraction>();
// 	if (ActorCompInteraction == nullptr)
// 		return;
// 
// 	ActorCompInteraction->RemoveInteractionableComponent(nullptr);
// 
// 	// + Flag
// 	if (IsContainedFlag(EDMInteractionFunctionFlag::Widget))
// 	{
// 		DMUIManager::Get()->CloseWidget(this);
// 	}
// }

bool UDMInteractionComponent::IsContainedFlag(const EDMInteractionFunctionFlag IN InFlag)
{
	if (((EDMInteractionFunctionFlag)FunctionFlags & InFlag) != EDMInteractionFunctionFlag::None)
	{
		return true;
	}
	return false;
}

bool UDMInteractionComponent::IsContainedInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	for (auto InputData : InputEventList)
	{
		if (InputData.InputKey == InInputType
			&& InputData.InputEvent == InEventType)
		{
			return true;
		}
	}
	return false;
}

void UDMInteractionComponent::OnTargeted()
{
	if (IsContainedFlag(EDMInteractionFunctionFlag::Widget))
	{
		if (WidgetClass != nullptr)
		{
			FDMOpenWidgetInfo OpenWidgetInfo(WidgetClass.Get()->GetPathName(), GetOwner()->GetActorTransform(), WidgetLocation, WidgetRotator, WidgetScale, EDMWidgetType::Widget3D, EDMWidgetComponentFlag::Billboarrd, this);
			DMUIManager::Get()->OpenPanel(OpenWidgetInfo);
		}
	}
}

void UDMInteractionComponent::OnTargetReleased()
{
	if (IsContainedFlag(EDMInteractionFunctionFlag::Widget))
	{
		DMUIManager::Get()->CloseWidget(this);
	}
}

bool UDMInteractionComponent::OnInputEvent(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	if (IsContainedFlag(EDMInteractionFunctionFlag::Input) == false)
		return false;

	if (IsContainedInputEvent(InInputType, InEventType) == false)
		return false;

	if (IsContainedFlag(EDMInteractionFunctionFlag::Targeting))
	{
		// Targeting Flag 가 있으면 같은 대상일 때 입력처리
		if (DMCharacterManager::Get()->IsTargetedActor(GetParentActor()))
		{
			if (OnInputEventProcess(InInputType, InEventType))
				return true;
		}
	}
	else
	{
		// Targeting Flag 가 없으면 바로 입력처리
		if (OnInputEventProcess(InInputType, InEventType))
			return true;
	}

	return false;
}

bool UDMInteractionComponent::OnInputEventProcess(const EDMInput IN InInputType, const EInputEvent IN InEventType)
{
	for (auto InputData : InputEventList)
	{
		if (InputData.InputKey == InInputType
			&& InputData.InputEvent == InEventType)
		{
			if (InputData.ResultForm.IsDone())
			{
				// Debug
				FString strLog = FString::Printf(TEXT("Interaction Matched Input Event : exception is Done"));
				DMUIUtil::UEDebug(strLog);
				continue;
			}
			else if (InputData.IsDoing())
			{
				// Debug
				FString strLog = FString::Printf(TEXT("Interaction Matched Input Event : exception is Doing"));
				DMUIUtil::UEDebug(strLog);
				continue;
			}

			// Debug
			FString strLog = FString::Printf(TEXT("Interaction Matched Input Event !!"));
			DMUIUtil::UEDebug(strLog);


			InputData.SetDoing(true);
			CurrentInputEventIndex = InputData.InputEventIndex;

			// Result
			OnResultProcess(InputData.ResultForm);			

			return true;;
		}
	}

	return false;
}

void UDMInteractionComponent::OnResultProcess(FDMInteractionResultForm& IN InResultForm)
{
	// + Animation
	if (InResultForm.ResultAnimAsset.IsNull() == false)
	{
		if (GetOwner()->IsA(ADMIOSkeletal::StaticClass()))
		{
			ADMIOSkeletal* CastedOwner = Cast<ADMIOSkeletal>(GetOwner());
			CastedOwner->PlayAnimationSequence(InResultForm.ResultAnimAsset.LoadSynchronous());
		}
	}

	for (auto& EachResult : InResultForm.ResultList)
	{
		FDMInteractionResult& RefResult = EachResult;
		
		if (RefResult.ResultDelayTime != 0)
		{
			// Debug
			FString strLog = FString::Printf(TEXT("Start Result Delay : %d"), RefResult.ResultDelayTime);
			DMUIUtil::UEDebug(strLog);

			RefResult.TimerHandle = DMActorUtil::SetTimer(FTimerDelegate::CreateUObject(this, &UDMInteractionComponent::OnResultProcess_Implementation, RefResult.ResultIndex, true), RefResult.ResultDelayTime, false);
		}
		else
		{
			OnResultProcess_Implementation(RefResult.ResultIndex);
		}
	}
}

void UDMInteractionComponent::OnResultProcess_Implementation(const int32 InResultIndex, const bool InIsTimerFinished /*= false */)
{
	FDMInteractionResult ResultData;
	if (GetResultData(InResultIndex, ResultData) == false)
		return;

	if (InIsTimerFinished)
	{
		// Debug
		FString strLog = FString::Printf(TEXT("Finished Result Delay : %s, %d"), *DMEnumUtil::EnumToString<EDMInteractionResult>("EDMInteractionResult", ResultData.ResultType), ResultData.ResultValue);
		DMUIUtil::UEDebug(strLog);
	}

	switch (ResultData.ResultType)
	{
	case EDMInteractionResult::DropItem:
	{

	}
	break;

	case EDMInteractionResult::OpenWidget:
	{

	}
	break;
	}

	SetCurrentInputResultDone(InResultIndex, true);

	DetermineResultAfter();
}

void UDMInteractionComponent::DetermineResultAfter()
{
	if (InputEventList.IsValidIndex(CurrentInputEventIndex) == false)
		return;

	FDMInteractionInput& InputEvent = InputEventList[CurrentInputEventIndex];
	for (auto& EachResult : InputEvent.ResultForm.ResultList)
	{
		FDMInteractionResult& RefResult = EachResult;
		if (RefResult.IsDone() == false)
			return;
	}

	// All Done
	InputEvent.ResultForm.AddInteracitonCount();

	// Reset Current Data
	CurrentInputEventIndex = DEF_INTERACTION_INPUT_INVALID_INDEX;
	InputEvent.SetDoing(false);

	// Max Count
	if (InputEvent.ResultForm.IsDone() == false)
	{
		// Debug
		FString strLog = FString::Printf(TEXT("Interactionable Count Remaning : %d / %d"), InputEvent.ResultForm.InteractionCount, InputEvent.ResultForm.InteractionableCount);
		DMUIUtil::UEDebug(strLog);

		return;
	}

	switch (InputEvent.ResultForm.ResultAfter)
	{
	case EDMInteractionResultAfter::Remain:
	{

	}
	break;

	case EDMInteractionResultAfter::Delete:
	{

	}
	break;
	}

	// Debug
	FString strLog = FString::Printf(TEXT("Result All Done"));
	DMUIUtil::UEDebug(strLog);
}

bool UDMInteractionComponent::SetCurrentInputResultDone(const int32 IN InResultIndex, const bool IN InSet)
{
	if (InputEventList.IsValidIndex(CurrentInputEventIndex) == false)
		return false;

	FDMInteractionInput& InputEvent = InputEventList[CurrentInputEventIndex];
	if (InputEvent.ResultForm.ResultList.IsValidIndex(InResultIndex) == false)
		return false;

	FDMInteractionResult& ResultData = InputEvent.ResultForm.ResultList[InResultIndex];
	ResultData.SetDone(InSet);
	return true;
}

bool UDMInteractionComponent::GetResultData(const int32 IN InResultIndex, FDMInteractionResult& OUT OutResultData)
{
	if (InputEventList.IsValidIndex(CurrentInputEventIndex) == false)
		return false;

	FDMInteractionInput& InputEvent = InputEventList[CurrentInputEventIndex];
	if (InputEvent.ResultForm.ResultList.IsValidIndex(InResultIndex) == false)
		return false;

	OutResultData = InputEvent.ResultForm.ResultList[InResultIndex];
	return true;
}
