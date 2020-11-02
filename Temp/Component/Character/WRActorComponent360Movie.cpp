// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponent360Movie.h"
#include "CustomAsset/WRPlay360VideoAsset.h"
#include "MediaPlayer.h"
#include "Actor/Character/WRCharacterMine.h"
#include "Utility/WRActorUtility.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Ticker.h"
#include "MediaSoundComponent.h"
#include "Camera/CameraComponent.h"
#include "Manager/WRSFXManager.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRAsyncLoadUObjectManager.h"

// For Cpp
DEFINE_LOG_CATEGORY(WRPlay360Video)

#define MEDIAPLAYER_FOR_360MOVIE_PATH TEXT("/Game/Movie/MediaPlayer/360MoviePlayer.360MoviePlayer")
#define CREATE_SPHERE_MATERIAL_PARAMETER_NAME "VR360Spawn"
#define LOADING_DIRECTION_MATERIAL_PARAMETER_NAME "VR360Loading"

void UWRActorComponent360Movie::BeginPlay()
{
	Super::BeginPlay();
}

void UWRActorComponent360Movie::OnCreateComponent()
{
	Super::OnCreateComponent();

	if (AsyncLoadKey.IsEmpty() == true)
	{
		AsyncLoadKey = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MEDIAPLAYER_FOR_360MOVIE_PATH, FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InHash) {

			MediaPlayer = Cast<UMediaPlayer>(InObject);
			checkf(MediaPlayer != nullptr, TEXT("Null MediaPlayer : %s"), MEDIAPLAYER_FOR_360MOVIE_PATH);
			AsyncLoadKey = "";

		}));
	}
}

void UWRActorComponent360Movie::OnDestroyComponent()
{
	Super::OnDestroyComponent();

	if (DirectTickHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(DirectTickHandle);
		DirectTickHandle.Reset();

		this->ReleaseDirectionData();
	}

	this->OnEndReached();

	if (MediaPlayer->IsValidLowLevel() == true)
	{
		MediaPlayer->RemoveFromRoot();
	}

	if (AsyncLoadKey.IsEmpty() == false)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(AsyncLoadKey);
		AsyncLoadKey = "";
	}

	MediaPlayer = nullptr;
}



