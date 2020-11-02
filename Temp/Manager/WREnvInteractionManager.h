// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "SingletonClass/WRSingleton.h"
#include "Utility/WRActorSpawnUtility.h"
#include "Core/WRGameInstance.h"
#include "../Enum/EWRPath.h"
#include "Network/WRProtocolHeaders.h"
#include "Enum/EWRCharacter.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "WRPathManager.h"
#include "Struct/WREnvStruct.h"

#define DEF_CONVERT_IO_CLASS

// For Header
DECLARE_LOG_CATEGORY_EXTERN(EnvInteractionManager, Display, All);

struct FWREnvSpawnParams
{
public:
	FWREnvSpawnParams(const EWREnvType& InEnvType, const AID_t& InActorID, const WRTableID& InEnvTID, const FString& InBPPath, const FVector& InLocation, const FRotator& InRotation, FVector InScale = FVector::OneVector, int32 InTriggerSpawnID = 0)
		: 
		EnvType(InEnvType)
		, ActorID(InActorID)
		, EnvTableID(InEnvTID)
		, BPPath(InBPPath)
		, Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
		, TriggerSpawnID(InTriggerSpawnID)
	{}

	EWREnvType EnvType;
	AID_t ActorID;
	WRTableID EnvTableID;
	FString BPPath;
	FVector Location;
	FRotator Rotation;
	FVector Scale;
	int32	TriggerSpawnID;
};

class AWRSpiderWeb;

class WR_API WREnvInteractionManager : public WRSingleton<WREnvInteractionManager>
{
	CDECLARE_MANAGER(WREnvInteractionManager);

private:
	WREnvInteractionManager();
	virtual ~WREnvInteractionManager();
		
public:
	virtual void OnInitialize() override;
	virtual void OnShutdown() override;

	virtual void OnLoadLevelStart() override;
	virtual void OnLoadLevelComplete() override;

	virtual void OnRenderStart() override;

private:
	TArray<struct FWRFoliageTable*> FoliageDataArray;
	TArray<class UWRFoliageInstancedStaticMeshComponent*> InstancedFoliageComponents;
	//TMap<int32, class UWRFoliageInstancedStaticMeshComponent*> InstancedFoliageComponents;
	void CutStaticFoliage(UWRFoliageInstancedStaticMeshComponent* InFoliageComponent, const struct FHitResult& SweepResult);
	void CutDynamicFoliage(class AWRDynamicFoliage* InDynamicFoliage, const FHitResult& SweepResult);
	
public:
	void InitFoliageComponents();
	void InitializeTable();
	bool IsFoliage(const FHitResult& SweepResult);
	void OverlapFoliage(const FHitResult& SweepResult);
	void CutFoliage(const FHitResult& SweepResult);
	void ConvertDynamicFoliage(class AWRCharacter* OverlappedActor, const FHitResult& SweepResult);
	
	bool IsSpiderWeb(const FHitResult& SweepResult);
	void KickOffSpiderWeb(const FHitResult& SweepResult, AWRCharacter* InAttackedActor);
	void TangledInSpiderWeb(const FHitResult & SweepResult, AWRCharacter* InOverlappedActor);

	bool IsEnvActor(const FHitResult& SweepResult);
	void BeginEnvHittedInteraction(AWRCharacter* InCharacter, const FHitResult& InSweepResult);
	void BeginEnvOverlapInteraction(AWRCharacter* InCharacter, const FHitResult& InSweepResult);

	void AddMovingWalkway(class AWREnvMovingWalkway* IN InEnvMovingWalkway);
	void GetMovingWalkway(TArray<class AWREnvMovingWalkway*>& OUT InMovingwalkways);

	//====================================================================================
	// Spawned Env
	//====================================================================================
public:
	// Common
	void RemoveAll();

	// Find
	class AWREnvActor* FindEnvByAID(const AID_t IN InAID);
	class AWREnvVehicleBase* FindEnvVehicleByPassengerAID(const AID_t IN InPassengerAID);
	class AWREnvMovingWalkway* FindOverlapingMWByCharacterAID(const AID_t IN InCharacterAID);
	class AWREnvMovingWalkway* FindOverlappedMWByCharacterAID(const AID_t IN InCharacterAID);
	template <class TEnvType>
	TEnvType* FindbyAID_Casted(const AID_t IN InAID);

	template <class TEnvType>
	TEnvType* FindEnvVehicle(const AID_t IN InAID);

	// For Vehicle Etc.
	void UpdatePassengerLocation();
	bool IsEnvVehiclePassenger(const AID_t IN InPlayerAID, const EWREnvType IN InEnvType = EWREnvType::None);

	// Packet
	void Send_GetOnEnvVehicle(class AWRCharacterMine* IN InMine, class AWREnvVehicleBase* IN InEnvVehicle);
	void Send_GetOffEnvVehicle(class AWRCharacterPlayer* IN InPlayer);

	void OnInf_VehicleState(const pt_Game_Vehicle::Inf_TransportState_G2U& IN InInform);
	void OnInf_GetOn(const pt_Game_Vehicle::Inf_GetOnTransport_G2U& IN InInform);
	void OnInf_GetOff(const pt_Game_Vehicle::Inf_GetOffTransport_G2U& IN InInform);
	bool OnInf_ActorMoveToPos_G2U(const pt_Game_User::Inf_ActorMoveToPos_G2U& IN InInform);	
	bool OnInf_ActorMoveStop_G2U(const pt_Game_User::Inf_ActorMoveStop_G2U& IN InInform);
	void OnInf_GroupWarpTimerCount_G2U(const pt_Game_Vehicle::Inf_GroupWarpTimerCount_G2U& IN InInform);
	void OnInf_GroupWarpStop_G2U(const pt_Game_Vehicle::Inf_GroupWarpStop_G2U& IN InInform);

