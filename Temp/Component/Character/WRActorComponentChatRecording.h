// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRActorComponentChatRecording.generated.h"


/**
 * 
 */
// AWRCharacterMine��  ������ �ִ� Component�̴�.
UCLASS()
class WR_API UWRActorComponentChatRecording : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void OnMyCharacterTargetChanged(const AWRCharacter* IN InTargetCharacter);
	void OnGaugingCompleted();

	void CheckTickNeeded();											// Tick�� �ʿ����� üũ�϶�
private:
	void CheckTickNeeded(const AWRCharacter* IN InTargetCharacter);	// Tick�� �ʿ����� üũ�϶�
	void CheckLookingTargetHead();
	void CheckMyTranslatingPlayerDistance();
	bool CheckToEnterHeadArea(const AWRCharacter* IN InTargetCharacter);

	void ActiveGuageIcon();
	void DeActiveGuageIcon();

	void ActiveDisabledIcon();
	void DeActiveDisabledIcon();

	void StartLookingTarget();
	void StopLookingTarget();
	void OnLookTargetCompleted();
private: 
	
	bool bActiveGaugeIcon = false;
	bool bActiveDisabledIcon = false;
	bool bLookingTarget = false;	// bActiveGaugeIcon�� true�ϋ��� false�̴�
	
	FString CreatingGuageIconHash;	// ������ ������ ������ ���� Key

	float FlowLookingTargetTime;
};