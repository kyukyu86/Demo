// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRCharacterCustomizingManager.h"
#include "WRCharacterManager.h"
#include "WRUIManager.h"
#include "WRAccountManager.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Actor/StaticMeshActor/WRStaticMeshActor.h"
#include "Actor/SkeletalMeshActor/WRSkeletalMeshActor.h"

#include "Component/Character/WRActorComponentEquipmentLook.h"
#include "Component/Character/WRActorComponentManageMaterial.h"

#include "UI/Panel/WRUIPanel_SubColorSelect.h"
#include "UI/Panel/WRUIPanel_NewCharacterCreate.h"
#include "UI/Panel/WRUIPanel_SubCharacterCreate.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Core/WRGameInstance.h"
#include "Define/WRPathDefine.h"
#include "Engine/StaticMeshActor.h"
#include "Table/Base/WRTableManager.h"
#include "Struct/WRUIStruct.h"
#include "WidgetComponent/WRWidgetComponentBase.h"
#include "Animation/SkeletalMeshActor.h"
#include "Table/WRCustomizingCategoryTable.h"
#include "Utility/WRActorUtility.h"


#define DEFAULT_MALE_PRESET_TID 21
#define DEFAULT_FEMALE_PRESET_TID 51
#define MAX_MALE_PRESET_TID 26
#define MAX_FEMALE_PRESET_TID 56

#define WR_DISSOLVE_PARAMETER_NAME "ENPC_Size"
#define WR_WIREFRAME_PARAMETER_NAME "00.ClothChangingMain"
#define WR_WIREFRAME_PARAMETER_MIN_VALUE 0.3F
#define WR_WIREFRAME_PARAMETER_MAX_VALUE 1.0F

#define WR_PRESETMESH_SIZE 0.4F

#define DEFAULT_MALE_PART_UPPER_CATEGORY_TID 1
#define DEFAULT_FEMALE_PART_UPPER_CATEGORY_TID 2

#define NONE_PHASE_COMPLETE_BIT_MASK 0x0000
#define GENDERSELECT_PHASE_COMPLETE_BIT_MASK 0xFFFF

FWRCustomizeDirection::FWRCustomizeDirection()
{
	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FWRCustomizeDirection::Tick));
}

FWRCustomizeDirection::~FWRCustomizeDirection()
{
	Actors.Empty();
	AxisLocation = FVector::ZeroVector;

	if (TickHandle.IsValid() == false)
	{
		return;
	}
	FTicker::GetCoreTicker().RemoveTicker(TickHandle);
	TickHandle.Reset();
}

void FWRCustomizeDirection::Initialize(const TArray<AActor*>& InActors, const TArray<float>& InScales, const FVector& InLocation, const bool InIsCharacter /*= false*/)
{
	Actors = InActors;
	Scales = InScales;
	AxisLocation = InLocation;
	IsCharacter = InIsCharacter;
}

void FWRCustomizeDirection::Release()
{
	Actors.Empty();

	AxisLocation = FVector::ZeroVector;

	IsCharacter = false;
}

bool FWRCustomizeDirection::BeginPlay(const EWRCustomizeDirectionAction InAction, FWRCostomizeDirectionComplete InDelegate, const bool InIsClockwise /*= true*/, const int32 j /*= 0*/, const float InDegree /*= 0.0f*/, const float InDirectionSpeed /*= 1.0f*/, const FName& InBoneName /*= NAME_None*/)
{
	if (IsDirecting())
	{
		return false;
	}

	DirectionData.AccDeltaTime = 0.0F;
	DirectionData.DirectionSpeed = InDirectionSpeed;
	DirectionData.Action = InAction;
	DirectionData.DirectionDelegate = InDelegate;
	DirectionData.IsClockwise = InIsClockwise;
	DirectionData.IsInitialized = true;
	DirectionData.PlayerLocation = GetPlayerStart();

	switch (InAction)
	{
	case EWRCustomizeDirectionAction::ZoomIn:
		{
			AWRCharacter* Character = WRCharacterCustomizingManager::Get()->GetSelectedCharacter();
			if (Character)
			{
				FVector Destination = WRActorUtility::GetBoneTransform(Character, InBoneName).GetLocation();

				DirectionData.CameraDirection = WRActorUtility::GetDirection(DirectionData.PlayerLocation, Destination);
			}
		}
		break;
	case EWRCustomizeDirectionAction::ZoomOut:
		{
			AWRCharacter* Character = WRCharacterManager::Get()->GetMyCharacter();
			if (Character)
			{
				FVector Destination = Character->GetActorLocation();

				DirectionData.CameraDirection = WRActorUtility::GetDirection(DirectionData.PlayerLocation, Destination);
			}
		}
		break;
	}

	float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeDistance);

	GetLocations(DirectionData.Locations, Actors, Distance, AxisLocation, j, InDegree);

	return true;
}

void FWRCustomizeDirection::GetLocations(TArray<FVector>& OutLocations, const TArray<AActor*>& InActors, const float InDistance, const FVector& InLocation, const int32 j /*= 0*/, const float InDegree /*= 0.0f*/)
{
	int32 Nums = InActors.Num();
	float Degree = 360.0f / Nums;

	for (int32 i = 0; i < Nums; ++i)
	{
		FVector ForwardLocation = FVector::BackwardVector.RotateAngleAxis(FMath::Min(Degree, InDegree), FVector::UpVector) * InDistance;
		FVector RotatedLocation = ForwardLocation.RotateAngleAxis((Degree * i) + (Degree * j), FVector::UpVector);

		OutLocations.Add(RotatedLocation + InLocation);

//#if UE_EDITOR
//		UKismetSystemLibrary::DrawDebugSphere(UWRGameInstance::GetGameInstance()->GetWorld(), RotatedLocation + InLocation, 10.0f, 12, FLinearColor::Green, 10.0f, 1.0f);
//#else // UE_EDITOR
//#endif // UE_EDITOR
	}
}

bool FWRCustomizeDirection::Tick(float DeltaTime)
{
	float MovementSpeed = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeMovementSpeed);

	//LocationAccumulation += DeltaTime * MovementSpeed;
	RotationAccumulation += DeltaTime * MovementSpeed;

	float WaveHeight = 0.0F;

	int32 Nums = Actors.Num();
	float Degree = 360.0f / Nums;

	if (IsCharacter == false)
	{
		WaveHeight = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeWaveHeight);

		for (int32 i = 0; i < Nums; ++i)
		{
			if (Actors[i] == nullptr)
			{
				continue;
			}

			float Z = FMath::Cos(FMath::DegreesToRadians(LocationAccumulation + (Degree * i))) * WaveHeight;

			FVector NewLocation = Actors[i]->GetActorLocation();
			NewLocation.Z = AxisLocation.Z + Z;

			if (IsDirecting() == false)
			{
				//Actors[i]->SetActorLocation(NewLocation);
			}
		}

		FRotator NewRotation = FRotator::ZeroRotator;
		NewRotation.Yaw = RotationAccumulation;

		for (auto& Actor : Actors)
		{
			if (Actor == nullptr)
			{
				continue;
			}

			Actor->SetActorRotation(NewRotation);
		}
	}

	if (IsDirecting())
	{
		DirectionData.AccDeltaTime += DeltaTime * DirectionData.DirectionSpeed;

		EWRCustomizeDirectionAction Action = DirectionData.Action;
		switch (Action)
		{
		case EWRCustomizeDirectionAction::Spawn:
		{
			LocationAccumulation = 0.0F;

			float Ratio = FMath::Min(FMath::Pow(DirectionData.AccDeltaTime, 10.0F), 1.0F);

			for (int32 i = 0; i < Nums; ++i)
			{
				if (Actors[i] == nullptr)
				{
					continue;
				}

				float Z = FMath::Cos(FMath::DegreesToRadians(LocationAccumulation + (Degree * i))) * WaveHeight;

				FVector WaveLocation = AxisLocation;
				//WaveLocation.Z = AxisLocation.Z + Z;

				FVector RotatedLocation = DirectionData.Locations[i];
				//RotatedLocation.Z = AxisLocation.Z + Z;

				FVector NewLocation = FMath::Lerp(WaveLocation, RotatedLocation, Ratio);

				Actors[i]->SetActorLocation(NewLocation);
				Actors[i]->SetActorScale3D(FMath::Lerp(FVector::ZeroVector, FVector::OneVector * Scales[i], FMath::Min(Ratio * 2.0F, 1.0F)));
				//Actors[i]->SetActorScale3D(FMath::Lerp(FVector::ZeroVector, FVector::OneVector * Scales[i], Ratio));

				if (Actors[i]->IsHidden() == false)
				{
					continue;
				}

				Actors[i]->SetActorHiddenInGame(false);
			}
		}
		break;
		case EWRCustomizeDirectionAction::Unspawn:
		{
			LocationAccumulation = 0.0F;

			float Ratio = FMath::Min(FMath::Pow(DirectionData.AccDeltaTime, 10.0F), 1.0F);

			for (int32 i = 0; i < Nums; ++i)
			{
				if (Actors[i] == nullptr)
				{
					continue;
				}

				float Z = FMath::Cos(FMath::DegreesToRadians(LocationAccumulation + (Degree * i))) * WaveHeight;

				FVector WaveLocation = AxisLocation;
				//WaveLocation.Z = AxisLocation.Z + Z;

				FVector RotatedLocation = Actors[i]->GetActorLocation();
				FVector NewLocation = FMath::Lerp(RotatedLocation, WaveLocation, Ratio);

				Actors[i]->SetActorLocation(NewLocation);
				Actors[i]->SetActorScale3D(FMath::Lerp(FVector::OneVector * Scales[i], FVector::ZeroVector, FMath::Min(Ratio * 2.0F, 1.0F)));
				//Actors[i]->SetActorScale3D(FMath::Lerp(FVector::OneVector * Scales[i], FVector::ZeroVector, Ratio));
			}
		}
		break;
		case EWRCustomizeDirectionAction::Rotation:
		{
			float Ratio = FMath::Sin(FMath::DegreesToRadians(DirectionData.AccDeltaTime * 90.0f));

			for (int32 i = 0; i < Nums; ++i)
			{
				if (Actors[i] == nullptr)
				{
					continue;
				}

				float Z = FMath::Cos(FMath::DegreesToRadians(LocationAccumulation + (Degree * i))) * WaveHeight;

				FVector DirectionVector = DirectionData.Locations[i] - AxisLocation;
				DirectionVector.Normalize();

				float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeDistance);

				FVector RotatedLocation = DirectionVector * Distance;

				float AngleDeg = (DirectionData.IsClockwise) ? FMath::Lerp(0.0F, Degree, Ratio) : FMath::Lerp(0.0F, Degree, Ratio) * -1.0F;
				FVector NewLocation = AxisLocation + RotatedLocation.RotateAngleAxis(AngleDeg, FVector::UpVector);
				//NewLocation.Z += Z;

				Actors[i]->SetActorLocation(NewLocation);
			}
		}
		break;
		case EWRCustomizeDirectionAction::ZoomIn:
		{
			AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
			if (MyCharacter == nullptr)
			{
				return true;
			}

			float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeCameraMovementLength);
			//float Speed = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeCameraMovementSpeed);

			float Ratio = FMath::Sin(FMath::DegreesToRadians(DirectionData.AccDeltaTime * 90.0f));

			FVector PlayerLocation = DirectionData.PlayerLocation;
			FVector CameraLocation = PlayerLocation + DirectionData.CameraDirection * Distance;
			FVector NewLocation = FMath::Lerp(PlayerLocation, CameraLocation, FMath::Min(Ratio, 1.0F));
			FRotator NewRotation = FMath::Lerp(FRotator::ZeroRotator, DirectionData.CameraDirection.Rotation(), FMath::Min(Ratio, 1.0F));

			MyCharacter->SetActorLocation(NewLocation);
			//MyCharacter->SetActorLocationAndRotation(NewLocation, NewRotation);
		}
		break;
		case EWRCustomizeDirectionAction::ZoomOut:
		{
			AWRCharacterMine* MyCharacter = WRCharacterManager::Get()->GetMyCharacter();
			if (MyCharacter == nullptr)
			{
				return true;
			}

			float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeCameraMovementLength);
			//float Speed = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeCameraMovementSpeed);

			float Ratio = FMath::Sin(FMath::DegreesToRadians(DirectionData.AccDeltaTime * 90.0f));

			FVector PlayerLocation = DirectionData.PlayerLocation;
			FVector CameraLocation = PlayerLocation + DirectionData.CameraDirection * Distance;
			FVector NewLocation = FMath::Lerp(CameraLocation, PlayerLocation, FMath::Min(Ratio, 1.0F));
			FRotator NewRotation = FMath::Lerp(DirectionData.CameraDirection.Rotation(), FRotator::ZeroRotator, FMath::Min(Ratio, 1.0F));

			MyCharacter->SetActorLocation(NewLocation);
			//MyCharacter->SetActorLocationAndRotation(NewLocation, NewRotation);
		}
		break;
		}

		// [ 2020-7-13 : animal14 ] 연출 완료
		if (DirectionData.AccDeltaTime < 1.0f)
		{
			return true;
		}

		// [ 2020-7-13 : animal14 ] 연출 완료 전 이벤트
		OnPreComplete();

		// [ 2020-7-16 : animal14 ] 연출 완료 이벤트
		DirectionData.DirectionDelegate.ExecuteIfBound(Action);

		// [ 2020-7-13 : animal14 ] 연출 완료 후 이벤트
		OnPostComplete();
	}
	
	return true;
}

