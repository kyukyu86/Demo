// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "Table/WRCharacterPresetTable.h"
#include "../Enum/EWRCharacter.h"


/**
 * 
 */

DECLARE_DELEGATE_OneParam(FWRCostomizeDirectionComplete, EWRCustomizeDirectionAction);


struct FWRCustomizeDirectionData
{
public:
	TArray<FVector> Locations;

	EWRCustomizeDirectionAction Action = EWRCustomizeDirectionAction::None;

	float AccDeltaTime = 0.0F;
	float DirectionSpeed = 1.0F;

	bool IsClockwise = true;
	bool IsInitialized = false;

	FWRCostomizeDirectionComplete DirectionDelegate = nullptr;

	FVector PlayerLocation = FVector::ZeroVector;
	FVector CameraDirection = FVector::ForwardVector;
};

struct FWRCustomizeDirection
{
public:
	FWRCustomizeDirection();
	virtual ~FWRCustomizeDirection();

public:
	void Initialize(const TArray<AActor*>& InActors, const TArray<float>& InScales, const FVector& InLocation, const bool InIsCharacter = false);
	void Release();

	bool BeginPlay(const EWRCustomizeDirectionAction InAction, FWRCostomizeDirectionComplete InDelegate, const bool InIsClockwise = true, const int32 InSelectedIndex = 0, const float InDegree = 0.0f, const float InDirectionSpeed = 1.0f, const FName& InBoneName = NAME_None);

	void GetLocations(TArray<FVector>& OutLocations, const TArray<AActor*>& InActors, const float InLength, const FVector& InLocation, const int32 InSelectedIndex = 0, const float InDegree = 0.0f);

	bool IsDirecting() {
		return DirectionData.IsInitialized;
	}

private:
	bool Tick(float DeltaTime);

	void OnPreComplete();
	void OnPostComplete();

	FVector GetPlayerStart();

private:
	FWRCustomizeDirectionData DirectionData;

	FDelegateHandle TickHandle;

	TArray<AActor*> Actors;
	TArray<float> Scales;

	float LocationAccumulation = 0.0f;
	float RotationAccumulation = 0.0f;

	FVector AxisLocation = FVector::ZeroVector;

	bool IsCharacter = false;
};

struct FWRPartsData
{
	int32 Index = 0;
	struct FWRCustomizingCategoryTable* CategoryTable = nullptr;
	bool IsLoaded = false;

	void Clear() { Index = 0; IsLoaded = false; CategoryTable = nullptr; }
};

enum EWRCharacterCustomizingPhase
{
	None,
	GenderSelect,
	PresetSelect,
	SkinColor,
	PartsDetailSetting,
	Naming,
	MAX_PHASE_COUNT,
};

enum EWRCharacterCustomizingPhaseMove
{
	Next,
	Previous,
	End,
};

enum EWRPhaseMovableBitFlag : uint16
{
	Character = 0x0001,
	Parts = 0x0002,
	UIAnimation = 0x0004,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FWRCharacterCostomizingPhaseCompleted, const EWRCharacterCustomizingPhase&);
DECLARE_MULTICAST_DELEGATE_OneParam(FWRCharacterCostomizingPhaseChanged, const EWRCharacterCustomizingPhase&);
// [ 2020-8-21 : vuvle TODO] : 현재 Character Manager 와 분리 하기 위해 Singleton 을 사용 하였지만 추후 Game Mode 에 종속 해도 되는지 고려
class WR_API WRCharacterCustomizingManager : public WRSingleton<WRCharacterCustomizingManager>
{
	CDECLARE_MANAGER(WRCharacterCustomizingManager);

private:
	WRCharacterCustomizingManager() {}
	virtual ~WRCharacterCustomizingManager() {}

public:
	enum Gender
	{
		Female,
		Male,
		
		MAX_GENDER_COUNT,
	};

  	enum PartsCategory
  	{
		/*Hair,
		Head,
		Glasses,
		Beard,
		Mask,
		Outfit,
		Backpack,
		Gloves,
		Shoes,*/
		PART01,
		PART02,
		PART03,
		PART04,
		PART05,
		PART06,
		PART07,
		PART08,
		PART09,
		PART10,
		PART11,
		PART12,
 		Upper = 12,
 		MAX_CATEGORY_COUNT,
  	};

private:
	struct SpawningPresetCharacterInformation
	{
		EWRCharacterGender	  SpawningPresetGender = EWRCharacterGender::Female_A;
		WRTableID SpawningPresetTID = INVALID_TID;
		FString	  SpawningPresetHashKey = NULL_STRING;
		//AWRCharacter* SpawnedCharacter = nullptr;

