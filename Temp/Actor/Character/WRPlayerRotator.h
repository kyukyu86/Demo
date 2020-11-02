// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWRCharacterRotationType : uint8
{
	ThumbStick,
	ThumbStickPerAction,
	ThumbStickPerAction2,
	HeadMoundDisplay,
};

class WR_API WRPlayerCharacterRotator
{
public:
	WRPlayerCharacterRotator(class AWRCharacterPlayer* InCharacter)
		: PlayerCharacter(InCharacter) {}
	virtual ~WRPlayerCharacterRotator() {}

	virtual void OnEnterThis() {}
	virtual void OnLeaveThis() {}
	virtual void Rotation(const float InScale) {}
	virtual void Rotation180();
	virtual void UnlockRotation180() { bLockRotation180 = false; }
	virtual bool IsLockRotation180() const { return bLockRotation180; }
	virtual void SetSyncRotationByHMD(const bool bInEnable) {}
	virtual void SetSyncRotationByHMD_Once() {}
	
	// [ 2020-9-8 : magedoga ] Common
	virtual void RotateCharacterWithHMD(const FRotator& InRotator);

	static WRPlayerCharacterRotator* CreateRotator(EWRCharacterRotationType InRotatorType, class AWRCharacterPlayer* InCharacter);

protected:
	void SetRotation(const float InDeltaYaw, const float InScale);
	void SetRotationCharacter(const float InDeltaYaw, const float InScale);
	void SetRotationHMD(const float InDeltaYaw, const float InScale);

	void SyncRotationbyHMD_Internal();
	void SyncRotationbyHMDDelta(const float InDeltaTime, const float InSpeed);
	void RequestPlayerRotationToSync();

protected:

	class AWRCharacterPlayer* PlayerCharacter;
	bool bLockRotation180 = false;
};

class WR_API WRRotationbyThumbStick : public WRPlayerCharacterRotator
{
public:
	WRRotationbyThumbStick(class AWRCharacterPlayer* InCharacter) : WRPlayerCharacterRotator(InCharacter) {}

// 	virtual void OnEnterThis() override;
// 	virtual void OnLeaveThis() override;
	virtual void Rotation(const float InScale) override;
	//virtual void Rotation180() override;
};

class WR_API WRRotationbyThumbStickPerAction : public WRPlayerCharacterRotator
{
public:
	WRRotationbyThumbStickPerAction(class AWRCharacterPlayer* InCharacter) : WRPlayerCharacterRotator(InCharacter) {}

	virtual void OnEnterThis() override;
	virtual void OnLeaveThis() override;
	virtual void Rotation(const float InScale) override;
	//virtual void Rotation180() override;

private:

	bool TickRotationPerAction(float DeltaTime);
	bool TickSyncRotation(float DeltaTime);

	//float PreScale = 0.0f;
	bool bAllowAction = false;
	float MoveScale = 0.0f;
	float AccYaw = 0.0f;

	bool bTrace = false;

	float DeltaYawPerAction;
	float RotationPerActionTime;

	FDelegateHandle TickRotationPerActionDelegateHandle;
	FDelegateHandle TickSyncRotationDelegateHandle;
};

class WR_API WRRotationbyRotatorHMD : public WRPlayerCharacterRotator
{
public:
	WRRotationbyRotatorHMD(class AWRCharacterPlayer* InCharacter) : WRPlayerCharacterRotator(InCharacter) {}

	virtual void OnEnterThis() override;
	virtual void OnLeaveThis() override;
	//virtual void Rotation(const float InScale) override;
	//virtual void Rotation180() override;

private:

	bool Tick(float DeltaTime);

	FDelegateHandle TickDelegateHandle;
};

//====================================================================================

// [ 2020-7-24 : magedoga ] Thumbstrick이 더블로 입력됐을때 회전, 그 외에는 약회전
/*
1. R3 1회 축 이동
   Left, Right 회전

2. R3 빠르게 2회 축 이동
   기존 15도 회전

3. L3 버튼
  HMD와 Character Rotation Sync
*/
class WR_API WRRotationbyThumbStickPerAction2 : public WRPlayerCharacterRotator
{
public:
	WRRotationbyThumbStickPerAction2(class AWRCharacterPlayer* InCharacter) : WRPlayerCharacterRotator(InCharacter) {}

	virtual void OnEnterThis() override;
	virtual void OnLeaveThis() override;
	virtual void Rotation(const float InScale) override;
	virtual void SetSyncRotationByHMD(const bool bInEnable) override;
	virtual void SetSyncRotationByHMD_Once() override;

	enum class EWRThumbStickRotationState : uint8
	{
		Wait,
		Continuous,
		FixedRotation,
		EndFixedRotation,
	};



private:

	bool TickRotationPerAction(float InDeltaTime);
	bool TickSyncRotationByHMD(float InDeltaTime);
	void SetState(EWRThumbStickRotationState InNewState);
	void OnChangeState(EWRThumbStickRotationState InOldState, EWRThumbStickRotationState InNewState);

	void StartFixedRotation(float InMoveScale);

	bool IsPossibleFixedRotation(const float InScale) const;


private:

	EWRThumbStickRotationState	RotationState = EWRThumbStickRotationState::Wait;

	float PreScale = 0.0f;

	float RotationPerActionTime;
	float DeltaYawPerAction;
	float AccYaw;
	float MoveScale;


	FDelegateHandle	TickRotationPerActionHandle;
	FDelegateHandle TickSyncRotationByHMDHandle;
};