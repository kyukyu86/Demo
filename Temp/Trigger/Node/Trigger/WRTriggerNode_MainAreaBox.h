#pragma once
#include "WRTriggerNode_Box.h"

class WR_API WRTriggerNode_MainAreaBox : public WRTriggerNode_Box
{
public:
	WRTriggerNode_MainAreaBox();
	virtual ~WRTriggerNode_MainAreaBox();

	virtual int32 GetAreaGroupID() { return AreaGroupID; }

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

	const FString& GetLocalKey() { return LocalKey;	}

	bool IsMinimapNameUse() { return bUseMinimapName; }
#ifdef WITH_EDITOR 
	virtual void DrawTriggerShape() override;
#endif // WITH_EDITOR

protected:
	int32			AreaGroupID;
	FString			LocalKey;

	bool			bUseMinimapName = true;
};