void FWRCustomizeDirection::OnPreComplete()
{
}

void FWRCustomizeDirection::OnPostComplete()
{
	DirectionData.Locations.Empty();

	DirectionData.Action = EWRCustomizeDirectionAction::None;

	DirectionData.AccDeltaTime = 0.0F;

	DirectionData.IsClockwise = true;
	DirectionData.IsInitialized = false;

	DirectionData.DirectionDelegate.Unbind();
}

FVector FWRCustomizeDirection::GetPlayerStart()
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	if (World == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector Location = FVector::ZeroVector;
	uint32 Index = 0;

	for (TActorIterator<APlayerStart> iter(World); iter; ++iter)
	{
		if (Index)
		{
			UE_LOG(LogTemp, Display, TEXT("[FWRCustomizeDirection::GetPlayerStart] PlayerStart was duplicated."));
		}

		Location = iter->GetActorLocation();
		++Index;
	}

	return Location;
}

void WRCharacterCustomizingManager::OnInitialize()
{
	for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
	{
		SpawnedPresetCharacter[i] = nullptr;
	}
}

void WRCharacterCustomizingManager::OnShutdown()
{
	ClearAllInformations();
	RemoveCharacters(false);

	for (auto& Complete : Completes)
	{
		delete Complete.Value;
		Complete.Value = nullptr;
	}
	Completes.Empty();
}

void WRCharacterCustomizingManager::OnLoadLevelStart()
{
	ClearAllInformations();
	IsGenderSelected = false;

	PresetSelectInformation[Gender::Male].EquippedPresetTID = DEFAULT_MALE_PRESET_TID;
	PresetSelectInformation[Gender::Female].EquippedPresetTID = DEFAULT_FEMALE_PRESET_TID;
}

int32 WRCharacterCustomizingManager::ConvertPresetTIDToDirectionArrayIndex(const WRTableID IN InTableID)
{
	return (GenderSelectInformation.FocusedGender == Gender::Male) ? InTableID - DEFAULT_MALE_PRESET_TID : InTableID - DEFAULT_FEMALE_PRESET_TID;
}

WRTableID WRCharacterCustomizingManager::ConvertDirectionArrayIndexToPresetTID(const int32 IN InArrayIndex)
{
	return (GenderSelectInformation.FocusedGender == Gender::Male) ? InArrayIndex + DEFAULT_MALE_PRESET_TID : InArrayIndex + DEFAULT_FEMALE_PRESET_TID;
}

void WRCharacterCustomizingManager::RemoveCharacters(const bool IN bInDissolve)
{
	for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
	{
		if (SpawnedPresetCharacter[i] != nullptr)
		{
			WRCharacterManager::Get()->Remove(SpawnedPresetCharacter[i], bInDissolve);
			SpawnedPresetCharacter[i] = nullptr;
		}
	}
}

void WRCharacterCustomizingManager::RemoveCharacter(class AWRCharacter* IN InCharacterToRemove)
{
	if (InCharacterToRemove == nullptr)
		return;

	for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
	{
		if (SpawnedPresetCharacter[i] == InCharacterToRemove)
		{
			WRCharacterManager::Get()->Remove(SpawnedPresetCharacter[i], true);
			SpawnedPresetCharacter[i] = nullptr;
		}
	}
}

void WRCharacterCustomizingManager::RemoveCharacter(const Gender IN InGenderToRemove, const bool IN bInDissolve)
{
	if (SpawnedPresetCharacter[InGenderToRemove] == nullptr)
			return;

	WRCharacterManager::Get()->Remove(SpawnedPresetCharacter[InGenderToRemove], bInDissolve);
	SpawnedPresetCharacter[InGenderToRemove] = nullptr;
}

void WRCharacterCustomizingManager::StartCharacterCreationProcess()
{
	OpenUI();
	SpawnPresetCharacters();
}

void WRCharacterCustomizingManager::SetEnableDisplayURODebug(const bool bInEnable)
{
	for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
	{
		SpawnedPresetCharacter[i]->SetEnableDisplayURODebug(bInEnable);
	}
}

void WRCharacterCustomizingManager::MovePhaseTo(const EWRCharacterCustomizingPhase& IN InPhase, const bool IN bInForceToReleaseLock)
{
	if (bInForceToReleaseLock)
		ReleasePhaseLock();

	if (IsPhaseLocked() == false)
	{
		EWRCharacterCustomizingPhase TempPhase = EWRCharacterCustomizingPhase(InPhase);

		if (TempPhase < EWRCharacterCustomizingPhase::MAX_PHASE_COUNT)
		{
			OnPrePhaseChanged(TempPhase);
			Phase = TempPhase;
			LockPhase();
			OnPostPhaseChanged(Phase);
		}
	}
}

void WRCharacterCustomizingManager::MoveToNextPhase(const bool IN bInForceToReleaseLock)
{
	if (bInForceToReleaseLock)
		ReleasePhaseLock();

	if (IsPhaseLocked() == false)
	{
		EWRCharacterCustomizingPhase TempPhase = EWRCharacterCustomizingPhase((int32)Phase + 1);

		if (TempPhase < EWRCharacterCustomizingPhase::MAX_PHASE_COUNT)
		{
			OnPrePhaseChanged(TempPhase);
			Phase = TempPhase;
			LockPhase();
			OnPostPhaseChanged(Phase);
		}
	}
}

void WRCharacterCustomizingManager::MoveToPreviousPhase(const bool IN bInForceToReleaseLock /*= false*/)
{
	if (bInForceToReleaseLock)
		ReleasePhaseLock();

	if (IsPhaseLocked() == false)
	{
		EWRCharacterCustomizingPhase TempPhase = EWRCharacterCustomizingPhase((int32)Phase - 1);

		if (TempPhase > EWRCharacterCustomizingPhase::None)
		{
			OnPrePhaseChanged(TempPhase);
			Phase = TempPhase;
			LockPhase();
			OnPostPhaseChanged(Phase);
		}
	}
}

void WRCharacterCustomizingManager::CompletePhase(const EWRCharacterCustomizingPhaseMove IN InPhaseMove)
{
	ReleasePhaseLock();
	ReleaseInput();
	OnPhaseCompleted(Phase, InPhaseMove);
}

void WRCharacterCustomizingManager::CompletePhase(const EWRPhaseMovableBitFlag IN InFlagToAdd)
{
	ReleasePhaseLock();
	ReleaseInput();
	AddPhaseMoveBit(InFlagToAdd);
	OnPhaseCompleted(Phase, EWRCharacterCustomizingPhaseMove::Next);
}

//void WRCharacterCustomizingManager::CompletePhase(const EWRCharacterCustomizingPhase IN InNextPhaseToMove)
//{
//	ReleasePhaseLock();
//	ReleaseInput();
//	OnPhaseCompleted(Phase, EWRCharacterCustomizingPhaseMove::None);
//}

