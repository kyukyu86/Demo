// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Table/WREnpcTable.h"
#include "WRActorComponentEventAction.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentEventAction : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void UpdateEventActoinCodition();
private:
	bool CheckEventActionCondition();
	bool CheckEventActionCondition(const FWREventActionCondition& IN InEventActionCondition);

	bool CheckDistance();

	void ProgressEventAction();
	void ProgressEventAction(const FWREventAction& IN InEventAction);
private:
	bool bPreviousEventActionCondition = false; // 이전 Condition Check에서의 상태
	bool bAvailableAction = false;	// 액션이 가능한 상태인가
};
