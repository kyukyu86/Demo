// Fill out your copyright notice in the Description page of Project Settings.

#include "WRSequence.h"
#include <LevelSequence.h>
#include "Core/WRGameInstance.h"
#include <MovieSceneSequencePlayer.h>
#include <LevelSequencePlayer.h>
#include <LevelSequenceActor.h>
#include "Manager/WRTriggerManager.h"
#include "Manager/WRSequenceManager.h"

// Sets default values
UWRSequence::UWRSequence()
{
	
}

UWRSequence::~UWRSequence()
{
}

void UWRSequence::Init(const FString& InPath, const FMovieSceneSequencePlaybackSettings& InSettings/* = FMovieSceneSequencePlaybackSettings()*/)
{
	if (SequenceActor == nullptr)
	{
		Path = InPath;
		ULevelSequence* LevelSequence = LoadObject<ULevelSequence>(nullptr, *(InPath));

		UWorld* world = UWRGameInstance::GetGameInstance()->GetWorld();
		ALevelSequenceActor* ResultActor = nullptr;

		ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(world, LevelSequence, InSettings, SequenceActor);

		if (SequenceActor && SequencePlayer)
		{
			SequencePlayer->OnFinished.AddDynamic(this, &UWRSequence::OnFinished);
			SequencePlayer->OnPause.AddDynamic(this, &UWRSequence::OnPause);
			SequencePlayer->OnPlay.AddDynamic(this, &UWRSequence::OnPlay);
			SequencePlayer->OnStop.AddDynamic(this, &UWRSequence::OnStop);
		}
	}
}

void UWRSequence::Clear()
{
	if (SequenceActor != nullptr)
	{
		ULevelSequencePlayer* SequencePlayer = SequenceActor->SequencePlayer;
		if (SequencePlayer != nullptr)
		{
			SequencePlayer->OnFinished.Clear();
			SequencePlayer->OnPause.Clear();
			SequencePlayer->OnPlay.Clear();
			SequencePlayer->OnStop.Clear();
		}
		SequenceActor->Destroy();
		SequenceActor = nullptr;
	}

	PostTriggerIDs.Empty();
}

bool UWRSequence::PlaySequence()
{
	if (SequenceActor && SequenceActor->SequencePlayer)
	{
		SequenceActor->SequencePlayer->Play();
		//ULevelSequence* LevelSequence;
		//LevelSequence->Setstart
	}

	return true;
}

void UWRSequence::StopSequence()
{
	if (SequenceActor && SequenceActor->SequencePlayer)
	{
		SequenceActor->SequencePlayer->Stop();
	}
}

void UWRSequence::OnPlay()
{

}

void UWRSequence::OnStop()
{

}

void UWRSequence::OnFinished()
{
	RunActiveTrigger();
	WRSequenceManager::Get()->OnFinished(Path);
}

void UWRSequence::OnPause()
{

}

void UWRSequence::RunActiveTrigger()
{
	for (int i = 0; i < PostTriggerIDs.Num(); ++i)
	{
		WRTriggerManager::Get()->RunActiveTrigger(PostTriggerIDs[i]);
	}
}