void WRCharacterCustomizingManager::OnPhaseCompleted(const EWRCharacterCustomizingPhase IN InCompletedPhase, const EWRCharacterCustomizingPhaseMove IN InPhaseMove)
{
	if (OnPhaseCompletedDelegate.IsBound())
		OnPhaseCompletedDelegate.Broadcast(InCompletedPhase);

	LockInput();

	// None 에서는 UI Animation 에 의해 MoveToNextPhase 가 되지 않으므로 Phase를 이동시킨다.
	switch (Phase)
	{
	case None:
		ReservePhase(EWRCharacterCustomizingPhase::GenderSelect);
		//MoveToNextPhase();
		if (GenderSelectInformation.FocusedGender == Gender::Male)
			PresetSelectInformation[Gender::Male].EquippedPresetTID = DEFAULT_MALE_PRESET_TID;
		else if (GenderSelectInformation.FocusedGender == Gender::Female)
			PresetSelectInformation[Gender::Female].EquippedPresetTID = DEFAULT_FEMALE_PRESET_TID;
		break;
	case GenderSelect:
		OnGenderSelected();
		//MoveToNextPhase(true);
		ReservePhase(EWRCharacterCustomizingPhase::PresetSelect);
		break;
	case PresetSelect:
	{
		// Preset Actor Hide 여기
		// 타이밍상 PrePhaseChanged 시점에서 할수 없고 이전단계의 Complete 시점에서 진행해야 한다.
		FWRCostomizeDirectionComplete Delegate;
		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted);
		CustomizingDirection.BeginPlay(EWRCustomizeDirectionAction::Unspawn, Delegate);
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::OnPhaseCompleted] Spawn Direction called"));

		if (InPhaseMove == EWRCharacterCustomizingPhaseMove::Next)
		{
			//MovePhaseTo(EWRCharacterCustomizingPhase::PartsDetailSetting, true);
			ReservePhase(EWRCharacterCustomizingPhase::PartsDetailSetting);
		}
		else if (InPhaseMove == EWRCharacterCustomizingPhaseMove::Previous)
		{
			//SetPresetPartsVisibility(false);
			PlaybackWireframeDirection();
			PlaybackUnspawnDirection(SpawnedPresetCharacter[GetUnFocusedGender()]);
			InitializeDirection();
			//MoveToPreviousPhase();
			ReservePhase(EWRCharacterCustomizingPhase::GenderSelect);
		}
		else if (InPhaseMove == EWRCharacterCustomizingPhaseMove::End)
		{
			//MovePhaseTo(EWRCharacterCustomizingPhase::Naming, true);
			ReservePhase(EWRCharacterCustomizingPhase::Naming);
		}
	}
		break;
	case PartsDetailSetting:
	{
		FWRCostomizeDirectionComplete Delegate;
		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnPhaseEndPartsDirectionCompleted);
		PartsDirection.BeginPlay(EWRCustomizeDirectionAction::Unspawn, Delegate);
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::OnPhaseCompleted] Spawn Direction called"));

		if (InPhaseMove == EWRCharacterCustomizingPhaseMove::Next)
		{
			ReservePhase(EWRCharacterCustomizingPhase::Naming);
		}
		else if (InPhaseMove == EWRCharacterCustomizingPhaseMove::Previous)
		{
			ReservePhase(EWRCharacterCustomizingPhase::PresetSelect);
		}
		else if (InPhaseMove == EWRCharacterCustomizingPhaseMove::End)
		{
			//MovePhaseTo(EWRCharacterCustomizingPhase::Naming, true);
			ReservePhase(EWRCharacterCustomizingPhase::Naming);
		}
	}
		break;
	case Naming:
		if (InPhaseMove == EWRCharacterCustomizingPhaseMove::Next)
		{
			//ManufactureAllData();
			RemoveCharacter(GetUnFocusedGender(), false);
			RemoveCharacter(GenderSelectInformation.FocusedGender, true);
			ClearAllInformations();
		}
		else if (InPhaseMove == EWRCharacterCustomizingPhaseMove::Previous)
		{
			ReservePhase(EWRCharacterCustomizingPhase::PartsDetailSetting);
		}
		
		
		break;

	default:
		break;
	}
}

void WRCharacterCustomizingManager::OnPrePhaseChanged(const EWRCharacterCustomizingPhase IN InNewPhase)
{
	if (OnPrePhaseChangedDelegate.IsBound())
		OnPrePhaseChangedDelegate.Broadcast(InNewPhase);

	ResetReservedPhase();

	switch (InNewPhase)
	{
	case GenderSelect:
		//PlayWireframeDirection();
		
		break;
	case PresetSelect:
		SetDefaultPartsCategoryTID();
		LoadPresetPartsAsync();
		break;
	case PartsDetailSetting:
		SelectedPartsCategoryIndex = PartsCategory::Upper;
		// Parts 선택하는 최상위 카테고리 소환
		LoadUpperPartsAsync();

		break;
	default:
		break;
	}
}

void WRCharacterCustomizingManager::OnPostPhaseChanged(const EWRCharacterCustomizingPhase IN InNewPhase)
{
	if (OnPostPhaseChangedDelegate.IsBound())
		OnPostPhaseChangedDelegate.Broadcast(InNewPhase);
}

void WRCharacterCustomizingManager::SwapGender(const bool IN bInClockWise /*= true*/)
{
	if (GenderDirection.IsDirecting())
		return;

	LockInput();

	float Degree = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeDegree);
	FWRCostomizeDirectionComplete Delegate;
	Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnSwapGenderDirectionCompleted);
	GenderDirection.BeginPlay(EWRCustomizeDirectionAction::Rotation, Delegate, !bInClockWise, int32(GenderSelectInformation.FocusedGender), Degree);

	if (GenderSelectInformation.FocusedGender == Gender::Male)
	{
		FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(DEFAULT_FEMALE_PRESET_TID);
		GenderSelectInformation.FocusedPresetTID = PresetTable->GetTableID();
		GenderSelectInformation.FocusedGender = Gender::Female;
	}

	else if (GenderSelectInformation.FocusedGender == Gender::Female)
	{
		FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(DEFAULT_MALE_PRESET_TID);
		GenderSelectInformation.FocusedPresetTID = PresetTable->GetTableID();
		GenderSelectInformation.FocusedGender = Gender::Male; 
	}
}

void WRCharacterCustomizingManager::SwapParts(const bool IN bInClockWise, bool& OUT bOutSwappedItemSameAsEquippedItem)
{
	if (PartsDirection.IsDirecting())
		return;

	if (PartsSelectInformation[SelectedPartsCategoryIndex].Parts.Num() <=1)
		return;

	LockInput();
	LockPhase();
	int32 ClockwiseCount = ConvertIndexToCount(PartsSelectInformation[SelectedPartsCategoryIndex].SelectedIndex, PartsSelectInformation[SelectedPartsCategoryIndex].Parts.Num());
	FWRCostomizeDirectionComplete Delegate;
	Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted);
	PartsDirection.BeginPlay(EWRCustomizeDirectionAction::Rotation, Delegate, !bInClockWise, ClockwiseCount);


	int32 MaxIndexCount = PartsSelectInformation[SelectedPartsCategoryIndex].Parts.Num();

	if (bInClockWise)
	{
		--ClockwiseCount;
	}
	else
	{
		++ClockwiseCount;
	}

	PartsSelectInformation[SelectedPartsCategoryIndex].SelectedIndex = ConvertCountToIndex(ClockwiseCount, PartsSelectInformation[SelectedPartsCategoryIndex].Parts.Num());

	//if (PartsSelectInformation[SelectedPartsCategoryIndex].SelectedPartsTID == PartsSelectInformation[SelectedPartsCategoryIndex].EquippedPartsTID)
	//	bOutSwappedItemSameAsEquippedItem = true;

	bOutSwappedItemSameAsEquippedItem = false;

	UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::SwapParts] rotation : %d"), ClockwiseCount);
}


void WRCharacterCustomizingManager::SwapPresetPart(const bool IN bInClockWise, bool& OUT bOutSwappedPresetAsEquippedPreset)
{
	if (CustomizingDirection.IsDirecting())
		return;

	LockInput();
	LockPhase();

	int32 ClockwiseCount = ConvertIndexToCount(PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex, PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Num());
	FWRCostomizeDirectionComplete Delegate;
	Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted);
	CustomizingDirection.BeginPlay(EWRCustomizeDirectionAction::Rotation, Delegate, !bInClockWise, ClockwiseCount);

	int32 MaxIndexCount = PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Num();

	if (bInClockWise)
	{
		--ClockwiseCount;
	}
	else
	{
		++ClockwiseCount;
	}

	PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex = ConvertCountToIndex(ClockwiseCount, PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Num());

	if(IsEquippedPresetAndSelectedPresetSame())
		bOutSwappedPresetAsEquippedPreset = true;
	else
		bOutSwappedPresetAsEquippedPreset = false;
	//if (PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID == ConvertDirectionArrayIndexToPresetTID(PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex))

	

	UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::SwapPresetPart] rotation : %d"), ClockwiseCount);
}

void WRCharacterCustomizingManager::OnSwapGenderDirectionCompleted(EWRCustomizeDirectionAction IN InAction)
{
	ReleaseInput();
}

void WRCharacterCustomizingManager::OnGenderSelected()
{
	WRTableID PresetTID = ConvertDirectionArrayIndexToPresetTID(PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex);
// 	if (PresetTID == PresetSelectInformation.EquippedPresetTID)
// 		return;

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(PresetTID);
	if (PresetTable == nullptr)
		return;

	AWRCharacter* SelectedCharacter = GetSelectedCharacter();
	if (SelectedCharacter == nullptr)
		return;

	UWRActorComponentEquipmentLook* ActorComponentEquipmentLook = SelectedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (ActorComponentEquipmentLook == nullptr)
		return;

	for (auto& PresetDefaultPart : PresetTable->PresetDefaultParts)
	{
		if (PresetDefaultPart.Key < EWREquipPart::Top)
		{
			ActorComponentEquipmentLook->Equip(PresetDefaultPart.Value, NAME_None, nullptr);
		}
	}

	//PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID = PresetTID;

	PlayWireframeDirection();
	PlayUnspawnDirection(SpawnedPresetCharacter[GetUnFocusedGender()]);
	//RemoveCharacter(GetUnFocusedGender());
}

bool WRCharacterCustomizingManager::IsEquippedPresetAndSelectedPresetSame()
{
	WRTableID PresetTID = ConvertDirectionArrayIndexToPresetTID(PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex);
	if (PresetTID == PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID)
		return true;

	return false;
}

bool WRCharacterCustomizingManager::EquipPreset()
{
	bool bResult = false;

	WRTableID PresetTID = ConvertDirectionArrayIndexToPresetTID(PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex);
	if (PresetTID == PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID)
		return false;

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(PresetTID);

	if (PresetTable == nullptr)
		return false;

	AWRCharacter* SelectedCharacter = GetSelectedCharacter();
	LockInput();
	EquippingParts.Empty();
	UWRActorComponentEquipmentLook* EquipLook = SelectedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
	if (EquipLook)
	{
		for (int32 i = 0; i < (int32)EWREquipPart::Max; ++i)
		{
			EquipLook->Unequip((EWREquipPart)i);
		}

		for (auto& PresetDefaultPart : PresetTable->PresetDefaultParts)
		{
			
			bool Equippable = EquipLook->Equip(PresetDefaultPart.Value, NAME_None, FWREquipmentComplete::CreateRaw(this, &WRCharacterCustomizingManager::OnPresetEquipCompleted));
			if (Equippable)
			{
				EquippingParts.Add(PresetDefaultPart.Key);
				bResult = true;
			}
		}

		PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID = PresetTID;
	}

	return bResult;
}

void WRCharacterCustomizingManager::OnPresetEquipCompleted(AActor* IN InActor, EWREquipPart IN InEquipPart)
{

	if (EquippingParts.Contains(InEquipPart))
	{
		EquippingParts.Remove(InEquipPart);
	}
	else
	{
		ensureMsgf(false, TEXT("Parts Error"));
	}

	if (EquippingParts.Num() == 0)
	{
		ReleaseInput();
	}
		

}

void WRCharacterCustomizingManager::OnPartsEquipCompleted(AActor* IN InActor, EWREquipPart IN InEquipPart)
{
	ReleaseInput();
}

