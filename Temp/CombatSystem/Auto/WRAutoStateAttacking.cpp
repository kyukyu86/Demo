// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAutoStateAttacking.h"

WRAutoStateAttacking::WRAutoStateAttacking()
	:WRAutoStateBase(EWRAutoSystemState::Searching)
{
}

WRAutoStateAttacking::~WRAutoStateAttacking()
{
}

void WRAutoStateAttacking::ResetState()
{
	WRAutoStateBase::ResetState();
}

void WRAutoStateAttacking::Begin()
{
	WRAutoStateBase::Begin();

}

void WRAutoStateAttacking::Tick(float DeltaTime)
{
	WRAutoStateBase::Tick(DeltaTime);

}

void WRAutoStateAttacking::Stop()
{
	WRAutoStateBase::Stop();

}

void WRAutoStateAttacking::Pause()
{
	WRAutoStateBase::Pause();

}

void WRAutoStateAttacking::Resume()
{
	WRAutoStateBase::Resume();

}
