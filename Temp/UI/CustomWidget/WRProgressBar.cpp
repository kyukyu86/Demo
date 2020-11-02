// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRProgressBar.h"

void UWRProgressBar::OnConstruct()
{

}

void UWRProgressBar::OnDestruct()
{

}

void UWRProgressBar::OnTick(float IN InDeltaTime)
{
	if (UseTimer == false)
		return;

	if (fProgressGoalTime == 0.f || fProgressTime >= fProgressGoalTime)
		return;

	fProgressTime += InDeltaTime;
	if (fProgressTime >= fProgressGoalTime)
	{
		SetPercent(1.f);
		OnProgressCompleted.ExecuteIfBound();		
	}
	else
	{
		float fPercent = fProgressTime / fProgressGoalTime;
		if (bStep)
		{
			float ConvertPercent = int32(fPercent / (1.f / StepNum)) * (1.f / StepNum);
			SetPercent(ConvertPercent);
		}
		else
		{
			SetPercent(fPercent);
		}
	}
}

void UWRProgressBar::Clear()
{
	UseTimer = false;
	fProgressTime = 0.f;
	fProgressGoalTime = 0.f;
	OnProgressCompleted = nullptr;
}

// Timer

void UWRProgressBar::SetTimer(const float IN InGoalTimeMS, FWROnProgressCompleted IN InDelegate /*= nullptr*/, const int32 IN InStepNum /*= 0*/)
{
	UseTimer = true;
	fProgressTime = 0.f;
	fProgressGoalTime = InGoalTimeMS / 1000.f;
	OnProgressCompleted = InDelegate;
	
	SetStep(InStepNum);
}

void UWRProgressBar::SetTimer(const float IN InCurTimeMS, const float IN InGoalTimeMS, FWROnProgressCompleted IN InDelegate /*= nullptr*/, const int32 IN InStepNum /*= 0*/)
{
	UseTimer = true;
	fProgressTime = InCurTimeMS / 1000.f;
	fProgressGoalTime = InGoalTimeMS / 1000.f;
	OnProgressCompleted = InDelegate;

	SetStep(InStepNum);
}

void UWRProgressBar::StopTimer()
{
	UseTimer = false;
}

void UWRProgressBar::ResumeTimer()
{
	UseTimer = true;
}

void UWRProgressBar::ResetTimer()
{
	UseTimer = true;
	fProgressTime = 0.f;
}

// Normal

void UWRProgressBar::SetProgress(const float IN InPercent)
{
	UseTimer = false;
	SetPercent(InPercent);
}

void UWRProgressBar::SetStep(const int32 IN InStepNum /*= 10*/)
{
	if (InStepNum <= 0)
		return;

	bStep = true;
	StepNum = InStepNum;
}
