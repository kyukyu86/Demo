// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRSingletonObject.h"

#include "Core/WRGameInstance.h"

#include <Ticker.h>
#include <CoreDelegates.h>

WRSingletonObject::WRSingletonObject()
{
}

WRSingletonObject::~WRSingletonObject()
{
}


//====================================================================================

void WRSingletonObject::Initialize(const FName& InName, const bool bInEnableTick, EWRManagerTickGroup InTickGroupType /*= EWRManagerTickGroup::TG_Normal*/)
{
	Name = InName;
	this->SetEnableTick(bInEnableTick, InTickGroupType);

	OnInitialize();
}

void WRSingletonObject::Initialize(const FName& InName, FWRBindDelegateFlag InFlag, const bool bInEnableTick /*= false*/, EWRManagerTickGroup InTickGroupType /*= EWRManagerTickGroup::TG_Normal*/)
{
	this->SetEnableCoreDelegate(InFlag);
	this->Initialize(InName, bInEnableTick, InTickGroupType);
}

void WRSingletonObject::Shutdown()
{
	this->SetEnableTick(false);
	this->SetEnableCoreDelegate(FWRBindDelegateFlag(0));

#if WITH_EDITOR
	bShutdown = true;
#endif // WITH_EDITOR

	OnShutdown();
}

void WRSingletonObject::DestroyInstance()
{
	OnDestroyInstance();
}

//====================================================================================

void WRSingletonObject::SetEnableTick(const bool bInEnable, EWRManagerTickGroup InTickGroupType /*= EWRManagerTickGroup::TG_Normal*/)
{
	if (bInEnable == false)
	{
		TickGroup = EWRManagerTickGroup::TG_None;
		return;
	}

	if (InTickGroupType == EWRManagerTickGroup::TG_None || InTickGroupType == EWRManagerTickGroup::TG_Max)
		return;

	TickGroup = InTickGroupType;
	

// 	if (bInEnable == true && HandleTicker.IsValid() == false)
// 	{
// 		HandleTicker = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &WRSingletonObject::OnTick));
// 		return;
// 	}
// 
// 	if (bInEnable == false && HandleTicker.IsValid() == true)
// 	{
// 		FTicker::GetCoreTicker().RemoveTicker(HandleTicker);
// 		HandleTicker.Reset();
// 	}

}

void WRSingletonObject::SetEnableCoreDelegate(FWRBindDelegateFlag InFlag)
{
	check(UWRGameInstance::GetGameInstance() != nullptr);

	//====================================================================================
	// [ 2019-6-17 : magedoga ] Deactivate
	if (InFlag.CheckFlag(FWRBindDelegateFlag::Deactivate) == true)
	{
		HandleDeactivate = BIND_DELEGATE_DEACTIVATE_RAW(this, &WRSingletonObject::OnDeactivate);
	}
	else
	{
		REMOVE_FROM_DEACTIVATE(HandleDeactivate);
	}


	//====================================================================================
	// [ 2019-6-17 : magedoga ] Reactivate
	if (InFlag.CheckFlag(FWRBindDelegateFlag::Reactivate) == true)
	{
		HandleReactivate = BIND_DELEGATE_REACTIVATE_RAW(this, &WRSingletonObject::OnDeactivate);
	}
	else
	{
		REMOVE_FROM_REACTIVATE(HandleReactivate);
	}

	//====================================================================================
	// [ 2019-6-17 : magedoga ] Enter Background
	if (InFlag.CheckFlag(FWRBindDelegateFlag::EnterBackground) == true)
	{
		HandleEnterBackground = BIND_DELEGATE_ENTER_BACKGROUND_RAW(this, &WRSingletonObject::OnEnterBackground);
	}
	else
	{
		REMOVE_FROM_ENTER_BACKFROUND(HandleEnterBackground);
	}

	//====================================================================================
	// [ 2019-6-17 : magedoga ] Enter Foreground
	if (InFlag.CheckFlag(FWRBindDelegateFlag::EnterForeground) == true)
	{
		HandleEnterForeground = BIND_DELEGATE_ENTER_FOREGROUND_RAW(this, &WRSingletonObject::OnEnterBackground);
	}
	else
	{
		REMOVE_FROM_ENTER_FOREGROUND(HandleEnterForeground);
	}

	//====================================================================================
	// [ 2019-6-17 : magedoga ] Terminate
	if (InFlag.CheckFlag(FWRBindDelegateFlag::Terminate) == true)
	{
		HandleTerminate = BIND_DELEGATE_TERMINATEAPP_RAW(this, &WRSingletonObject::OnEnterBackground);
	}
	else
	{
		REMOVE_FROM_TERMINATEAPP(HandleTerminate);
	}

	//====================================================================================
	// [ 2019-6-17 : magedoga ] Screen Orientation Changed
	if (InFlag.CheckFlag(FWRBindDelegateFlag::ScreenOrientationChanged) == true)
	{
		HandleScreenOrientationChanged = BIND_DELEGATE_SCREEN_ORIENTATION_CHANGED_RAW(this, &WRSingletonObject::OnScreenOrientationChanged);
	}
	else
	{
		REMOVE_FROM_SCREEN_ORIENTATION_CHANGED(HandleScreenOrientationChanged);
	}
}

//====================================================================================