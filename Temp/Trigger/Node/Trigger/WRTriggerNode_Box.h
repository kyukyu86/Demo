#pragma once
#include "WRTriggerNodeBase.h"

class WR_API WRTriggerNode_Box : public WRTriggerNodeBase
{
public:
	WRTriggerNode_Box(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/);
	virtual ~WRTriggerNode_Box();

	virtual void ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject) override;

// 	virtual void OnEtc() override;
 	virtual void OnEnter(const uint64 InUniqueID = -1) override;
// 	virtual void OnExit() override;
// 	virtual void OnActive() override;
// 	virtual void OnDeactive() override;
// 	virtual void OnEnable() override;
// 	virtual void OnDisable() override;

	virtual bool IsIntersect(class AWRCharacter* InCharacter) override;
	virtual bool IsSelfDisable() override { return bSelfDisable; }

	virtual FVector GetRandomPositionFromShape(const EWRTriggerRandomPosLockAxis::Type& LockAxis) override;

	virtual int32 GetSaveNumber() { return SaveNum; }
	virtual void EnteringActorReset(bool bForced = false);
	virtual void EnteringActorResetCheckUsed();

#ifdef WITH_EDITOR
	virtual void DrawTriggerShape() override;
#endif // WITH_EDITOR

protected:
	FBox		BoxShape;
	bool		bSelfDisable;
	bool		bEnteringActorReset = false;

	// [ 2018-6-23 : hansang88 ] SelfDisable ����� ��� 1���� �ߵ��ؾ� ������ �����ӽ� ���� ��� �ִ�Ʈ���� ������ �ʱ�ȭ �Ͽ�
	// [ 2018-6-23 : hansang88 ] �̹� �ߵ��� Ʈ���Ÿ� ��ߵ� ��Ű�� ��찡 �־� �ش� �ߵ� ��Ȳ�� üũ�ϱ� ���� ���� bUsed �߰�
	bool		bUsed = false;			
	int32		SaveNum = 0;
};