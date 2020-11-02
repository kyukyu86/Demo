#include "WRTriggerTransitionNode_Etc.h"
#include "../../WRTriggerSystem.h"


WRTriggerTransitionNode_Etc::WRTriggerTransitionNode_Etc(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/)//:
	//WRTriggerTransitionNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerTransitionNode_Etc::~WRTriggerTransitionNode_Etc()
{

}

void WRTriggerTransitionNode_Etc::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerTransitionNodeBase::ImportDataFromJson(InJsonObject);
}

// void WRTriggerTransitionNode_Etc::SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode)
// {
// 	InTriggerNode->OnEtc(0);
// }

