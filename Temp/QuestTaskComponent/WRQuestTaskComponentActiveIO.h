// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once
#include "WRQuestTaskComponentBase.h"


class AWRCharacter;

class WR_API WRQuestTaskComponentActiveIO : public WRQuestTaskComponentBase
{
public:
	WRQuestTaskComponentActiveIO(const WRTableID IN InQuestTID, const WRTableID IN InTaskTID, const FWRQuestTaskTable* IN InQuestTaskTable);

public:
	virtual void Tick(const float IN InDeltaTime) override;
	virtual void OnDestroy() 	override;
	virtual void OnLoadLevelStart() 	override;
	virtual void OnLoadLevelProcessEnded() 	override;

	virtual void OnTaskCompleted() override;
	virtual void OnTaskGaveUp() override;

	virtual void OnCharacterSpawned(AWRCharacter* IN InCharacter) override;
	virtual void OnCharacterUnSpawned(AWRCharacter* IN InCharacter) override;

private:
	void AddMark();
	void RemoveMark();

private:
	TDoubleLinkedList<TPair<WRCompassMarkHandle, AActor*>> CompassMarkHandles;
};
