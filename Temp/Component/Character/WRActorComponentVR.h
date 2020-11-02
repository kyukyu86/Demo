// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Base/WRActorComponent.h"

#include "WRActorComponentVR.generated.h"

enum class EWRCameraOffset : uint8
{
	None,	// 해제
	Add,	// 추가
	New,	// 신규
	Replace, // 덮어쓰자
};

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentVR : public UWRActorComponent
{
	GENERATED_BODY()

private:
	FVector BeforeCameraOffset = FVector::ZeroVector;

public:	
	virtual void OnCreateComponent()	override;
	virtual void OnDestroyComponent()	override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddRotationYaw(const float InDeltaYaw, const float InScale);
	void SetBaseOrientationRotator(const FRotator& InRotator);
	FRotator GetBaseOrientationRotator() const;
	// [ 2020-9-8 : magedoga ] HMD - Character
	FRotator GetHMDRelativeRotator(const bool bInUseOnlyInputSystem = false) const;

	// [ 2020-9-8 : magedoga ]
	// @ InYaw : 초기화 시킬 World 축 기준 Yaw
	void ResetOrientation(const float InYaw, const bool bInUseOnlyInputSystem = false);

	// [ 2020-4-24 : kyu ] 
	void SetCameraOffset(const EWRCameraOffset IN InType, const FVector IN InOffset = FVector::ZeroVector);
	// [ 2020-9-4 : magedoga ] PlayerTable로 Camera Offset 초기화
	void InitCameraOffsetByPlayerTable();
};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRActorComponentVR, Display, All);
