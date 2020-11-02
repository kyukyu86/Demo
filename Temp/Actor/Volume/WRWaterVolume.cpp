// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRWaterVolume.h"
#include "Engine/Engine.h"
#if WITH_EDITOR
#include "LevelEditorViewport.h"
#include "Editor.h"
#endif //WITH_EDITOR
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "Engine/ExponentialHeightFog.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Manager/WRCharacterManager.h"
#include "Component/Character/WRActorComponentVFX.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Manager/WRSFXManager.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/WRVFXManager.h"
#include "Curves/CurveFloat.h"
#include "../Character/WRCharacterMine.h"
#include "Sound/ReverbEffect.h"

// For Cpp
DEFINE_LOG_CATEGORY(WRWaterVolume)

AWRWaterVolume::AWRWaterVolume(const FObjectInitializer& ObjectInitializer) : AWRVolume(ObjectInitializer)
{
	// [ 2020-2-26 : magedoga ] Disable Collision
	this->SetActorEnableCollision(false);

	PrimaryActorTick.bCanEverTick = true;

	// [ 2020-2-26 : magedoga ] Set Tick
#if WITH_EDITOR
	this->SetEnableTick(IsDebug);
#endif // WITH_EDITOR

	bEnabled = true;
}

void AWRWaterVolume::BeginPlay()
{
	Super::BeginPlay();
}

void AWRWaterVolume::BeginDestroy()
{
	UnderWaterAudioComponent = nullptr;

	if (HandleTickScreenWater.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(HandleTickScreenWater);
		HandleTickScreenWater.Reset();
	}

	Super::BeginDestroy();
}

#if WITH_EDITOR
void AWRWaterVolume::PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent)
{

	Super::PostEditChangeProperty(InPropertyChangedEvent);


	// [ 2020-2-27 : magedoga ] Debug Draw에 따른 Tick On/Off
	this->SetEnableTick(IsDebug);


	// [ 2020-2-27 : magedoga ] PostProcess Setting 관련
	static const FName NAME_Blendables = FName(TEXT("Blendables"));

	if (InPropertyChangedEvent.Property && InPropertyChangedEvent.Property->GetFName() == NAME_Blendables)
	{
		uint32 Count = WaterPostProcess.WeightedBlendables.Array.Num();
		for (uint32 i = 0; i < Count; ++i)
		{
			UObject* Obj = WaterPostProcess.WeightedBlendables.Array[i].Object;

			if (!Cast<IBlendableInterface>(Obj))
			{
				WaterPostProcess.WeightedBlendables.Array[i] = FWeightedBlendable();
			}
		}
	}


	if (InPropertyChangedEvent.Property)
	{
#define CHECK_VIRTUALTEXTURE_USAGE(property)	{																											\
													static const FName PropertyName = GET_MEMBER_NAME_STRING_CHECKED(FPostProcessSettings, property);		\
													if (InPropertyChangedEvent.Property->GetFName() == PropertyName)										\
													{																										\
														VirtualTextureUtils::CheckAndReportInvalidUsage(this, PropertyName, WaterPostProcess.property);		\
													}																										\
												}

		CHECK_VIRTUALTEXTURE_USAGE(BloomDirtMask);
		CHECK_VIRTUALTEXTURE_USAGE(ColorGradingLUT);
		CHECK_VIRTUALTEXTURE_USAGE(LensFlareBokehShape);
#undef CHECK_VIRTUALTEXTURE_USAGE
	}
}

void AWRWaterVolume::SetEnableTick(const bool bInEnable)
{
	this->SetActorTickEnabled(bInEnable);
}

bool AWRWaterVolume::ShouldTickIfViewportsOnly() const
{
	return IsDebug;
}
#endif // WITH_EDITOR

bool AWRWaterVolume::EncompassesPoint(FVector InPoint, float InSphereRadius/*=0.f*/, float* OutDistanceToPoint)
{
	bool bIn = AVolume::EncompassesPoint(InPoint, InSphereRadius, OutDistanceToPoint);

	UWorld* OwnerWorld = GetWorld();
	// [ 2020-2-28 : magedoga ] Active된 카메라에 대해서만 밑의 로직 실행
	FVector ViewPoint;
	switch (OwnerWorld->WorldType)
	{
#if WITH_EDITOR
	case EWorldType::Editor:
	{
		for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
		{
			if (LevelVC && LevelVC->IsPerspective())
			{
				ViewPoint = LevelVC->GetViewLocation();
				break;
			}
		}
	}break;
	case EWorldType::PIE:
#endif // WITH_EDITOR
	case EWorldType::Game:
	{
		FRotator ViewRotation;
		OwnerWorld->GetFirstPlayerController()->PlayerCameraManager->GetCameraViewPoint(ViewPoint, ViewRotation);
	}break;
	}

	float ViewPointSize = ViewPoint.Size();
	float InPointSize = InPoint.Size();

	if (FMath::Abs(ViewPointSize - InPointSize) > 0.1)
		return bIn;

#if WITH_EDITOR
	if (OwnerWorld->WorldType == EWorldType::Editor)
	{
		this->ProcessCommon(bIn);
	}
	else if( OwnerWorld->WorldType == EWorldType::PIE)
#endif // WITH_EDITOR
	{
		// [ 2020-2-26 : magedoga ] Call Enter/Leave Event
		if (bIn == true && bEntering != bIn)
		{
			this->OnEnter();
		}
		else if (bIn == false && bEntering != bIn)
		{
			this->OnLeave();
		}

		bEntering = bIn;
	}

	return bIn;
}

