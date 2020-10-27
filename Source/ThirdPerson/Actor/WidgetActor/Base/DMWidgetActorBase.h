// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../Util/DMEnumUtil.h"
#include "DMWidgetActorBase.generated.h"


class UDMWidgetComponentBase;
class UActorSequenceComponent;
class UActorSequencePlayer;


#define ACTOR_SEQUENCE_NONE_ID 0

#define BIND_WIDGET_ACTOR_COMPONENT(_EnumValue, _Name)																				\
{																																	\
	UDMWidgetComponentBase* FoundWidgetComponent = Cast<UDMWidgetComponentBase>(GetDefaultSubobjectByName(_Name));					\
	if (FoundWidgetComponent)																										\
	{																																\
		WidgetComponentList.Add((int32)_EnumValue, FoundWidgetComponent);															\
	}																																\
}

#define BIND_WIDGET_ACTOR_COMPONENT_IMPLEMENTATION(_EnumValue, _Name, _Implementation)												\
{																																	\
	UDMWidgetComponentBase* FoundWidgetComponent = Cast<UDMWidgetComponentBase>(GetDefaultSubobjectByName(_Name));					\
	if (FoundWidgetComponent)																										\
	{																																\
		WidgetComponentList.Add((int32)_EnumValue, FoundWidgetComponent);															\
		_Implementation																												\
	}																																\
}

#define BIND_WIDGET_ACTOR_SEQUENCE(_EnumValue, _Name)																				\
{																																	\
	UActorSequenceComponent* FoundActorSequenceComponent = Cast<UActorSequenceComponent>(GetDefaultSubobjectByName(_Name));			\
	if (FoundActorSequenceComponent)																								\
	{																																\
		ActorSequenceList.Add((int32)_EnumValue, FoundActorSequenceComponent);														\
		UActorSequencePlayer* SequencePlayer = FoundActorSequenceComponent->GetSequencePlayer();									\
		if (SequencePlayer != nullptr)																								\
		{																															\
 			SequencePlayer->OnFinished.AddDynamic(this, &ADMWidgetActorBase::OnActorSequenceFinished);								\
		}																															\
	}																																\
}


UCLASS()
class THIRDPERSON_API ADMWidgetActorBase : public AActor
{
	GENERATED_BODY()
	
private:
	int32 CurrentPlayingActorSequenceID = ACTOR_SEQUENCE_NONE_ID;

protected:
	TMap<int32, UDMWidgetComponentBase*>	WidgetComponentList;
	TMap<int32, UActorSequenceComponent*>	ActorSequenceList;

protected:
	virtual void BeginPlay();
	virtual void OnActorSequenceFinished_Implementation();
	UActorSequencePlayer* GetActorSequence(int32 InActorSequenceID);
	virtual FString GetActorSequenceTypeName(int32 InActorSequenceID);

	FORCEINLINE int32 GetCurrentPlayingActorSequenceID() { return CurrentPlayingActorSequenceID; }
	FORCEINLINE void SetCurrentPlayingActorSequenceID(const int32 IN InID) { CurrentPlayingActorSequenceID = InID; }

public:	
	// Called every frame
	virtual void Tick(float DeltaTime);
	virtual void OnAppear();
	virtual void OnDisappear();

	UFUNCTION(BlueprintCallable)
		void OnActorSequenceFinished();

	template<class T>
	T* GetWidget(int32 InType);
	template<class T>
	T* GetWidgetComponent(int32 InType);

	void PlayActorSequence(int32 InNewID);
	void StopActorSequence(int32 InNewID, const bool IN InJumpToEnd = false);
	void ScrubActorSequence(int32 InNewID);
	void ChangeActorSequence(int32 InNewID);
	bool IsPlayingActorSequence();
};


template<class T>
T* ADMWidgetActorBase::GetWidget(int32 InType)
{
	UDMWidgetComponentBase* FoundWidgetComponent = WidgetComponentList.FindRef(InType);
	if (FoundWidgetComponent == nullptr)
		return nullptr;

	UUserWidget* pWidet = FoundWidgetComponent->GetUserWidgetObject();
	if (pWidet == nullptr)
		return nullptr;

	return Cast<T>(pWidet);
}

template<class T>
T* ADMWidgetActorBase::GetWidgetComponent(int32 InType)
{
	UDMWidgetComponentBase* FoundWidgetComponent = WidgetComponentList.FindRef(InType);
	if (FoundWidgetComponent == nullptr)
		return nullptr;

	return Cast<T>(FoundWidgetComponent);
}