void WRCharacterCustomizingManager::ManufactureAllData()
{
	TArray<WRTableID> CostumeItemArray;
	std::string Hair;

	for (int32 i = 0; i < PartsCategory::Upper; ++i)
	{
		switch (PartsSelectInformation[i].PartType)
		{
			// 가공데이터, 현재 Byte이슈로 헤어만 가공
		case EWREquipPart::Body:
			break;
		case EWREquipPart::FacialHair:
			break;
		case EWREquipPart::Face:
			break;
		case EWREquipPart::Hair:
			{
				//if (PartsSelectInformation[i].EquippedPartsTID == INVALID_TID)
				//	continue;

				// //= &(PartsSelectInformation[i].EquippedPartsTID);
				WRTableID TempTID = PartsSelectInformation[i].EquippedPartsTID;
				////const int32* Src = &TempTID;

				//memcpy(Buffer + WrittenSize, &TempTID, sizeof(WRTableID));

				//WrittenSize += sizeof(WRTableID);
// 				char TempChar[10];
// 				itoa(TempTID, TempChar, 10);
// 				Hair.assign(TempChar);

				FString Temp = FString::FromInt(TempTID);

				Hair.assign(TCHAR_TO_ANSI(*Temp));
			}
			break;
			 //PartsSelectInformation[i].EquippedPartsTID
 			//uint8 Type = uint8(PartsSelectInformation[i].PartType);
 			//((uint8)PartsSelectInformation[i].PartType);
			
			// 여기부터 아이템
		case EWREquipPart::Neckwear:
		case EWREquipPart::Ring:
		case EWREquipPart::Glasses:
		case EWREquipPart::Mask:
		case EWREquipPart::FaceMask:
		case EWREquipPart::Backpack:
			// 		case EWREquipPart::RightHand:
			// 		case EWREquipPart::LeftHand:
		case EWREquipPart::Top:
		case EWREquipPart::Bottom:
		case EWREquipPart::Outfit:
		case EWREquipPart::Gloves:
		case EWREquipPart::Shoes:
		{
			FWRCustomizingCategoryTable* CategoryTable = PartsSelectInformation[i].CategoryTable;
			if(CategoryTable == nullptr)
				continue;

			for (int32 j = 0; j < CategoryTable->DataSet.Num(); ++j)
			{
				if (CategoryTable->DataSet[j].PartEquipmentLookTID == PartsSelectInformation[i].EquippedPartsTID)
				{

					WRTableID FoundItemTID = CategoryTable->DataSet[j].ItemTID;
					if (FoundItemTID == INVALID_TID)
						continue;
				
					CostumeItemArray.Add(FoundItemTID);
				}
			}
		}
		break;
		default:
			//ensureMsgf(false, TEXT("Not defined EquipPart Type entered."));
			break;
		}
	}

	WRAccountManager::Get()->SetCustomizingSelectedCostumeItemArray(CostumeItemArray);
	WRAccountManager::Get()->SetCustomizingSelectedNoneItemEquipmentLookData(Hair);
}

bool WRCharacterCustomizingManager::IsPhaseMovable()
{
	switch (Phase)
	{
	case None:
		return true;
	case GenderSelect:
		if (PhaseMovableBitFlag == (/*EWRPhaseMovableBitFlag::Parts | */EWRPhaseMovableBitFlag::UIAnimation))
			return true;
		break;
	case PresetSelect:
		if (PhaseMovableBitFlag == (EWRPhaseMovableBitFlag::Parts | EWRPhaseMovableBitFlag::UIAnimation))
			return true;
		break;
	case PartsDetailSetting:
		if (PhaseMovableBitFlag == (EWRPhaseMovableBitFlag::Parts | EWRPhaseMovableBitFlag::UIAnimation))
			return true;
		break;
	
	case SkinColor:
	
// 		if (PhaseMovableBitFlag == (EWRPhaseMovableBitFlag::Parts | EWRPhaseMovableBitFlag::UIAnimation))
// 			return true;
	case Naming:
		return true;
		break;
	default:
		break;
	}

	return false;
}

void WRCharacterCustomizingManager::ReservePhase(const EWRCharacterCustomizingPhase IN InPhase)
{
	if (IsPhaseMovable())
	{
		MovePhaseTo(InPhase);
		ReservedPhase = EWRCharacterCustomizingPhase::None;
		PhaseMovableBitFlag = 0/*(uint16)EWRPhaseMovableBitFlag::None*/;
	}
	else
	{
		ReservedPhase = InPhase;
	}
}

void WRCharacterCustomizingManager::SetDefaultPartsCategoryTID()
{
	if (GetSelectedGender() == Gender::Male)
	{
		SelectedPartsCategoryTID = DEFAULT_MALE_PART_UPPER_CATEGORY_TID;
	}
	else
	{
		SelectedPartsCategoryTID = DEFAULT_FEMALE_PART_UPPER_CATEGORY_TID;
	}
}

//EWREquipPart WRCharacterCustomizingManager::ConvertPartsCategoryToEquipPart(const PartsCategory IN InCategory)
//{
//	EWREquipPart Result = EWREquipPart::Hair;
//
//	switch (InCategory)
//	{
//	case PartsCategory::Hair:
//		Result = EWREquipPart::Hair;
//		break;
//	case PartsCategory::Head:
//		Result = EWREquipPart::Face;
//		break;
//	case PartsCategory::Glasses:
//		Result = EWREquipPart::Glasses;
//		break;
//	case PartsCategory::Beard:
//		Result = EWREquipPart::FacialHair;
//		break;
//	case PartsCategory::Mask:
//		Result = EWREquipPart::Mask;
//		break;
//	case PartsCategory::Outfit:
//		Result = EWREquipPart::Outfit;
//		break;
//	case PartsCategory::Backpack:
//		Result = EWREquipPart::Backpack;
//		break;
//	case PartsCategory::Gloves:
//		Result = EWREquipPart::Gloves;
//		break;
//	case PartsCategory::Shoes:
//		Result = EWREquipPart::Shoes;
//		break;
//	default:
//		ensureMsgf(false, TEXT("PartsCategory is not defined."));
//		break;
//	}
//
//	return Result;
//}

void WRCharacterCustomizingManager::AddPhaseMoveBit(const EWRPhaseMovableBitFlag IN InFlag)
{
	PhaseMovableBitFlag |= InFlag;

	if (IsPhaseMovable())
	{
		MovePhaseTo(ReservedPhase);
		ReservedPhase = EWRCharacterCustomizingPhase::None;
		PhaseMovableBitFlag = 0/*(uint16)EWRPhaseMovableBitFlag::None*/;
	}
}

void WRCharacterCustomizingManager::SubtractPhaseMoveBit(const EWRPhaseMovableBitFlag IN InFlag)
{
	PhaseMovableBitFlag &= ~InFlag;
}

void WRCharacterCustomizingManager::OnPresetSelectCompleted()
{
	MovePhaseTo(EWRCharacterCustomizingPhase::Naming, true);
}

void WRCharacterCustomizingManager::SelectPart()
{
	if (SelectedPartsCategoryIndex == PartsCategory::Upper)
	{
		// 상위 카테고리 일경우 Unspawn 을 하고 해당 연출에 연결되어 하부 Part를 호출한다.
		FWRCostomizeDirectionComplete Delegate;
		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnUpperPartsDirectionCompleted);
		PartsDirection.BeginPlay(EWRCustomizeDirectionAction::Unspawn, Delegate);
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::SelectPart] Spawn Direction called"));

		// Unspawn 연출부에서 다음파츠들을 Loading 하고 연출한다.
	}
	else
	{
		EquipParts();
	}
}

void WRCharacterCustomizingManager::BackwardFromDetailPart()
{
	if (SelectedPartsCategoryIndex == PartsCategory::Upper)
	{
		CompletePhase(EWRCharacterCustomizingPhaseMove::Previous);
		// 상위 카테고리 일경우 Unspawn 을 하고 해당 연출에 연결되어 하부 Part를 호출한다.
// 		FWRCostomizeDirectionComplete Delegate;
// 		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnUpperPartsDirectionCompleted);
// 		PartsDirection.BeginPlay(EWRCustomizeDirectionAction::Unspawn, Delegate);
// 		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::SelectPart] Spawn Direction called"));

		// Unspawn 연출부에서 다음파츠들을 Loading 하고 연출한다.
	}
	else
	{
		FWRCostomizeDirectionComplete Delegate;
		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnBackwardPartsDirectionComplelted);
		PartsDirection.BeginPlay(EWRCustomizeDirectionAction::Unspawn, Delegate);
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::SelectPart] Spawn Direction called"));
	}
}

void WRCharacterCustomizingManager::EquipParts()
{
	FWRCustomizingCategoryTable* FoundCategoryTable = WRTableManager::Get()->FindRow<FWRCustomizingCategoryTable>(SelectedPartsCategoryTID);
	if (FoundCategoryTable == nullptr)
		return;

	uint8 SelectedIndex = PartsSelectInformation[SelectedPartsCategoryIndex].SelectedIndex;
	//WRTableID LowerCategoryTID = FoundUpperCategoryTable->LowerCategoryTID[SelectedIndex];
	//FWRCustomizingCategoryTable* FoundUpperCategoryTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(LowerCategoryTID);
	WRTableID SelectedPartEqupmentLookTID = FoundCategoryTable->PartEquipmentLookTID[SelectedIndex];
 	FWREquipmentLookTable* SelectedPartEquipmentLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(FoundCategoryTable->PartEquipmentLookTID[SelectedIndex]);
 	if (SelectedPartEquipmentLookTable == nullptr)
 		return;

	// 같은 TID 를 가지고 있으면 return;
	if (PartsSelectInformation[SelectedPartsCategoryIndex].EquippedPartsTID == SelectedPartEqupmentLookTID)
		return;

 	AWRCharacter* SelectedCharacter = GetSelectedCharacter();
 	
 	EquippingParts.Empty();
 	UWRActorComponentEquipmentLook* EquipLook = SelectedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
 	if (EquipLook)
 	{
 		EquipLook->Unequip(SelectedPartEquipmentLookTable->EquipPart);
		LockInput();
 		bool Equippable = EquipLook->Equip(SelectedPartEqupmentLookTID, NAME_None, FWREquipmentComplete::CreateRaw(this, &WRCharacterCustomizingManager::OnPartsEquipCompleted));
//  		if (Equippable)
//  			EquippingParts.Add(PresetDefaultPart.Key);
 
		PartsSelectInformation[SelectedPartsCategoryIndex].EquippedPartsTID = SelectedPartEqupmentLookTID;
		PartsSelectInformation[SelectedPartsCategoryIndex].CategoryTable = FoundCategoryTable;
		PartsSelectInformation[SelectedPartsCategoryIndex].PartType = SelectedPartEquipmentLookTable->EquipPart;
	}
}

FString WRCharacterCustomizingManager::GetSelectedPartsName()
{
	WRTableID TIDToFind = INVALID_TID;
	uint8 SelectedIndex = PartsSelectInformation[SelectedPartsCategoryIndex].SelectedIndex;

	if (SelectedPartsCategoryIndex == PartsCategory::Upper)
	{
		//if(SelectedPartsCategoryTID == INVALID_TID)

		FWRCustomizingCategoryTable* FoundTable = WRTableManager::Get()->FindRow<FWRCustomizingCategoryTable>(SelectedPartsCategoryTID);

		if (FoundTable == nullptr)
			return NULL_STRING;

		return FoundTable->PartName[SelectedIndex];
	}
	else
	{
		TIDToFind = SelectedPartsCategoryTID;

		FWRCustomizingCategoryTable* FoundTable = WRTableManager::Get()->FindRow<FWRCustomizingCategoryTable>(TIDToFind);
		if (FoundTable == nullptr)
			return NULL_STRING;

		if (SelectedPartsCategoryIndex > FoundTable->PartName.Num())
			return NULL_STRING;

		return FoundTable->PartName[SelectedIndex];
	}


	
}

