#pragma once
#include "WRTriggerActionNodeBase.h"

//====================================================================================================================
// [2017-1-10 : magedoga] Action Processor
class WR_API WRTriggerActionProcessor
{
public:
	WRTriggerActionProcessor(WRTriggerActionNodeBase* InActionNode) : ActionNode(InActionNode) {}
	virtual ~WRTriggerActionProcessor() {}

	virtual void ProcessAction(class WRTriggerNodeBase* Caller)
	{
		if (ActionNode)
		{
			FString NodeName;// = ActionNode->GetNodeName();
			UE_LOG(WRTriggerLog, Display, TEXT("Called Trigger Name : %s"), *(ActionNode->GetNodeName()));
		}
		else
		{
			UE_LOG(WRTriggerLog, Error, TEXT("ActionNode is null"));
		}
	}

protected:
	
	AWRCharacter* FindCharacter(const uint64& InUID, bool bSpawnTid = true);
protected:
	WRTriggerActionNodeBase* ActionNode;


public:
	static WRTriggerActionProcessor* CreateLegalProcessor(WRTriggerActionNodeBase* InActionNode);
};