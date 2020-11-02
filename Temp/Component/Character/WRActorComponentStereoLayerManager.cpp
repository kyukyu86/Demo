// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRActorComponentStereoLayerManager.h"
#include "Manager/WRCharacterManager.h"
#include <Camera/CameraComponent.h>
#include "Actor/Character/WRCharacterMine.h"
#include <Kismet/KismetRenderingLibrary.h>
#include "../Custom/WRStereoLayerComponent.h"
#include <Materials/MaterialInstanceDynamic.h>
#include <Ticker.h>
#include <UnrealMathUtility.h>
#include <MediaPlayer.h>
#include <MediaSource.h>
#include <MediaTexture.h>
#include "Table/WRMovieTable.h"
#include "Table/Base/WRTableManager.h"
#include "Manager/WRTriggerManager.h"
#include <Engine/Texture.h>
#include <Materials/MaterialInterface.h>
#include "Manager/WRConsoleManager.h"

void UWRActorComponentStereoLayerManager::OnCreateComponent()
{
	Super::OnCreateComponent();
}

void UWRActorComponentStereoLayerManager::OnDestroyComponent()
{
	for (auto StereoLayer : StereoLayers)
	{
		DestroyStereoLayer(StereoLayer.Key);
	}
	StereoLayers.Empty();

	Super::OnDestroyComponent();
}

void UWRActorComponentStereoLayerManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (RenderTarget && DynamicMaterial)
	{
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, DynamicMaterial);
	}
}

void UWRActorComponentStereoLayerManager::DestroyComponent(bool bPromoteChildren /*= false*/)
{
	for (auto StereoLayer : StereoLayers)
	{
		DestroyStereoLayer(StereoLayer.Key, false);
	}
	StereoLayers.Empty();

	Super::DestroyComponent();
}

void UWRActorComponentStereoLayerManager::SetEnableTick(UWRStereoLayerComponent* InStereoLayer, bool bInEnable)
{
	if (bInEnable == true)
	{
		if (InStereoLayer->PrimaryComponentTick.IsTickFunctionRegistered() == false)
		{
			InStereoLayer->PrimaryComponentTick.RegisterTickFunction(InStereoLayer->GetWorld()->GetCurrentLevel());
			InStereoLayer->PrimaryComponentTick.Target = this;
		}
	}

	InStereoLayer->PrimaryComponentTick.bCanEverTick = bInEnable;
	InStereoLayer->PrimaryComponentTick.SetTickFunctionEnable(bInEnable);

	if (bInEnable == false)
	{
		if (InStereoLayer->PrimaryComponentTick.IsTickFunctionRegistered() == true)
		{
			InStereoLayer->PrimaryComponentTick.UnRegisterTickFunction();
		}
	}
}

void UWRActorComponentStereoLayerManager::SetEnableTick(FString Name, bool bInEnable)
{
	if (StereoLayers.Contains(Name) == true)
	{
		SetEnableTick(StereoLayers[Name], bInEnable);
	}
}

UWRStereoLayerComponent* UWRActorComponentStereoLayerManager::CreateStereoLayer(FString Name, bool bInEnableTick/* = false*/)
{
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (/*MyChar && Cast<AActor>(MyChar) == */GetOwner())
	{
		if (StereoLayers.Contains(Name) == true)
		{
			return StereoLayers[Name];
		}
		UCameraComponent* MyCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
		if (MyCamera)
		{
			UWRStereoLayerComponent* StereoLayerComponent = NewObject<UWRStereoLayerComponent>(GetOwner());
			if (bInEnableTick == true)
			{
				SetEnableTick(StereoLayerComponent, bInEnableTick);
			}
			StereoLayerComponent->AttachToComponent(MyCamera, FAttachmentTransformRules::KeepRelativeTransform);
			
			StereoLayerComponent->AddToRoot();
			GetOwner()->AddOwnedComponent(StereoLayerComponent);
			GetOwner()->AddInstanceComponent(StereoLayerComponent);
			StereoLayerComponent->RegisterComponent();
			
			

			StereoLayers.Add(Name, StereoLayerComponent);

			return StereoLayerComponent;
		}
		
	}
	return nullptr;
}

void UWRActorComponentStereoLayerManager::DestroyStereoLayer(FString Name, bool bInRemoveFromContainer /*= true*/)
{
	if (StereoLayers.Contains(Name) == true)
	{
		if (StereoLayers[Name]->IsRegistered() == true)
			StereoLayers[Name]->UnregisterComponent();

		StereoLayers[Name]->RemoveFromRoot();

		GetOwner()->RemoveOwnedComponent(StereoLayers[Name]);
		StereoLayers[Name]->DestroyComponent();

		if(bInRemoveFromContainer == true)
			StereoLayers.Remove(Name);
	}
}