//====================================================================================
// [ 2020-2-27 : magedoga ] For PostProcess
void AWRWaterVolume::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	// [ 2020-2-27 : magedoga ] Register PostProcess Volume to World
	GetWorld()->InsertPostProcessVolume(this);
}

void AWRWaterVolume::PostUnregisterAllComponents(void)
{
	Super::PostUnregisterAllComponents();

	if (GetWorld())
	{
		GetWorld()->RemovePostProcessVolume(this);
		GetWorld()->PostProcessVolumes.RemoveSingle(this);
	}
}
//====================================================================================

void AWRWaterVolume::OnEnter()
{
	UE_LOG(WRWaterVolume, Warning, TEXT("AWRWaterVolume::OnEnter - Enter Camera"));
	
	this->ProcessCommon(true);

	// [ 2020-2-26 : magedoga ] Disable direct of Screen water. if, Directing.
	if (HandleTickScreenWater.IsValid() == true)
	{
		this->EndScreenWaterDirect();
	}

	// [ 2020-2-27 : magedoga ] Register ActorFX for Bubble
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	UWRActorComponentVFX* VFXComponent = MyCharacter->GetComponent<UWRActorComponentVFX>();
	if (VFXComponent->IsValidLowLevel() == true && UnderWaterFX != nullptr)
	{
		VFXComponent->AddVFX(EWRVFXActor::UnderWater, UnderWaterFX, MyCharacter, nullptr);
	}

	// [ 2020-2-28 : magedoga ] Spawn Enter FX
	if (EnterWaterFX != nullptr)
	{
		WRVFXManager::Get()->Spawn(EnterWaterFX, Cast<AActor>(MyCharacter), nullptr);
	}
}

void AWRWaterVolume::OnLeave()
{
	UE_LOG(WRWaterVolume, Warning, TEXT("AWRWaterVolume::OnLeave - Leave Camera"));

	this->ProcessCommon(false);

	// [ 2020-2-26 : magedoga ] Eanble direct of Screen water 
	if (ScreenWaterDirectCurveAsset.IsNull() == false && HandleTickScreenWater.IsValid() == false)
	{
		ScreenWaterCurve = ScreenWaterDirectCurveAsset.LoadSynchronous();
		WRMaterialParameterCollectionManager::Get()->SetScreenWater(1.0f);
		HandleTickScreenWater = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &AWRWaterVolume::TickScreenWater));
	}

	// [ 2020-2-27 : magedoga ] UnRegister ActorFX
	AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
	UWRActorComponentVFX* VFXComponent = MyCharacter->GetComponent<UWRActorComponentVFX>();
	if (VFXComponent->IsValidLowLevel() == true)
	{
		VFXComponent->RemoveVFX(EWRVFXActor::UnderWater);
	}

	// [ 2020-2-28 : magedoga ] Spawn Leave FX
	if (LeaveWaterFX != nullptr)
	{
		WRVFXManager::Get()->Spawn(LeaveWaterFX, Cast<AActor>(MyCharacter), nullptr);
	}
}

