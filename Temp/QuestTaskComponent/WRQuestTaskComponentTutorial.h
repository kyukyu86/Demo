// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "WRQuestTaskComponentBase.h"




class WR_API WRQuestTaskComponentTutorial : public WRQuestTaskComponentBase
{
public:
	WRQuestTaskComponentTutorial(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);

public:
	virtual void Tick(const float IN InDeltaTime) override;
public:
	void OnTutorialCompleted(const WRTableID IN InTutorialTID);
};
