// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "UI/Base/WRUISlot.h"
#include "WRUISlot_TargetLocationGuide.generated.h"

struct FWRTargetLocationInfoBase;
class UImage;
UCLASS()
class WR_API UWRUISlot_TargetLocationGuide : public UWRUISlot
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
public:
	virtual void SetTargetLocationInfo(const FWRTargetLocationInfoBase* IN InTargetLocationInfo);
	FORCEINLINE void SetRadius(const float IN InRadius) { Radius = InRadius; }
private:
	void Tick_UpdatePosition();
	float GetDegreesBetween2Vectors(const FVector& IN InToVector, const FVector& IN InFromVector, const FVector& IN InBaseVector);
	bool IsABehindB(const FVector& IN InAPosition, const FVector& IN InBPosition, const FVector& IN InBFowardVector);	// A가 B 뒤에 있나요?
protected:
	UPROPERTY(EditAnywhere)
		float ArrowRadius = 100;
	UPROPERTY(EditAnywhere)
		float ArrowRadiusOffset = 15;
	UPROPERTY(EditAnywhere)
		float ArrowAnimationTime = 0.2f;

	UImage* Image_Arrow;
	const FWRTargetLocationInfoBase* TargetLocationInfoBase;
	float CurrentArrowRadiusOffset = 0.f;
	float ArrowAnimationFlowTime = 0.f;
	float Radius;
};