bool UWRActorComponent360Movie::Play(class UWRPlay360VideoAsset* InAsset, FWR360MovieDelegate InChangeStateDelegate)
{
	if (MediaPlayer->IsValidLowLevel() == false)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::Play - Null MediaPlayer."));
		return false;
	}

	// [ 2020-3-23 : magedoga ] Playing currently?
	if (MediaPlayer->IsPlaying() == true)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::Play - Playing already."));
		return false;
	}

	if (InAsset->MediaSource.IsNull() == true)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::Play - Null Media Source."));
		return false;
	}

	AWRCharacterMine* MyCharacter = Cast<AWRCharacterMine>(this->GetOwnerCharacter());
	if (MyCharacter == nullptr)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::Play - Owner Character is not AWRCharacterMine type."));
		return false;
	}

	UCameraComponent* CameraComponent = Cast<UCameraComponent>(MyCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CameraComponent == nullptr)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::Play - Null FlippedNormalSphere."));
		return false;
	}

	// [ 2020-3-23 : magedoga ] Get flipped sphere for play 360 movie from AWRCharacterMine
	SphereForPlay = MyCharacter->GetSphereFlippedNormalComponent();
	if (SphereForPlay == nullptr)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::Play - Null FlippedNormalSphere."));
		return false;
	}

	Play360VideoAsset = InAsset;
	
	// [ 2020-3-23 : magedoga ] Set Translucency Priority
	SphereForPlay->TranslucencySortPriority = 1000;

	// [ 2020-3-23 : magedoga ] Set scale
	SphereForPlay->SetWorldScale3D(FVector(InAsset->SphereMeshScale));

	// [ 2020-3-24 : magedoga ] Create & Attach MediaSoundComponent
	MediaSoundComponent = NewObject<UMediaSoundComponent>(MyCharacter);
	MediaSoundComponent->RegisterComponentWithWorld(MyCharacter->GetWorld());
	MediaSoundComponent->AttachToComponent(CameraComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	MediaSoundComponent->SetMediaPlayer(MediaPlayer);

	// [ 2020-3-23 : magedoga ] Check direction for create sphere
	if (this->IsPossibleCreateDirection() == true)
	{
		this->StartDirection(Play360VideoAsset->CreateSphereMaterial.LoadSynchronous(),
							 CREATE_SPHERE_MATERIAL_PARAMETER_NAME,
							 Play360VideoAsset->CreateSphereCurve.LoadSynchronous(),
							 FWROnEndDirection::CreateUObject(this, &UWRActorComponent360Movie::OnEndCreateSphereDirection));

		if (Play360VideoAsset->CreateDirectionSound.IsNull() == false)
		{
			WRSFXManager::Get()->SpawnAttached(Play360VideoAsset->CreateDirectionSound.LoadSynchronous(), CameraComponent);
		}
	}
	// [ 2020-3-23 : magedoga ] check direction for loading, if nothing create sphere direction
	else if (this->IsPossibleLoadingDirection() == true)
	{
		this->StartDirection(Play360VideoAsset->PlayMovieMaterial.LoadSynchronous(),
							 LOADING_DIRECTION_MATERIAL_PARAMETER_NAME,
							 Play360VideoAsset->LoadingCurve.LoadSynchronous(),
							 FWROnEndDirection::CreateUObject(this, &UWRActorComponent360Movie::OnEndLoadingDirection));
	}
	// [ 2020-3-23 : magedoga ] Nothing any direction
	else
	{
		this->PlayMovie();
	}

	SphereForPlay->SetVisibility(true);

	FRotator OwnerRotation = MyCharacter->GetActorRotation();
	SphereForPlay->SetWorldRotation(FRotator(OwnerRotation.Pitch, OwnerRotation.Yaw - 80.0f, OwnerRotation.Roll));
	//SphereForPlay->SetRelativeRotation(FRotator(OwnerRotation.Pitch, OwnerRotation.Yaw - 80.0f, OwnerRotation.Roll));

	OnChangeStateMovie = InChangeStateDelegate;
	OnChangeStateMovie.ExecuteIfBound(EWR360MovieEvent::StartPlay);

	WRInputSystemManager::Get()->SetLocationLock(true);
	
	return true;
}

void UWRActorComponent360Movie::Pause()
{
	if (MediaPlayer->IsPlaying() == false)
		return;

	MediaPlayer->Pause();
}

void UWRActorComponent360Movie::Resume()
{
	if (MediaPlayer->IsPaused() == false)
		return;
	
	MediaPlayer->Play();
}

void UWRActorComponent360Movie::Stop()
{
	if (MediaPlayer->IsPlaying() == false)
		return;

	this->OnEndReached();
}



bool UWRActorComponent360Movie::PlayMovie()
{
	// [ 2020-3-24 : magedoga ] Set play movie material
	SphereForPlay->SetMaterial(0, Play360VideoAsset->PlayMovieMaterial.LoadSynchronous());

	// [ 2020-3-24 : magedoga ] Bind end event
	MediaPlayer->OnEndReached.AddDynamic(this, &UWRActorComponent360Movie::OnEndReached);

	FRotator OwnerRotation = GetOwner()->GetActorRotation();
	SphereForPlay->SetWorldRotation(FRotator::ZeroRotator);
	SphereForPlay->SetRelativeRotation(FRotator(OwnerRotation.Pitch, OwnerRotation.Yaw - 80.0f, OwnerRotation.Roll));
	SphereForPlay->SetUsingAbsoluteRotation(true);
	SphereForPlay->UpdateComponentToWorld();

	// [ 2020-3-24 : magedoga ] Play Movie
	if (MediaPlayer->OpenSource(Play360VideoAsset->MediaSource.LoadSynchronous()) == false)
	{
		UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::PlayMovie - Open failed [%s]"), *(Play360VideoAsset->MediaSource.GetAssetName()));
		return false;
	}
	
	UE_LOG(WRPlay360Video, Display, TEXT("UWRActorComponent360Movie::PlayMovie - Play Movie [%s]"), *(Play360VideoAsset->MediaSource.GetAssetName()));
	return true;
}


