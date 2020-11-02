// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Animation/WRAnimInstance.h"
#include "Component/IK/WRSpiderLegIKComponent.h"
#include "WRIKAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRIKAnimInstance : public UWRAnimInstance
{
	GENERATED_BODY()
public:
	UWRIKAnimInstance();
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:
	//! IK Component Init
	void InitIKFootRef();

	//! IK Component Tick
	void TickIKFoot(float DeltaSeconds);

	//! IK Component Ref 
	UPROPERTY()
		class UWRSpiderLegIKComponent* InSpiderLegComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKFoot Value", meta = (AllowPrivateAccess = "true"))
		bool IsInitialize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKFoot Value", meta = (AllowPrivateAccess = "true"))
		bool IsCanUse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKFoot Value", meta = (AllowPrivateAccess = "true"))
		bool IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKFoot Value", meta = (AllowPrivateAccess = "true"))
		FWRFablikLegTransforms InLegTransforms;

public:
	void SetIsMoving(bool InIsMoving)
	{
		if (IsCanUse == true)
			IsMoving = InIsMoving;
	}
};
