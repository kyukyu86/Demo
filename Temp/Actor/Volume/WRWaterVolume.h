// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Volume/WRVolume.h"
#include "Engine/Scene.h"
#include "Interfaces/Interface_PostProcessVolume.h"
//#include "Engine/Scene.h"
#include "WRWaterVolume.generated.h"

// USTRUCT(BlueprintType)
// struct FWRWaterVolumeEffect
// {
// 	GENERATED_BODY()
// public:
// 
// 
// };

// USTRUCT(BlueprintType)
// struct FWRWaterFX
// {
// 	GENERATED_USTRUCT_BODY()
// 
// public:
// 
// 	//FWRWaterFX() {}
// 	
// 	UPROPERTY(EditAnywhere)
// 	TSubclassOf<class AWRVFX>	VFX = nullptr;
// 
// 	UPROPERTY(EditAnywhere)
// 	TAssetPtr<class USoundCue>	SFX = nullptr;
// };

/**
 * 
 */
UCLASS(meta=(DisplayName="WR Water Volume"), hidecategories = (Collision))
class WR_API AWRWaterVolume : public AWRVolume, public IInterface_PostProcessVolume
{
	GENERATED_UCLASS_BODY()

public:
	//====================================================================================
	// [ 2020-2-27 : magedoga ] Water Setting Values
	UPROPERTY(EditAnywhere, Category = "WRWater | Fog")
	class AExponentialHeightFog*	WaterFogActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWater | PostProcess")
	FPostProcessSettings	WaterPostProcess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWater | PostProcess")
	uint32 bEnabled : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWater | PostProcess")
	float Priority;

	UPROPERTY(interp, Category = "WRWater | PostProcess", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "6000.0"))
	float BlendRadius;

	UPROPERTY(interp, Category = "WRWater | PostProcess", BlueprintReadWrite, meta = (UIMin = "0.0", UIMax = "1.0"))
	float BlendWeight;

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRWater | PostProcess", meta = (DisplayName = "Infinite Extent (Unbound)"))
// 	uint32 bUnbound : 1;

	UPROPERTY(EditAnywhere, Category = "WRWater | FX")
	TAssetPtr<class UReverbEffect>	UnderWaterReverbEffect;

	//UPROPERTY(EditAnywhere, Category = "WRWater | FX")
	//TAssetPtr<class USoundCue>	UnderWaterSound;

	UPROPERTY(EditAnywhere, Category = "WRWater | FX", meta=(Tooltip="Spawn FX when inside water."))
	TSubclassOf<class AWRVFX>	UnderWaterFX;

	UPROPERTY(EditAnywhere, Category = "WRWater | FX")
	TSubclassOf<class AWRVFX>	EnterWaterFX;

	UPROPERTY(EditAnywhere, Category = "WRWater | FX")
	TSubclassOf<class AWRVFX>	LeaveWaterFX;

	UPROPERTY(EditAnywhere, Category = "WRWater | FX")
	TAssetPtr<UCurveFloat>	ScreenWaterDirectCurveAsset;
	//====================================================================================
	

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent) override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	void SetEnableTick(const bool bInEnable);
#endif //WITH_EDITOR

	virtual bool EncompassesPoint(FVector InPoint, float InSphereRadius/*=0.f*/, float* OutDistanceToPoint) override;
	virtual FPostProcessVolumeProperties GetProperties() const override
	{
		FPostProcessVolumeProperties PPProperty;
		PPProperty.bIsEnabled = bEnabled != 0;
		PPProperty.bIsUnbound = false;
		PPProperty.BlendRadius = BlendRadius;
		PPProperty.BlendWeight = BlendWeight;
		PPProperty.Priority = Priority;
		PPProperty.Settings = &WaterPostProcess;

		return PPProperty;
	}
	//====================================================================================

protected:
	virtual void PostRegisterAllComponents() override;
	virtual void PostUnregisterAllComponents(void) override;

private:

	void OnEnter();
	void OnLeave();

	void ProcessCommon(const bool bInEnter);

	bool TickScreenWater(float InDeltaTime);
	void EndScreenWaterDirect();

private:

	UPROPERTY(VisibleAnywhere, Category = "Info", meta=(Tooltip="only debugging"))
	bool bEntering = false;

	FDelegateHandle	HandleTickScreenWater;
	float AccScreenWaterDirectTime = 0.0f;

	class UAudioComponent* UnderWaterAudioComponent;

	class UCurveFloat* ScreenWaterCurve;

#if WITH_EDITOR
	bool bTempActivatedReverb = false;
#endif // WITH_EDITOR
};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRWaterVolume, Display, All);