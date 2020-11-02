// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "../Enum/EWRUIEnum.h"

class WR_API WRUIPathManager : public WRSingleton<WRUIPathManager>
{
	CDECLARE_MANAGER(WRUIPathManager);

private:
	WRUIPathManager() {}
	virtual ~WRUIPathManager() {}

private:
	TMap<EWRUIPanelKind, FString>		PanelPaths;
	TMap<EWRUISlotKind, FString>		SlotPaths;

public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override {};
	virtual void OnLoadLevelStart() override {};
	virtual void OnRenderStart() override {};

	FString GetPanelBPName(const FString& IN InBPName);
	FString GetSlotBPName(const FString& IN InBPName);
public:
	const FString& GetUIPath(const EWRUIPanelKind IN InPanelKind) const;
	const FString& GetUIPath(const EWRUISlotKind IN InSlotKind) const;
};
