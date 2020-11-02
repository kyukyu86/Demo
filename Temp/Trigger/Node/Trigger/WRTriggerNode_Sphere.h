#pragma once
#include "WRTriggerNodeBase.h"

class WR_API WRTriggerNode_Sphere : public WRTriggerNodeBase
{
public:
	WRTriggerNode_Sphere(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerNode_Sphere();

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

// 	virtual void OnEtc() override;
// 	virtual void OnEnter() override;
// 	virtual void OnExit() override;
// 	virtual void OnActive() override;
// 	virtual void OnDeactive() override;
// 	virtual void OnEnable() override;
// 	virtual void OnDisable() override;

	virtual bool IsIntersect(class AWRCharacter* InCharacter) override;
	virtual bool IsSelfDisable() override { return bSelfDisable; }

	virtual FVector GetRandomPositionFromShape(const EWRTriggerRandomPosLockAxis::Type& LockAxis) override;

#ifdef WITH_EDITOR
	virtual void DrawTriggerShape() override;
#endif // WITH_EDITOR

private:
	FSphere		SphereShape;
	bool		bSelfDisable;
};