#include "WRTriggerTransitionNode_Deactive.h"
#include "../Trigger/WRTriggerNodeBase.h"


WRTriggerTransitionNode_Deactive::WRTriggerTransitionNode_Deactive()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType) :
	//WRTriggerTransitionNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerTransitionNode_Deactive::~WRTriggerTransitionNode_Deactive()
{

}

// void WRTriggerTransitionNode_Deactive::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
// {
// 
// }

void WRTriggerTransitionNode_Deactive::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerTransitionNodeBase::ImportDataFromJson(InJsonObject);
}

// void WRTriggerTransitionNode_Deactive::SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode)
// {
// 	InTriggerNode->OnDeactive();
// }