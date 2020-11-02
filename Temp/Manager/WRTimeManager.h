// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"

class WR_API WRTimeManager : public WRSingleton<WRTimeManager>
{

public:
	WRTimeManager() {}
	virtual ~WRTimeManager() {}

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override;
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override;

	void SetServerTime(const struct stSmallDateTime& InTime);
	void SetServerMapOpenTime(const struct stSmallDateTime& InTime);

	// [ 2020-1-10 : hansang88 ] 서버의 mapopen 부터 얼마나 지났는지
	FTimespan GetMapOpenElapsedTime();
	FDateTime GetCurTime();

private:

	FDateTime ServerTime;
	FDateTime ServerMapOpenTime;

	double StartFlatformTime;
	double TimeSecondSpan;

	float StartCountDown;
	float EndCountDown;
};
