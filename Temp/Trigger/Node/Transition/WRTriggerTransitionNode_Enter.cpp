#include "WRTriggerTransitionNode_Enter.h"
#include "../Trigger/WRTriggerNodeBase.h"


WRTriggerTransitionNode_Enter::WRTriggerTransitionNode_Enter()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType):
	//WRTriggerTransitionNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerTransitionNode_Enter::~WRTriggerTransitionNode_Enter()
{

}

// void WRTriggerTransitionNode_Enter::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
// {
// 	
// }

void WRTriggerTransitionNode_Enter::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerTransitionNodeBase::ImportDataFromJson(InJsonObject);
}

// void WRTriggerTransitionNode_Enter::SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode)
// {
// 	InTriggerNode->OnEnter();
// }