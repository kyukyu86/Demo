// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRMediaPlayerManager.h"
#include "WRMaterialParameterCollectionManager.h"
#include "WRSequenceManager.h"
#include "WRSFXManager.h"
#include "WRPathManager.h"
#include "WRTimeManager.h"
#include "WRCharacterManager.h"

#include "Table/Base/WRTableManager.h"
#include "Table/WRMovieTable.h"

#include "Actor/Character/WRCharacterMine.h"

#include <MediaPlaylist.h>
#include <MediaSource.h>
#include <MediaPlayer.h>
#include <WeakObjectPtr.h>
#include <LevelSequence.h>

void WRMediaPlayerManager::OnInitialize()
{
	
}

void WRMediaPlayerManager::OnShutdown()
{
	TheaterMovieInfos.Empty();
	Sounds.Empty();
}

void WRMediaPlayerManager::OnLoadLevelComplete()
{

}

void WRMediaPlayerManager::OnLoadLevelStart()
{
	TArray<int32> Tids;
	for (auto iter = TheaterMovieInfos.CreateIterator(); iter; ++iter)
	{
		Tids.Add(iter.Key());
	}
	
	for (int i = 0; i < Tids.Num(); ++i)
	{
		StopTheaterMovie(Tids[i]);
	}
}

void WRMediaPlayerManager::OnRenderStart()
{

}

void WRMediaPlayerManager::PlayTheaterMovie(int32 MoiveTid)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return;

	if (TheaterMovieInfos.Contains(MoiveTid) == true)
		return;

	FWRMovieTable* MovieTable = WRTableManager::Get()->FindRow<FWRMovieTable>(MoiveTid);
	UMediaPlaylist* MediaPlayList = NewObject<UMediaPlaylist>();
	if (MovieTable != nullptr && MediaPlayList->IsValidLowLevel())
	{
		float MoviesTotalTime = 0.f;
		TArray<float> RunTimes;
		UMediaSource* MediaSource = LoadObject<UMediaSource>(nullptr, *MovieTable->MediaSource);
	
		if (MediaSource->IsValidLowLevel() == true)
		{
			MoviesTotalTime = MovieTable->RunTime;
			RunTimes.Add(MovieTable->RunTime);
			MediaPlayList->Add(MediaSource);

			TheaterMovieInfo MovieInfo;
			MovieInfo.Tid = MoiveTid;
			TMap<FString, TheaterMovieInfo> MovieInfoMap;
			MovieInfoMap.Add(MediaSource->GetUrl(), MovieInfo);
			TheaterMovieInfos.Add(MoiveTid, MovieInfoMap);
		}

		int NextMovieTid = MoiveTid + 1;
		while(true)
		{
			FWRMovieTable* NextMovieTable = WRTableManager::Get()->FindRow<FWRMovieTable>(NextMovieTid);
			if (NextMovieTable)
			{
				if (MovieTable->PanelID == NextMovieTable->PanelID)
				{
					UMediaSource* NextMediaSource = LoadObject<UMediaSource>(nullptr, *NextMovieTable->MediaSource);
					if (NextMediaSource->IsValidLowLevel() == true)
					{
						MoviesTotalTime += NextMovieTable->RunTime;
						RunTimes.Add(NextMovieTable->RunTime);
						MediaPlayList->Add(NextMediaSource);

						TheaterMovieInfo MovieInfo;
						MovieInfo.Tid = NextMovieTid;
						if (TheaterMovieInfos.Contains(MoiveTid) == true)
						{
							TheaterMovieInfos[MoiveTid].Add(NextMediaSource->GetUrl(), MovieInfo);
						}
						else
						{
							TMap<FString, TheaterMovieInfo> MovieInfoMap;
							MovieInfoMap.Add(NextMediaSource->GetUrl(), MovieInfo);
							TheaterMovieInfos.Add(MoiveTid, MovieInfoMap);
						}
					}
				}
				else
					break;
			}
			else
				break;

			++NextMovieTid;
		}

		FString MediaPlayerPath = WRPathManager::Get()->GetPath(EWRPath::MediaPlayer) + FString::Printf(TEXT("MoviePlayer%d"), MovieTable->PanelID);
		UMediaPlayer* MediaPlayer = LoadObject<UMediaPlayer>(NULL, *MediaPlayerPath);

		if (MediaPlayer->IsValidLowLevel() == true)
		{
			float MapOpenElapsedTime = WRTimeManager::Get()->GetMapOpenElapsedTime().GetTotalSeconds();
			float CalculateTime = MapOpenElapsedTime;
			if (MapOpenElapsedTime >= MoviesTotalTime)
			{
				CalculateTime = (int)MapOpenElapsedTime % (int)MoviesTotalTime;
			}

			int Index = 0;
			for (; Index < RunTimes.Num(); ++Index)
			{
				if (CalculateTime > RunTimes[Index])
				{
					CalculateTime -= RunTimes[Index];
				}
				else
				{
					break;
				}
			}
			
			FString URL = MediaPlayList->Get(Index)->GetUrl();

			if (TheaterMovieInfos.Contains(MoiveTid) == true)
			{
				if (TheaterMovieInfos[MoiveTid].Contains(URL) == true)
				{
					TheaterMovieInfos[MoiveTid][URL].MediaPlayer = MediaPlayer;
					TheaterMovieInfos[MoiveTid][URL].SeekTime = CalculateTime;
				}
			}
			
			MediaPlayer->OpenPlaylistIndex(MediaPlayList, Index);
			MediaPlayer->OnMediaOpened.AddDynamic(MyChar, &AWRCharacterMine::OnMediaOpened);
		}
	}
}

