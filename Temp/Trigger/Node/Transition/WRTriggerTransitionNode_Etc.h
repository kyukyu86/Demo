#pragma once
#include "WRTriggerTransitionNodeBase.h"

class WR_API WRTriggerTransitionNode_Etc : public WRTriggerTransitionNodeBase
{
public:
	WRTriggerTransitionNode_Etc(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerTransitionNode_Etc();

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

protected:
	//virtual void SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode) override;
};