// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Singleton/DMSingleton.h"



class THIRDPERSON_API DMTargetingManager : public DMSingleton<DMTargetingManager>
{
private:

private:
	void UpdateTarget();

public:
	DMTargetingManager();
	virtual ~DMTargetingManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;
	virtual bool OnTick(float InDeltaTime) override;
};
