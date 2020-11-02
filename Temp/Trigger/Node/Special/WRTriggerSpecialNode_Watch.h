#pragma once
#include "WRTriggerSpecialNodeBase.h"

class WR_API WRTriggerSpecialNode_Watch : public WRTriggerSpecialNodeBase
{
public:
	WRTriggerSpecialNode_Watch(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerSpecialNode_Watch();

	virtual void Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/) override;

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

	void RemoveFollowerID(int32 nActorID);

protected:
	virtual void SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode) override;

private:
	TArray<int32> WatchPinNodeUids;
	TArray<int32> WatchActorUids;
	int32		  TransitionPinNodeUid;
};