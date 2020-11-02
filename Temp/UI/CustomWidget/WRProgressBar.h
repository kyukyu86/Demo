// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "WRProgressBar.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FWROnProgressCompleted)

UCLASS()
class WR_API UWRProgressBar : public UProgressBar
{
	GENERATED_BODY()
	
private:
	bool UseTimer = false;
	float fProgressTime = 0.f;
	float fProgressGoalTime = 0.f;
	FWROnProgressCompleted OnProgressCompleted;
	bool bStep = false;
	int32 StepNum = 0;

public:
	void OnConstruct();
	void OnDestruct();
	void OnTick(float IN InDeltaTime);
	void Clear();
	// TImer
	void SetTimer(const float IN InGoalTimeMS, FWROnProgressCompleted IN InDelegate = nullptr, const int32 IN InStepNum = 0); // ms
	void SetTimer(const float IN InCurTimeMS, const float IN InGoalTimeMS, FWROnProgressCompleted IN InDelegate = nullptr, const int32 IN InStepNum = 0); // ms
	void StopTimer();
	void ResumeTimer();
	void ResetTimer();
	// Normal
	void SetProgress(const float IN InPercent);
private:
	void SetStep(const int32 IN InStepNum);
};
