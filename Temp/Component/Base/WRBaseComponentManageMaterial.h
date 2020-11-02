// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRBaseComponentManageMaterial.generated.h"

DECLARE_MULTICAST_DELEGATE(FWRComplete);

enum class EWRManageMaterial : uint8
{
	HeavyAttack,
	//SkillAttack,
	Weakness,
	Hit,
	Spawn,
	Dissolve,
	Drunk,
	Contruct,
	ChangeCloth,
	Max,
};

struct FWRProcess
{
	float DeltaTime;
	float Duration;

	bool IsReverse;
};

/**
 * 
 */
UCLASS()
class WR_API UWRBaseComponentManageMaterial : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Spawn(const EWRManageMaterial InKey, const float InDuration);
	void Remove(const EWRManageMaterial InKey, const float InDuration);

	FWRComplete& OnComplete(const EWRManageMaterial& InKey);

protected:
	void Remove_Internal(const EWRManageMaterial InKey);

	void Process(float DeltaTime);

protected:
	DECLARE_DELEGATE_OneParam(FWRTick, float);
	TArray<FWRTick> Sets;

	TMap<EWRManageMaterial, FWRComplete> Completes;
	TMap<EWRManageMaterial, FWRProcess> Processes;
};
