// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Base/WRActorComponent.h"

#include "WRActorComponentStereoLayerManager.generated.h"

class UWRStereoLayerComponent;
/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentStereoLayerManager : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void OnCreateComponent()	override;
	virtual void OnDestroyComponent()	override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	void SetEnableTick(UWRStereoLayerComponent* InStereoLayer, bool bInEnable);
	void SetEnableTick(FString Name, bool bInEnable);

	UWRStereoLayerComponent* CreateStereoLayer(FString Name, bool bInEnableTick = false);
	void DestroyStereoLayer(FString Name, bool bInRemoveFromContainer = true);

	void PlayStereoLayerMovie(int32 InMovieTid, const TArray<int32>& InTriggerIDs);
	void PlayStereoLayer(class UTexture* InSourceTexture, float InMaskTime, float InMaskValue);
	void StopStereoLayer(float InMaskTime);

	bool InterpolateMask(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void OnEndReached_Movie();

	UPROPERTY()
	class UTextureRenderTarget2D*	RenderTarget = nullptr;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	UPROPERTY()
	class UMediaTexture* MediaTexture = nullptr;

	class UMaterialInterface* Material = nullptr;
	
	TMap<FString, UWRStereoLayerComponent*> StereoLayers;
	FDelegateHandle		MaskHandle;
	float				MaskTime = 0.f;
	float				MaskStartValue = 0.f;
	float				MaskEndValue = 0.f;
	
	float ElapsedTime = 0.f;

private:
	TArray<int32> TriggerIDs;

};

