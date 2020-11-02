// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "Define/WRTypeDefine.h"
#include "Manager/WRObjectInteractionManager.h"

#include "WRActorComponentLadder.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentLadder : public UWRActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

	void OnFSMLadderBegin(const FWRLadderData& IN InData);
	void OnFSMLadderTick(float DeltaTime);
	void OnFSMLadderEnd();

public:
	FORCEINLINE class AWRLadder* GetUsingLadder() { return UsingLadder; }
	FORCEINLINE EWRLadderDirection GetLadderDirection() { return LadderData.Direction; }
	bool IsUsingLadder();

	void SetLadderOnAppear(const int32 IN InHandleIndex, const int32 IN InHand);
	void SetOverlapLadderArea(class AWRLadder* IN InLadder);

	bool DetermineLadder(EWRInput IN InInput, EWRLadderDirection IN InDirection);
	void CancelLadder();

	// :: Packet
	void OnAck_BeginLadder(const int32 IN InHandleIndex, const int32 IN InHand);
	void OnAck_MoveLadder(const int32 IN InHandleIndex, const int32 IN InHand, const int32 IN InDirection);
	void OnAck_EndLadder(const int32 IN InHandleIndex);
	void OnAck_CancelLadder();

protected:
	bool OnBeginLadder(class AWRLadder* IN InLadder, const int32 IN InHandleIndex, const EWRLadderHand IN InHand);
	void OnMoveLadder(class AWRLadder* IN InLadder, const int32 IN InHandleIndex, const EWRLadderHand IN InHand, const EWRLadderDirection IN InDirection);
	void OnEndLadder(class AWRLadder* IN InLadder, const int32 IN InHandleIndex);

private:
	bool CheckLadderFinishLine(EWRLadderDirection IN InDirection);

private:
	class AWRLadder* UsingLadder = nullptr;
	class AWRLadder* OverlapedLadder = nullptr;
	FWRLadderData LadderData;
	FVector vSrcCameraRelativeLocation;

	// Move
	float fDeltaTime = 0.f;
	float fMaxTime = 3.f;
	float fMaxMove = 0.f;
	float fSrcZ = 0.f;
	bool bMove = false;
};
