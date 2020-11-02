// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAutoStateMoveTo.h"

WRAutoStateMoveTo::WRAutoStateMoveTo()
	:WRAutoStateBase(EWRAutoSystemState::None)
{
}

WRAutoStateMoveTo::~WRAutoStateMoveTo()
{
}

void WRAutoStateMoveTo::ResetState()
{
	WRAutoStateBase::ResetState();
}

void WRAutoStateMoveTo::Begin()
{
	WRAutoStateBase::Begin();

}

void WRAutoStateMoveTo::Tick(float DeltaTime)
{
	WRAutoStateBase::Tick(DeltaTime);

}

void WRAutoStateMoveTo::Stop()
{
	WRAutoStateBase::Stop();

}

void WRAutoStateMoveTo::Pause()
{
	WRAutoStateBase::Pause();

}

void WRAutoStateMoveTo::Resume()
{
	WRAutoStateBase::Resume();
}

void WRAutoStateMoveTo::MoveTo(const FVector& InDest)
{

}