void WRCharacterCustomizingManager::SetPresetPartsVisibility(const bool IN InVisibility)
{
	for (int32 i = 0; i < PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Num(); ++i)
	{
		AWRStaticMeshActor* MeshActor = Cast<AWRStaticMeshActor>(PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts[i]);
		if (MeshActor == nullptr)
			continue;

		UStaticMeshComponent* StaticMeshComponent = MeshActor->GetStaticMeshComponent();
		if (StaticMeshComponent == nullptr)
			continue;

		StaticMeshComponent->SetVisibility(InVisibility);
	}
}

void WRCharacterCustomizingManager::SetPartsVisibility(TArray<AActor*>& IN InActorArray, const bool IN InVisibility)
{
	for (int32 i = 0; i < InActorArray.Num(); ++i)
	{
		if(InActorArray[i] == nullptr)
			continue;

		AWRStaticMeshActor* MeshActor = Cast<AWRStaticMeshActor>(InActorArray[i]);
		if (MeshActor)
		{
			UStaticMeshComponent* StaticMeshComponent = MeshActor->GetStaticMeshComponent();
			if (StaticMeshComponent == nullptr)
				continue;

			StaticMeshComponent->SetVisibility(InVisibility);
		}
		else
		{
			AWRSkeletalMeshActor* SkeletalMeshActor = Cast<AWRSkeletalMeshActor>(InActorArray[i]);
			if(SkeletalMeshActor == nullptr)
				continue;

			USkeletalMeshComponent* SkeletalMeshComponent = SkeletalMeshActor->GetSkeletalMeshComponent();
			if (SkeletalMeshComponent == nullptr)
				continue;

			SkeletalMeshComponent->SetVisibility(InVisibility);
		}
	}
}

void WRCharacterCustomizingManager::OnSelectPlayerNameReceived(const FString& IN InName)
{
	NameInformation.CharacterName = InName;

	UWRUIPanel_NewCharacterCreate* FoundPanel = WRUIManager::Get()->FindUI<UWRUIPanel_NewCharacterCreate>(EWRUIPanelKind::NewCharacterCreate);
	if (FoundPanel)
	{
		FoundPanel->SetPlayerName(InName);
	}
}

void WRCharacterCustomizingManager::animal14(const FString& InVar)
{
	static FString animal20;
	if (InVar.Contains("0") == false)
	{
		animal20 = InVar;

		GenderDirection.BeginPlay(EWRCustomizeDirectionAction::ZoomIn, nullptr, true, 0, 0.0f, 1.0f, *animal20);
	}
	else
	{
		GenderDirection.BeginPlay(EWRCustomizeDirectionAction::ZoomOut, nullptr, true, 0, 0.0f, 1.0f, *animal20);
	}
}

bool WRCharacterCustomizingManager::OnTick(float InDeltaTime)
{
	Tick4WireframeDirection(InDeltaTime);

// 	if (ReservedPhase != EWRCharacterCustomizingPhase::None)
// 	{
// 		if (PhaseMovable & 0xFFFF)
// 		{
// 			MovePhaseTo(ReservedPhase);
// 			PhaseMovable = 0x0000;
// 			ReservedPhase = EWRCharacterCustomizingPhase::None;
// 		}
// 	}

	return true;
}

void WRCharacterCustomizingManager::OpenUI()
{
	FWRUILoadCompletedDelegate NewCharacterCreateUIDelegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			if (InCombinedPanelData.UIPanel)
			{
				Panel_NewCharacterCreate = Cast<UWRUIPanel_NewCharacterCreate>(InCombinedPanelData.UIPanel);
			}
		}
	});

	FWRUILoadCompletedDelegate SubCharacterCreateUIDelegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			if (InCombinedPanelData.UIPanel)
			{
				Panel_SubCharacterCreate = Cast<UWRUIPanel_SubCharacterCreate>(InCombinedPanelData.UIPanel);
			}
		}
	});
	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::NewCharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth, &NewCharacterCreateUIDelegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	WidgetInfo = FWRWidgetInfo(EWRUIPanelKind::SubCharacterCreate, EWRUIWidgetCreationBitFlag::IgnoreDepth, &SubCharacterCreateUIDelegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			// InCombinedPanelData.UIPanel;

// 			UWRUIPanel_SubColorSelect* Panel_Dialogue = Cast<UWRUIPanel_SubColorSelect>(InCombinedPanelData.UIPanel);
// 			if (Panel_Dialogue)
// 				Panel_Dialogue->OpenDialgoue(DialogueTable);
		}
	});
	WidgetInfo = FWRWidgetInfo(EWRUIPanelKind::SubColorSelect, EWRUIWidgetCreationBitFlag::IgnoreDepth);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

void WRCharacterCustomizingManager::SpawnPresetCharacters()
{
	// 남 녀 캐릭터 1종식 Spawn
	SpawningPresetCharacterInfo[Female] = LoadPresetCharacterAsync(DEFAULT_FEMALE_PRESET_TID);
	SpawningPresetCharacterInfo[Male] = LoadPresetCharacterAsync(DEFAULT_MALE_PRESET_TID);
	
}

void WRCharacterCustomizingManager::CreateSubUI()
{

}

void WRCharacterCustomizingManager::ClearAllInformations()
{
	CustomizingDirection.Release();
	GenderDirection.Release();
	PartsDirection.Release();
	GenderSelectInformation.Initialize();

	for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
	{
		PresetSelectInformation[i].Initialize();
	}

	for (int32 i = 0; i < (int32)PartsCategory::MAX_CATEGORY_COUNT; ++i)
	{
		PartsSelectInformation[i].Initialize();
	}

	NameInformation.Initialize();
	Phase = EWRCharacterCustomizingPhase::None;
	bPhaseLocked = true;
	bReadyToAllowInput = false;
}

void WRCharacterCustomizingManager::ClearInformation(const FString& IN InHash)
{
	for (int32 i = 0; i < MAX_GENDER_COUNT; ++i)
	{
		if (SpawningPresetCharacterInfo[i].SpawningPresetHashKey == InHash)
		{
			SpawningPresetCharacterInfo[i].Clear();
		}
	}
}

WRCharacterCustomizingManager::SpawningPresetCharacterInformation WRCharacterCustomizingManager::LoadPresetCharacterAsync(const WRTableID IN InTableID)
{
	SpawningPresetCharacterInformation Information;

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(InTableID);

	if (PresetTable == nullptr)
		return Information;
	    
	FWRCompleteAsyncLoad AsyncLoadDelegate = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterCustomizingManager::OnPresetCharacterSpawnCompleted);
	FString Hash = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(PresetTable->PresetBlueprint, AsyncLoadDelegate);

	if (Hash == WRASYNCLOADMANAGER_INVALID)
		return Information;

	// vuvle TODO : Gender 는 기존 캐릭터 생성을 삭제 하게 되면 변경해야한다.
	Information = SpawningPresetCharacterInformation(PresetTable->Gender, InTableID, Hash);

	return Information;
}

void WRCharacterCustomizingManager::LoadPresetPartsAsync()
{
	WRTableID MinTID = INVALID_TID, MaxTID = INVALID_TID;

	if (GenderSelectInformation.FocusedGender == Gender::Male)
	{
		MinTID = DEFAULT_MALE_PRESET_TID;
		MaxTID = MAX_MALE_PRESET_TID;

		PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Init(nullptr, MAX_MALE_PRESET_TID - DEFAULT_MALE_PRESET_TID + 1);
	}
	else if (GenderSelectInformation.FocusedGender == Gender::Female)
	{
		MinTID = DEFAULT_FEMALE_PRESET_TID;
		MaxTID = MAX_FEMALE_PRESET_TID;

		PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Init(nullptr, MAX_FEMALE_PRESET_TID - DEFAULT_FEMALE_PRESET_TID + 1);
	}

	uint8 PresetPartsCount = 0;
	for (int32 TID = MinTID; TID <= MaxTID; ++TID)
	{
		FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(TID);

		if (PresetTable == nullptr)
			continue;

		FWREquipmentLookTable* EquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(PresetTable->HoloPreviewEquipmentLookTID);
		if (EquipLookTable == nullptr)
			continue;

		FWRCompleteAsyncLoad AsyncLoadDelegate = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterCustomizingManager::OnPresetPartsSpawnCompleted);
		FString Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(EquipLookTable->StaticMesh, AsyncLoadDelegate);

		if (Hash == WRASYNCLOADMANAGER_INVALID)
			continue;

		FWRPartsData Data;
		Data.Index = PresetPartsCount;
		Datas.Add(Hash, Data);

 		++PresetPartsCount;
 	}
 
 	PresetSelectInformation[GenderSelectInformation.FocusedGender].TryingLoadPartsCount = PresetPartsCount;
	
}

void WRCharacterCustomizingManager::LoadUpperPartsAsync()
{
	Datas.Empty();

	WRTableID StartCustomizingCategoryTID = INVALID_TID;

	if (GenderSelectInformation.FocusedGender == Gender::Male)
	{
		StartCustomizingCategoryTID = DEFAULT_MALE_PART_UPPER_CATEGORY_TID;
	}
	else if (GenderSelectInformation.FocusedGender == Gender::Female)
	{
		StartCustomizingCategoryTID = DEFAULT_FEMALE_PART_UPPER_CATEGORY_TID;
	}

	FWRCustomizingCategoryTable* FoundCategoryTable = WRTableManager::Get()->FindRow<FWRCustomizingCategoryTable>(StartCustomizingCategoryTID);
	if (FoundCategoryTable == nullptr)
		return;

	uint8 PartsCount = 0;
	for (int32 i = 0; i < FoundCategoryTable->PartEquipmentLookTID.Num(); ++i)
	{
		if(FoundCategoryTable->PartEquipmentLookTID[i] == INVALID_TID)
			continue;

		FWREquipmentLookTable* EquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(FoundCategoryTable->PartEquipmentLookTID[i]);
		if (EquipLookTable == nullptr)
			continue;

		FString	MeshPath = NULL_STRING;
		if (EquipLookTable->StaticMesh.IsEmpty() == false)
		{
			MeshPath = EquipLookTable->StaticMesh;
		}
		else if (EquipLookTable->SkeletalMesh.IsEmpty() == false)
		{
			MeshPath = EquipLookTable->SkeletalMesh;
		}
		else
		{
			continue;
		}

		FWRCompleteAsyncLoad AsyncLoadDelegate = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterCustomizingManager::OnUpperPartsSpawnCompleted);
		FString Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MeshPath, AsyncLoadDelegate);

		if (Hash == WRASYNCLOADMANAGER_INVALID)
			continue;

		FWRPartsData Data;
		Data.Index = PartsCount;
		Data.CategoryTable = FoundCategoryTable;
		Datas.Add(Hash, Data);

		++PartsCount;
	}

	SelectedPartsCategoryTID = StartCustomizingCategoryTID;
	PartsSelectInformation[PartsCategory::Upper].Parts.Init(nullptr, PartsCount);
	//PartsSelectInformation[ARRAY_INDEX_UPPER].Parts.Init(nullptr, PartsCount);
}


