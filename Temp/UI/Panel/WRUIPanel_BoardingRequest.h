// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRUIPanel_BoardingRequest.generated.h"

DECLARE_DELEGATE_ThreeParams(FWROnFinishedBoardingRequestTime, AID_t, AID_t, int8);

enum class EWRBoardingRequestType : uint8
{
	Driver,
	Passenger,
};

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel_BoardingRequest : public UWRUIPanel
{
	GENERATED_BODY()
	
private:
	class UWRTextBlock* WRTextBlock_Comment;
	class UCanvasPanel* CanvasPanel_Accept;
	class UCanvasPanel* CanvasPanel_Reject;
	class UWRProgressBar* ProgressBar_Timer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FText DriverContents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FText PassengerContents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float RequestTime = 5.f;

	AID_t PassengerAID = INVALID_AID;
	AID_t VehicleAID = INVALID_AID;
	int8 SeatIndex = INVALID_SEAT_INDEX;
	FWROnFinishedBoardingRequestTime TimeFinishedDelegate;
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetBoardingType(const EWRBoardingRequestType IN InType, FWROnFinishedBoardingRequestTime IN InDelegate, const AID_t IN InPassengerAID = INVALID_AID, const AID_t IN InVehicleAID = INVALID_AID, const int8 IN InSeatIndex = INVALID_SEAT_INDEX);
	void SetBoardingContents(const FText IN InContents);

	void OnCompleteProgress();
	FORCEINLINE AID_t GetPassengerAID() { return PassengerAID; }
	FORCEINLINE AID_t GetVehicleAID() { return VehicleAID; }
	FORCEINLINE int8 GetSeatIndex() { return SeatIndex; }
};
