#pragma once
#include "CoreMinimal.h"
#include "Manager/SingletonClass/WRSingleton.h"
#include "Network/Share/Define/Define_Type.h"
#include "Define/WRTypeDefine.h"
#include "Enum/EWRCharacter.h"
#include "Network/WRProtocolHeaders.h"
#include "WRSpawnSimulationManager.h"

DECLARE_LOG_CATEGORY_EXTERN(VehicleManager, Display, All);
class AWRVehicle;
class AActor;

class WR_API WRVehicleManager : public WRSingleton<WRVehicleManager>
{
	CDECLARE_MANAGER(WRVehicleManager);

public:
	WRVehicleManager();
	virtual ~WRVehicleManager();

public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;
	//====================================================================================

	virtual void EndLoadLevelProcess() override;

	virtual bool OnTick(float InDeltaTime) override;

public:
	int32 GetSeatIndex(AID_t InVehicleAID, AID_t InPassengerAID);
	AWRVehicle* FindVehicleByAID(AID_t InAID);
	EWRVehicleSeatSide GetSeatingSide(AID_t InPassengerAID);
	bool IsDriver(AID_t InActorID, AID_t InVehicleAID);
	void GetAllVehicle(TArray<AActor*>& InoutActor);

public: //Only for me
	void SetMyVehicleInfoToBoarding(const FString& InName, AID_t InAID);
	FString GetSeatName2Boarding() { return SeatName2Boarding; }
	void SpawnVehicleFromCheat();
	bool IsValidSapwnedVehicleISN(const ISN_t IN InCompareISN);
	bool IsSpawnedVehicle();

public:	//request
	void RequestSpawnVehicle(ISN_t InISN, WRTableID InTID);
	void RequestUnspawnVehicle(ISN_t InISN);
	bool RequestGetOnVehicle();
	bool RequestGetOffVehicle();

public: //receive
	void OnInformSpawnVehicle(const pt_Game_Actor::Inf_VehicleAppear_G2U& InPacket);
	void OnAckSpawnVehicle(const pt_Game_Vehicle::Ack_SpawnVehicle_G2U& InAck);
	void OnAckUnspawnVehicle(const pt_Game_Vehicle::Ack_UnspawnVehicle_G2U& InAck);

public:
	void SpawnVehiceToSimulate(ISN_t InISN, WRTableID InTID);
	void OnSpawnVehicleCompleted(UObject* InActor, FString InHashKey);
	void OnSpawnVehicleCompletedWhenSimulating(UObject* InActor, FString InHashKey);

	//�������� ������
	void OnSetVehicleState(AWRVehicle* InVehicle, const pt_Game_Actor::Inf_VehicleAppear_G2U& InPacket);

	void RemoveVehicle(AWRVehicle* InVehicle, bool IsDirecting = false);
	void RemoveVehicle(AID_t InAID, bool IsDirecting = false);
	void RemoveSimulationVehicle();

	bool InputEvent(EWRInput InInputType, EInputEvent InEventType);
	bool InputEventToControllVehicle(EWRInput InInputType, EInputEvent InEventType);
	bool InputEventToSimulateSpawn(EWRInput InInputType, EInputEvent InEventType);

	void OnRequestLetMeBoardingTimeOver(AID_t InAID, AID_t InVehicleAID, int8 InSeatIndex);

	void MakeSpawnArea();
	EWRSimulationResultType IsValidSpawnLocation(AActor* InActor);

	void Message(EWRSimulationResultType InType);

private:
	void Remove_Internal(AWRVehicle* InVehicle);

private:
	bool SetMaterialParameter_deprecated(AActor* InActor, float InValue);

	
private://Only for me
	FString SeatName2Boarding = "";
	AID_t VehicleAID2Boarding = INVALID_AID;
	ISN_t VehicleItemID = INVALID_ISN;

private:
	FVector BoxPoint[4];
	FVector EdgeLines[4];
	static const int32 SpawnVehicleAreaLength;

private: //all about vehicle Info
	TMap<AID_t, AWRVehicle*> VehicleContainer;	//<vehicle aid, vehicle ptr>

	//Async load
	TMap<FString, AID_t> AsyncSpawnVehicleKey;
	TMap<AID_t, pt_Game_Actor::Inf_VehicleAppear_G2U> VehicleSpawnInfo;




// 	SpawnSimulationState SimulationState = SpawnSimulationState::None;
// 	AWRVehicle* Vehicle = nullptr;
// 	FString HashKeyToSimulate = "";
// 	FVector ForwardVector = FVector::ZeroVector;
// 	FVector RightVector = FVector::ZeroVector;
};