void WRCharacterCustomizingManager::LoadDetailPartsAsync()
{
	Datas.Empty();

	// 선택된 TID 에서 하위 파츠를 선택한다.
	FWRCustomizingCategoryTable* FoundUpperCategoryTable = WRTableManager::Get()->FindRow<FWRCustomizingCategoryTable>(SelectedPartsCategoryTID);
	if (FoundUpperCategoryTable == nullptr)
		return;

	WRTableID SelectedUpperCategoryIndex = PartsSelectInformation[PartsCategory::Upper].SelectedIndex;
	WRTableID SelectedPartLowerCategoryTID = FoundUpperCategoryTable->LowerCategoryTID[SelectedUpperCategoryIndex];
	if (SelectedPartLowerCategoryTID == INVALID_TID)
		return;

	FWRCustomizingCategoryTable* FoundLowerCategoryTable = WRTableManager::Get()->FindRow<FWRCustomizingCategoryTable>(SelectedPartLowerCategoryTID);
	if (FoundLowerCategoryTable == nullptr)
		return;
	
	uint8 PartsCount = 0;
	for (int32 i = 0; i < FoundLowerCategoryTable->PartEquipmentLookTID.Num(); ++i)
	{
		if (FoundLowerCategoryTable->PartEquipmentLookTID[i] == INVALID_TID)
			continue;

		FWREquipmentLookTable* EquipLookTable = WRTableManager::Get()->FindRow<FWREquipmentLookTable>(FoundLowerCategoryTable->PartEquipmentLookTID[i]);
		if (EquipLookTable == nullptr)
			continue;

		FString	MeshPath = NULL_STRING;
		if (EquipLookTable->StaticMesh.IsEmpty() == false)
		{
			MeshPath = EquipLookTable->StaticMesh;
		}
		else if (EquipLookTable->SkeletalMesh.IsEmpty() == false)
		{
			MeshPath = EquipLookTable->SkeletalMesh;
		}
		else
		{
			continue;
		}

		FWRCompleteAsyncLoad AsyncLoadDelegate = FWRCompleteAsyncLoad::CreateRaw(this, &WRCharacterCustomizingManager::OnLowerPartsSpawnCompleted);
		FString Hash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(MeshPath, AsyncLoadDelegate);

		if (Hash == WRASYNCLOADMANAGER_INVALID)
			continue;

		FWRPartsData Data;
		Data.Index = PartsCount;
		Data.CategoryTable = FoundLowerCategoryTable;
		Datas.Add(Hash, Data);

		++PartsCount;
	}

	SelectedPartsCategoryTID = SelectedPartLowerCategoryTID;
	PartsSelectInformation[SelectedUpperCategoryIndex].Parts.Init(nullptr, PartsCount);
	//PartsSelectInformation[PartsCategory::Upper].SelectedPartsTID = 
}

// TODO : 다른 OnSpawnCompleted 함수와 합칠수 있으면 합치도록하자
void WRCharacterCustomizingManager::OnUpperPartsSpawnCompleted(UObject* InActor, FString InHash)
{
	if (InActor == nullptr)
		return;

	int32 Index = ConvertHashToTID(InHash);

	UStaticMesh* StaticMesh = Cast<UStaticMesh>(InActor);
	USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(InActor);
// 	if (StaticMesh == nullptr && SkeletalMesh == nullptr)
// 		return;
	
	if(StaticMesh)
	{
		AWRStaticMeshActor* StaticActor = CreateStaticMeshActor(StaticMesh);
		if (StaticActor == nullptr)
			return;

		USceneComponent* SceneComponent = StaticActor->GetRootComponent();
		if (SceneComponent == nullptr)
		{
			return;
		}

		UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent);
		if (CapsuleComponent == nullptr)
		{
			return;
		}

		//CapsuleComponent->SetWorldScale3D(FVector::OneVector * WR_PRESETMESH_SIZE);

		UStaticMeshComponent* StaticMeshComponent = StaticActor->GetStaticMeshComponent();
		if (StaticMeshComponent == nullptr)
		{
			return;
		}

		//StaticMeshComponent->SetVisibility(false);
		//StaticMeshComponent->SetRelativeLocation(FVector::DownVector * 92.0F);

		PartsSelectInformation[PartsCategory::Upper].Parts[Index] = StaticActor;
	}
	else if (SkeletalMesh)
	{
		AWRSkeletalMeshActor* SkeletalMeshActor = CreateSkeletalMeshActor(SkeletalMesh);

		USceneComponent* SceneComponent = SkeletalMeshActor->GetRootComponent();
		if (SceneComponent == nullptr)
		{
			return;
		}

		UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent);
		if (CapsuleComponent == nullptr)
		{
			return;
		}

		//CapsuleComponent->SetWorldScale3D(FVector::OneVector * WR_PRESETMESH_SIZE);

		USkeletalMeshComponent* SkeletalMeshComponent = SkeletalMeshActor->GetSkeletalMeshComponent();
		if (SkeletalMeshComponent == nullptr)
		{
			return;
		}

		//SkeletalMeshComponent->SetVisibility(false);
		//SkeletalMeshComponent->SetRelativeLocation(FVector::DownVector * 92.0F);

		PartsSelectInformation[PartsCategory::Upper].Parts[Index] = SkeletalMeshActor;
	}
	else
	{
		return;
	}

	if (Datas.Contains(InHash) == false)
	{
		return;
	}

	Datas[InHash].IsLoaded = true;

	for (auto& Data : Datas)
	{
		if (Data.Value.IsLoaded == false)
		{
			return;
		}
	}

	InitializePartsDirection(PartsCategory::Upper);
}

void WRCharacterCustomizingManager::OnLowerPartsSpawnCompleted(UObject* InActor, FString InHash)
{
	if (InActor == nullptr)
		return;

	int32 Index = ConvertHashToTID(InHash);

	UStaticMesh* StaticMesh = Cast<UStaticMesh>(InActor);
	USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(InActor);
	// 	if (StaticMesh == nullptr && SkeletalMesh == nullptr)
	// 		return;

	WRTableID SelectedUpperCategoryIndex = PartsSelectInformation[PartsCategory::Upper].SelectedIndex;

	if (StaticMesh)
	{
		AWRStaticMeshActor* StaticActor = CreateStaticMeshActor(StaticMesh);
		if (StaticActor == nullptr)
			return;

		USceneComponent* SceneComponent = StaticActor->GetRootComponent();
		if (SceneComponent == nullptr)
		{
			return;
		}

		UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent);
		if (CapsuleComponent == nullptr)
		{
			return;
		}

		//CapsuleComponent->SetWorldScale3D(FVector::OneVector * WR_PRESETMESH_SIZE);

		UStaticMeshComponent* StaticMeshComponent = StaticActor->GetStaticMeshComponent();
		if (StaticMeshComponent == nullptr)
		{
			return;
		}

		//StaticMeshComponent->SetVisibility(false);
		//StaticMeshComponent->SetRelativeLocation(FVector::DownVector * 92.0F);

		PartsSelectInformation[SelectedUpperCategoryIndex].Parts[Index] = StaticActor;
	}
	else if (SkeletalMesh)
	{
		AWRSkeletalMeshActor* SkeletalMeshActor = CreateSkeletalMeshActor(SkeletalMesh);

		USceneComponent* SceneComponent = SkeletalMeshActor->GetRootComponent();
		if (SceneComponent == nullptr)
		{
			return;
		}

		UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent);
		if (CapsuleComponent == nullptr)
		{
			return;
		}

		//CapsuleComponent->SetWorldScale3D(FVector::OneVector * WR_PRESETMESH_SIZE);

		USkeletalMeshComponent* SkeletalMeshComponent = SkeletalMeshActor->GetSkeletalMeshComponent();
		if (SkeletalMeshComponent == nullptr)
		{
			return;
		}

		float RelativeLocationZ = Datas[InHash].CategoryTable->RelativeLocationZ;

		//SkeletalMeshComponent->SetVisibility(false);
		SkeletalMeshComponent->SetRelativeLocation(FVector::DownVector * RelativeLocationZ);

		PartsSelectInformation[SelectedUpperCategoryIndex].Parts[Index] = SkeletalMeshActor;
	}
	else
	{
		return;
	}

	if (Datas.Contains(InHash) == false)
	{
		return;
	}

	Datas[InHash].IsLoaded = true;

	for (auto& Data : Datas)
	{
		if (Data.Value.IsLoaded == false)
		{
			return;
		}
	}

	//InitializePartsDirection((PartsCategory)SelectedUpperCategoryIndex);
	InitializePartsDirection(SelectedUpperCategoryIndex);
}

void WRCharacterCustomizingManager::OnPresetCharacterSpawnCompleted(UObject* InActor, FString InHash)
{
	SpawningPresetCharacterInformation FoundInformation = FindSpawningInformation(InHash);

	FWRCharacterPresetTable* PresetTable = WRTableManager::Get()->FindRow<FWRCharacterPresetTable>(FoundInformation.SpawningPresetTID);

	if (PresetTable == nullptr)
		return;

	AWRCharacter* SpawnedCharacter = Cast<AWRCharacter>(InActor);
	if (SpawnedCharacter != nullptr)
	{
		SpawnedCharacter->OnPostSpawn();
		SpawnedCharacter->SetLocationAndRotation(PresetTable->SpawnPosition, FRotator(0.0f, PresetTable->SpawnAngle, 0.0f));
		// [ 2020-4-10 : vuvle ] : Z 를 땅에 붙임 (좌표 문제가 있어서 사용 하지 않음. 확인할 것)
		//SpawnedCharacter->SetLocation(PresetTable->SpawnPosition, true);

		// [ 2020-2-18 : vuvle ] : Preset 이 Spawn 되는 Lab 은 광원이 없어 강제로 Light Channel 을 변경한다.
		// [ 2020-2-19 : vuvle ] : Light 채널 변경을 하지 않고 광원 설정 변경하는것으로 변경
		//USkeletalMeshComponent* MeshComponent = SpawnedCharacter->GetMesh();
		//if (MeshComponent != nullptr)
		//{
		//	MeshComponent->LightingChannels.bChannel1 = true;
		//}

		// [ 2020-2-18 : vuvle ] : Spawn 시 반드시 Equip 을 해야 다음 연출이 정상적으로 나옴.
		UWRActorComponentEquipmentLook* EquipLook = SpawnedCharacter->GetComponent<UWRActorComponentEquipmentLook>();
		if (EquipLook)
		{
			// [ 2020-08-26 : vuvle ] : 커스터마이징 관련 진용이 의견으로 커맨트화
			//EquipLook->Equip(PresetTable->HoloEquipmentLookTID);

			for (auto& PresetDefaultPart : PresetTable->PresetDefaultParts)
			{
				if (PresetDefaultPart.Key < EWREquipPart::Top)
				{
					continue;
				}

				bool IsEquipped = EquipLook->Equip(PresetDefaultPart.Value, NAME_None, FWREquipmentComplete::CreateRaw(this, &WRCharacterCustomizingManager::EquipmentComplete));
				if (IsEquipped)
				{
					if (Completes.Contains(SpawnedCharacter) == false)
					{
						Completes.Add(SpawnedCharacter, new TMap<EWREquipPart, bool>());
					}

					Completes[SpawnedCharacter]->Add(PresetDefaultPart.Key, false);
				}
			}
		}

		// [ 2020-3-23 : vuvle ] : Equip을 다시 해야 연출이 발생된다. 기획 변경으로 첫 캐릭터 부터 연출이 나와야 하는 상황이라 Equip 을 다시 한다.
		// [ 2020-6-01 : vuvle ] : EquipmentLook Component 구조 변경으로 인해 Equip을 한번 더 하지 않아도 정상 연출이 나오는걸 확인하여 삭제
		//if (EquipLook)
		//{
		//	EquipLook->Equip(PresetTable->HoloEquipmentLookTID);
		//}

		AWRCharacterPlayer* CastedSpawnedCharacter = Cast<AWRCharacterPlayer>(SpawnedCharacter);
		if (CastedSpawnedCharacter->IsValidLowLevel() == true)
		{
			CastedSpawnedCharacter->SetUsingSyncHMDHead(false);
		}
	}

	//FoundInformation.SpawnedCharacter = SpawnedCharacter;

	if (FoundInformation.SpawningPresetGender == EWRCharacterGender::Male_A) SpawnedPresetCharacter[Gender::Male] = SpawnedCharacter;
	else if (FoundInformation.SpawningPresetGender == EWRCharacterGender::Female_A) SpawnedPresetCharacter[Gender::Female] = SpawnedCharacter;

	InitializeDirection();

	if (IsAllCharacterSpawned())
	{
		WRVFXManager::Get()->Spawn(EFFECT_PATH_4_LOBBY_BASE, SpawnedPresetCharacter[Gender::Female], nullptr);
	}
}

