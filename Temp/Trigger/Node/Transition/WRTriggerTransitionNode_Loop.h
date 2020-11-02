#pragma once
#include "WRTriggerTransitionNodeBase.h"

class WR_API WRTriggerTransitionNode_Loop : public WRTriggerTransitionNodeBase
{
public:
	WRTriggerTransitionNode_Loop();
	virtual ~WRTriggerTransitionNode_Loop();

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

	void UpdateLoop(float DeltaTime, WRTriggerNodeBase* CallFrom);
private:

	float LoopPeriod = 0.f;
	float ElapsedTime = 0.f;
};