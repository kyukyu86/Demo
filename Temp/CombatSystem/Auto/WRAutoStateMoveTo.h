// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRAutoStateBase.h"

/**
 * 
 */
class WR_API WRAutoStateMoveTo : public WRAutoStateBase
{
public:
	WRAutoStateMoveTo();
	virtual ~WRAutoStateMoveTo();
	friend class IMAutoSystemManager;

public:
	virtual void ResetState() override;
	virtual void Begin() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Resume() override;

public:
	void MoveTo(const FVector& InDest);


private:
	//오토가 시작할때 지정되는 포지션, TemporiryDestPosition 값이 ZeroVector면 항상 여기로 이동
	FVector DestPosition = FVector::ZeroVector;
	//목적지로 이동하는 도중에 일시적으로(지나가는 길에 퀘스트 몹이 잇다던가) 사용하게 될 포지션
	FVector TemporiryDestPosition = FVector::ZeroVector;

};

class CAutoStateMoveToEnemy : public WRAutoStateMoveTo
{
public:
	CAutoStateMoveToEnemy() {};
	virtual ~CAutoStateMoveToEnemy() {};
	friend class IMAutoSystemManager;
};
