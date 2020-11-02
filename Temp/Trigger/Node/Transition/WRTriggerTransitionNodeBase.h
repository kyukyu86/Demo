#pragma once
#include "../WRTriggerBaseNode.h"

class WR_API WRTriggerTransitionNodeBase : public WRTriggerBaseNode
{
public:
	WRTriggerTransitionNodeBase(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	~WRTriggerTransitionNodeBase();

	virtual void Signal(class WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/);

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject);

	int32 GetInPinNodeUid() { return InPinNodeUid; }
	const TArray<int32>& GetOutPinNodeUid() { return OutPinNodeUids; }

// protected:
// 	virtual void SendSignalToTriggerNode(class WRTriggerNodeBase* InTriggerNode) {}

private:
	int32 InPinNodeUid = -1;
	TArray<int32> OutPinNodeUids;


};