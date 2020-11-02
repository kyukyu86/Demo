// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRPathManager.h"
#include <Engine/Engine.h>

DEFINE_LOG_CATEGORY(PathManager)

WRPathManager::WRPathManager()
{
}

WRPathManager::~WRPathManager()
{
}

void WRPathManager::OnInitialize()
{
	// [ 2019-6-17 : magedoga ] 일반 추가
	ADDPATH(EWRPath::Asset, TEXT("/Game/Asset"));
	ADDPATH(EWRPath::Movie, TEXT("/Game/Movie"));

	// [ 2019-6-17 : magedoga ] 상위 경로 존재하는 경우의 추가
	ADDPATH_COMBINE_UPPER(EWRPath::Animation, TEXT("Animation"), EWRPath::Asset);
	ADDPATH_COMBINE_UPPER(EWRPath::Blueprint, TEXT("BluePrint"), EWRPath::Asset);	
	ADDPATH_COMBINE_UPPER(EWRPath::Character, TEXT("Character"), EWRPath::Asset);
	ADDPATH_COMBINE_UPPER(EWRPath::Data, TEXT("Data"), EWRPath::Asset);
	ADDPATH_COMBINE_UPPER(EWRPath::Effect, TEXT("Effect"), EWRPath::Asset);	
	ADDPATH_COMBINE_UPPER(EWRPath::Shader, TEXT("Shader"), EWRPath::Asset);
	ADDPATH_COMBINE_UPPER(EWRPath::Sound, TEXT("Sound"), EWRPath::Asset);
	ADDPATH_COMBINE_UPPER(EWRPath::UI, TEXT("UI"), EWRPath::Asset);

	//ADDPATH_COMBINE_UPPER(EWRPath::Blueprint_Character, TEXT("Character"), EWRPath::Blueprint);
	//ADDPATH_COMBINE_UPPER(EWRPath::Blueprint_UI, TEXT("UI"), EWRPath::Blueprint);
	//ADDPATH_COMBINE_UPPER(EWRPath::Blueprint_Effect, TEXT("Effect"), EWRPath::Blueprint);

	ADDPATH_COMBINE_UPPER(EWRPath::Data_Map, TEXT("Map"), EWRPath::Data);
	ADDPATH_COMBINE_UPPER(EWRPath::Data_Table, TEXT("Table"), EWRPath::Data);

	//ADDPATH_COMBINE_UPPER(EWRPath::Shader_PostProcess, TEXT("PostProcess"), EWRPath::Shader);

	ADDPATH_COMBINE_UPPER(EWRPath::Sound_SoundCue, TEXT("Sound_Cues"), EWRPath::Sound);

	//ADDPATH_COMBINE_UPPER(EWRPath::Blueprint_UI_Panel, TEXT("Panel"), EWRPath::Blueprint_UI);
	//ADDPATH_COMBINE_UPPER(EWRPath::Blueprint_UI_Slot, TEXT("Slot"), EWRPath::Blueprint_UI);

	ADDPATH_COMBINE_UPPER(EWRPath::UI_Panel, TEXT("00_Panel"), EWRPath::UI);
	ADDPATH_COMBINE_UPPER(EWRPath::UI_Slot, TEXT("00_Slot"), EWRPath::UI);
	ADDPATH_COMBINE_UPPER(EWRPath::UI_Images, TEXT("Images"), EWRPath::UI);	

	// [ 2020-1-10 : hansang88 ] Moive Root 1 Depth
	ADDPATH_COMBINE_UPPER(EWRPath::MediaMaterial, TEXT("MediaMaterial"), EWRPath::Movie);
	ADDPATH_COMBINE_UPPER(EWRPath::MediaPlayer, TEXT("MediaPlayer"), EWRPath::Movie);
	ADDPATH_COMBINE_UPPER(EWRPath::MediaSource, TEXT("MediaSource"), EWRPath::Movie);
	ADDPATH_COMBINE_UPPER(EWRPath::MediaTexture, TEXT("MediaTexture"), EWRPath::Movie);
}

void WRPathManager::OnShutdown()
{
	CollectedPaths.Empty();
}

void WRPathManager::OnLoadLevelStart()
{

}

void WRPathManager::OnLoadLevelComplete()
{

}

void WRPathManager::OnRenderStart()
{

}

//====================================================================================

void WRPathManager::CheckAndAddSlash(FString& InPath)
{
	if (InPath.Len() < 1)
		return;

	FString Right = InPath.Right(1);
	if (Right == "/" || Right == "\\")
		return;

	InPath.AppendChar('/');
}

void WRPathManager::AddPath(EWRPath InKindofPath, FString InPath)
{
	checkf(CollectedPaths.Contains(InKindofPath) == false, TEXT("Already Exist - %d"), (int32)InKindofPath);
	CheckAndAddSlash(InPath);
	CollectedPaths.Add(InKindofPath, InPath);
}

void WRPathManager::AddPath(EWRPath InKindofPath, FString InPath, EWRPath InKindofUpperPath)
{
	checkf(CollectedPaths.Contains(InKindofUpperPath) == true, TEXT("Not Exist Upperpath - %d"), (int32)InKindofUpperPath);
	ADDPATH(InKindofPath, CollectedPaths[InKindofUpperPath] + InPath);
}

FString WRPathManager::GetPath(EWRPath InKindofPath)
{
	checkf(CollectedPaths.Contains(InKindofPath) == true, TEXT("Not Exist - %d"), (int32)InKindofPath);
	return CollectedPaths[InKindofPath];
}

//====================================================================================