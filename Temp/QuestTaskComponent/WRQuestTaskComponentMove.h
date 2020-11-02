// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "WRQuestTaskComponentBase.h"
#include "Define/WRTypeDefine.h"




class WR_API WRQuestTaskComponentMove : public WRQuestTaskComponentBase
{
public:
	WRQuestTaskComponentMove(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);

public:
	virtual void Tick(const float IN InDeltaTime) override;
	virtual void OnDestroy() 	override;
	virtual void OnLoadLevelStart() 	override;
	virtual void OnLoadLevelProcessEnded() 	override;

	virtual void OnTaskCompleted() override;
	virtual void OnTaskGaveUp() override;
private:
	void CheckArriveTargetPosition();
	void SpawnFX();
	void RemoveFX();
	void SpawnCompleteQuestFX();
	void AddMark();
	void RemoveMark();

	void AddTargetLocationGuide();
	void RemoveTargetLocationGuide();

	uint32 FXUID;
	WRCompassMarkHandle CompassMarkHandle;
	WRTargetLocationGuideHandle TargetLocationGuideHandle;
};
