// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRSequenceManager.h"

#include "Sequence/WRSequence.h"


void WRSequenceManager::OnInitialize()
{
	
}

void WRSequenceManager::OnShutdown()
{
	for (auto iter = SequenceObjects.CreateIterator(); iter; ++iter)
	{
		RemoveSeqObject(iter.Key(), false);
	}
	SequenceObjects.Empty();

}

void WRSequenceManager::OnLoadLevelComplete()
{

}

void WRSequenceManager::OnLoadLevelStart()
{
	for (auto iter = SequenceObjects.CreateIterator(); iter; ++iter)
	{
		RemoveSeqObject(iter.Key(), false);
	}

	SequenceObjects.Empty();
}

void WRSequenceManager::OnRenderStart()
{

}

void WRSequenceManager::OnFinished(const FString& InPath)
{
	RemoveSeqObject(InPath);
}

void WRSequenceManager::OnSkip()
{

}

void WRSequenceManager::RemoveSeqObject(const FString& InPath, bool bRemoveContainer /*= true*/)
{
	if (SequenceObjects.Contains(InPath) == true)
	{
		SequenceObjects[InPath]->Clear();
		SequenceObjects[InPath]->RemoveFromRoot();

		if(bRemoveContainer == true)
			SequenceObjects.Remove(InPath);
	}
}

bool WRSequenceManager::PlaySequence(const FString& InPath, const  FMovieSceneSequencePlaybackSettings& InSettings/* = FMovieSceneSequencePlaybackSettings()*/)
{
	if (SequenceObjects.Contains(InPath) == false)
	{
		UWRSequence* SequenceObject = NewObject<UWRSequence>();
		SequenceObject->Init(InPath, InSettings);
		SequenceObject->AddToRoot();
		SequenceObjects.Add(InPath, SequenceObject);
		
	}
	
	SequenceObjects[InPath]->PlaySequence();
	//SequencePlayer->OnFinished.AddDynamic()

	return false;
}

void WRSequenceManager::StopSequence(const FString& InPath)
{
	if (SequenceObjects.Contains(InPath) == true)
	{
		SequenceObjects[InPath]->StopSequence();
	}
}

void WRSequenceManager::SetPostTriggerIDs(const FString& InPath, const TArray<int32>& InPostTriggerIDs)
{
	if (SequenceObjects.Contains(InPath) == true)
	{
		SequenceObjects[InPath]->PostTriggerIDs = InPostTriggerIDs;
	}
}

