// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRBaseComponentManageMaterial.h"

void UWRBaseComponentManageMaterial::OnCreateComponent()
{
	Super::OnCreateComponent();
}

void UWRBaseComponentManageMaterial::OnDestroyComponent()
{
	Super::OnDestroyComponent();

	// [ 2019-10-15 : animal14 ] 언바인딩
	for (auto& Set : Sets)
	{
		Set.Unbind();
	}
	Sets.Empty();

	// [ 2019-10-15 : animal14 ] 
	Completes.Empty();
	Processes.Empty();
}

void UWRBaseComponentManageMaterial::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->Process(DeltaTime);
}

void UWRBaseComponentManageMaterial::Spawn(const EWRManageMaterial InKey, const float InDuration)
{
	// [ 2020-4-2 : animal14 ] 생성 연출이 시작하면 틱 시작
	this->SetEnableTick(true);

	FWRProcess Value;
	Value.DeltaTime = 0.0F;
	Value.Duration = InDuration;
	Value.IsReverse = false;

	Processes.Add(InKey, Value);
}

void UWRBaseComponentManageMaterial::Remove(const EWRManageMaterial InKey, const float InDuration)
{
	// [ 2020-4-2 : animal14 ] 삭제 연출이 시작하면 틱 시작
	this->SetEnableTick(true);

	FWRProcess Value;
	Value.DeltaTime = 0.0F;
	Value.Duration = InDuration;
	Value.IsReverse = true;

	Processes.Add(InKey, Value);
}

FWRComplete& UWRBaseComponentManageMaterial::OnComplete(const EWRManageMaterial& InKey)
{
	if (Completes.Contains(InKey) == false)
	{
		FWRComplete Complete;

		Completes.Add(InKey, Complete);
	}
	return Completes[InKey];
}

void UWRBaseComponentManageMaterial::Remove_Internal(const EWRManageMaterial InKey)
{
	// [ 2020-1-30 : animal14 ] 
	if (Processes.Contains(InKey) == false)
	{
		return;
	}
	Processes.Remove(InKey);

	// [ 2020-4-2 : animal14 ] 등록된 연출이 없다면 틱을 멈충
	if (Processes.Num() == 0)
	{
		this->SetEnableTick(false);
	}

	// [ 2019-10-30 : animal14 ] 완료 콜백
	if (Completes.Contains(InKey) == false)
	{
		return;
	}
	Completes[InKey].Broadcast();
}

void UWRBaseComponentManageMaterial::Process(float DeltaTime)
{
	for (int32 i = 0; i < (int32)EWRManageMaterial::Max; ++i)
	{
		EWRManageMaterial Key = (EWRManageMaterial)i;
		if (Processes.Contains(Key) == false)
		{
			continue;
		}

		Processes[Key].DeltaTime += DeltaTime;

		float Duration = Processes[Key].Duration;
		float AccDeltaTime = Processes[Key].DeltaTime;

		bool IsReverse = Processes[Key].IsReverse;

		if (AccDeltaTime > Duration + DeltaTime)
		{
			this->Remove_Internal(Key);
			continue;
		}

		float Ratio = FMath::Clamp(AccDeltaTime / Duration, 0.0F, 1.0F);

		// [ 2019-10-15 : animal14 ] 실행
		IsReverse ? Sets[i].ExecuteIfBound(Ratio) : Sets[i].ExecuteIfBound(1.0F - Ratio);
	}
}