// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "../Network/Share/Define/Define_Type.h"
#include <string>

struct FWRCustomizingSelectedCostumeItem
{
	TArray<WRTableID> CostumeItemArray;
	std::string NoneItemEquipmentLookData;
};

class WR_API WRAccountManager : public WRSingleton<WRAccountManager>
{
	CDECLARE_MANAGER(WRAccountManager);

private:
	WRAccountManager()
	{
		UserID = *FGenericPlatformMisc::GetDeviceId();
		MySpawnPosition = FVector::ZeroVector;
		MySpawnRotator = FRotator::ZeroRotator;
	}
	virtual ~WRAccountManager() {}

private:
	ID_t UserID;
	DKEY_t Dkey						= INVALID_DKEY;
	USER_GRADE_t UserGrade			= BASE_USER_GRADE;
	USN_t UserSerialNumber			= INVALID_USN;
	AID_t ActorID					= INVALID_AID;
	PSN_t PlayerSerialNumber		= INVALID_PSN;
	WRTableID PresetTableID			= INVALID_TABLE_TID;
	FWRCustomizingSelectedCostumeItem CustomizingSelectedCostumeItem;
	ID_t MacAddressTest;
	FName UserName;
	FVector MySpawnPosition;
	FRotator MySpawnRotator;
	WRTableID SpawnCoordinateTID = INVALID_TABLE_TID;

public:
	virtual void OnInitialize() override {};
	virtual void OnShutdown() override {};
	virtual void OnLoadLevelComplete() { InitializeMySpawnCoordinateTID(); };
	virtual void OnLoadLevelStart() override {};
	virtual void OnRenderStart() override {};

	FORCEINLINE ID_t					GetUserID() const { return UserID; }
	FORCEINLINE DKEY_t					GetDKey() const { return Dkey; }
	FORCEINLINE USN_t					GetUSN() const { return UserSerialNumber; }
	FORCEINLINE AID_t					GetAID() const { return ActorID; }
	FORCEINLINE PSN_t					GetPSN() const { return PlayerSerialNumber; }
	FORCEINLINE WRTableID				GetMyCharacterPresetTID() const { return PresetTableID; }
	FORCEINLINE WRTableID				GetMySpawnCoordinateTID() const { return SpawnCoordinateTID; }
	FORCEINLINE FVector					GetMySpawnPosition() { return MySpawnPosition; }
	FORCEINLINE FRotator				GetMySpawnRotator() { return MySpawnRotator; }
	FORCEINLINE FName					GetMyCharacterName() const { return UserName; }
	FORCEINLINE std::string&			GetCustomizingSelectedNoneItemEquipmentLookData() { return CustomizingSelectedCostumeItem.NoneItemEquipmentLookData; }
	FORCEINLINE TArray<WRTableID>&		GetCustomizingSelectedCostumeItemArray() { return CustomizingSelectedCostumeItem.CostumeItemArray; }
	FORCEINLINE void SetDKey(const DKEY_t IN InDKey) { Dkey = InDKey; }
	FORCEINLINE void SetUserGrade(const USER_GRADE_t IN InUserGrade) { UserGrade = InUserGrade; }
	FORCEINLINE void SetUSN(const USN_t IN InUserSerialNumber) { UserSerialNumber = InUserSerialNumber; }
	FORCEINLINE void SetAID(const AID_t IN InActorID) { ActorID = InActorID; }
	FORCEINLINE void SetPSN(const PSN_t IN InPlayerSerialNumber) { PlayerSerialNumber = InPlayerSerialNumber; }
	FORCEINLINE void SetMyCharacterPresetTID(const WRTableID IN InPresetTID) { PresetTableID = InPresetTID; }
	FORCEINLINE void SetMySpawnPosition(const FVector IN InPosition) { MySpawnPosition = InPosition; }
	FORCEINLINE void SetMySpawnRotator(const float IN InAngle) { MySpawnRotator = FRotator(0.0f, InAngle, 0.0f); }
	FORCEINLINE void SetMyCharacterName(const FName& IN InUserName) { UserName = InUserName; }
	FORCEINLINE void SetMySpawnCoordinateTID(const WRTableID IN InCoordinateTID) { SpawnCoordinateTID = InCoordinateTID; }
	FORCEINLINE void InitializeMySpawnCoordinateTID() { SetMySpawnCoordinateTID(INVALID_TABLE_TID); }
	FORCEINLINE void SetUserIDByConsole(const FName IN InUserID) { UserID = *InUserID.ToString(); }
	FORCEINLINE void RevertUserID() { FString Value = FGenericPlatformMisc::GetDeviceId(); SetUserIDByConsole(*Value); }
	FORCEINLINE void SetCustomizingSelectedCostumeItemArray(const TArray<WRTableID>& IN InArray) { CustomizingSelectedCostumeItem.CostumeItemArray = InArray; }
	FORCEINLINE void SetCustomizingSelectedNoneItemEquipmentLookData(const std::string IN InData) { CustomizingSelectedCostumeItem.NoneItemEquipmentLookData = InData; }
};
