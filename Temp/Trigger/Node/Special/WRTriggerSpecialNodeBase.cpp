#include "WRTriggerSpecialNodeBase.h"
#include "../Trigger/WRTriggerNodeBase.h"
#include "../../WRTriggerSystem.h"
#include "../Action/WRTriggerActionNodeBase.h"


WRTriggerSpecialNodeBase::WRTriggerSpecialNodeBase()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType) :
	//WRTriggerBaseNode(InUid, InNodeName, InNodeType)
{

}

WRTriggerSpecialNodeBase::~WRTriggerSpecialNodeBase()
{

}

void WRTriggerSpecialNodeBase::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
{
	
}



void WRTriggerSpecialNodeBase::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerBaseNode::ImportDataFromJson(InJsonObject);
}

bool WRTriggerSpecialNodeBase::SetTransition(const EWRTriggerTransitionType::en Type, WRTriggerTransitionNodeBase* InTransition)
{
	if (Transitions.Contains(Type) == true)
		return false;

	Transitions.Add(Type, InTransition);
	return true;
}
