// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include <MovieSceneSequencePlayer.h>

class UWRSequence;

class WR_API WRSequenceManager : public WRSingleton<WRSequenceManager>
{

public:
	WRSequenceManager() {}
	virtual ~WRSequenceManager() {}

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override;
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override;

	void OnFinished(const FString& InPath);
	void OnSkip();

	void RemoveSeqObject(const FString& InPath, bool bRemoveContainer = true);

	bool PlaySequence(const FString& InPath, const FMovieSceneSequencePlaybackSettings& InSettings = FMovieSceneSequencePlaybackSettings());
	void StopSequence(const FString& InPath);
	void SetPostTriggerIDs(const FString& InPath, const TArray<int32>& InPostTriggerIDs);

	TMap<FString, UWRSequence*>		SequenceObjects;
	
};
