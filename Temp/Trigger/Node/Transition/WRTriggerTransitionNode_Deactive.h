#pragma once
#include "WRTriggerTransitionNodeBase.h"

class WR_API WRTriggerTransitionNode_Deactive : public WRTriggerTransitionNodeBase
{
public:
	WRTriggerTransitionNode_Deactive(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerTransitionNode_Deactive();

	//virtual void Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/) override;

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

protected:
	//virtual void SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode) override;
};