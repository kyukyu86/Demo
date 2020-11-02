// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WR.h"
#include "Modules/ModuleManager.h"
#include "Localization/WRLocalizationProcessor.h"

void FWR::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
	WRLocalizationProcessor::InitializeStringTable();
}

void FWR::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE( FWR, WR, "WR" );

DEFINE_LOG_CATEGORY(WRLog)