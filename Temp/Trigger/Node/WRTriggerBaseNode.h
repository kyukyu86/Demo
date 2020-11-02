#pragma once
#include "Enum/EWRTriggerEnum.h"
#include <JsonObject.h>
#include "Manager/WRTriggerManager.h"
#include "Network/Share/Define/Define_Trigger.h"

class WR_API WRTriggerBaseNode
{
public:
	WRTriggerBaseNode();
	virtual ~WRTriggerBaseNode();


	void SetTriggerSystem(class WRTriggerSystem* InTriggerSystem) { TriggerSystem = InTriggerSystem; }
	class WRTriggerSystem* GetTriggetSystem()const { return TriggerSystem; }

	int32 GetUid() { return Uid; }
	const FString& GetNodeName() { return NodeName; }
	EWRTriggerNodeType::en GetNodeType() { return NodeType; }

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
	{
		FString Buffer;
		if ((*InJsonObject)->TryGetStringField(TEXT("nodetype"), Buffer) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerBaseNode] Not Found NodeType From Json"));
		}

		NodeType = EWRTriggerNodeType::GetEnum(Buffer);
		if (EWRTriggerNodeType::IsVaild(NodeType) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerBaseNode] Invalid NodeType (%s)"), (*Buffer));
		}

		if ((*InJsonObject)->TryGetNumberField(TEXT("tid"), Uid) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerBaseNode] Not Found Uid From Json"));
		}

		if ((*InJsonObject)->TryGetStringField(TEXT("name"), NodeName) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerBaseNode] Not Found Name From Json"));
		}

	}


protected:
	class WRTriggerSystem*		TriggerSystem;

private:
	int32 Uid;
	FString NodeName;
	EWRTriggerNodeType::en NodeType;
};