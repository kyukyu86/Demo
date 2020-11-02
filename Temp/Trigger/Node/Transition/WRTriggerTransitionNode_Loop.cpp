#include "WRTriggerTransitionNode_Loop.h"
#include "../../WRTriggerSystem.h"


WRTriggerTransitionNode_Loop::WRTriggerTransitionNode_Loop()
{

}

WRTriggerTransitionNode_Loop::~WRTriggerTransitionNode_Loop()
{

}

void WRTriggerTransitionNode_Loop::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerTransitionNodeBase::ImportDataFromJson(InJsonObject);

	double Temp = 0;

	if ((*InJsonObject)->TryGetNumberField("loop_period", Temp) == true)
	{
		LoopPeriod = (float)Temp;
	}
}

void WRTriggerTransitionNode_Loop::UpdateLoop(float DeltaTime, WRTriggerNodeBase* CallFrom)
{
	ElapsedTime += DeltaTime;
	if (ElapsedTime > LoopPeriod)
	{
		ElapsedTime = 0.0f;
		WRTriggerTransitionNodeBase::Signal(CallFrom);
	}
}

