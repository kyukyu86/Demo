// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Singleton/DMSingleton.h"



class THIRDPERSON_API DMDebugManager : public DMSingleton<DMDebugManager>
{
private:
	void MakeDebugString(FString& OUT OutString);

public:
	DMDebugManager();
	virtual ~DMDebugManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;
	virtual bool OnTick(float InDeltaTime) override;
};
