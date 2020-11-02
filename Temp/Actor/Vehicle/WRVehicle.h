// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Enum/EWRActorComponent.h"

#include "Interface/Actor/WRActorComponentInterface.h"

#include "Network/Share/Define/Define_Type.h"
#include "Network/WRProtocolHeaders.h"

#include "Manager/WRVFXManager.h"

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "WRVehicle.generated.h"

struct FPassengerInfo
{
	AID_t PassengerAID;
	int8 RidePositionIndex;
};

UCLASS()
class WR_API AWRVehicle : public AWheeledVehicle, public IWRActorComponentInterface
{
	GENERATED_BODY()

	class USpringArmComponent* SpringArm;

	class UCameraComponent* Camera;

	UCameraComponent* InternalCamera;

	class USceneComponent* InternalCameraBase;


public:
	AWRVehicle(const FObjectInitializer & ObjectInitializer);
	AWRVehicle();
	virtual ~AWRVehicle();
	FText SpeedDisplayString;
	FText GearDisplayString;
	FColor	GearDisplayColor;
	FColor	GearDisplayReverseColor;
	bool bInCarCameraActive;
	bool bInReverseGear;
	FVector InternalCameraOrigin;

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void OutsideWorldBounds() override;
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	void PopupForOutofBound();

private:
	bool bOutsideWorldBounds;

public:
	// Begin Actor interface
	virtual void Tick(float Delta) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnPostSpawn();

