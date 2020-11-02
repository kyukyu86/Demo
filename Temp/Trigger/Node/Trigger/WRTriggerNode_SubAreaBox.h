#pragma once
#include "WRTriggerNode_Box.h"

class WR_API WRTriggerNode_SubAreaBox : public WRTriggerNode_Box
{
public:
	WRTriggerNode_SubAreaBox(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerNode_SubAreaBox();

	virtual int32 GetAreaGroupID() { return AreaGroupID; }

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

#ifdef WITH_EDITOR
	virtual void DrawTriggerShape() override;
#endif // WITH_EDITOR

protected:
	int32			AreaGroupID;
};