// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "WRQuestTaskComponentBase.h"




class WR_API WRQuestTaskComponentTrigger : public WRQuestTaskComponentBase
{
public:
	WRQuestTaskComponentTrigger(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);

public:
	virtual void Tick(const float IN InDeltaTime) override;
	virtual void OnDestroy() 	override;
	virtual void OnLoadLevelStart() 	override;
	virtual void OnLoadLevelProcessEnded() 	override;

	virtual void OnTaskCompleted() override;
	virtual void OnTaskGaveUp() override;

	void OnQuestCompleteTriggered(const int32 IN InTriggerUID);

private:
	void AddMark();
	void RemoveMark();

private:
	WRCompassMarkHandle CompassMarkHandle;
};