		SpawningPresetCharacterInformation() {}
		explicit SpawningPresetCharacterInformation(EWRCharacterGender IN InGender, WRTableID IN InTableID, FString IN InHashKey) : SpawningPresetGender(InGender), SpawningPresetTID(InTableID), SpawningPresetHashKey(InHashKey) {};
		void Clear() { SpawningPresetTID = INVALID_TID, SpawningPresetHashKey = NULL_STRING; }
		void Initialize() {	SpawningPresetGender = EWRCharacterGender::Female_A; SpawningPresetTID = INVALID_TID; SpawningPresetHashKey = NULL_STRING; }
	};

	struct FWRGenderSelectInformation
	{
		Gender FocusedGender = Gender::Female;
		WRTableID FocusedPresetTID = INVALID_TID;
		void Initialize() { FocusedGender = Gender::Female; FocusedPresetTID = INVALID_TID; }
	};

	struct FWRPresetSelectInformation
	{
		WRTableID EquippedPresetTID = INVALID_TID;
		uint8 SelectedIndex = 0;
		int32 ClockwiseCount = 0;
		uint8 TryingLoadPartsCount = 0;
		TArray<AActor*> Parts;
		
		void ClearParts()
		{
			for (auto& Part : Parts)
			{
				Part->Destroy();
			}
			Parts.Empty();
		}
		
		void Initialize() { EquippedPresetTID = INVALID_TID; SelectedIndex = 0; TryingLoadPartsCount = 0; ClearParts(); }
	};

	struct FWRPartsSelectInformation
	{
		uint8 SelectedIndex = 0;
		WRTableID SelectedPartsTID = INVALID_TID;
		WRTableID EquippedPartsTID = INVALID_TID;
		struct FWRCustomizingCategoryTable* CategoryTable = nullptr;
 		TArray<AActor*> Parts;
		EWREquipPart PartType;
 		
		void ClearParts()
		{
			for (auto& Part : Parts)
			{
				Part->Destroy();
			}
			Parts.Empty();
		}

		void Initialize() {
			SelectedPartsTID = INVALID_TID; EquippedPartsTID = INVALID_TID; SelectedIndex = 0; ClearParts(); PartType = EWREquipPart::None; CategoryTable = nullptr; }
	};

	struct FWRNameInformation
	{
		FString CharacterName = NULL_STRING;

		void Initialize() { CharacterName = NULL_STRING; }
	};

	bool bPhaseLocked = false;
	bool bReadyToAllowInput = false;
	EWRCharacterCustomizingPhase ReservedPhase = EWRCharacterCustomizingPhase::None;
	BitFlag16 PhaseMovableBitFlag = 0/*(uint16)EWRPhaseMovableBitFlag::None*/;
	AWRCharacter* SpawnedPresetCharacter[MAX_GENDER_COUNT];
	EWRCharacterCustomizingPhase Phase = EWRCharacterCustomizingPhase::None;
	
	// Informations
	
	SpawningPresetCharacterInformation SpawningPresetCharacterInfo[MAX_GENDER_COUNT];
	FWRCustomizeDirection GenderDirection;
	FWRCustomizeDirection CustomizingDirection;
	FWRCustomizeDirection PartsDirection;
	FWRGenderSelectInformation GenderSelectInformation;
	FWRPresetSelectInformation PresetSelectInformation[MAX_GENDER_COUNT];
	PartsCategory SelectedPartsCategoryIndex = PartsCategory::Upper;
	WRTableID SelectedPartsCategoryTID = INVALID_TID;
	FWRPartsSelectInformation PartsSelectInformation[MAX_CATEGORY_COUNT];
	FWRNameInformation NameInformation;

	// Delegate
	FWRCharacterCostomizingPhaseCompleted OnPhaseCompletedDelegate;
	FWRCharacterCostomizingPhaseChanged OnPrePhaseChangedDelegate;
	FWRCharacterCostomizingPhaseChanged OnPostPhaseChangedDelegate;