	virtual void OnPreDestroy();
	void RegistOverlapCollision();

protected:
	void BuildComponent();

public:
	bool SetLocation(const FVector& NewLocation, bool bModifyZ, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	bool SetRotation(FRotator NewRotation, ETeleportType Teleport = ETeleportType::None);
	bool SetLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bModifyZ, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	bool SetActorLocation(const FVector& NewLocation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);
	bool SetActorLocationAndRotation(FVector NewLocation, FRotator NewRotation, bool bSweep = false, FHitResult* OutSweepHitResult = nullptr, ETeleportType Teleport = ETeleportType::None);

public:
	UFUNCTION()
	void OnBeginOverlapVehicleSeatPosition(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapVehicleSeatPosition(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnComponentHitPhysics(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlapVehicleForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapVehicleForSimulating(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBeginOverlapAutoPilotSensor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapAutoPilotSensor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	
	void MoveForwardCar(float Val);
	void MoveRight(float Val);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void TickExecuteThrottleAndSteering(float InDelta);
	void TickRequestThrottleAndSteering(float InDelta);

	void TurnRight(float InScale);
	void OnToggleCamera();
	void OnResetVR();
	void SetupInCarHUD();
	void UpdatePhysicsMaterial();

	void OpenLeftDoor(int32 InSeatIndex, bool InOpen);
	void OpenRightDoor(int32 InSeatIndex, bool InOpen);

	void EnableIncarView(const bool bState);
	void UpdateHUDStrings();

	void SetPassengerInfo(int32 InIndex, AID_t InAID);
	void MakePassengerInfo(WRTableID InTID, std::vector<AID_t> InPassengerInfo);

	class UWRVehicleMovementComponent4W* GetWRVehicleMovement();

	void SwitchCamera();
	void InitVehicleState();
	void ToggleAutoPilot(bool bToggle);
	void ToggleAutoPilotIndicator(bool bToggle);

	void PlayEngineSound();
	void SetVehicleSimulatePhysics(bool bFlag);

public:
	bool OwnerIsMe();
	bool DriverIsMe();
	bool IsSomeoneDriving();
	int32 GetSeatIndex(AID_t InPassengerAID);
	void SetVehicleAndOwnerAID(AID_t InAID, AID_t InOwnerAID);
	AID_t GetVehicleAID();
	AID_t GetVehicleOwnerAID();
	void OnReceivedLetSomeoneBoardingTimeOver(AID_t InAID, AID_t InVehicleAID, int8 InSeatIndex);

	float GetCurrentRPM();
	float GetCurrentKPH();

public:	//packet
	void OnInfGetOnVehicle(const pt_Game_Vehicle::Inf_GetOnVehicle_G2U& IN InPacket);
	void OnInfGetOffVehicle(const pt_Game_Vehicle::Inf_GetOffVehicle_G2U& IN InPacket);
	void OnInfBeginBrake(const pt_Game_Vehicle::Inf_BeginBrake_G2U& IN InPacket);
	void OnInfEndBrake(const pt_Game_Vehicle::Inf_EndBrake_G2U& IN InPacket);
	void OnInfAcceleration(const pt_Game_Vehicle::Inf_Acceleration_G2U& IN InPacket);
	void OnInfSteering(const pt_Game_Vehicle::Inf_Steering_G2U& IN InPacket);
	void OnCmdAgreeToBoard(const pt_Game_Vehicle::Cmd_AgreeToBoard_G2U& IN InPacket);
	void OnInfHorning(const pt_Game_Vehicle::Inf_BeHorning_G2U& IN InPacket);
	void OnInfUpdateExterior(const pt_Game_Vehicle::Inf_UpdateExterior_G2U& IN InPacket);
	void OnInfCancelOnBoard(const pt_Game_Vehicle::Inf_CancelOnBoard_G2U& IN InPacket);
	void OnInfSyncPhysicForce();

public:
	bool RequestAccelator(float InValue, bool bResetBrake = true);
	bool RequestSteering(float InValue);
	void RequestBeginBrake();
	void RequestEndBrake();
	void RequestToggleRoof();
	void RequestToggleLight();
	void RequestTurnSignal(bool IsLeft);

public:
	template <typename T>
	void SetMaterialParameterValue(FName InParameterName, T InValue, class UMeshComponent* InMeshComponent = nullptr);

public:
	void RemoveVehicle();

private:
	void OnCompleteDissolve();

public:
	WRDestoryHandle& OnDestroy() { 
		return DestoryHandle;
	}

private:
	WRDestoryHandle DestoryHandle;

private:
	void SetMaterialParameterValue_Internal(FName InParameterName, float InValue, class UMeshComponent* InMeshComponent = nullptr);
	void SetMaterialParameterValue_Internal(FName InParameterName, FLinearColor InValue, class UMeshComponent* InMeshComponent = nullptr);

public:
	void StartSmoke(float InValue);
private:
	void SteerSmoke(float InValue);

private:
	bool IsSpawnedStartSmoke = false;
	bool IsSpawnedSteerSmoke = false;

private:
	bool bIsLowFriction;
	UPhysicalMaterial* SlipperyMaterial;
	UPhysicalMaterial* NonSlipperyMaterial;

	//서버에 요청
	float RequestedAccelationValue = 0.0f;
	float RequestedSteeringValue = 0.0f;

	//실제 받아서 처리 해야 할 내용
	float ReceivedAccelationValue = 0.0f;
	float ReceivedSteeringValue = 0.0f;
	FVector ReceivedAccLocation = FVector::ZeroVector;
	FRotator ReceivedAccRotation = FRotator::ZeroRotator;
	float ReceivedSpeed = 0.0f;

	float ElapsedTime2RequestAccelation = 0.0f;
	float ElapsedTime2RequestSteering = 0.0f;

private:
	AID_t VehicleAID;
	AID_t VehicleOwnerAID;
	TArray<AID_t>	PassengerInfo; //
	TArray<FPassengerInfo> tempPassengerInfo;

public:
	static const FName LookUpBinding;
	static const FName LookRightBinding;
	static const FName EngineAudioRPM;	

private:
	bool bVehicleSpawnArea = false;

	UStaticMeshComponent* RPMComponent = nullptr;
	UStaticMeshComponent* KPHComponent = nullptr;

public:
	void SetVehicleInSpawnVolume(bool bInVehicleSpawnArea);
	bool GetVehicleInSpawnVolume() { return bVehicleSpawnArea; }
};

template <typename T>
void AWRVehicle::SetMaterialParameterValue(FName InParameterName, T InValue, UMeshComponent* InMeshComponent /*= nullptr*/)
{
	SetMaterialParameterValue_Internal(InParameterName, InValue, InMeshComponent);
}

DECLARE_LOG_CATEGORY_EXTERN(WRVehicle, Display, All);