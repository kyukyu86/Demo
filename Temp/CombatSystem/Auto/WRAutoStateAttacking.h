// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "WRAutoStateBase.h"

/**
 * 
 */
class WR_API WRAutoStateAttacking : public WRAutoStateBase
{
public:
	WRAutoStateAttacking();
	virtual ~WRAutoStateAttacking();

public:
	virtual void ResetState() override;
	virtual void Begin() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Resume() override;
};
