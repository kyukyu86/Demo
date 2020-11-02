#pragma once
#include "WRTriggerTransitionNodeBase.h"

class WR_API WRTriggerTransitionNode_Active : public WRTriggerTransitionNodeBase
{
public:
	WRTriggerTransitionNode_Active(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerTransitionNode_Active();

	bool UpdateActive(float DeltaTime, WRTriggerNodeBase* CallFrom);

	virtual void Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/) override;

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

protected:
	//virtual void SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode) override;

private:

	struct FWRTriggerActiveNodeData
	{
	public:
		float FirstTimeDelay;
		float LoopingDelay;
		int32 LoopCount;

		bool bProcessedFirstTime = false;
	};
	

	FWRTriggerActiveNodeData LoadedData;
	FWRTriggerActiveNodeData UpdateData;
	float AccTime = 0.0f;
};