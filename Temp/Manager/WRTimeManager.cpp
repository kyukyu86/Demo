// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRTimeManager.h"
#include "Network/Share/Define/Define_Time.h"


void WRTimeManager::OnInitialize()
{
	StartFlatformTime = 0;
}

void WRTimeManager::OnShutdown()
{
	
}

void WRTimeManager::OnLoadLevelComplete()
{

}

void WRTimeManager::OnLoadLevelStart()
{

}

void WRTimeManager::OnRenderStart()
{

}

void WRTimeManager::SetServerTime(const stSmallDateTime& InTime)
{
	if (false == InTime.IsValid())
		return;

	StartFlatformTime = FPlatformTime::Seconds();
	ServerTime = FDateTime(InTime.year, InTime.month, InTime.day, InTime.hour, InTime.minute, InTime.second);
}

void WRTimeManager::SetServerMapOpenTime(const struct stSmallDateTime& InTime)
{
	if (false == InTime.IsValid())
		return;

	ServerMapOpenTime = FDateTime(InTime.year, InTime.month, InTime.day, InTime.hour, InTime.minute, InTime.second);
}

FTimespan WRTimeManager::GetMapOpenElapsedTime()
{
	FTimespan ResultTime = GetCurTime() - ServerMapOpenTime;
	return ResultTime;
}

FDateTime WRTimeManager::GetCurTime()
{
	double FlatformTimeSecond = FPlatformTime::Seconds();
	double SecondGap = FlatformTimeSecond - StartFlatformTime;
	FTimespan span = FTimespan::FromSeconds(SecondGap);
	FDateTime CurrenTime = ServerTime + span;
	return CurrenTime;
}

