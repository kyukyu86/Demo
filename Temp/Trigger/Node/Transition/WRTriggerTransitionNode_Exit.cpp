#include "WRTriggerTransitionNode_Exit.h"


WRTriggerTransitionNode_Exit::WRTriggerTransitionNode_Exit()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType):
	//WRTriggerTransitionNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerTransitionNode_Exit::~WRTriggerTransitionNode_Exit()
{

}

// void WRTriggerTransitionNode_Exit::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
// {
// 
// }

void WRTriggerTransitionNode_Exit::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerTransitionNodeBase::ImportDataFromJson(InJsonObject);
}

// void WRTriggerTransitionNode_Exit::SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode)
// {
// }

