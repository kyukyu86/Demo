#include "WRTriggerTransitionNodeBase.h"
#include "../Trigger/WRTriggerNodeBase.h"
#include "../../WRTriggerSystem.h"
#include "../Action/WRTriggerActionNodeBase.h"

WRTriggerTransitionNodeBase::WRTriggerTransitionNodeBase()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType) :
	//WRTriggerBaseNode(InUid, InNodeName, InNodeType)
{

}

WRTriggerTransitionNodeBase::~WRTriggerTransitionNodeBase()
{

}

void WRTriggerTransitionNodeBase::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
{
	if (TriggerSystem == nullptr)
	{
		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] TriggerSystem is Null"));
		return;
	}


	// [2017-1-14 : magedoga] 연결된 OutPinNode에 신호전달 ~
	for (int32 i = 0; i < OutPinNodeUids.Num(); ++i)
	{
		int32 OutPinNodeUid = OutPinNodeUids[i];
		WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(OutPinNodeUid);
		if (BaseNode == nullptr)
			continue;

		EWRTriggerNodeType::en TempNodeType = BaseNode->GetNodeType();
		// ActionNode
		if (EWRTriggerNodeType::IsActionNode(TempNodeType) == true)
		{
			((WRTriggerActionNodeBase*)BaseNode)->ProcessAction(CallFrom);
		}
		// TriggerNode
// 		else if (EWRTriggerNodeType::IsTriggerNode(TempNodeType) == true)
// 		{
// 			SendSignalToTriggerNode((WRTriggerNodeBase*)BaseNode);
// 		}
// 		// TransitionNode
// 		else if (EWRTriggerNodeType::IsTransitionNode(TempNodeType) == true)
// 		{
// 			((WRTriggerTransitionNodeBase*)BaseNode)->Signal(CallFrom);
// 		}
	}
}



void WRTriggerTransitionNodeBase::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerBaseNode::ImportDataFromJson(InJsonObject);

// 	const TArray<TSharedPtr<FJsonValue>>* InputLinkedUids;
// 	// Input Linked Node Uid
// 	if ((*InJsonObject)->TryGetArrayField("Input", InputLinkedUids) == false)
// 	{
// 		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] ] Input Linked Uids is Empty"));
// 		return;
// 	}
// 
// 	if (InputLinkedUids->Num() == 0 || InputLinkedUids->Num() > 1)
// 	{
// 		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] Input Linked Uid .. Invalid Num [%d]"), InputLinkedUids->Num());
// 		return;
// 	}
// 
// 	if ((*InputLinkedUids)[0]->TryGetNumber(InPinNodeUid) == false)
// 	{
// 		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] Input (*LinkedUids)[0]->TryGetNumber(InPinNodeUid) false"));
// 		return;
// 	}

	if ((*InJsonObject)->TryGetNumberField(TEXT("Input"), InPinNodeUid) == false)
	{
		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] Input (*LinkedUids)[0]->TryGetNumber(InPinNodeUid) false"));
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* OutputLinkedUids;
	// Output Lined Node Uids
	if ((*InJsonObject)->TryGetArrayField("Output", OutputLinkedUids) == false)
	{
		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] ] Output Linked Uids is Empty"));
		return;
	}
	
	if (OutputLinkedUids->Num() == 0)
	{
		//UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] Output Linked Uid .. Invalid Num [%d]"), OutputLinkedUids->Num());
		return;
	}
	
	for (int32 i = 0; i < OutputLinkedUids->Num(); ++i)
	{
		int32 TempUid;
		if ((*OutputLinkedUids)[i]->TryGetNumber(TempUid) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerTransitionNodeBase] Output (*LinkedUids)[0]->TryGetNumber(InPinNodeUid) false"));
			continue;
		}

		OutPinNodeUids.Add(TempUid);
	}
}
