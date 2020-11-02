// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/Base/WRActorComponent.h"
#include "WRActorComponent360Movie.generated.h"

UENUM(BlueprintType)
enum class EWR360MovieEvent : uint8
{
	StartPlay,
	EndPlay,
};

DECLARE_DELEGATE_OneParam(FWR360MovieDelegate, EWR360MovieEvent);

DECLARE_DELEGATE(FWROnEndDirection);

/**
 * 
 */
UCLASS()
class WR_API UWRActorComponent360Movie : public UWRActorComponent
{
	GENERATED_BODY()

public:

	bool Play(class UWRPlay360VideoAsset* InAsset, FWR360MovieDelegate InChangeStateDelegate);
	void Pause();
	void Resume();
	void Stop();

	bool IsPlaying() const { return SphereForPlay != nullptr; }

protected:

	virtual void BeginPlay() override;

	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

private:
	
	bool PlayMovie();
	bool TickDirection(float InDeltaTime);
	void StartDirection(class UMaterialInstance* InMaterial, const FName InParameterName, class UCurveFloat* InDirectionCurve, FWROnEndDirection InOnEndDirection);

	bool IsPossibleCreateDirection() const;
	bool IsPossibleLoadingDirection() const;

	UFUNCTION()
	void OnEndCreateSphereDirection();

	UFUNCTION()
	void OnEndLoadingDirection();

	UFUNCTION()
	void OnEndReached();

	void ReleasePlayMovieData();
	void ReleaseDirectionData();

private:

	UPROPERTY(VisibleAnywhere, Category="MediaPlayer")
	class UMediaPlayer* MediaPlayer = nullptr;

	class UWRPlay360VideoAsset* Play360VideoAsset = nullptr;

	class UStaticMeshComponent* SphereForPlay = nullptr;
	class UMediaSoundComponent* MediaSoundComponent = nullptr;

	FWR360MovieDelegate OnChangeStateMovie;

	// [ 2020-3-23 : magedoga ] Need for directing variables
	class UCurveFloat*	DirectionCurve = nullptr;
	FName ParameterName;
	FWROnEndDirection OnEndDirection;
	FDelegateHandle DirectTickHandle;
	float AccDirectTime = 0.0f;

	FString AsyncLoadKey = "";
	
};

// For Header
DECLARE_LOG_CATEGORY_EXTERN(WRPlay360Video, Display, All);