bool UWRActorComponent360Movie::TickDirection(float InDeltaTime)
{
	AccDirectTime += InDeltaTime;
	float Value = DirectionCurve->GetFloatValue(AccDirectTime);

	SET_MATERIAL_PARAMETER(ParameterName, Value, SetScalarParameterValue, SphereForPlay);

	float MinTime, MaxTime;
	DirectionCurve->GetTimeRange(MinTime, MaxTime);
	if (AccDirectTime >= MaxTime)
	{
		OnEndDirection.Execute();
	}

	return true;
}



void UWRActorComponent360Movie::StartDirection(class UMaterialInstance* InMaterial, const FName InParameterName, class UCurveFloat* InDirectionCurve, FWROnEndDirection InOnEndDirection)
{
	if (InMaterial->IsValidLowLevel() == false)
		return;

	if (InDirectionCurve->IsValidLowLevel() == false)
		return;

	if (InOnEndDirection.IsBound() == false)
		return;

	SphereForPlay->SetMaterial(0, InMaterial);

	DirectionCurve = InDirectionCurve;
	OnEndDirection = InOnEndDirection;
	ParameterName = InParameterName;

	AccDirectTime = 0.0f;

	// [ 2020-3-24 : magedoga ] Setting start material parameter value
	SET_MATERIAL_PARAMETER(ParameterName, 0.0f, SetScalarParameterValue, SphereForPlay);


	if (DirectTickHandle.IsValid() == false)
	{
		DirectTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UWRActorComponent360Movie::TickDirection));
	}
}


bool UWRActorComponent360Movie::IsPossibleCreateDirection() const
{
	return Play360VideoAsset->CreateSphereCurve.IsNull() == false && Play360VideoAsset->CreateSphereMaterial.IsNull() == false;
}

bool UWRActorComponent360Movie::IsPossibleLoadingDirection() const
{
	return Play360VideoAsset->LoadingCurve.IsNull() == false;
}

void UWRActorComponent360Movie::OnEndCreateSphereDirection()
{
	// [ 2020-3-24 : magedoga ] Loading direction
	if (this->IsPossibleLoadingDirection() == true)
	{
		this->StartDirection(Play360VideoAsset->PlayMovieMaterial.LoadSynchronous(),
							 LOADING_DIRECTION_MATERIAL_PARAMETER_NAME,
							 Play360VideoAsset->LoadingCurve.LoadSynchronous(),
							 FWROnEndDirection::CreateUObject(this, &UWRActorComponent360Movie::OnEndLoadingDirection));
	}
	// [ 2020-3-24 : magedoga ] Play movie
	else
	{
		ReleaseDirectionData();

		if (this->PlayMovie() == false)
		{
			this->OnEndReached();
		}
	}
}

void UWRActorComponent360Movie::OnEndLoadingDirection()
{
	ReleaseDirectionData();

	if (this->PlayMovie() == false)
	{
		this->OnEndReached();
	}
}

void UWRActorComponent360Movie::OnEndReached()
{
	if (MediaPlayer->IsValidLowLevel() == true)
	{
		MediaPlayer->OnEndReached.RemoveDynamic(this, &UWRActorComponent360Movie::OnEndReached);
		MediaPlayer->Close();
	}

	OnChangeStateMovie.ExecuteIfBound(EWR360MovieEvent::EndPlay);

	ReleasePlayMovieData();

	WRInputSystemManager::Get()->SetLocationLock(false);
}

void UWRActorComponent360Movie::ReleasePlayMovieData()
{
	if (SphereForPlay->IsValidLowLevel() == true)
	{
		SphereForPlay->SetRelativeRotation(FRotator::ZeroRotator);
		SphereForPlay->SetVisibility(false);
		SphereForPlay->SetMaterial(0, nullptr);
		SphereForPlay->SetWorldScale3D(FVector::OneVector);
		SphereForPlay->SetUsingAbsoluteRotation(false);
		SphereForPlay = nullptr;
	}

	if (MediaSoundComponent->IsValidLowLevel() == true)
	{
		MediaSoundComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		MediaSoundComponent->ConditionalBeginDestroy();
		MediaSoundComponent = nullptr;
	}

	Play360VideoAsset = nullptr;
}

void UWRActorComponent360Movie::ReleaseDirectionData()
{
	DirectionCurve = nullptr;
	ParameterName = NAME_None;
	OnEndDirection.Unbind();
	
	if (DirectTickHandle.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(DirectTickHandle);
	}

	DirectTickHandle.Reset();
	AccDirectTime = 0.0f;
}
