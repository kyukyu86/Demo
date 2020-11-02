// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAutoStateSearching.h"

WRAutoStateSearching::WRAutoStateSearching()
	:WRAutoStateBase(EWRAutoSystemState::None)
{
}

WRAutoStateSearching::~WRAutoStateSearching()
{
}

void WRAutoStateSearching::ResetState()
{
	WRAutoStateBase::ResetState();
}

void WRAutoStateSearching::Begin()
{
	WRAutoStateBase::Begin();
}

void WRAutoStateSearching::Tick(float DeltaTime)
{
	WRAutoStateBase::Tick(DeltaTime);
}

void WRAutoStateSearching::Stop()
{
	WRAutoStateBase::Stop();
}

void WRAutoStateSearching::Pause()
{
	WRAutoStateBase::Pause();
}

void WRAutoStateSearching::Resume()
{
	WRAutoStateBase::Resume();
}
