// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../WREnvActor.h"
#include "Array.h"
#include "WRSpiderWeb.generated.h"

/**
 *
 */
UCLASS()
class WR_API AWRSpiderWeb : public AWREnvActor
{
	GENERATED_BODY()
private:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	class UStaticMeshComponent* StaticMeshComp = nullptr;
	class UMaterialInstanceDynamic* DynamicMaterial = nullptr;


	float CurrentDissolve = 0.0f;

	bool IsTangled = false;
	bool IsKickOffStart = false;

private:
	void InitializeWeb();
	void RemoveWeb();

public:
	FORCEINLINE	void KickOffStart() { IsKickOffStart = true; };
	FORCEINLINE bool GetIsTangled()
	{
		bool ReturnIsTangled = IsTangled;
		IsTangled = IsTangledOnce;
		return ReturnIsTangled;
	}
	FORCEINLINE bool IsExceptCharacter(uint32 InTargetTID) { return ExceptTIDs.Contains(InTargetTID); }
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		TSoftObjectPtr<USoundCue> kickOffSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		TSoftObjectPtr<USoundCue> TangledSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		float IncreaseDissolveSize = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		float TangledDuration = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		bool IsTangledOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpiderWebInfo)
		TSet<int32> ExceptTIDs;
};