void WRCharacterCustomizingManager::OnPresetPartsSpawnCompleted(UObject* InActor, FString InHash)
{
	if (InActor == nullptr)
	{
		return;
	}

	UStaticMesh* StaticMesh = Cast<UStaticMesh>(InActor);
	if (StaticMesh == nullptr)
	{
		return;
	}

	AWRStaticMeshActor* StaticActor = CreateStaticMeshActor(StaticMesh, FVector::OneVector * WR_PRESETMESH_SIZE, FVector::DownVector * 92.0F);
	if (StaticActor == nullptr)
	{
		return;
	}

	UStaticMeshComponent* StaticMeshComponent = StaticActor->GetStaticMeshComponent();
	if (StaticMeshComponent == nullptr)
	{
		return;
	}

	//StaticMeshComponent->SetVisibility(false);
	
	int32 Index = ConvertHashToTID(InHash);

	PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts[Index] = StaticActor;

	if (Datas.Contains(InHash) == false)
	{
		return;
	}

	Datas[InHash].IsLoaded = true;

	for (auto& Data : Datas)
	{
		if (Data.Value.IsLoaded == false)
		{
			return;
		}
	}

	InitializePresetPartsDirection();
}

void WRCharacterCustomizingManager::EquipmentComplete(AActor* InActor, EWREquipPart InPart)
{
	AWRCharacter* Character = Cast<AWRCharacter>(InActor);
	if (Character == nullptr)
	{
		return;
	}

	if (Completes.Contains(Character) == false)
	{
		return;
	}

	bool IsLoaded = true;

	TMap<EWREquipPart, bool>* Parts = Completes[Character];

	(*Parts)[InPart] = true;

	for (auto& Part : *Parts)
	{
		if (Part.Value == false)
		{
			IsLoaded = false;
			break;
		}
	}

	UWRActorComponentEquipmentLook* ActorComponentEquipmentLook = Character->GetComponent<UWRActorComponentEquipmentLook>();
	if (ActorComponentEquipmentLook == nullptr)
	{
		return;
	}

	if (IsLoaded)
	{
		ActorComponentEquipmentLook->SetMaterialParameterValue(WR_WIREFRAME_PARAMETER_NAME, WR_WIREFRAME_PARAMETER_MIN_VALUE);

		for (auto& Part : *Parts)
		{
			if (Part.Key == EWREquipPart::Body)
			{
				continue;
			}
			ActorComponentEquipmentLook->SetVisibility(Part.Key, true);
		}

		delete Parts;
		Parts = nullptr;

		Completes.Remove(Character);
	}
	else
	{
		ActorComponentEquipmentLook->SetVisibility(InPart, false);
	}
}

void WRCharacterCustomizingManager::Tick4WireframeDirection(float InDeltaTime)
{
	if (IsGenderSelected == false)
	{
		return;
	}
	if (AccDeltaTime > WR_WIREFRAME_PARAMETER_MAX_VALUE)
	{
		IsGenderSelected = false;
	}

	AccDeltaTime += InDeltaTime / WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeWireframeDuration);

	float Ratio = 0.0F;
	if (IsPlayback)
	{
		Ratio = FMath::Lerp(WR_WIREFRAME_PARAMETER_MAX_VALUE, WR_WIREFRAME_PARAMETER_MIN_VALUE, FMath::Clamp(AccDeltaTime, 0.0F, 1.0F));
	}
	else
	{
		Ratio = FMath::Lerp(WR_WIREFRAME_PARAMETER_MIN_VALUE, WR_WIREFRAME_PARAMETER_MAX_VALUE, FMath::Clamp(AccDeltaTime, 0.0F, 1.0F));
	}

	for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
	{
		if (SpawnedPresetCharacter[i])
		{
			UWRActorComponentEquipmentLook* ActorComponentEquipmentLook = SpawnedPresetCharacter[i]->GetComponent<UWRActorComponentEquipmentLook>();
			if (ActorComponentEquipmentLook == nullptr)
			{
				continue;
			}

			ActorComponentEquipmentLook->SetMaterialParameterValue(WR_WIREFRAME_PARAMETER_NAME, Ratio);
		}
		
	}
}

void WRCharacterCustomizingManager::PlayWireframeDirection()
{
	AccDeltaTime = 0.0F;

	IsGenderSelected = true;
	IsPlayback = false;
}

void WRCharacterCustomizingManager::PlaybackWireframeDirection()
{
	AccDeltaTime = 0.0F;

	IsGenderSelected = true;
	IsPlayback = true;
}

void WRCharacterCustomizingManager::PlayUnspawnDirection(AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
	{
		return;
	}

	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_CASCADE_PATH_4_DEAD, InCharacter, nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	FVector Scale = SkeletalMeshComponent->GetComponentScale();

	VFX->SetFloatParameter(WR_DISSOLVE_PARAMETER_NAME, Scale.Size() / 3.0F);

	UWRActorComponentManageMaterial* ActorComponentManageMaterial = InCharacter->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	ActorComponentManageMaterial->Spawn(EWRManageMaterial::Dissolve, WR_WIREFRAME_PARAMETER_MAX_VALUE);
}

void WRCharacterCustomizingManager::PlaybackUnspawnDirection(AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
	{
		return;
	}

	AWRVFX* VFX = WRVFXManager::Get()->Spawn(EFFECT_CASCADE_PATH_4_DEAD, InCharacter, nullptr);
	if (VFX == nullptr)
	{
		return;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = InCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		return;
	}

	FVector Scale = SkeletalMeshComponent->GetComponentScale();

	VFX->SetFloatParameter(WR_DISSOLVE_PARAMETER_NAME, Scale.Size() / 3.0F);

	UWRActorComponentManageMaterial* ActorComponentManageMaterial = InCharacter->GetComponent<UWRActorComponentManageMaterial>();
	if (ActorComponentManageMaterial == nullptr)
	{
		return;
	}

	ActorComponentManageMaterial->Remove(EWRManageMaterial::Dissolve, WR_WIREFRAME_PARAMETER_MAX_VALUE);
}

int32 WRCharacterCustomizingManager::ConvertHashToTID(const FString& InHash)
{
	if (Datas.Contains(InHash) == false)
	{
		return 0;
	}

	return Datas[InHash].Index;
}

uint8 WRCharacterCustomizingManager::ConvertCountToIndex(const int32 InCount, const int32 IN InPartsCount)
{
	int32 PartsCount = InPartsCount;

	uint8 CaculatedIndex = FMath::Abs(InCount) % PartsCount;
	uint8 Index = CaculatedIndex;

	if (InCount > 0)
	{
		Index = (PartsCount - CaculatedIndex) % PartsCount;
	}

	return Index;
}

int32 WRCharacterCustomizingManager::ConvertIndexToCount(const uint8 InIndex, const int32 IN InPartsCount)
{
	int32 PartsCount = InPartsCount;

	return PartsCount - InIndex;
}

class AWRStaticMeshActor* WRCharacterCustomizingManager::CreateStaticMeshActor(class UStaticMesh* InStaticMesh, const FVector& InScale /*= FVector::OneVector*/, const FVector& InPivotLocation /*= FVector::ZeroVector*/)
{
	if (InStaticMesh == nullptr)
	{
		return nullptr;
	}

	AWRStaticMeshActor* StaticActor = WRActorSpawnUtility::Spawn<AWRStaticMeshActor>(UWRGameInstance::GetGameInstance()->GetWorld(), AWRStaticMeshActor::StaticClass());
	if (StaticActor == nullptr)
	{
		return nullptr;
	}

	StaticActor->SetActorHiddenInGame(true);

	USceneComponent* SceneComponent = StaticActor->GetRootComponent();
	if (SceneComponent == nullptr)
	{
		return nullptr;
	}

	UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent);
	if (CapsuleComponent == nullptr)
	{
		return nullptr;
	}

	CapsuleComponent->SetWorldScale3D(InScale);
	CapsuleComponent->SetCollisionProfileName(TEXT("NoCollision"));

	UStaticMeshComponent* StaticMeshComponent = StaticActor->GetStaticMeshComponent();
	if (StaticMeshComponent == nullptr)
	{
		return nullptr;
	}

	StaticMeshComponent->SetStaticMesh(InStaticMesh);
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshComponent->SetRelativeLocation(InPivotLocation);

	return StaticActor;
}

class AWRSkeletalMeshActor* WRCharacterCustomizingManager::CreateSkeletalMeshActor(class USkeletalMesh* InSkeletalMesh, const FVector& InScale /*= FVector::OneVector*/, const FVector& InPivotLocation /*= FVector::ZeroVector*/)
{
	if (InSkeletalMesh == nullptr)
	{
		return nullptr;
	}

	AWRSkeletalMeshActor* SkeletalActor = WRActorSpawnUtility::Spawn<AWRSkeletalMeshActor>(UWRGameInstance::GetGameInstance()->GetWorld(), AWRSkeletalMeshActor::StaticClass());
	if (SkeletalActor == nullptr)
	{
		return nullptr;
	}

	SkeletalActor->SetActorHiddenInGame(true);

