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
	//���䰡 �����Ҷ� �����Ǵ� ������, TemporiryDestPosition ���� ZeroVector�� �׻� ����� �̵�
	FVector DestPosition = FVector::ZeroVector;
	//�������� �̵��ϴ� ���߿� �Ͻ�������(�������� �濡 ����Ʈ ���� �մٴ���) ����ϰ� �� ������
	FVector TemporiryDestPosition = FVector::ZeroVector;

};

class CAutoStateMoveToEnemy : public WRAutoStateMoveTo
{
public:
	CAutoStateMoveToEnemy() {};
	virtual ~CAutoStateMoveToEnemy() {};
	friend class IMAutoSystemManager;
};