void AWRWaterVolume::ProcessCommon(const bool bInEnter)
{
	// [ 2020-2-27 : magedoga ] Fog
	if (WaterFogActor != nullptr)
	{
		if (WaterFogActor->IsValidLowLevel() == true)
		{
			WaterFogActor->GetRootComponent()->SetVisibility(bInEnter);

			// [ 2020-3-6 : magedoga ] 그 외 Fog는 전부 반대로 세팅
			for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
			{
				AExponentialHeightFog* ExponentialFog = *It;
				if (ExponentialFog != WaterFogActor)
				{
					ExponentialFog->GetRootComponent()->SetVisibility(!bInEnter);
				}
			}
		}
	}

#if WITH_EDITOR
	UWorld* OwnerWorld = GetWorld();
	if (OwnerWorld->WorldType == EWorldType::Editor)
	{
		UMaterialParameterCollection* MPC = FindObject<UMaterialParameterCollection>(nullptr, TEXT("/Game/Asset/Shader/PostProcess/PP_TargetParameterCollection.PP_TargetParameterCollection"));
		if (MPC == nullptr)
		{
			MPC = LoadObject<UMaterialParameterCollection>(nullptr, TEXT("/Game/Asset/Shader/PostProcess/PP_TargetParameterCollection.PP_TargetParameterCollection"));
		}

		// [ 2020-2-27 : magedoga ] Post Process Material Parameter
		UKismetMaterialLibrary::SetScalarParameterValue(OwnerWorld,
			MPC,
			TEXT("OceanIn"),
			bInEnter == true ? 1.0f : 0.0f);

		// [ 2020-2-28 : magedoga ] Under Water Sound
		//if (UnderWaterSound.IsNull() == false)
		//{
		//	if (bInEnter == true)
		//	{
		//		if (UnderWaterAudioComponent == nullptr)
		//		{
		//			UnderWaterAudioComponent = UGameplayStatics::SpawnSound2D(this, Cast<USoundCue>(UnderWaterSound.LoadSynchronous()));
		//		}
		//	}
		//	else
		//	{
		//		if (UnderWaterAudioComponent != nullptr)
		//		{
		//			UnderWaterAudioComponent->ConditionalBeginDestroy();
		//			UnderWaterAudioComponent = nullptr;
		//		}
		//	}
		//}

		if (UnderWaterReverbEffect.IsNull() == false)
		{
			if (bInEnter == true && bTempActivatedReverb == false)
			{
				UGameplayStatics::ActivateReverbEffect(this, UnderWaterReverbEffect.LoadSynchronous(), FName("UnderWaterReverb"));
				bTempActivatedReverb = true;
			}
			else if(bInEnter == false && bTempActivatedReverb == true)
			{
				UGameplayStatics::DeactivateReverbEffect(this, FName("UnderWaterReverb"));
				bTempActivatedReverb = false;
			}
		}
	}
	else
	{
		// [ 2020-2-27 : magedoga ] Post Process Material Parameter
		WRMaterialParameterCollectionManager::Get()->SetEnableWater(bInEnter);

		// [ 2020-2-28 : magedoga ] Under Water Sound
		//if (UnderWaterSound.IsNull() == false)
		//{
		//	if (bInEnter)
		//	{
		//		//if (UnderWaterAudioComponent != nullptr)
		//		{
		//			UnderWaterAudioComponent = WRSFXManager::Get()->Spawn2D(Cast<USoundCue>(UnderWaterSound.LoadSynchronous()));
		//		}
		//	}
		//	else
		//	{
		//		WRSFXManager::Get()->Remove(UnderWaterAudioComponent);
		//	}
		//}

		if (UnderWaterReverbEffect.IsNull() == false)
		{
			if (bInEnter == true)
			{
				UGameplayStatics::ActivateReverbEffect(this, UnderWaterReverbEffect.LoadSynchronous(), FName("UnderWaterReverb"));
			}
			else
			{
				UGameplayStatics::DeactivateReverbEffect(this, FName("UnderWaterReverb"));
			}
		}
	}
#else
	// [ 2020-2-27 : magedoga ] Post Process Material Parameter
	WRMaterialParameterCollectionManager::Get()->SetEnableWater(bInEnter);

	// [ 2020-2-28 : magedoga ] Under Water Sound
	//if (UnderWaterSound.IsNull() == false)
	//{
	//	if (bInEnter)
	//	{
	//		//if (UnderWaterAudioComponent != nullptr)
	//		{
	//			UnderWaterAudioComponent = WRSFXManager::Get()->Spawn2D(Cast<USoundCue>(UnderWaterSound.LoadSynchronous()));
	//		}
	//	}
	//	else
	//	{
	//		WRSFXManager::Get()->Remove(UnderWaterAudioComponent);
	//	}
	//}

	if (UnderWaterReverbEffect.IsNull() == false)
	{
		if (bInEnter == true)
		{
			UGameplayStatics::ActivateReverbEffect(this, UnderWaterReverbEffect.LoadSynchronous(), FName("UnderWaterReverb"));
		}
		else
		{
			UGameplayStatics::DeactivateReverbEffect(this, FName("UnderWaterReverb"));
		}
	}
#endif // WITH_EDITOR
}

bool AWRWaterVolume::TickScreenWater(float InDeltaTime)
{
	AccScreenWaterDirectTime += InDeltaTime;

	float Value = ScreenWaterCurve->GetFloatValue(AccScreenWaterDirectTime);
	WRMaterialParameterCollectionManager::Get()->SetScreenWater(FMath::Clamp(Value, 0.0f, 1.0f));

	float TimeRangeMin, TimeRangeMax = 0.0f;
	ScreenWaterCurve->GetTimeRange(TimeRangeMin, TimeRangeMax);
	if (TimeRangeMax < AccScreenWaterDirectTime)
	{
		this->EndScreenWaterDirect();
		return false;
	}

	return true;
}

void AWRWaterVolume::EndScreenWaterDirect()
{
	WRMaterialParameterCollectionManager::Get()->SetScreenWater(0.0f);
	if (HandleTickScreenWater.IsValid() == true)
	{
		FTicker::GetCoreTicker().RemoveTicker(HandleTickScreenWater);
	}
	HandleTickScreenWater.Reset();

	AccScreenWaterDirectTime = 0.0f;
	ScreenWaterCurve = nullptr;
}

