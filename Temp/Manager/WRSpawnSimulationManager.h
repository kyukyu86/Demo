#pragma once
#include "Manager/SingletonClass/WRSingleton.h"
#include "Network/Share/Define/Define_Type.h"
#include "Manager/WRAsyncLoadUObjectManager.h"
#include "GameFramework/Actor.h"
#include "Enum/EWRCharacter.h"
#include "Engine/EngineBaseTypes.h"
#include "Utility/WRActorSpawnUtility.h"
#include "Enum/EWRUIEnum.h"
#include "Core/WRGameInstance.h"

UENUM(BlueprintType)
enum class EWRSimulationResultType : uint8
{
	//common
	None,
	Success,
	InvalidLocation,
	NotSpawnArea,

	//vehicle
	NotVolumeArea,
};

DECLARE_DELEGATE(FWROKDelegate);
DECLARE_DELEGATE(FWRCancelDelegate);
DECLARE_DELEGATE_OneParam(FWRMessageDelegate, EWRSimulationResultType);
DECLARE_DELEGATE_RetVal_OneParam(EWRSimulationResultType, FWRValidLocationDelegate, AActor*);
// DECLARE_DELEGATE_RetVal_TwoParams(bool, FWRSetMaterialParameterDelegate, AActor*, float);

#define MAX_DISTANCE_TO_SPAWN 2000.0f
struct FSimulationDelegateHandles
{
	//비동기 로드시 스폰완료될때 받을 델리
	FWRCompleteAsyncLoad SpawnCompletedDele;
	//확인버튼 누를때 받을 델리
	FWROKDelegate OKDele;
	//취소버튼 누를때 받을 델리
	FWRCancelDelegate CancelDele;
	//메세지 용
	FWRMessageDelegate MessageDele;
	//맞는 위치인지 체크
	FWRValidLocationDelegate ValidLocationDele;

	void Reset()
	{
		SpawnCompletedDele.Unbind();
		OKDele.Unbind();
		CancelDele.Unbind();
		MessageDele.Unbind();
		ValidLocationDele.Unbind();
	}
};

struct FSpawnSimulationParam
{
	EWRSpawnSimulationState SimulationState = EWRSpawnSimulationState::None;
	AActor* Actor = nullptr;
	FString HashKeyToSimulate = "";
	FVector ForwardVector = FVector::ZeroVector;
	FVector RightVector = FVector::ZeroVector;
	FSimulationDelegateHandles SimulationDelegateHandles;


	void Reset()
	{
		SimulationState = EWRSpawnSimulationState::None;
		Actor = nullptr;
		HashKeyToSimulate = "";
		ForwardVector = FVector::ZeroVector;
		RightVector = FVector::ZeroVector;
		SimulationDelegateHandles.Reset();
	}

	bool IsValid()
	{
		if (Actor != nullptr)
		{
			return true;
		}
		
		return false;
	}
};

class WR_API WRSpawnSimulationManager : public WRSingleton<WRSpawnSimulationManager>
{
public:
	WRSpawnSimulationManager();
	virtual ~WRSpawnSimulationManager();

public:
	//====================================================================================
// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================

	virtual bool OnTick(float InDeltaTime);


public:
	template <class T>
	T* SyncSpawnSimulationActor(const FString& InBpPath, const FVector& InLocation, const FRotator& InRotation, const FVector& InForward, const FVector& InRight, const FSimulationDelegateHandles& InDelegateHandles);

	template <class T>
	T* GetSimulationActor();

public:
	UFUNCTION()
	void OnBeginOverlapSimulationActor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapSimulationActor(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void AsyncSpawnSimulationActor(const FString& InBpPath, const FVector& InForward, const FVector& InRight, const FSimulationDelegateHandles& InDelegateHandles);
	void OnSpawnCompleted(UObject* InActor, FString InHashKey);
	
	bool InputEvent(EWRInput InInputType, EInputEvent InEventType, float InScale = 0.0f);
	bool IsValidSimulation();
	void InitializeSimulationParam();
	void ResetSimulationParam();
	void ExecuteCancelDele();
	FSpawnSimulationParam GetSimulationParam();
	FVector GetSimulationActorLocation();
	FRotator GetSimulationActorRotation();
	bool IsMySpawnSimulationActor(const FString& InHashKey);
	EWRSpawnSimulationState GetSimulationState();
	
	void MoveActorToForward(float InDistance);
	void MoveActorToRight(float InDistance);
	void DetermineValidLocation(FVector InLocation);
	void RotateActor(float InAngle);
	void CloseUI(const EWRUIPanelKind IN InUIKind);

private:
	void OnComplete(UObject* InObject, FString InHash);

	//deprecated
private:
	void UpdateActorLocationInVolume_deprecated(AActor* InActor, bool bInVolume);


private:
	TMap<FString, AID_t> AsyncSpawnVehicleKey;
	FSpawnSimulationParam SpawnSimulationParam;

	FString strSimulationUIAsyncKey = "";

};

template <class T> 
T* WRSpawnSimulationManager::SyncSpawnSimulationActor(const FString& InBpPath, const FVector& InLocation, const FRotator& InRotation, const FVector& InForward, const FVector& InRight, const FSimulationDelegateHandles& InDelegateHandles)
{
	SpawnSimulationParam.Actor = WRActorSpawnUtility::Spawn<T>(UWRGameInstance::GetGameInstance()->GetWorld(), InBpPath, InLocation, InRotation);
	if (SpawnSimulationParam.Actor)
	{
		SpawnSimulationParam.SimulationState = EWRSpawnSimulationState::ReadyToSimulation;
		SpawnSimulationParam.ForwardVector = InForward;
		SpawnSimulationParam.RightVector = InRight;
		SpawnSimulationParam.SimulationDelegateHandles = InDelegateHandles;
		if (SpawnSimulationParam.SimulationDelegateHandles.OKDele.IsBound() == false)
		{
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SyncSpawnSimulationActor"), TEXT("OKDele must be bound"));
		}

		if (SpawnSimulationParam.SimulationDelegateHandles.CancelDele.IsBound() == false)
		{
			FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SyncSpawnSimulationActor"), TEXT("CancelDele must be bound"));
		}
	}
	else
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("SyncSpawnSimulationActor"), TEXT("Spawn failed to simulate"));
	}

	return Cast<T>(SpawnSimulationParam.Actor);
}

template <class T>
T* WRSpawnSimulationManager::GetSimulationActor()
{
	return Cast<T>(SpawnSimulationParam.Actor);
}
