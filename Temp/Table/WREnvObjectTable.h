#pragma once
#include "Base/WRTableRow.h"
#include "WREnvObjectTable.generated.h"


USTRUCT(BlueprintType)
struct FWREnvObjectTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT(""); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnvObjectBaseInfo)
		int32 EnvObjectTID;

};


USTRUCT(BlueprintType)
struct FWRMovingWalkwayTable : public FWREnvObjectTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("EnvData_MovingWalkway"); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnvMovingWalkwayInfo)
		FString BlueprintPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnvMovingWalkwayInfo)
		FString SoundEffectPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnvMovingWalkwayInfo)
		float  MoveMeterPerSpeed = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnvMovingWalkwayInfo)
		float DistanceToSpawnSound = -1.f;


};

USTRUCT(BlueprintType)
struct FWRSecretDialogAreaTable : public FWREnvObjectTable
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("EnvData_SecretDialogArea"); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		FString SecretDialogBlueprintPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		FString SecretDialogAreaBlueprintPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		FString SecretDialogDomeBlueprintPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		FString SecretDialogSpawnAudioPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		FString SecretDialogUnspawnAudioPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		FVector AreaLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float LeaveDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float MinPortalSize = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float MaxPortalSize = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float ClosePanelTime = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float DomeOpenTimer = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float MinVolume = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecretDialogInfo)
		float MaxVolume = 1.0f;
};