#pragma once
#include "SingletonClass/WRSingleton.h"
#include "Engine/EngineTypes.h"

class AWRVehicle;
class UPrimitiveComponent;
class AActor;
struct FHitResult;

UENUM(BlueprintType)
enum class EWRAutoPilotStep : uint8
{
	None = 0,
	PauseAuto = 1 << 1,
	CommonAutoPilot = 1 << 2,
	AvoidingFront = 1 << 3,
	AvoidingLeft = 1 << 4,
	AvoidingRight = 1 << 5,
	FullBrake = 1 << 6,
};
ENUM_CLASS_FLAGS(EWRAutoPilotStep);


UENUM(BlueprintType)
enum class EWROverlappedAutoPilotSensor: uint8
{
	None = 0,
	FRSensor = 1 << 1,
	FLSensor = 1 << 2,
	RSensor = 1 << 3,
};
ENUM_CLASS_FLAGS(EWROverlappedAutoPilotSensor);

struct FWRUpdateElapsedTime
{
	float ElapsedTime2Update = 0.0f;
	float MaxTime = 0.2f;

	bool TickElapsedTime(float InDelta)
	{
		ElapsedTime2Update += InDelta;
		if (ElapsedTime2Update < 0.2f)
			return false;

		ElapsedTime2Update = 0.0f;

		return true;
	}

	void Reset(float InMaxTime = 0.2f)
	{
		ElapsedTime2Update = 0.0f;
		MaxTime = InMaxTime;
	}
};

class WR_API WRAutoPilotManager : public WRSingleton<WRAutoPilotManager>
{
public:
	WRAutoPilotManager();
	virtual ~WRAutoPilotManager();

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

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

public:
	void OnBeginOverlapAutoPilot(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void OnEndOverlapAutoPilot(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void BeginAutoPilot(AWRVehicle* InVehicle);
	void EndAutoPilot(AWRVehicle* InVehicle);
	void PauseAutoPilot();
	void ResumeAutoPilot();
	bool IsActiveAutoPilot();

	bool IsValidSteFlag(EWRAutoPilotStep InFlag);
	bool IsvalidSensorFlag(EWROverlappedAutoPilotSensor InFlag);

private:
	void AddOverlappedSensor(EWROverlappedAutoPilotSensor InState);
	void RemoveOverlappedSensor(EWROverlappedAutoPilotSensor InState);

	//autopilot step�� ���� ������
	void ProcessBraking(float InDeltaTime, AWRVehicle* InVehicle);
	void ProcessPauseAutoPilot(float InDeltaTime, AWRVehicle* InVehicle);
	void ProcessCommonAutoPilot(float InDeltaTime, AWRVehicle* InVehicle);
	void ProcessBrake(float InDeltaTime, AWRVehicle* InVehicle);
	void ProcessAvoiding(float InDeltaTime, AWRVehicle* InVehicle);

	FVector GetDestLocation_Test(AWRVehicle* InVehicle);

private:
	EWROverlappedAutoPilotSensor OverlappedSensor = EWROverlappedAutoPilotSensor::None;
	EWRAutoPilotStep AutoPilotStep = EWRAutoPilotStep::None;


	FVector FinalDestLocation = FVector(-3920.0f, -8695.0f, 512.0f);
	FVector DestLocation1 = FVector(-3920.0f, -8695.0f, 512.0f);
	FVector DestLocation2 = FVector(-68890.0f, -8655.0f, 512.f);

	FWRUpdateElapsedTime ElapsedTime;
};

