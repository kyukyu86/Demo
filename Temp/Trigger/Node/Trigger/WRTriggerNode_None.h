#pragma once
#include "WRTriggerNodeBase.h"

class WR_API WRTriggerNode_None : public WRTriggerNodeBase
{
public:
	WRTriggerNode_None(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerNode_None();


	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;
	virtual bool IsStartTrigger() override { return bStartTrigger; }

	virtual int32 GetSaveNumber() { return SaveNum; }
private:
	bool		bStartTrigger;
	int32		SaveNum = 0;
};