void WRMediaPlayerManager::StopTheaterMovie(int32 MoiveTid)
{
	FWRMovieTable* MovieTable = WRTableManager::Get()->FindRow<FWRMovieTable>(MoiveTid);
	if (MovieTable)
	{
		FString MediaPlayerPath = WRPathManager::Get()->GetPath(EWRPath::MediaPlayer) + FString::Printf(TEXT("MoviePlayer%d"), MovieTable->PanelID);
		UMediaPlayer* MediaPlayer = LoadObject<UMediaPlayer>(NULL, *MediaPlayerPath);
		if (MediaPlayer->IsValidLowLevel())
		{
			if (TheaterMovieInfos.Contains(MoiveTid) == true)
			{
				TheaterMovieInfos.Remove(MoiveTid);
			}
// 			UMediaPlaylist* MediaPlayList = MediaPlayer->GetPlaylist();
// 			if (MediaPlayList->IsValidLowLevel())
// 			{
// 				FString URL;
// 				for (int i = 0; i < MediaPlayList->Num(); ++i)
// 				{
// 					URL = MediaPlayList->Get(i)->GetUrl();
// 					TheaterMovieInfos.Remove(URL);
// 				}
// 			}

			AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
			if (MyChar->IsValidLowLevel() == true)
			{
				MediaPlayer->OnMediaOpened.RemoveDynamic(MyChar, &AWRCharacterMine::OnMediaOpened);
			}

			MediaPlayer->Close();
		}

		WRSequenceManager::Get()->StopSequence(MovieTable->EventSequence);

		for (auto& Sound : Sounds)
		{
			WRSFXManager::Get()->Remove(Sound);
		}
		Sounds.Empty();
	}
}

void WRMediaPlayerManager::OnMediaOpened(FString OpenedURL)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar->IsValidLowLevel() == false)
		return;

	// [ 2020-2-13 : hansang88 ] 같은 url 이 여러곳에서 동시에 재생된다면 문제 발생(현재는 기획자가 상황 인지하고 해당 상황 만들지 않기로함)
	// [ 2020-2-13 : hansang88 ] 추후 엔진수정 하여 OnMediaOpened 해당 함수를 통해 tid 등 다른정보가 실려야 해당 문제 대응가능
	for (auto iter = TheaterMovieInfos.CreateIterator(); iter; ++iter)
	{
		if (iter.Value().Contains(OpenedURL) == true)
		{
			FWRMovieTable* MovieTable = WRTableManager::Get()->FindRow<FWRMovieTable>(iter.Value()[OpenedURL].Tid);
			if (MovieTable)
			{
				WRMaterialParameterCollectionManager::Get()->SetScalarParameterValue(EWRMPC::Movie, "3DMovieUse", MovieTable->Use3D);
				if (MovieTable->EventSequence.IsEmpty() == false)
				{
					FMovieSceneSequencePlaybackSettings SequenceSettings;
					SequenceSettings.StartTime = iter.Value()[OpenedURL].SeekTime;
					WRSequenceManager::Get()->PlaySequence(MovieTable->EventSequence, SequenceSettings);
				}

				if (MovieTable->MediaSound.IsEmpty() == false)
				{
					UAudioComponent* AudioComponent = WRSFXManager::Get()->Spawn2D(MovieTable->MediaSound);
					if (AudioComponent)
					{
						AudioComponent->Play(iter.Value()[OpenedURL].SeekTime);

						int32 UID = AudioComponent->GetUniqueID();
						Sounds.Add(UID);
					}
				}
			}

			if (iter.Value()[OpenedURL].MediaPlayer->IsValidLowLevel() == true)
			{
				FTimespan TempSeekTime(0, 0, iter.Value()[OpenedURL].SeekTime);
				iter.Value()[OpenedURL].MediaPlayer->Seek(TempSeekTime);

				iter.Value()[OpenedURL].SeekTime = 0.f;
				iter.Value()[OpenedURL].MediaPlayer = nullptr;
				//iter.Value()[OpenedURL].MediaPlayer->OnMediaOpened.RemoveDynamic(MyChar, &AWRCharacterMine::OnMediaOpened);
			}		

			break;
		}
	}
	
}

