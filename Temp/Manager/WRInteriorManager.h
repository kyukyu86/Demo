#pragma once
#include "Manager/SingletonClass/WRSingleton.h"
#include "Network/Share/Define/Define_Type.h"
#include "Enum/EWRCharacter.h"
#include "Engine/EngineBaseTypes.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Network/Share/Protocol/Protocol_Game_User_Inventory.h"
#include "WRSpawnSimulationManager.h"

class AWREnvInterior;
class WR_API WRInteriorManager : public WRSingleton<WRInteriorManager>
{
public:
	WRInteriorManager();
	virtual ~WRInteriorManager();

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

public:
	void SpawnInteriorToSimulate(ISN_t InISN, WRTableID InTID);
	bool InputEvent(EWRInput InInputType, EInputEvent InEventType);
	bool IsSpawnedByMe(ISN_t InISN);

public: // spawn simulation delegate func
	void OnSpawnInteriorCompletedWhenSimulating(UObject* InActor, FString InHashKey);
	void RequestSpawnInterior(ISN_t InISN, WRTableID InTID);
	void RequestRemoveInteriorItem(ISN_t InISN);
	void RemoveSimulationInterior();
	void MakeSpawnArea();
	EWRSimulationResultType IsValidSpawnLocation(AActor* InActor);
	void Message(EWRSimulationResultType InType);

public:
	void AckArrangeInteriorItem(const pt_Game_User_Inventory::Ack_ArrangeInteriorItem_G2U& InAck);
	void AckRemoveInteriorItem(const pt_Game_User_Inventory::Ack_RemoveInteriorItem_G2U& InAck);

public:
	UFUNCTION()
	void OnBeginOverlapWhenNotSpawned(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapWhenNotSpawned(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	bool SetIOMaterialParameter_deprecated(AActor* InActor, float InValue);

private:
// 	TMap<FString, AID_t> AsyncSpawnInteriorKey;
// 	TMap<FString, int32> InteriorSpawnInfo;	//int32 인테리어 스폰패킷으로 바꿀것	
// 	TMap<AID_t, AWREnvInterior*> InteriorAccContainer;	//<Interior aid, Acc ptr>

	FVector BoxPoint[4];
	FVector EdgeLines[4];
	static const int32 SpawnInteriorAreaLength;

	TSet<ISN_t> InteriorItemSpawnedByMe;
};