	// UI
	class UWRUIPanel_NewCharacterCreate* Panel_NewCharacterCreate;
	class UWRUIPanel_SubCharacterCreate* Panel_SubCharacterCreate;

private:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;
	virtual void OnLoadLevelComplete() override {};
	virtual void OnLoadLevelStart() override;
	virtual void OnRenderStart() override {};
	virtual bool OnTick(float InDeltaTime);

	bool IsAllCharacterSpawned() { return SpawnedPresetCharacter[Gender::Male] && SpawnedPresetCharacter[Gender::Female] ? true : false; }
	Gender GetUnFocusedGender() { return GenderSelectInformation.FocusedGender == Gender::Male ? Gender::Female : Gender::Male; }
	void OpenUI();
	void SpawnPresetCharacters();
	void CreateSubUI();
	void ClearAllInformations();
	void ClearInformation(const FString& IN InHash);
	SpawningPresetCharacterInformation LoadPresetCharacterAsync(const WRTableID IN InTableID);
	void LoadPresetPartsAsync();
	void LoadUpperPartsAsync();
	void LoadDetailPartsAsync();

	void OnPresetCharacterSpawnCompleted(UObject* InActor, FString InHash);
	void OnPresetPartsSpawnCompleted(UObject* InActor, FString InHash);
	void OnUpperPartsSpawnCompleted(UObject* InActor, FString InHash);
	void OnLowerPartsSpawnCompleted(UObject* InActor, FString InHash);
	void OnSwapGenderDirectionCompleted(EWRCustomizeDirectionAction IN InAction);
	void OnPresetPartsDirectionCompleted(EWRCustomizeDirectionAction IN InAction);
	void OnUpperPartsDirectionCompleted(EWRCustomizeDirectionAction IN InAction);
	void OnBackwardPartsDirectionComplelted(EWRCustomizeDirectionAction IN InAction);
	void OnPhaseEndPartsDirectionCompleted(EWRCustomizeDirectionAction IN InAction);
	SpawningPresetCharacterInformation FindSpawningInformation(FString& InHash);
	void InitializeDirection();
	//void InitializePartsDirection(const PartsCategory IN InCategory);
	void InitializePartsDirection(const uint8 IN InCategoryIndex);
	void InitializePresetPartsDirection();
	void OnPresetEquipCompleted(AActor* IN InActor, EWREquipPart IN InEquipPart);
	void OnPartsEquipCompleted(AActor* IN InActor, EWREquipPart IN InEquipPart);
	

	// Phase
	bool IsPhaseMovable();
	void ReservePhase(const EWRCharacterCustomizingPhase IN InPhase);
	void ResetReservedPhase() { ReservedPhase = EWRCharacterCustomizingPhase::None; }

	// Parts
	//EWREquipPart ConvertPartsCategoryToEquipPart(const PartsCategory IN InCategory);
	void SetDefaultPartsCategoryTID();
	
public:
	AWRCharacter* GetSelectedCharacter() { return SpawnedPresetCharacter[GenderSelectInformation.FocusedGender]; }

	// Preset
	void SetPresetPartsVisibility(const bool IN InVisibility);
	void SetPartsVisibility(TArray<AActor*>& IN InActorArray, const bool IN InVisibility);

public:
	//void StartPresetCharacterSpawnProcess();
	FString GetCharacterName() { return NameInformation.CharacterName; }
	void LockInput() { bReadyToAllowInput = false; }
	void ReleaseInput() { bReadyToAllowInput = true; }
	bool IsInputAllowed() { return bReadyToAllowInput; }
	int32 ConvertPresetTIDToDirectionArrayIndex(const WRTableID IN InTableID);
	WRTableID ConvertDirectionArrayIndexToPresetTID(const int32 IN InArrayIndex); 
	WRTableID GetSelectedPresetTID() { return PresetSelectInformation[GenderSelectInformation.FocusedGender].EquippedPresetTID; }
	void ManufactureAllData();
	// Character
	void RemoveCharacters(const bool IN bInDissolve);
	void RemoveCharacter(class AWRCharacter* IN InCharacterToRemove);
	void RemoveCharacter(const Gender IN InGenderToRemove, const bool IN bInDissolve);
	void StartCharacterCreationProcess();
	void FinishCharacterCreationProcess();
	void SwapPresetCharacter();
	void OnEachProcessCompleted();
	void SetEnableDisplayURODebug(const bool bInEnable);

