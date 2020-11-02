#include "WRTriggerSpecialNode_Watch.h"
#include "../../WRTriggerSystem.h"
#include "../Action/WRTriggerActionNodeBase.h"
#include "../Transition/WRTriggerTransitionNodeBase.h"
#include "../Trigger/WRTriggerNodeBase.h"
#include "../../../Enum/EWRTriggerEnum.h"


WRTriggerSpecialNode_Watch::WRTriggerSpecialNode_Watch(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/)//:
	//WRTriggerTransitionNodeBase(InUid, InNodeName, InNodeType)
{
}

WRTriggerSpecialNode_Watch::~WRTriggerSpecialNode_Watch()
{

}

void WRTriggerSpecialNode_Watch::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
{

}

void WRTriggerSpecialNode_Watch::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerSpecialNodeBase::ImportDataFromJson(InJsonObject);
	
	const TArray<TSharedPtr<FJsonValue>>* WatchActorIDs;
	// Input Linked Node Uid
	if ((*InJsonObject)->TryGetArrayField("WatchActorIDs", WatchActorIDs) == false)
	{
		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSpecialNode_Watch] ] WatchActorIDs Uids is Empty"));
		return;
	}

	for (int32 i = 0; i < WatchActorIDs->Num(); ++i)
	{
		int32 TempUid;
		if ((*WatchActorIDs)[i]->TryGetNumber(TempUid) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSpecialNode_Watch] WatchActorID (*WatchActorIDs)[0]->TryGetNumber(InActorUid) false"));
			continue;
		}

		WatchActorUids.Add(TempUid);
	}
	
 	const TArray<TSharedPtr<FJsonValue>>* WatchPinLinkedUids;
 	// Output Lined Node Uids
 	if ((*InJsonObject)->TryGetArrayField("Watch", WatchPinLinkedUids) == false)
 	{
 		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSpecialNode_Watch] ] WatchPin Linked Uids is Empty"));
 		return;
 	}
 
 	for (int32 i = 0; i < WatchPinLinkedUids->Num(); ++i)
 	{
 		int32 TempUid;
 		if ((*WatchPinLinkedUids)[i]->TryGetNumber(TempUid) == false)
 		{
 			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSpecialNode_Watch] WatchPin (*LinkedUids)[0]->TryGetNumber(InPinNodeUid) false"));
 			continue;
 		}
 
		WatchPinNodeUids.Add(TempUid);
 	}

	// Output Lined Node Uids
	if ((*InJsonObject)->TryGetNumberField("tran", TransitionPinNodeUid) == false)
	{
		TransitionPinNodeUid = 0;
		UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerSpecialNode_Watch] ] TransitionPin Linked Uids is Empty"));
		return;
	}
}

void WRTriggerSpecialNode_Watch::RemoveFollowerID(int32 nActorID)
{
	if (WatchActorUids.Num() == 0 && WatchPinNodeUids.Num() == 0)
		return;

	if (WatchActorUids.Contains(nActorID) == true)
	{
		WatchActorUids.Remove(nActorID);
	}
	else
	{
		int32 WatchPinNodeUid = 0;
		for (int i = 0; i < WatchPinNodeUids.Num(); ++i)
		{
			WatchPinNodeUid = WatchPinNodeUids[i];
			WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(WatchPinNodeUid);
			if (BaseNode)
			{
				if (((WRTriggerNodeBase*)BaseNode)->RemoveFollowerID(nActorID) == true)
				{
					if (((WRTriggerNodeBase*)BaseNode)->IsMyFollowIDsEmpty() == true)
					{
						WatchPinNodeUids.Remove(WatchPinNodeUid);
						break;
					}
				}
			}
		}
	}

	if (WatchActorUids.Num() == 0 && WatchPinNodeUids.Num() == 0)
		SendSignalToTriggerNode(nullptr);
}

void WRTriggerSpecialNode_Watch::SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode)
{
	WRTriggerBaseNode* BaseNode = TriggerSystem->GetNodeByUid(TransitionPinNodeUid);
	if (BaseNode && BaseNode->GetNodeType() == EWRTriggerNodeType::Transition_Etc)
	{
		((WRTriggerTransitionNodeBase*)BaseNode)->Signal(nullptr);
	}
}

