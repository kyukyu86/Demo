#pragma once
#include "WRTriggerTransitionNodeBase.h"

class WR_API WRTriggerTransitionNode_Exit : public WRTriggerTransitionNodeBase
{
public:
	WRTriggerTransitionNode_Exit(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerTransitionNode_Exit();

	//virtual void Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/) override;

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

// protected:
// 	virtual void SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode) override;
};