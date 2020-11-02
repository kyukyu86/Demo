// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actor/EnvActor/WREnvActor.h"
#include "Network/Share/Define/Define_Type.h"
#include "Define/WRUIDefine.h"
#include "WREnvSlideDoor.generated.h"

/*
	[ 2020-2-18 : kyu ] 
	Elevator Door, Vehicle Outter Door 등 Slide 문이 필요할 때 이 문을 사용하여 열고 닫기를 설정 할 수 있습니다.
	PositionIndex 로 문별로 인덱싱합니다
*/

UCLASS()
class WR_API AWREnvSlideDoor : public AWREnvActor
{
	GENERATED_BODY()
	
public:
	AWREnvSlideDoor();

private:
	TArray<FString> MakeDebugString();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnBeginOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnEndOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	FORCEINLINE int32 GetPositionIndex() { return PositionIndex; }
	FORCEINLINE AID_t GetOwnerVehicleAID() { return OwnerVehicleAID; }
	FORCEINLINE bool IsOutterDoor() { return IsVehicleOutterDoor; }

	void OpenDoor();
	void CloseDoor();

	void SetOwnerVehicle(class AWREnvVehicleBase* IN InOwnerVehicle);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		int32 PositionIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		bool IsAutoOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		bool IsVehicleOutterDoor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<class UAnimSequenceBase> OpenAnimAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
		TSoftObjectPtr<class UAnimSequenceBase> CloseAnimAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* InteractionArea;

	class USkeletalMeshComponent* SkeletalMesh = nullptr;

	// Owner
	AID_t OwnerVehicleAID = INVALID_AID;
	WRUIHandle UIHandle = INVALID_UI_HANDLE;	// [ 2020-4-6 : kyu ] 스폰된 대상이 아니기 때문에 UIHandle로 UI해제요청해야한다.


};
