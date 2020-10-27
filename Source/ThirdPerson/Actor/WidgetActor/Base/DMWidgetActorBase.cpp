// Fill out your copyright notice in the Description page of Project Settings.


#include "DMWidgetActorBase.h"
#include "../Plugins/MovieScene/ActorSequence/Source/ActorSequence/Public/ActorSequenceComponent.h"
#include "../Plugins/MovieScene/ActorSequence/Source/ActorSequence/Public/ActorSequencePlayer.h"
#include "../../../Util/DMUIUtil.h"


void ADMWidgetActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADMWidgetActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UActorSequencePlayer* ADMWidgetActorBase::GetActorSequence(int32 InActorSequenceID)
{
	UActorSequenceComponent* FoundActorSequenceComponent = ActorSequenceList.FindRef(InActorSequenceID);
	if (FoundActorSequenceComponent == nullptr)
		return nullptr;

	UActorSequencePlayer* SequencePlayer = FoundActorSequenceComponent->GetSequencePlayer();
	if (SequencePlayer == nullptr)
		return nullptr;

	return SequencePlayer;
}

FString ADMWidgetActorBase::GetActorSequenceTypeName(int32 InActorSequenceID)
{
	return FString::FormatAsNumber(InActorSequenceID);
}

void ADMWidgetActorBase::OnAppear()
{

}

void ADMWidgetActorBase::OnDisappear()
{
	this->RemoveFromRoot();
	this->Destroy();
}

void ADMWidgetActorBase::OnActorSequenceFinished()
{
	OnActorSequenceFinished_Implementation();
}

void ADMWidgetActorBase::OnActorSequenceFinished_Implementation()
{
	
}

void ADMWidgetActorBase::PlayActorSequence(int32 InNewID)
{
 	if (GetCurrentPlayingActorSequenceID() != ACTOR_SEQUENCE_NONE_ID)
 	{
 		// Debug
 		FString strLog = FString::Printf(TEXT("Already Play Sequence : %s"), *GetActorSequenceTypeName(InNewID));
 		DMUIUtil::UEDebug(strLog);
 		return;
 	}

	UActorSequencePlayer* SequencePlayer = GetActorSequence(InNewID);
	if (SequencePlayer == nullptr)
		return;

	SequencePlayer->Play();

	SetCurrentPlayingActorSequenceID(InNewID);

	// Debug
	FString strLog = FString::Printf(TEXT("Play : %s"), *GetActorSequenceTypeName(InNewID));
	DMUIUtil::UEDebug(strLog);
}

void ADMWidgetActorBase::StopActorSequence(int32 InNewID, const bool IN InJumpToEnd /*= false*/)
{
 	UActorSequencePlayer* SequencePlayer = GetActorSequence(InNewID);
 	if (SequencePlayer == nullptr)
 		return;
 
 	if (InJumpToEnd)
 	{
 		SequencePlayer->GoToEndAndStop();
 	}
 	else
 	{
 		SequencePlayer->Stop();
 	}
 
	SetCurrentPlayingActorSequenceID(ACTOR_SEQUENCE_NONE_ID);
 
	// Debug
 	FString strLog = FString::Printf(TEXT("Stop : %s"), *GetActorSequenceTypeName(InNewID));
 	DMUIUtil::UEDebug(strLog);
}

void ADMWidgetActorBase::ScrubActorSequence(int32 InNewID)
{
	UActorSequencePlayer* SequencePlayer = GetActorSequence(InNewID);
	if (SequencePlayer == nullptr)
		return;

	SequencePlayer->Scrub();

	SetCurrentPlayingActorSequenceID(ACTOR_SEQUENCE_NONE_ID);

	FString strLog = FString::Printf(TEXT("Scrub : %s"), *GetActorSequenceTypeName(InNewID));
	DMUIUtil::UEDebug(strLog);
}

void ADMWidgetActorBase::ChangeActorSequence(int32 InNewID)
{
	int32 PrevSeqID = GetCurrentPlayingActorSequenceID();

	if (GetCurrentPlayingActorSequenceID() != InNewID)
	{
		UActorSequencePlayer* SequencePlayer = GetActorSequence(PrevSeqID);
		if (SequencePlayer)
		{
			SequencePlayer->Stop();

			// Debug
			FString strLog = FString::Printf(TEXT("Change(Stop) : %s"), *GetActorSequenceTypeName(InNewID));
			DMUIUtil::UEDebug(strLog);
		}
	}
	SetCurrentPlayingActorSequenceID(ACTOR_SEQUENCE_NONE_ID);

	PlayActorSequence(InNewID);
}

bool ADMWidgetActorBase::IsPlayingActorSequence()
{
	if (GetCurrentPlayingActorSequenceID() != ACTOR_SEQUENCE_NONE_ID)
		return true;

	return false;
}