	// Packet [EZ] 자신이 무빙워크에 올라섬
	void OnReq_GetOnMW_U2G();
	void OnAck_GetOnMW_G2U(const stSmallDateTime& IN InStartTime);

	void OnReq_GetOffMW_U2G();
	void OnAck_GetOffMW_G2U();

	void OnInf_GetOnMW_G2U(const pt_Game_Vehicle::Inf_GetOnMW_G2U& IN InPacket);
	void OnInf_GetOffMW_G2U(const pt_Game_Vehicle::Inf_GetOffMW_G2U& IN InPacket);

	// Spawn Completed
	void OnPostSpawnCompleted(class AWREnvActor* InEnvActor);
	// Floating Board
	void SpawnFloatingBoard(const pt_Game_Actor::Inf_FloatingBoardAppear_G2U& IN InInform);
	void OnSpawnFloatingBoardCompleted(UObject* InObject, FString InHashKey);
	// Elevator
	void SpawnElevator(const pt_Game_Actor::Inf_ElevatorAppear_G2U& IN InInform);
	void OnSpawnElevatorCompleted(UObject* InObject, FString InHashKey);
	// Tardis
	void SpawnTardis(const pt_Game_Actor::Inf_TardisAppear_G2U& IN InInform);
	void OnSpawnTardisCompleted(UObject* InObject, FString InHashKey);

	// SecretDialogArea
	void ReserveDestroyDome();

	void SpawnSecretDialogDome(const CHAT_GROUP_ID_t IN InGroupID, const TArray<AID_t> InAIDs);
	void OnSpawnSecretDialogDomeCompleted(UObject* InObject, FString InHashKey);
	void UnSpawnSecretDialogDome();
	void SpawnSecretDialogPortal(const FWRSecretDialogInfo& InInfo);
	void OnSpawnSecretDialogPortalCompleted(UObject* InObject, FString InHashKey);
	void UnSpawnPortal(const AID_t& InPortalAID);
	void SpawnSecretDialogArea(const FWRSecretDialogInfo& InAreaInfo);
	void OnSpawnSecretDialogAreaCompleted(UObject* InObject, FString InHashKey);
	void UnSpawnSecretDialogArea();
	void StartSecretDialogProcess();

	FORCEINLINE class AWRSecretDialogArea* GetCurrentSecretArea() { return SpawnedSecretDialogArea; }
	class AWRSecretDialogPortal* FindPortalByGroupID(const CHAT_GROUP_ID_t IN InGroupID);



	void BeginClosePortalProcess(const AID_t& InPortalAID);

	//====================================================================================
	// Not Spawned (Env Placed On Level)
	//====================================================================================
	// Outter Door
	void AddVehicleOutterDoor(class AWREnvSlideDoor* IN InOutterDoor);
	
private:
	class AWRSecretDialogDome*                  SpawnedSecretDialogDome  = nullptr;   // Spawned
	class AWRSecretDialogArea*                  SpawnedSecretDialogArea = nullptr;    // Spawned
	TArray<class AWREnvActor*>			        SpawnedEnvActors;		              // Spawned
	TArray<class AWREnvVehicleBase*>	        SpawnedEnvVehicle;		              // Spawned
	TArray<class AWRSecretDialogPortal*>        SpawnedPortals;	     	              // Spawned
	TArray<class AWREnvSlideDoor*>		        VehicleOutterDoor;		              // Not Spawned
	TArray<class AWREnvMovingWalkway*>	        Movingwalkways;		                  // Not Spawned
	//Async
	TMap<FString, AID_t> AsyncSpawnkey;
	TMap<FString, pt_Game_Actor::Inf_FloatingBoardAppear_G2U> FloatingBoardSpawnInfo;
	TMap<FString, pt_Game_Actor::Inf_ElevatorAppear_G2U> ElevatorSpawnInfo;
	TMap<FString, pt_Game_Actor::Inf_TardisAppear_G2U> TardisSpawnInfo;
	TMap<FString, FWRSecretDialogInfo> SecretDialogSpawnInfo; // 추후 AreaInfo용 패킷 저장

	bool IsReserveSpawnDome = false;
	bool IsReserveSpawnEnvArea = false;
};


template <class TEnvType>
TEnvType* WREnvInteractionManager::FindbyAID_Casted(const AID_t IN InAID)
{
	if (InAID == INVALID_AID)
		return nullptr;

	class AWREnvActor* FoundEnvActor = FindEnvByAID(InAID);
	if (FoundEnvActor == nullptr)
		return nullptr;

	TEnvType* CastedEnv = Cast<TEnvType>(FoundEnvActor);
	return CastedEnv;
}

template <class TEnvType>
TEnvType* WREnvInteractionManager::FindEnvVehicle(const AID_t IN InAID)
{
	if (InAID == INVALID_AID)
		return nullptr;

	class AWREnvActor* FoundEnvActor = FindEnvByAID(InAID);
	if (FoundEnvActor == nullptr)
		return nullptr;

	if (FoundEnvActor->IsVehicle() == false)
		return nullptr;

	TEnvType* CastedEnvVehicle = Cast<TEnvType>(FoundEnvActor);
	return CastedEnvVehicle;
}


DECLARE_STATS_GROUP(TEXT("EnvInteractionManager"), STATGROUP_CEnvInteractionManager, STATCAT_Advanced);