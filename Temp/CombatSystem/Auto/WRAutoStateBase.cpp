// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRAutoStateBase.h"

WRAutoStateBase::WRAutoStateBase(EWRAutoSystemState InNextLinkedAutostate)
	:NextAutoStateType(InNextLinkedAutostate)
{
}

WRAutoStateBase::~WRAutoStateBase()
{
}

void WRAutoStateBase::ResetState()
{
	AutoProcessType = EWRAutoProcessType::None;

	switch (CurrentStateType)
	{
	case EWRAutoSystemState::Attacking:
	{
		NextAutoStateType = EWRAutoSystemState::Searching;
	}
	break;
	case EWRAutoSystemState::MoveTo:
	case EWRAutoSystemState::Searching:
	default:
	{
		NextAutoStateType = EWRAutoSystemState::None;
	}
	break;
	}
}

void WRAutoStateBase::Begin()
{

}

void WRAutoStateBase::Tick(float DeltaTime)
{
}

void WRAutoStateBase::Stop()
{

}

void WRAutoStateBase::Pause()
{

}

void WRAutoStateBase::Resume()
{

}