	// Phase
	void AddPhaseMoveBit(const EWRPhaseMovableBitFlag IN InFlag);
	void SubtractPhaseMoveBit(const EWRPhaseMovableBitFlag IN InFlag);
	void MovePhaseTo(const EWRCharacterCustomizingPhase& IN InPhase, const bool IN bInForceToReleaseLock = false);
	void MoveToNextPhase(const bool IN bInForceToReleaseLock = false);
	void MoveToPreviousPhase(const bool IN bInForceToReleaseLock = false);
	void LockPhase() { bPhaseLocked = true; }
	void ReleasePhaseLock() { bPhaseLocked = false; }
	bool IsPhaseLocked() const { return bPhaseLocked; }
	void CompletePhase(const EWRCharacterCustomizingPhaseMove IN InPhaseMove = EWRCharacterCustomizingPhaseMove::Next);
	void CompletePhase(const EWRPhaseMovableBitFlag IN InFlagToAdd);
	//void CompletePhase(const EWRCharacterCustomizingPhase IN InNextPhaseToMove);

	EWRCharacterCustomizingPhase& GetPhase() { return Phase; }
	void OnPhaseCompleted(const EWRCharacterCustomizingPhase IN InCompletedPhase, const EWRCharacterCustomizingPhaseMove IN InPhaseMove = EWRCharacterCustomizingPhaseMove::Next);
	void OnPrePhaseChanged(const EWRCharacterCustomizingPhase IN InNewPhase);
	void OnPostPhaseChanged(const EWRCharacterCustomizingPhase IN InNewPhase);
	FWRCharacterCostomizingPhaseChanged& GetPhaseCompletedDelegate() { return OnPhaseCompletedDelegate; }
	FWRCharacterCostomizingPhaseChanged& GetPrePhaseChangedDelegate() { return OnPrePhaseChangedDelegate; }
	FWRCharacterCostomizingPhaseChanged& GetPostPhaseChangedDelegate() { return OnPostPhaseChangedDelegate; }

	// Gender
	Gender GetSelectedGender() { return GenderSelectInformation.FocusedGender; }
	void SwapGender(const bool IN bInClockWise = true);
	void SwapPresetPart(const bool IN bInClockWise, bool& OUT bOutSwappedPresetAsEquippedPreset);
	void SwapParts(const bool IN bInClockWise, bool& OUT bOutSwappedItemSameAsEquippedItem);
	void OnGenderSelected();

	// Preset
	bool IsEquippedPresetAndSelectedPresetSame();
	bool EquipPreset();
	void OnPresetSelectCompleted();

	// Parts
	void SelectPart();
	void BackwardFromDetailPart();
	void EquipParts();
	FString GetSelectedPartsName();
	bool IsUpperPartsSelecting() { return SelectedPartsCategoryIndex == PartsCategory::Upper ? true : false; }

	// Packet
	void OnSelectPlayerNameReceived(const FString& IN InName);

	void animal14(const FString& InVar);
	
private:
	void EquipmentComplete(AActor* InActor, EWREquipPart InPart);

	void Tick4WireframeDirection(float InDeltaTime);
	void PlayWireframeDirection();
	void PlaybackWireframeDirection();

	void PlayUnspawnDirection(AWRCharacter* InCharacter);
	void PlaybackUnspawnDirection(AWRCharacter* InCharacter);

	int32 ConvertHashToTID(const FString& InHash);	

	uint8 ConvertCountToIndex(const int32 InCount, const int32 IN InPartsCount);
	int32 ConvertIndexToCount(const uint8 InIndex, const int32 IN InPartsCount);

	class AWRStaticMeshActor* CreateStaticMeshActor(class UStaticMesh* InStaticMesh, const FVector& InScale = FVector::OneVector, const FVector& InPivotLocation = FVector::ZeroVector);
	class AWRSkeletalMeshActor* CreateSkeletalMeshActor(class USkeletalMesh* InSkeletalMesh, const FVector& InScale = FVector::OneVector, const FVector& InPivotLocation = FVector::ZeroVector);

private:
	float AccDeltaTime = 0.0f;

	bool IsGenderSelected = false;
	bool IsPlayback = false;

	TArray<EWREquipPart> EquippingParts;

	TMap<FString, FWRPartsData> Datas;
	TMap<AWRCharacter*, TMap<EWREquipPart, bool>*> Completes;
};


