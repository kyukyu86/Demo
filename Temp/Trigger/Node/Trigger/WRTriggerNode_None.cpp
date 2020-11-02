#include "WRTriggerNode_None.h"
#include "Manager/WRTriggerManager.h"


WRTriggerNode_None::WRTriggerNode_None()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType):
	//WRTriggerNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerNode_None::~WRTriggerNode_None()
{

}

void WRTriggerNode_None::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerNodeBase::ImportDataFromJson(InJsonObject);
	(*InJsonObject)->TryGetBoolField("start_trigger", bStartTrigger);
	(*InJsonObject)->TryGetNumberField("save_num", SaveNum);

	FString Temp;
	if ((*InJsonObject)->TryGetStringField("condition_type", Temp) == true)
		ConditionType = EWRTriggerConditionType::GetEnum(Temp);

	(*InJsonObject)->TryGetNumberField("condition_value", ConditionValue);

	if (SaveNum != 0)
	{
		if (WRTriggerManager::Get()->IsUsedTriggerData(SaveNum) == true)
			bEnable = false;
	}
}