void UWRActorComponentStereoLayerManager::PlayStereoLayerMovie(int32 InMovieTid, const TArray<int32>& InTriggerIDs)
{
	FWRMovieTable* MovieTable = WRTableManager::Get()->FindRow<FWRMovieTable>(InMovieTid);
	if (MovieTable != nullptr)
	{
		FString StereoLayerMediaTexturePath = TEXT("/Game/Movie/MediaTexture/StereoLayerMediaTexture.StereoLayerMediaTexture");

		UMediaSource* MediaSource = LoadObject<UMediaSource>(nullptr, *MovieTable->MediaSource);
		MediaTexture = LoadObject<UMediaTexture>(nullptr, *(StereoLayerMediaTexturePath));


		if (MediaSource->IsValidLowLevel() == true && MediaTexture->IsValidLowLevel() == true)
		{
			if (MediaTexture->GetMediaPlayer()->IsValidLowLevel() == true && MediaSource->IsValidLowLevel() == true)
			{
				MediaTexture->GetMediaPlayer()->OpenSource(MediaSource);
				MediaTexture->GetMediaPlayer()->OnEndReached.AddDynamic(this, &UWRActorComponentStereoLayerManager::OnEndReached_Movie);

				TriggerIDs = InTriggerIDs;
				//PlayStereoLayer(MediaTexture, MovieTable->Time, MovieTable->Value);
			}
		}
	}
	
}

void UWRActorComponentStereoLayerManager::PlayStereoLayer(UTexture* InSourceTexture, float InMaskTime, float InMaskValue)
{
	if (GetOwner())
	{
		UWRStereoLayerComponent* StereoLayerCube = CreateStereoLayer("CubeMap", true);
		UWRStereoLayerComponent* StereoLayerSource = CreateStereoLayer("Source", true);

		StereoLayerCube->SetStereoLayerShape(SLSH_CubemapLayer);
		StereoLayerCube->SetStereoLayerType(SLT_TrackerLocked);

		StereoLayerSource->bLiveTexture = true;
		StereoLayerSource->SetStereoLayerType(SLT_TrackerLocked);
		StereoLayerSource->SetStereoLayerShape(SLSH_CylinderLayer);
		StereoLayerSource->SetPriority(1);

		FRotator MyRotation = GetOwner()->GetActorRotation();
		FVector RelativeLocation = FVector::ZeroVector;
		RelativeLocation.Z += 100;

 		UCameraComponent* CameraComponent = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
 		if (CameraComponent)
 		{
 			RelativeLocation = CameraComponent->GetRelativeTransform().GetLocation();
 		}
 
 		// [ 2019-10-16 : hansang88 ] 크기조절
 		
		float MovieDistance = WRConsoleManager::Get()->GetFloat(EWRConsole::HMD_Movie_Distance);
 		RelativeLocation += GetOwner()->GetActorForwardVector() * -MovieDistance;

		StereoLayerSource->SetRelativeLocation(RelativeLocation);
		StereoLayerSource->SetRelativeRotation(MyRotation);

		FString BlackTexturePath = TEXT("/Game/Asset/BG/Env_Backgrounds/Textures/black.black");
		UTexture* BlackTexture = LoadObject<UTexture>(NULL, *BlackTexturePath);
		StereoLayerCube->SetTexture(BlackTexture);

		FString TargetTexturePath = TEXT("/Game/Asset/UI/SceneCapture/Texture/StereoLayerTextureRenderTarget2D.StereoLayerTextureRenderTarget2D");
		RenderTarget = LoadObject<UTextureRenderTarget2D>(NULL, *TargetTexturePath);
		StereoLayerSource->SetTexture(RenderTarget);

		FString MaterialPath = TEXT("/Game/Asset/Shader/PostProcess/M_ViewMask.M_ViewMask");
		Material = LoadObject<UMaterialInterface>(NULL, *MaterialPath);
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);

		DynamicMaterial->SetTextureParameterValue("ViewTexture", InSourceTexture);
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, DynamicMaterial);

		MaskTime = InMaskTime;
		MaskStartValue = 1.5f;
		MaskEndValue = InMaskValue;
		ElapsedTime = 0.f;

		MaskHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UWRActorComponentStereoLayerManager::InterpolateMask));
	}
}

void UWRActorComponentStereoLayerManager::StopStereoLayer(float InMaskTime)
{
	MaskTime = InMaskTime;
	MaskStartValue = MaskEndValue;
	MaskEndValue = 1.5f;
	ElapsedTime = 0.f;

	MaskHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UWRActorComponentStereoLayerManager::InterpolateMask));
}

bool UWRActorComponentStereoLayerManager::InterpolateMask(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float ResultValue = FMath::Lerp(MaskStartValue, MaskEndValue, FMath::Pow(FMath::Clamp(ElapsedTime, 0.f, MaskTime) / MaskTime, 0.5f));
	DynamicMaterial->SetScalarParameterValue("MaskValue", ResultValue);
	if (ElapsedTime >= MaskTime)
	{
		if (MaskStartValue > MaskEndValue)
		{
			DestroyStereoLayer(TEXT("CubeMap"));
			DestroyStereoLayer(TEXT("Source"));
		}
		FTicker::GetCoreTicker().RemoveTicker(MaskHandle);
		MaskHandle.Reset();
	}

	return true;
}

void UWRActorComponentStereoLayerManager::OnEndReached_Movie()
{
	if (MediaTexture->IsValidLowLevel() == true && MediaTexture->GetMediaPlayer()->IsValidLowLevel() == true)
	{
		MediaTexture->GetMediaPlayer()->Close();
		MediaTexture->GetMediaPlayer()->OnEndReached.RemoveAll(this);
		MediaTexture = nullptr;
	}

	DestroyStereoLayer(TEXT("CubeMap"));
	DestroyStereoLayer(TEXT("Source"));

	for (int i = 0; i < TriggerIDs.Num(); ++i)
	{
		WRTriggerManager::Get()->RunActiveTrigger(TriggerIDs[i]);
	}

	TriggerIDs.Empty();
}

