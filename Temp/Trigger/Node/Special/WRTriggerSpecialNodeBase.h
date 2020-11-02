#pragma once
#include "../WRTriggerBaseNode.h"

class WR_API WRTriggerSpecialNodeBase : public WRTriggerBaseNode
{
public:
	WRTriggerSpecialNodeBase(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	~WRTriggerSpecialNodeBase();

	virtual void Signal(class WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/);

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject);

	bool SetTransition(const EWRTriggerTransitionType::en Type, class WRTriggerTransitionNodeBase* InTransition);

 protected:
 	virtual void SendSignalToTriggerNode(class WRTriggerNodeBase* InTriggerNode) {}

private:
	TMap<EWRTriggerTransitionType::en, class WRTriggerTransitionNodeBase*> Transitions;
};