	USceneComponent* SceneComponent = SkeletalActor->GetRootComponent();
	if (SceneComponent == nullptr)
	{
		return nullptr;
	}

	UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(SceneComponent);
	if (CapsuleComponent == nullptr)
	{
		return nullptr;
	}

	CapsuleComponent->SetWorldScale3D(InScale);
	CapsuleComponent->SetCollisionProfileName(TEXT("NoCollision"));

	USkeletalMeshComponent* SkeletalMeshComponent = SkeletalActor->GetSkeletalMeshComponent();
	if (SkeletalMeshComponent == nullptr)
	{
		return nullptr;
	}

	SkeletalMeshComponent->SetSkeletalMesh(InSkeletalMesh);
	SkeletalMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshComponent->SetRelativeLocation(InPivotLocation);

	return SkeletalActor;
}

WRCharacterCustomizingManager::SpawningPresetCharacterInformation WRCharacterCustomizingManager::FindSpawningInformation(FString& InHash)
{
	SpawningPresetCharacterInformation ResultInfo;

	for (int32 i = 0; i < MAX_GENDER_COUNT; ++i)
	{
		if (SpawningPresetCharacterInfo[i].SpawningPresetHashKey == InHash)
		{
			return SpawningPresetCharacterInfo[i];
		}
	}

	return ResultInfo;
}

void WRCharacterCustomizingManager::InitializeDirection()
{
	if (IsAllCharacterSpawned())
	{
		TArray<AActor*> CharacterArray;

		for (int32 i = 0; i < MAX_GENDER_COUNT; ++i)
		{
			CharacterArray.Add(SpawnedPresetCharacter[i]);
		}

		TArray<FVector> Locations;

		float Distance = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeDistance);
		float Degree = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizeDegree);

		FVector AxisLocation = FVector::ForwardVector.RotateAngleAxis(Degree, FVector::UpVector) * Distance;

//#if UE_EDITOR
//		UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
//
//		FVector PureLocation = FVector::ForwardVector * Distance;
//
//		UKismetSystemLibrary::DrawDebugSphere(World, PureLocation - PureLocation, 10.0f, 12, FLinearColor::Red, 1000.0f, 1.0f);
//		UKismetSystemLibrary::DrawDebugSphere(World, PureLocation, 10.0f, 12, FLinearColor::Red, 1000.0f, 1.0f);
//		UKismetSystemLibrary::DrawDebugSphere(World, PureLocation + PureLocation, 10.0f, 12, FLinearColor::Red, 1000.0f, 1.0f);
//
//		UKismetSystemLibrary::DrawDebugSphere(World, AxisLocation, 10.0f, 12, FLinearColor::Yellow, 1000.0f, 1.0f);
//#else // UE_EDITOR
//#endif // UE_EDITOR

		GenderDirection.GetLocations(Locations, CharacterArray, Distance, AxisLocation, GenderSelectInformation.FocusedGender, Degree);

		for (int32 i = 0; i < CharacterArray.Num(); ++i)
		{
			CharacterArray[i]->SetActorLocation(Locations[i]);
		}

		TArray<float> CharacterScales;
		CharacterScales.Init(1.0F, MAX_GENDER_COUNT);

		GenderDirection.Initialize(CharacterArray, CharacterScales, AxisLocation, true);
	}
}

void WRCharacterCustomizingManager::InitializePartsDirection(const uint8 IN InCategoryIndex)
{
	if (GetSelectedCharacter())
	{
		LockInput();
		LockPhase();

		FVector CharacterLocation = GetSelectedCharacter()->GetActorLocation();

		//float Scale = WR_PRESETMESH_SIZE;
		float AdjustZ = WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizePresetHeight);
		float TableZValue = 0.0f;
		TArray<float> PartsScales;
		PartsScales.Init(WR_PRESETMESH_SIZE, PartsSelectInformation[InCategoryIndex].Parts.Num());

		uint8 Counter = 0;
		for (auto& Data : Datas)
		{
			if (Data.Value.CategoryTable == nullptr)
				continue;
			
			//TODO : 진용이 코드 수정으로 각각에 Scale 과 Z 값을 넣을 수 있게 해야됨.
			PartsScales[Counter] = Data.Value.CategoryTable->Scale[Data.Value.Index];
			// 광님 의견으로 맨 앞만 넣도록 임시수정 
			TableZValue = Data.Value.CategoryTable->PositionZ[0];
			++Counter;
		}

		// [ 2020-9-2 : vuvle ] 피드백으로 인한 Z값 보정
		//CharacterLocation.Z -= AdjustZ - TableZValue;
		CharacterLocation.Z -= WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizePresetHeight);
		PartsDirection.Initialize(PartsSelectInformation[InCategoryIndex].Parts, PartsScales, CharacterLocation);
		//SetPartsVisibility(PartsSelectInformation[InCategoryIndex].Parts, true);

		FWRCostomizeDirectionComplete Delegate;
		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnUpperPartsDirectionCompleted);

		int32 SelectedIndex = PartsSelectInformation[InCategoryIndex].SelectedIndex;
		//PartsSelectInformation[InCategory].SelectedIndex = SelectedIndex;
		int32 ClockwiseCount = ConvertIndexToCount(SelectedIndex, PartsSelectInformation[InCategoryIndex].Parts.Num());
		SubtractPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
		PartsDirection.BeginPlay(EWRCustomizeDirectionAction::Spawn, Delegate, true, ClockwiseCount);

		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::InitializeUpperPartsDirection] spawn : %d"), ClockwiseCount);
	}
}

void WRCharacterCustomizingManager::InitializePresetPartsDirection()
{
	if (GetSelectedCharacter())
	{
		LockInput();
		LockPhase();

		FVector CharacterLocation = GetSelectedCharacter()->GetActorLocation();

		TArray<float> PresetPartScales;
		PresetPartScales.Init(WR_PRESETMESH_SIZE, PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Num());

		// [ 2020-9-2 : vuvle ] 피드백으로 인한 Z값 보정
		CharacterLocation.Z -= WRConsoleManager::Get()->GetFloat(EWRConsole::CustomizePresetHeight);
		CustomizingDirection.Initialize(PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts, PresetPartScales, CharacterLocation, false);
		//SetPresetPartsVisibility(true);

		FWRCostomizeDirectionComplete Delegate;
		Delegate.BindRaw(this, &WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted);

		int32 SelectedIndex = ConvertPresetTIDToDirectionArrayIndex(PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID);
		PresetSelectInformation[GenderSelectInformation.FocusedGender].SelectedIndex = SelectedIndex;
		int32 ClockwiseCount = ConvertIndexToCount(SelectedIndex, PresetSelectInformation[GenderSelectInformation.FocusedGender].Parts.Num());
		//SubtractPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
		CustomizingDirection.BeginPlay(EWRCustomizeDirectionAction::Spawn, Delegate, true, ClockwiseCount);

		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::InitializePresetPartsDirection] spawn : %d"), ClockwiseCount);
	}	
}

void WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted(EWRCustomizeDirectionAction IN InAction)
{
	switch (InAction)
	{
	case EWRCustomizeDirectionAction::Spawn:
		
		ReleaseInput();
		break;
	case EWRCustomizeDirectionAction::Unspawn:
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted] Unspawn Direction Completed"));
		//Datas.Empty();
		//GenderDirection.Release();
		CustomizingDirection.Release();

		for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
		{
			PresetSelectInformation[i].ClearParts();
		}
		ReleaseInput();
		AddPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
		
		break;
	case EWRCustomizeDirectionAction::Rotation:
		ReleaseInput();
		ReleasePhaseLock();
		break;
	default:
		break;
	}
}

void WRCharacterCustomizingManager::OnUpperPartsDirectionCompleted(EWRCustomizeDirectionAction IN InAction)
{
	switch (InAction)
	{
	case EWRCustomizeDirectionAction::Spawn:
		ReleaseInput();
		break;
	case EWRCustomizeDirectionAction::Unspawn:
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted] Unspawn Direction Completed"));
		//Datas.Empty();
		//GenderDirection.Release();
// 		CustomizingDirection.Release();
// 
// 		for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
// 		{
// 			PresetSelectInformation[i].ClearParts();
// 		}
// 		ReleaseInput();
// 		AddPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
		//ReleaseInput();
// 		if (Panel_SubCharacterCreate)
// 		{
// 			Panel_SubCharacterCreate->SetBackButtonText(TEXT("AAAAA"));
// 		}
		
		LoadDetailPartsAsync();
		//SelectedPartsCategoryIndex = PartsCategory(PartsSelectInformation[SelectedPartsCategory].SelectedIndex);
		SelectedPartsCategoryIndex = (PartsCategory)PartsSelectInformation[SelectedPartsCategoryIndex].SelectedIndex;
		/*SelectedPartsCategoryTID = */
		break;
	case EWRCustomizeDirectionAction::Rotation:
		ReleaseInput();
		ReleasePhaseLock();
		break;
	default:
		break;
	}
}

void WRCharacterCustomizingManager::OnBackwardPartsDirectionComplelted(EWRCustomizeDirectionAction IN InAction)
{
	switch (InAction)
	{
	case EWRCustomizeDirectionAction::Spawn:
		ReleaseInput();
		break;
	case EWRCustomizeDirectionAction::Unspawn:
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::OnPresetPartsDirectionCompleted] Unspawn Direction Completed"));
		//Datas.Empty();
		//GenderDirection.Release();
// 		CustomizingDirection.Release();
// 
// 		for (int32 i = 0; i < (int32)Gender::MAX_GENDER_COUNT; ++i)
// 		{
// 			PresetSelectInformation[i].ClearParts();
// 		}
// 		ReleaseInput();
// 		AddPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
		if (SelectedPartsCategoryIndex == PartsCategory::Upper)
		{
			AddPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
		}
		else
		{
			//ReleaseInput();
			LoadUpperPartsAsync();
			SelectedPartsCategoryIndex = PartsCategory::Upper;
			/*SelectedPartsCategoryTID = */
		}
		break;
	case EWRCustomizeDirectionAction::Rotation:
		ReleaseInput();
		ReleasePhaseLock();
		break;
	default:
		break;
	}
}

void WRCharacterCustomizingManager::OnPhaseEndPartsDirectionCompleted(EWRCustomizeDirectionAction IN InAction)
{
	switch (InAction)
	{
// 	case EWRCustomizeDirectionAction::Spawn:
// 		/*ReleaseInput();*/
// 		break;
	case EWRCustomizeDirectionAction::Unspawn:
		UE_LOG(LogTemp, Log, TEXT("[WRCharacterCustomizingManager::OnPhaseEndPartsDirectionCompleted] Unspawn Direction Completed"));
		//ReleaseInput();
		AddPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
// 		if (SelectedPartsCategoryIndex == PartsCategory::Upper)
// 		{
// 			AddPhaseMoveBit(EWRPhaseMovableBitFlag::Parts);
// 			ReleaseInput();
// 		}
// 		else
// 		{
// 			ReleaseInput();
// 		}
		break;
	case EWRCustomizeDirectionAction::Rotation:
		ReleaseInput();
		ReleasePhaseLock();
		break;
	default:
		break;
	}
}
