// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"

struct TheaterMovieInfo
{
public:
	TheaterMovieInfo()
		: MediaPlayer(nullptr)
		, SeekTime(0.f)
		, Tid(0)
	{

	}
	class UMediaPlayer*	MediaPlayer;
	float				SeekTime;
	int32				Tid;
};

class WR_API WRMediaPlayerManager : public WRSingleton<WRMediaPlayerManager>
{

public:
	WRMediaPlayerManager() {}
	virtual ~WRMediaPlayerManager() {}

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override;
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override;

	void PlayTheaterMovie(int32 MoiveTid);
	void StopTheaterMovie(int32 MoiveTid);

	void OnMediaOpened(FString OpenedURL);

private:
	TMap<int32, TMap<FString, TheaterMovieInfo>>		TheaterMovieInfos;

	TArray<int32> Sounds;
};
